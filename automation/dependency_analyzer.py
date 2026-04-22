"""
Dependency analyzer for function call graphs.

Analyzes assembly files to determine which functions call which other functions,
enabling dependency-aware scheduling for decompilation.
"""

import re
import sqlite3
from pathlib import Path
from typing import Dict, Set, List, Optional
from collections import defaultdict


class DependencyAnalyzer:
    """Analyzes function dependencies from assembly files."""
    
    def __init__(self, project_root: Path, db_path: Optional[Path] = None,
                 blocker_multiplier: int = 10):
        self.project_root = project_root
        self.asm_dir = project_root / "asm"

        # Multiplier for the "number of dependents" bonus in scoring.
        # Higher values surface high-blocker functions more aggressively.
        self._blocker_multiplier = blocker_multiplier

        # Database connection (optional, for sibling completion scoring)
        self.db_path = db_path or (project_root / "automation" / "functions.db")
        self.db_conn = None
        
        # Dependency graph: function_name -> set of functions it calls
        self.calls: Dict[str, Set[str]] = defaultdict(set)
        
        # Reverse graph: function_name -> set of functions that call it
        self.called_by: Dict[str, Set[str]] = defaultdict(set)
        
        # All known function names
        self.all_functions: Set[str] = set()
        
        # Cache for C file completion rates
        self.c_file_completion: Dict[str, float] = {}
    
    def analyze_asm_file(self, asm_file: Path) -> Dict[str, Set[str]]:
        """
        Analyze a single assembly file to extract function calls.
        
        Each .s file typically contains one function. We extract:
        - Function name from glabel
        - All functions it calls via 'jal' instructions
        
        Returns:
            Dict mapping function names to the set of functions they call
        """
        if not asm_file.exists() or asm_file.suffix != '.s':
            return {}
        
        try:
            content = asm_file.read_text()
        except:
            return {}
        
        result = {}
        
        # Split into function blocks using glabel markers
        # Format: glabel func_name
        function_pattern = r'^\s*glabel\s+(\w+)\s*$'
        
        current_function = None
        current_calls = set()
        
        for line in content.split('\n'):
            # Check for function label
            func_match = re.match(function_pattern, line)
            if func_match:
                # Save previous function
                if current_function:
                    result[current_function] = current_calls
                
                # Start new function
                current_function = func_match.group(1)
                current_calls = set()
                continue
            
            # Look for function calls (jal instruction)
            # Format: jal func_name or /* ... */ jal func_name
            if current_function:
                # Match jal with optional comment before it
                call_match = re.search(r'jal\s+(\w+)', line)
                if call_match:
                    called_func = call_match.group(1)
                    # Filter out register names and common false positives
                    if not called_func.startswith('$') and not called_func in ('delay', 'nop'):
                        current_calls.add(called_func)
        
        # Save last function
        if current_function:
            result[current_function] = current_calls
        
        return result
    
    def build_call_graph(self, module: Optional[str] = None) -> None:
        """
        Build the complete call graph from assembly files.
        
        Args:
            module: Optional module filter (e.g., 'battle', 'world')
        """
        self.calls.clear()
        self.called_by.clear()
        self.all_functions.clear()
        
        # Find all assembly files
        # Structure: asm/us/MODULE/nonmatchings/**/*.s
        if module:
            # Look in specific module directory
            search_dirs = [
                self.asm_dir / "us" / module / "nonmatchings",
            ]
        else:
            # Look in all module directories
            search_dirs = [
                self.asm_dir / "us",
            ]
        
        asm_files = []
        for search_dir in search_dirs:
            if search_dir.exists():
                asm_files.extend(search_dir.rglob("*.s"))
        
        # Analyze each file
        for asm_file in asm_files:
            # Skip data and bss files
            if '.data.s' in asm_file.name or '.bss.s' in asm_file.name:
                continue
            
            file_calls = self.analyze_asm_file(asm_file)
            
            for func_name, called_funcs in file_calls.items():
                self.all_functions.add(func_name)
                self.calls[func_name].update(called_funcs)
                
                # Build reverse graph
                for called_func in called_funcs:
                    self.all_functions.add(called_func)
                    self.called_by[called_func].add(func_name)
    
    def get_dependencies(self, func_name: str) -> Set[str]:
        """Get the set of functions that this function calls."""
        return self.calls.get(func_name, set())
    
    def get_dependents(self, func_name: str) -> Set[str]:
        """Get the set of functions that call this function."""
        return self.called_by.get(func_name, set())
    
    def get_unresolved_dependencies(self, func_name: str, decompiled_funcs: Set[str]) -> Set[str]:
        """
        Get dependencies that haven't been decompiled yet.
        
        Args:
            func_name: Function to check
            decompiled_funcs: Set of functions already decompiled
            
        Returns:
            Set of function names that are dependencies but not yet decompiled
        """
        deps = self.get_dependencies(func_name)
        return deps - decompiled_funcs
    
    def get_c_file_completion_rate(self, c_file_path: str) -> float:
        """
        Get the completion rate (% of verified functions) for a C file.
        
        Args:
            c_file_path: Path to C file (e.g., 'src/battle/battle.c')
            
        Returns:
            Completion rate as percentage (0-100)
        """
        if c_file_path in self.c_file_completion:
            return self.c_file_completion[c_file_path]
        
        if not self.db_conn:
            try:
                self.db_conn = sqlite3.connect(self.db_path)
                self.db_conn.row_factory = sqlite3.Row
            except:
                return 0.0
        
        try:
            cursor = self.db_conn.cursor()
            cursor.execute("""
                SELECT 
                    COUNT(*) as total,
                    SUM(CASE WHEN status = 'verified' THEN 1 ELSE 0 END) as verified
                FROM functions
                WHERE c_file_path = ?
            """, (c_file_path,))
            row = cursor.fetchone()
            
            if row and row['total'] > 0:
                completion = (row['verified'] / row['total']) * 100.0
            else:
                completion = 0.0
            
            self.c_file_completion[c_file_path] = completion
            return completion
        except:
            return 0.0
    
    def compute_dependency_score(self, func_name: str, decompiled_funcs: Set[str], 
                                c_file_path: Optional[str] = None) -> float:
        """
        Compute a score for prioritizing decompilation.
        
        Higher score = better candidate for decompilation.
        
        Score factors:
        - Functions with all dependencies resolved: +100
        - Functions with some dependencies resolved: proportional bonus
        - Leaf functions (no dependencies): +50
        - Functions that many others depend on: bonus based on dependents
        - Sibling completion: Up to +50 based on how many functions in same C file are done
        
        Returns:
            Score (0-250+)
        """
        deps = self.get_dependencies(func_name)
        
        # Leaf function (calls nothing or only decompiled functions)
        if not deps:
            score = 150.0  # Very high priority
        else:
            # Count resolved vs unresolved dependencies
            unresolved = deps - decompiled_funcs
            resolved_ratio = (len(deps) - len(unresolved)) / len(deps)
            
            # Base score from dependency resolution
            score = resolved_ratio * 100.0
            
            # All dependencies resolved - major bonus
            if not unresolved:
                score += 100.0
        
        # Bonus for being a dependency of many functions (unlock more work).
        # Uses a higher multiplier and cap so high-blocker functions meaningfully
        # outrank leaf functions — blocking 9 callers gives +90, blocking 20 gives +200.
        dependents = len(self.get_dependents(func_name))
        blocker_multiplier = getattr(self, '_blocker_multiplier', 10)
        score += min(dependents * blocker_multiplier, 200)
        
        # Sibling completion bonus
        if c_file_path:
            completion_rate = self.get_c_file_completion_rate(c_file_path)
            # Higher completion = fewer sibling placeholder issues
            # At 50%+ completion, give full bonus. Below that, scale proportionally.
            sibling_bonus = min(completion_rate / 50.0, 1.0) * 50.0
            score += sibling_bonus
        
        return score
    
    def get_recommended_functions(self, 
                                 candidate_funcs: List[str],
                                 decompiled_funcs: Set[str],
                                 limit: int = 20) -> List[tuple]:
        """
        Get recommended functions to decompile based on dependencies.
        
        Args:
            candidate_funcs: List of function names to consider
            decompiled_funcs: Set of already decompiled function names
            limit: Maximum number of recommendations
            
        Returns:
            List of (func_name, score, unresolved_count) tuples, sorted by score
        """
        recommendations = []
        
        for func_name in candidate_funcs:
            score = self.compute_dependency_score(func_name, decompiled_funcs)
            unresolved = len(self.get_unresolved_dependencies(func_name, decompiled_funcs))
            
            recommendations.append((func_name, score, unresolved))
        
        # Sort by score (descending), then by unresolved count (ascending)
        recommendations.sort(key=lambda x: (-x[1], x[2]))
        
        return recommendations[:limit]
    
    # ============================================================
    # Type-Aware Decompilation Scoring
    # ============================================================
    
    def extract_extern_symbols(self, asm_file: Path) -> Set[str]:
        """
        Extract extern symbol references from an assembly file.
        
        Looks for patterns like:
        - lui/addiu pairs: lui $v0, %hi(D_800XXXXX)
        - lw/sw with symbol: lw $v0, D_800XXXXX
        
        Returns:
            Set of extern symbol names referenced
        """
        if not asm_file.exists():
            return set()
        
        try:
            content = asm_file.read_text()
        except:
            return set()
        
        symbols = set()
        
        # Pattern for %hi/%lo symbol references
        hi_lo_pattern = r'%(?:hi|lo)\(([A-Za-z_][A-Za-z0-9_]*)\)'
        
        # Pattern for direct symbol references (not registers)
        # Like: lw $v0, D_800F83D0
        direct_pattern = r'\s+(?:lw|lh|lb|sw|sh|sb|lui|addiu|la)\s+\$\w+,\s*(-?\d+\()?\s*([A-Za-z_][A-Za-z0-9_]*)'
        
        for line in content.split('\n'):
            # Check for %hi/%lo references
            for match in re.finditer(hi_lo_pattern, line):
                symbol = match.group(1)
                # Filter out obvious non-data symbols
                if symbol.startswith('D_') or symbol.startswith('g_') or symbol.isupper():
                    symbols.add(symbol)
            
            # Check for direct references
            match = re.search(direct_pattern, line)
            if match:
                symbol = match.group(2)
                # Filter: must start with D_, g_, or be all caps data symbols
                if symbol.startswith('D_') or symbol.startswith('g_') or (symbol.isupper() and '_' in symbol):
                    symbols.add(symbol)
        
        return symbols
    
    def get_function_extern_symbols(self, func_name: str) -> Set[str]:
        """
        Get extern symbols referenced by a specific function.
        
        Args:
            func_name: Function name
            
        Returns:
            Set of extern symbol names
        """
        # Find the assembly file for this function
        # Typical path: asm/us/MODULE/nonmatchings/SUBDIR/func_name.s
        asm_files = list(self.asm_dir.rglob(f"**/{func_name}.s"))
        
        symbols = set()
        for asm_file in asm_files:
            symbols.update(self.extract_extern_symbols(asm_file))
        
        return symbols
    
    def check_type_availability(self, func_name: str, known_types: Dict[str, Dict]) -> tuple:
        """
        Check if types are available for all extern symbols used by a function.
        
        Args:
            func_name: Function to check
            known_types: Dict from DB.get_all_known_types()
            
        Returns:
            (known_count, unknown_count, unknown_symbols)
        """
        extern_symbols = self.get_function_extern_symbols(func_name)
        
        if not extern_symbols:
            # No extern symbols = no type issues
            return (0, 0, set())
        
        known = 0
        unknown_symbols = set()
        
        for symbol in extern_symbols:
            if symbol in known_types and known_types[symbol]['confidence'] >= 0.3:
                known += 1
            else:
                unknown_symbols.add(symbol)
        
        return (known, len(unknown_symbols), unknown_symbols)
    
    def compute_type_aware_score(self, func_name: str, decompiled_funcs: Set[str],
                                 known_types: Dict[str, Dict],
                                 c_file_path: Optional[str] = None) -> float:
        """
        Enhanced dependency score that considers type availability.
        
        Score factors:
        - All base dependency factors from compute_dependency_score
        - Type availability bonus: Functions with all types known get +50
        - Type availability penalty: Functions with many unknown types penalized
        
        Returns:
            Score (0-300+)
        """
        # Start with base dependency score
        score = self.compute_dependency_score(func_name, decompiled_funcs, c_file_path)
        
        # Add type awareness
        known_count, unknown_count, unknown_symbols = self.check_type_availability(
            func_name, known_types
        )
        
        total_symbols = known_count + unknown_count
        
        if total_symbols == 0:
            # No extern symbols = no type issues = small bonus
            score += 25.0
        elif unknown_count == 0:
            # All types known = big bonus
            score += 50.0
        else:
            # Some types unknown = penalty proportional to unknowns
            type_ratio = known_count / total_symbols
            # Scale penalty: 0% known = -50, 100% known = +50
            type_adjustment = (type_ratio * 100) - 50
            score += type_adjustment
        
        return score
    
    def get_type_aware_recommendations(self,
                                      candidate_funcs: List[str],
                                      decompiled_funcs: Set[str],
                                      known_types: Dict[str, Dict],
                                      limit: int = 20) -> List[tuple]:
        """
        Get recommendations that consider both dependencies and type availability.
        
        Args:
            candidate_funcs: Functions to consider
            decompiled_funcs: Already decompiled functions
            known_types: Dict from DB.get_all_known_types()
            limit: Max recommendations
            
        Returns:
            List of (func_name, score, unresolved_deps, unknown_types) tuples
        """
        recommendations = []
        
        for func_name in candidate_funcs:
            score = self.compute_type_aware_score(
                func_name, decompiled_funcs, known_types
            )
            unresolved = len(self.get_unresolved_dependencies(func_name, decompiled_funcs))
            known, unknown, _ = self.check_type_availability(func_name, known_types)
            
            recommendations.append((func_name, score, unresolved, unknown))
        
        # Sort by score (descending)
        recommendations.sort(key=lambda x: -x[1])
        
        return recommendations[:limit]
    
    def get_function_status_map(self) -> Dict[str, str]:
        """
        Query the DB for the status of every known function.

        Returns:
            Dict mapping function_name -> status string (e.g. 'verified', 'todo', …)
            Functions not in the DB are mapped to 'unknown'.
        """
        if not self.db_conn:
            try:
                self.db_conn = sqlite3.connect(self.db_path)
                self.db_conn.row_factory = sqlite3.Row
            except Exception:
                return {}

        try:
            cursor = self.db_conn.cursor()
            cursor.execute("SELECT name, status FROM functions")
            return {row['name']: row['status'] for row in cursor.fetchall()}
        except Exception:
            return {}

    def generate_dependency_report(self, output_path: Optional[Path] = None) -> str:
        """
        Generate a report listing all known functions separated into two categories:

        1. Decompiled – functions whose status is 'verified', 'decompiled', or
           'decompiled_needs_refine', sorted descending by number of dependents.
        2. Not yet decompiled – all remaining functions (todo, blocked, failed, …),
           sorted descending by number of dependents.

        Within each category functions are additionally grouped by their exact DB
        status so the file is easy to scan.

        Args:
            output_path: If supplied, the report is written to this file in addition
                         to being returned as a string.

        Returns:
            The full report as a string.
        """
        DECOMPILED_STATUSES = {'verified', 'decompiled', 'decompiled_needs_refine', 'sdk_excluded'}

        status_map = self.get_function_status_map()

        # Functions with no ASM split file are either already matched or are SDK
        # stubs in the main binary — exclude them from the "not yet decompiled" list.
        funcs_with_asm = {asm_file.stem for asm_file in self.asm_dir.rglob('*.s') if 'nonmatchings' in asm_file.parts}

        decompiled: List[tuple] = []      # (func_name, dependents, status)
        not_decompiled: List[tuple] = []  # (func_name, dependents, status)

        for func in self.all_functions:
            dep_count = len(self.called_by.get(func, set()))
            status = status_map.get(func, 'unknown')
            if status in DECOMPILED_STATUSES:
                decompiled.append((func, dep_count, status))
            elif func in funcs_with_asm:
                not_decompiled.append((func, dep_count, status))

        # Sort each list by descending dependent count, then name for stable ordering
        decompiled.sort(key=lambda x: (-x[1], x[0]))
        not_decompiled.sort(key=lambda x: (-x[1], x[0]))

        lines: List[str] = []

        def _write_section(title: str, entries: List[tuple]) -> None:
            lines.append('=' * 72)
            lines.append(title)
            lines.append(f'  Total: {len(entries)} functions')
            lines.append('=' * 72)
            if not entries:
                lines.append('  (none)')
                lines.append('')
                return
            # Sub-group by status
            by_status: Dict[str, List[tuple]] = {}
            for entry in entries:
                by_status.setdefault(entry[2], []).append(entry)
            for status_key in sorted(by_status.keys()):
                group = by_status[status_key]
                lines.append(f'  [{status_key}]  ({len(group)} functions)')
                lines.append(f'  {"Function":<40}  {"Dependents":>10}  {"Calls":>8}')
                lines.append(f'  {"-" * 40}  {"-" * 10}  {"-" * 8}')
                for fname, dep_count, _ in group:
                    call_count = len(self.calls.get(fname, set()))
                    lines.append(f'  {fname:<40}  {dep_count:>10}  {call_count:>8}')
                lines.append('')

        lines.append('FF7 Decompilation Dependency Report')
        lines.append(f'Generated: {__import__("datetime").datetime.now().strftime("%Y-%m-%d %H:%M:%S")}')
        lines.append(f'Total functions in call graph: {len(self.all_functions)}')
        lines.append('')

        _write_section(
            f'SECTION 1 – DECOMPILED  ({len(decompiled)} functions)',
            decompiled,
        )
        _write_section(
            f'SECTION 2 – NOT YET DECOMPILED  ({len(not_decompiled)} functions)',
            not_decompiled,
        )

        report = '\n'.join(lines)

        if output_path is not None:
            output_path = Path(output_path)
            output_path.write_text(report)

        return report

    def get_function_status_map(self) -> Dict[str, str]:
        """
        Query the DB for the status of every known function.

        Returns:
            Dict mapping function_name -> status string (e.g. 'verified', 'todo', …)
            Functions not in the DB are mapped to 'unknown'.
        """
        if not self.db_conn:
            try:
                self.db_conn = sqlite3.connect(self.db_path)
                self.db_conn.row_factory = sqlite3.Row
            except Exception:
                return {}

        try:
            cursor = self.db_conn.cursor()
            cursor.execute("SELECT name, status FROM functions")
            return {row['name']: row['status'] for row in cursor.fetchall()}
        except Exception:
            return {}

    def generate_dependency_report(self, output_path: Optional[Path] = None) -> str:
        """
        Generate a report listing all known functions separated into two categories:

        1. Decompiled – functions whose status is 'verified', 'decompiled', or
           'decompiled_needs_refine', sorted descending by number of dependents.
        2. Not yet decompiled – all remaining functions (todo, blocked, failed, …),
           sorted descending by number of dependents.

        Within each category functions are additionally grouped by their exact DB
        status so the file is easy to scan.

        Args:
            output_path: If supplied, the report is written to this file in addition
                         to being returned as a string.

        Returns:
            The full report as a string.
        """
        DECOMPILED_STATUSES = {'verified', 'decompiled', 'decompiled_needs_refine', 'sdk_excluded'}

        status_map = self.get_function_status_map()

        # Functions with no ASM split file are either already matched or are SDK
        # stubs in the main binary — exclude them from the "not yet decompiled" list.
        funcs_with_asm = {asm_file.stem for asm_file in self.asm_dir.rglob('*.s') if 'nonmatchings' in asm_file.parts}

        decompiled: List[tuple] = []      # (func_name, dependents, status)
        not_decompiled: List[tuple] = []  # (func_name, dependents, status)

        for func in self.all_functions:
            dep_count = len(self.called_by.get(func, set()))
            status = status_map.get(func, 'unknown')
            if status in DECOMPILED_STATUSES:
                decompiled.append((func, dep_count, status))
            elif func in funcs_with_asm:
                not_decompiled.append((func, dep_count, status))

        # Sort each list by descending dependent count, then name for stable ordering
        decompiled.sort(key=lambda x: (-x[1], x[0]))
        not_decompiled.sort(key=lambda x: (-x[1], x[0]))

        lines: List[str] = []

        def _write_section(title: str, entries: List[tuple]) -> None:
            lines.append('=' * 72)
            lines.append(title)
            lines.append(f'  Total: {len(entries)} functions')
            lines.append('=' * 72)
            if not entries:
                lines.append('  (none)')
                lines.append('')
                return
            # Sub-group by status
            by_status: Dict[str, List[tuple]] = {}
            for entry in entries:
                by_status.setdefault(entry[2], []).append(entry)
            for status_key in sorted(by_status.keys()):
                group = by_status[status_key]
                lines.append(f'  [{status_key}]  ({len(group)} functions)')
                lines.append(f'  {"Function":<40}  {"Dependents":>10}  {"Calls":>8}')
                lines.append(f'  {"-" * 40}  {"-" * 10}  {"-" * 8}')
                for fname, dep_count, _ in group:
                    call_count = len(self.calls.get(fname, set()))
                    lines.append(f'  {fname:<40}  {dep_count:>10}  {call_count:>8}')
                lines.append('')

        lines.append('FF7 Decompilation Dependency Report')
        lines.append(f'Generated: {__import__("datetime").datetime.now().strftime("%Y-%m-%d %H:%M:%S")}')
        lines.append(f'Total functions in call graph: {len(self.all_functions)}')
        lines.append('')

        _write_section(
            f'SECTION 1 – DECOMPILED  ({len(decompiled)} functions)',
            decompiled,
        )
        _write_section(
            f'SECTION 2 – NOT YET DECOMPILED  ({len(not_decompiled)} functions)',
            not_decompiled,
        )

        report = '\n'.join(lines)

        if output_path is not None:
            output_path = Path(output_path)
            output_path.write_text(report)

        return report

    def get_function_status_map(self) -> Dict[str, str]:
        """
        Query the DB for the status of every known function.

        Returns:
            Dict mapping function_name -> status string (e.g. 'verified', 'todo', …)
            Functions not in the DB are mapped to 'unknown'.
        """
        if not self.db_conn:
            try:
                self.db_conn = sqlite3.connect(self.db_path)
                self.db_conn.row_factory = sqlite3.Row
            except Exception:
                return {}

        try:
            cursor = self.db_conn.cursor()
            cursor.execute("SELECT name, status FROM functions")
            return {row['name']: row['status'] for row in cursor.fetchall()}
        except Exception:
            return {}

    def generate_dependency_report(self, output_path: Optional[Path] = None) -> str:
        """
        Generate a report listing all known functions separated into two categories:

        1. Decompiled – functions whose status is 'verified', 'decompiled', or
           'decompiled_needs_refine', sorted descending by number of dependents.
        2. Not yet decompiled – all remaining functions (todo, blocked, failed, …),
           sorted descending by number of dependents.

        Within each category functions are additionally grouped by their exact DB
        status so the file is easy to scan.

        Args:
            output_path: If supplied, the report is written to this file in addition
                         to being returned as a string.

        Returns:
            The full report as a string.
        """
        DECOMPILED_STATUSES = {'verified', 'decompiled', 'decompiled_needs_refine', 'sdk_excluded'}

        status_map = self.get_function_status_map()

        # Functions with no ASM split file are either already matched or are SDK
        # stubs in the main binary — exclude them from the "not yet decompiled" list.
        funcs_with_asm = {asm_file.stem for asm_file in self.asm_dir.rglob('*.s') if 'nonmatchings' in asm_file.parts}

        decompiled: List[tuple] = []      # (func_name, dependents, status)
        not_decompiled: List[tuple] = []  # (func_name, dependents, status)

        for func in self.all_functions:
            dep_count = len(self.called_by.get(func, set()))
            status = status_map.get(func, 'unknown')
            if status in DECOMPILED_STATUSES:
                decompiled.append((func, dep_count, status))
            elif func in funcs_with_asm:
                not_decompiled.append((func, dep_count, status))

        # Sort each list by descending dependent count, then name for stable ordering
        decompiled.sort(key=lambda x: (-x[1], x[0]))
        not_decompiled.sort(key=lambda x: (-x[1], x[0]))

        lines: List[str] = []

        def _write_section(title: str, entries: List[tuple]) -> None:
            lines.append('=' * 72)
            lines.append(title)
            lines.append(f'  Total: {len(entries)} functions')
            lines.append('=' * 72)
            if not entries:
                lines.append('  (none)')
                lines.append('')
                return
            # Sub-group by status
            by_status: Dict[str, List[tuple]] = {}
            for entry in entries:
                by_status.setdefault(entry[2], []).append(entry)
            for status_key in sorted(by_status.keys()):
                group = by_status[status_key]
                lines.append(f'  [{status_key}]  ({len(group)} functions)')
                lines.append(f'  {"Function":<40}  {"Dependents":>10}  {"Calls":>8}')
                lines.append(f'  {"-" * 40}  {"-" * 10}  {"-" * 8}')
                for fname, dep_count, _ in group:
                    call_count = len(self.calls.get(fname, set()))
                    lines.append(f'  {fname:<40}  {dep_count:>10}  {call_count:>8}')
                lines.append('')

        lines.append('FF7 Decompilation Dependency Report')
        lines.append(f'Generated: {__import__("datetime").datetime.now().strftime("%Y-%m-%d %H:%M:%S")}')
        lines.append(f'Total functions in call graph: {len(self.all_functions)}')
        lines.append('')

        _write_section(
            f'SECTION 1 – DECOMPILED  ({len(decompiled)} functions)',
            decompiled,
        )
        _write_section(
            f'SECTION 2 – NOT YET DECOMPILED  ({len(not_decompiled)} functions)',
            not_decompiled,
        )

        report = '\n'.join(lines)

        if output_path is not None:
            output_path = Path(output_path)
            output_path.write_text(report)

        return report

    def get_function_status_map(self) -> Dict[str, str]:
        """
        Query the DB for the status of every known function.

        Returns:
            Dict mapping function_name -> status string (e.g. 'verified', 'todo', …)
            Functions not in the DB are mapped to 'unknown'.
        """
        if not self.db_conn:
            try:
                self.db_conn = sqlite3.connect(self.db_path)
                self.db_conn.row_factory = sqlite3.Row
            except Exception:
                return {}

        try:
            cursor = self.db_conn.cursor()
            cursor.execute("SELECT name, status FROM functions")
            return {row['name']: row['status'] for row in cursor.fetchall()}
        except Exception:
            return {}

    def generate_dependency_report(self, output_path: Optional[Path] = None) -> str:
        """
        Generate a report listing all known functions separated into two categories:

        1. Decompiled – functions whose status is 'verified', 'decompiled', or
           'decompiled_needs_refine', sorted descending by number of dependents.
        2. Not yet decompiled – all remaining functions (todo, blocked, failed, …),
           sorted descending by number of dependents.

        Within each category functions are additionally grouped by their exact DB
        status so the file is easy to scan.

        Args:
            output_path: If supplied, the report is written to this file in addition
                         to being returned as a string.

        Returns:
            The full report as a string.
        """
        DECOMPILED_STATUSES = {'verified', 'decompiled', 'decompiled_needs_refine', 'sdk_excluded'}

        status_map = self.get_function_status_map()

        # Functions with no ASM split file are either already matched or are SDK
        # stubs in the main binary — exclude them from the "not yet decompiled" list.
        funcs_with_asm = {asm_file.stem for asm_file in self.asm_dir.rglob('*.s') if 'nonmatchings' in asm_file.parts}

        decompiled: List[tuple] = []      # (func_name, dependents, status)
        not_decompiled: List[tuple] = []  # (func_name, dependents, status)

        for func in self.all_functions:
            dep_count = len(self.called_by.get(func, set()))
            status = status_map.get(func, 'unknown')
            if status in DECOMPILED_STATUSES:
                decompiled.append((func, dep_count, status))
            elif func in funcs_with_asm:
                not_decompiled.append((func, dep_count, status))

        # Sort each list by descending dependent count, then name for stable ordering
        decompiled.sort(key=lambda x: (-x[1], x[0]))
        not_decompiled.sort(key=lambda x: (-x[1], x[0]))

        lines: List[str] = []

        def _write_section(title: str, entries: List[tuple]) -> None:
            lines.append('=' * 72)
            lines.append(title)
            lines.append(f'  Total: {len(entries)} functions')
            lines.append('=' * 72)
            if not entries:
                lines.append('  (none)')
                lines.append('')
                return
            # Sub-group by status
            by_status: Dict[str, List[tuple]] = {}
            for entry in entries:
                by_status.setdefault(entry[2], []).append(entry)
            for status_key in sorted(by_status.keys()):
                group = by_status[status_key]
                lines.append(f'  [{status_key}]  ({len(group)} functions)')
                lines.append(f'  {"Function":<40}  {"Dependents":>10}  {"Calls":>8}')
                lines.append(f'  {"-" * 40}  {"-" * 10}  {"-" * 8}')
                for fname, dep_count, _ in group:
                    call_count = len(self.calls.get(fname, set()))
                    lines.append(f'  {fname:<40}  {dep_count:>10}  {call_count:>8}')
                lines.append('')

        lines.append('FF7 Decompilation Dependency Report')
        lines.append(f'Generated: {__import__("datetime").datetime.now().strftime("%Y-%m-%d %H:%M:%S")}')
        lines.append(f'Total functions in call graph: {len(self.all_functions)}')
        lines.append('')

        _write_section(
            f'SECTION 1 – DECOMPILED  ({len(decompiled)} functions)',
            decompiled,
        )
        _write_section(
            f'SECTION 2 – NOT YET DECOMPILED  ({len(not_decompiled)} functions)',
            not_decompiled,
        )

        report = '\n'.join(lines)

        if output_path is not None:
            output_path = Path(output_path)
            output_path.write_text(report)

        return report

    def get_ready_functions(
        self,
        candidate_names: List[str],
        resolved_funcs: Set[str],
        db_funcs: Set[str],
        func_to_c_file: Optional[Dict[str, str]] = None,
        check_cross_file: bool = False,
    ) -> List[str]:
        """
        Return the subset of *candidate_names* that are "ready" for strict
        bottom-up decompilation ordering.

        By default (check_cross_file=False):
          A function is ready when every callee that lives in the **same C file**
          is already resolved (verified / decompiled / sdk_excluded).
          Cross-file callees are excluded — decomp_decls.h provides their
          signatures to m2c regardless of decompilation status.

        When check_cross_file=True:
          A function is ready only when ALL db-tracked callees (same-file AND
          cross-file) are resolved. This ensures m2c never encounters a callee
          whose declaration is missing from decomp_decls.h, eliminating the
          "/*?*/" unknown-type markers that cause validation failures.

        Callees not tracked in *db_funcs* (SDK / extern symbols) are always
        treated as satisfied.
        """
        if func_to_c_file is None:
            func_to_c_file = {}

        ready = []
        for func_name in candidate_names:
            callees = self.get_dependencies(func_name)

            if check_cross_file:
                # All db-tracked callees must be resolved
                relevant_callees = callees & db_funcs
            else:
                my_c_file = func_to_c_file.get(func_name)
                if my_c_file:
                    relevant_callees = {
                        c for c in callees
                        if c in db_funcs and func_to_c_file.get(c) == my_c_file
                    }
                else:
                    relevant_callees = callees & db_funcs

            if relevant_callees <= resolved_funcs:
                ready.append(func_name)

        return ready

    def get_statistics(self) -> Dict:
        """Get statistics about the dependency graph."""
        if not self.all_functions:
            return {
                'total_functions': 0,
                'leaf_functions': 0,
                'avg_dependencies': 0.0,
                'max_dependencies': 0,
                'most_depended_on': None
            }
        
        leaf_count = sum(1 for f in self.all_functions if not self.calls.get(f))
        avg_deps = sum(len(deps) for deps in self.calls.values()) / len(self.all_functions)
        max_deps = max((len(deps) for deps in self.calls.values()), default=0)
        
        # Find most depended on function
        most_depended = max(
            ((f, len(self.called_by[f])) for f in self.all_functions),
            key=lambda x: x[1],
            default=(None, 0)
        )
        
        return {
            'total_functions': len(self.all_functions),
            'leaf_functions': leaf_count,
            'avg_dependencies': avg_deps,
            'max_dependencies': max_deps,
            'most_depended_on': most_depended[0],
            'most_depended_count': most_depended[1]
        }


def main():
    """CLI for dependency analysis."""
    import argparse
    from pathlib import Path

    parser = argparse.ArgumentParser(description='FF7 decomp dependency analyzer')
    parser.add_argument('--module', '-m', type=str, default=None,
                        help='Limit analysis to a specific module (e.g. battle, world)')
    parser.add_argument('--function', '-f', type=str, default=None,
                        help='Show detailed info for a specific function')
    parser.add_argument('--report', '-r', type=str, default=None, metavar='OUTPUT_FILE',
                        help='Generate a full dependency report and write it to OUTPUT_FILE')
    parser.add_argument('--top', type=int, default=30,
                        help='Number of top blocking functions to display (default: 30)')
    args = parser.parse_args()

    project_root = Path(__file__).parent.parent
    analyzer = DependencyAnalyzer(project_root)

    if args.module:
        print(f'Analyzing dependencies for module: {args.module}')
        analyzer.build_call_graph(args.module)
    else:
        print('Analyzing dependencies for all modules...')
        analyzer.build_call_graph()

    stats = analyzer.get_statistics()
    print(f'\nDependency Statistics:')
    print(f'  Total functions: {stats["total_functions"]}')
    if stats['total_functions']:
        print(f'  Leaf functions: {stats["leaf_functions"]} ({stats["leaf_functions"]/stats["total_functions"]*100:.1f}%)')
    print(f'  Avg dependencies per function: {stats["avg_dependencies"]:.1f}')
    print(f'  Max dependencies: {stats["max_dependencies"]}')
    print(f'  Most depended on: {stats["most_depended_on"]} ({stats.get("most_depended_count", 0)} dependents)')

    # Top blocking (not yet decompiled) functions
    try:
        status_map = analyzer.get_function_status_map()
        DECOMPILED = {'verified', 'decompiled', 'decompiled_needs_refine', 'sdk_excluded'}
        decompiled_funcs = {f for f, s in status_map.items() if s in DECOMPILED}

        # Only consider functions that have an actual ASM split file — functions
        # with no .s file are either already matched (removed from ASM) or are
        # PSX SDK stubs that live in the main binary and can never be decompiled.
        funcs_with_asm = {
            asm_file.stem
            for asm_file in analyzer.asm_dir.rglob('*.s')
            if 'nonmatchings' in asm_file.parts
        }

        undecompiled_with_dependents = [
            (func, len(analyzer.called_by[func]))
            for func in analyzer.all_functions
            if func not in decompiled_funcs
            and func in funcs_with_asm
            and len(analyzer.called_by[func]) > 0
        ]

        if undecompiled_with_dependents:
            top_blocking = sorted(undecompiled_with_dependents, key=lambda x: x[1], reverse=True)[:args.top]
            print(f'  Top {args.top} blocking functions (not decompiled):')
            for i, (func, dep_count) in enumerate(top_blocking, 1):
                print(f'    {i:2}. {func:<40} ({dep_count:3} dependents)')
        else:
            print('  Top blocking functions (not decompiled): None (all high-value targets done!)')

    except Exception as e:
        print(f'  Top blocking functions (not decompiled): Unable to determine (DB error: {e})')

    # Optional full report
    if args.report:
        output_path = Path(args.report)
        report = analyzer.generate_dependency_report(output_path)
        print(f'\nReport written to: {output_path} ({len(report.splitlines())} lines)')

    # Per-function detail
    if args.function:
        func_name = args.function
        deps = analyzer.get_dependencies(func_name)
        dependents = analyzer.get_dependents(func_name)
        print(f'\nFunction: {func_name}')
        print(f'  Calls {len(deps)} functions: {chr(44)+" ".join(sorted(deps)[:5])}{"..." if len(deps) > 5 else ""}')
        print(f'  Called by {len(dependents)} functions: {", ".join(sorted(dependents)[:5])}{"..." if len(dependents) > 5 else ""}')


if __name__ == '__main__':
    main()
