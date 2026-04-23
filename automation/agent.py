#!/usr/bin/env python3
"""
Autonomous Decompilation Agent

Orchestrates the full decompilation workflow:
1. Discover functions from priority queue
2. Decompile functions automatically
3. Periodically verify with builds
4. Retry failed functions with different strategies
5. Monitor progress and adapt

Usage:
    # Basic usage
    python agent.py --run                                    # Start the agent
    python agent.py --status                                 # Show current statistics
    python agent.py --verify-needs-refine                    # Promote fixed needs_refine functions to verified
    
    # Time and target limits
    python agent.py --run --duration 3600                    # Run for 1 hour
    python agent.py --run --target 100                       # Stop after 100 functions
    python agent.py --run --target-percent 25.0              # Stop at 25% completion
    
    # Batch processing and intervals
    python agent.py --run --batch 20                         # Process 20 functions at a time
    python agent.py --run --batch 20 --parallel 4            # Decompile 4 functions in parallel
    python agent.py --run --verify-interval 30               # Verify every 30 functions
    python agent.py --run --build-interval 100               # Full build every 100 functions
    
    # Module and size filtering
    python agent.py --run --module battle                    # Only process battle module
    python agent.py --run --max-lines 50                     # Skip functions >50 lines
    
    # LLM-assisted decompilation (requires Ollama)
    python agent.py --run --llm                              # Enable LLM analysis
    python agent.py --run --llm --llm-threshold 30           # Analyze functions ≥30 lines
    python agent.py --run --llm --llm-auto-fix               # Enable automatic LLM fixing
    python agent.py --run --llm --llm-model qwen2.5-coder:7b # Specify LLM model
    
    # Dependency-aware scheduling (enabled by default)
    python agent.py --run --dependency-weight 0.7            # Prioritize resolved dependencies
    python agent.py --run --no-dependency-analysis           # Disable dependency analysis
    
    # Combined examples (recommended)
    python agent.py --run --duration 28800 --batch 20 --parallel 4 --verbose
    python agent.py --run --batch 20 --parallel 4 --retry-blocked --target 500
    python agent.py --run --module world --target 50 --llm --llm-auto-fix
    python agent.py --run --target-percent 30.0 --batch 25 --llm --dependency-weight 0.8
    
    # Review LLM-fixed functions
    python review_llm_fixes.py --list                        # List functions needing review
    python review_llm_fixes.py --promote func_800A1234       # Approve after manual review
"""

import argparse
import sys
import time
import signal
import re
import os
from pathlib import Path
from datetime import datetime, timedelta
from typing import Optional, Dict, List
import json
from multiprocessing import Pool, cpu_count, Manager
from functools import partial

from database import DecompDatabase
from decompile import DecompilationRunner
from build import BuildVerifier
from dependency_analyzer import DependencyAnalyzer
from generate_decls import generate as generate_decls
from discover import populate_rank_scores

try:
    from fix_existing_errors import TypeErrorFixer
    TYPE_FIXER_AVAILABLE = True
except ImportError:
    TYPE_FIXER_AVAILABLE = False
    TypeErrorFixer = None

# Optional LLM support
try:
    from llm_helper import LLMHelper
    LLM_AVAILABLE = True
except ImportError:
    LLM_AVAILABLE = False
    LLMHelper = None


class _TeeOutput:
    """Splits writes to both the original stream and a log file."""

    def __init__(self, stream, filepath: str):
        self._stream = stream
        self._file = open(filepath, 'a', buffering=1)  # line-buffered

    def write(self, data: str) -> int:
        self._stream.write(data)
        self._file.write(data)
        return len(data)

    def flush(self):
        self._stream.flush()
        self._file.flush()

    def fileno(self):
        return self._stream.fileno()

    def isatty(self) -> bool:
        return self._stream.isatty()

    @property
    def encoding(self):
        return self._stream.encoding

    @property
    def errors(self):
        return self._stream.errors


# Module-level lock dictionary for file-level mutual exclusion
_file_locks = None

# Module-level worker function for parallel processing (must be picklable)
def _parallel_decompile_worker(data: Dict) -> Dict:
    """
    Worker function for parallel decompilation.
    Each process gets its own database connection.
    Uses file-level locking to prevent race conditions.
    
    Args:
        data: Dict with keys 'func', 'project_root', 'fix_structs', 'verbose', 'db_path', 'file_lock' (optional)
    
    Returns:
        Dict with 'success' bool and 'error' string (if failed)
    """
    func = data['func']
    func_name = func['name']
    project_root = Path(data['project_root'])
    fix_structs = data['fix_structs']
    strict_validation = data.get('strict_validation', False)
    verbose = data['verbose']
    db_path = data['db_path']
    c_file = func.get('c_file_path', 'unknown')
    
    # Reset signal handlers in worker processes so only the main process
    # handles shutdown signals. Child processes inherit handlers on fork,
    # which caused the worker processes to print the agent's handler and
    # set their own `should_stop` state multiple times.
    try:
        import signal as _signal
        _signal.signal(_signal.SIGINT, _signal.SIG_DFL)
        _signal.signal(_signal.SIGTERM, _signal.SIG_DFL)
        _signal.signal(_signal.SIGHUP, _signal.SIG_DFL)
    except Exception:
        pass
    # Acquire file lock if provided (prevents concurrent modification of same C file)
    file_lock = data.get('file_lock')
    if file_lock:
        if verbose:
            print(f"  [{func_name}] Acquiring lock for {c_file}...")
        file_lock.acquire()
        if verbose:
            print(f"  [{func_name}] Lock acquired for {c_file}")
    
    try:
        # Each worker needs its own database connection (SQLite is not thread-safe)
        db = DecompDatabase(db_path)

        # Each worker creates its own LLMHelper via enable_llm_fixes so the
        # runner-level auto-fix (void*/struct errors etc.) fires per-worker.
        # The main process's pre-analysis LLM pass is separate and runs after.
        enable_llm = data.get('enable_llm_fixes', False)

        # Create a minimal runner for this worker
        runner = DecompilationRunner(
            db,
            verbose=verbose,
            enable_llm_fixes=enable_llm,
            build_verifier=None,
            fix_structs=fix_structs,
            strict_validation=strict_validation,
        )

        success = runner.decompile_function(func_name)

        # Close database connection before releasing lock
        db.conn.close()

        if success:
            return {'success': True}
        else:
            return {'success': False, 'error': 'Decompilation failed'}
    except Exception as e:
        return {'success': False, 'error': str(e)}
    finally:
        # Always release lock
        if file_lock:
            file_lock.release()
            if verbose:
                print(f"  [{func_name}] Lock released for {c_file}")


class AgentConfig:
    """Configuration for the decompilation agent."""
    
    def __init__(self):
        # Batch processing
        self.batch_size = 10
        self.min_batch_size = 1
        self.max_batch_size = 50
        
        # Build verification
        self.verify_interval = 20  # Verify every N functions
        self.full_build_interval = 10  # Full build every N functions (kept low; per-file compile checks handle most errors)
        
        # Retry logic
        self.max_retries = 5
        self.retry_failed_after = 50  # Retry failed after N successful
        
        # Time limits
        self.max_duration = None  # seconds (None = unlimited)
        self.max_idle_time = 600  # Stop if no progress for 5 minutes
        
        # Targets
        self.target_count = None  # Stop after N functions (None = unlimited)
        self.target_progress = None  # Stop at percentage (None = unlimited)
        
        # Filtering
        self.module_filter = None  # Only process specific module
        self.max_function_lines = None  # Skip functions larger than N lines
        
        # LLM assistance
        self.use_llm = False  # Enable LLM-assisted decompilation
        self.llm_analyze_threshold = 20  # Analyze functions >= N lines with LLM
        self.llm_model = "qwen2.5-coder:7b"        
        self.llm_auto_fix = True  # Enable automatic LLM fixing of m2c errors
        self.fix_structs = True  # Pass --fix-structs to mako.sh dec (replaces D_8009XXXX with Savemap fields)
        self.strict_validation = False  # When True use only base soft errors; revert '?' type artifacts

        # Retry behaviour
        self.retry_blocked = True  # Retry blocked functions when queue is empty
        self.force_retry_blocked = False  # Bypass smart filter and retry all blocked (use after adding type defs)
        self.retry_verified = True  # Re-check verified/decompiled that still have INCLUDE_ASM (false positives)

        # Dependency-aware scheduling
        self.use_dependency_analysis = True  # Prioritize functions with resolved dependencies
        self.dependency_weight = 0.8  # How much to weight dependency score vs line count (0-1)
        self.blocker_multiplier = 2.0  # Per-dependent bonus added to agent-level score
        self.strict_dependency_ordering = True  # Only decompile when all in-DB callees are resolved
        self.filter_cross_module_unresolved = False  # Skip functions whose cross-module callees are not yet decompiled

        # Performance
        self.pause_between_batches = 0  # seconds
        self.pause_on_error = 0  # seconds
        self.parallel_workers = 2  # Number of parallel decompilation workers (1 = sequential)
        
        # Logging
        self.verbose = True
        self.log_file = "agent.log"


class DecompilationAgent:
    """Autonomous agent for decompilation workflow."""
    
    def __init__(self, config: AgentConfig):
        self.config = config
        self.db = DecompDatabase()
        self.verifier = BuildVerifier(self.db, verbose=config.verbose)
        self.runner = DecompilationRunner(
            self.db,
            verbose=config.verbose,
            enable_llm_fixes=config.llm_auto_fix,
            build_verifier=self.verifier,
            fix_structs=config.fix_structs,
            strict_validation=config.strict_validation,
        )
        
        # Dependency analysis
        self.dep_analyzer = None
        if config.use_dependency_analysis:
            self.dep_analyzer = DependencyAnalyzer(
                Path.cwd().parent if Path.cwd().name == 'automation' else Path.cwd(),
                blocker_multiplier=int(config.blocker_multiplier),
            )
            self._log("Dependency analysis enabled - building call graph...")
            
            # Build global call graph (analyze all modules for cross-module dependencies)
            # This takes <1 second for all 3500+ functions
            self.dep_analyzer.build_call_graph(None)  # None = all modules
            stats = self.dep_analyzer.get_statistics()
            self._log(f"  Analyzed {stats['total_functions']} functions across all modules")
            self._log(f"  {stats['leaf_functions']} leaf functions ({stats['leaf_functions']/stats['total_functions']*100:.1f}%)")
            self._log(f"  Most depended on: {stats['most_depended_on']} ({stats['most_depended_count']} dependents)")
            
            if config.module_filter:
                self._log(f"  Will prioritize functions from module '{config.module_filter}' with resolved dependencies")
        
        # LLM support
        self.llm = None
        if config.use_llm and LLM_AVAILABLE:
            self.llm = LLMHelper(model=config.llm_model, verbose=config.verbose)
            if not self.llm.available:
                self._log("LLM requested but not available, continuing without it", "WARN")
                self.llm = None
        # Post-run LLM pass controls
        self.config.llm_post_pass = getattr(self.config, 'llm_post_pass', True)
        self.config.llm_post_pass_limit = getattr(self.config, 'llm_post_pass_limit', 200)
        
        # Statistics
        self.stats = {
            'functions_processed': 0,
            'functions_verified': 0,
            'functions_failed': 0,
            'functions_analyzed_by_llm': 0,
            'batches_completed': 0,
            'builds_run': 0,
            'start_time': None,
            'last_progress_time': None,
            'errors': []
        }
        
        # Control flags
        self.should_stop = False
        self.paused = False
        # Count of shutdown signals received to allow forced exit on repeated SIGINT
        self._shutdown_signal_count = 0

        # Retry-round progress tracking:
        # _retry_round: how many retry cycles have been triggered (0 = none yet)
        # _retry_progress_since_last_retry: successes+needs_refine since last retry
        # A retry cycle is only attempted if the previous one produced progress.
        self._retry_round = 0
        self._retry_progress_since_last_retry = 0
        
        # Setup signal handlers
        signal.signal(signal.SIGINT, self._signal_handler)
        signal.signal(signal.SIGTERM, self._signal_handler)
    
    def _signal_handler(self, signum, frame):
        """Handle shutdown signals gracefully."""
        # First signal: request graceful stop. Second signal: force immediate exit.
        self._shutdown_signal_count += 1
        print("\n\n🛑 Received shutdown signal, stopping gracefully...")
        if self._shutdown_signal_count > 1:
            print("🧨 Received multiple shutdown signals — forcing exit now.")
            import os
            os._exit(1)
        self.should_stop = True
    
    def _log(self, message: str, level: str = "INFO"):
        """Log message to console and file."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log_line = f"[{timestamp}] [{level}] {message}"
        print(log_line)
    
    def _check_stop_conditions(self) -> bool:
        """Check if any stop conditions are met."""
        # Manual stop
        if self.should_stop:
            self._log("Manual stop requested", "INFO")
            return True
        
        # Duration limit
        if self.config.max_duration:
            elapsed = time.time() - self.stats['start_time']
            if elapsed >= self.config.max_duration:
                self._log(f"Duration limit reached ({self.config.max_duration}s)", "INFO")
                return True
        
        # Target count
        if self.config.target_count:
            if self.stats['functions_processed'] >= self.config.target_count:
                self._log(f"Target count reached ({self.config.target_count})", "INFO")
                return True
        
        # Target progress
        if self.config.target_progress:
            stats = self.db.get_statistics()
            total = stats['total']
            verified = stats['by_status'].get('verified', 0)
            progress = (verified / total * 100) if total > 0 else 0
            if progress >= self.config.target_progress:
                self._log(f"Target progress reached ({self.config.target_progress}%)", "INFO")
                return True
        
        # Idle timeout
        if self.config.max_idle_time and self.stats['last_progress_time']:
            idle_time = time.time() - self.stats['last_progress_time']
            if idle_time >= self.config.max_idle_time:
                self._log(f"No progress for {self.config.max_idle_time}s, stopping", "WARN")
                return True
        
        return False
    
    def _get_available_asm_modules(self) -> set:
        """Return the set of module names that have ASM files extracted on disk."""
        project_root = Path(__file__).parent.parent
        asm_root = project_root / 'asm' / 'us'
        if not asm_root.is_dir():
            return set()
        return {child.name for child in asm_root.iterdir() if child.is_dir()}

    def _get_next_batch(self) -> List[Dict]:
        """Get next batch of functions to process, prioritizing based on dependencies."""
        available_modules = self._get_available_asm_modules()

        # In strict ordering mode we need all TODO functions so we can find every
        # "ready" one, regardless of line count.  Otherwise the small fetch window
        # could miss ready large functions and keep picking not-ready small ones.
        # When dep_analyzer is active (even in soft-scoring mode) we also fetch all
        # candidates so that high-blocker large functions are never excluded from
        # the scoring window by the line_count ASC pre-sort.
        strict = self.dep_analyzer and self.config.strict_dependency_ordering
        candidate_limit = None if (strict or self.dep_analyzer) else self.config.batch_size

        if self.config.module_filter:
            project_root = Path(__file__).parent.parent
            functions = self.db.get_functions_by_status(
                'todo',
                limit=candidate_limit,
                module=self.config.module_filter,
                order_by='difficulty_score'
            )
            functions = [
                f for f in functions
                if not f.get('asm_file_path') or (project_root / f['asm_file_path']).exists()
            ]
        elif available_modules:
            cursor = self.db.conn.cursor()
            placeholders = ','.join('?' * len(available_modules))
            limit_clause = f"LIMIT {candidate_limit}" if candidate_limit else ""
            cursor.execute(
                f"SELECT id, name, c_file_path, asm_file_path, module, status, "
                f"difficulty_score, line_count, attempt_count, notes "
                f"FROM functions WHERE status='todo' AND module IN ({placeholders}) "
                f"ORDER BY CASE WHEN difficulty_score IS NULL THEN 1 ELSE 0 END ASC, "
                f"difficulty_score ASC, line_count ASC {limit_clause}",
                tuple(sorted(available_modules))
            )
            functions = [dict(row) for row in cursor.fetchall()]
        else:
            functions = self.db.get_functions_by_status(
                'todo', limit=candidate_limit, order_by='difficulty_score'
            )

        if self.config.max_function_lines:
            functions = [f for f in functions
                        if f['line_count'] <= self.config.max_function_lines]

        if not (self.dep_analyzer and functions):
            return functions[:self.config.batch_size]

        # ----------------------------------------------------------------
        # Build resolved / db_funcs sets used by both code paths below
        # ----------------------------------------------------------------
        cursor = self.db.conn.cursor()

        # "resolved" = functions whose C body is already concrete in the source file,
        # so m2c can read their signatures from the preprocessed context.
        cursor.execute("""
            SELECT name FROM functions
            WHERE status IN ('decompiled', 'verified', 'decompiled_needs_refine', 'sdk_excluded')
        """)
        resolved_funcs = {row['name'] for row in cursor}

        # All DB-tracked names (used to distinguish in-scope vs SDK/extern callees)
        cursor.execute("SELECT name, c_file_path FROM functions")
        func_to_c_file: Dict[str, str] = {}
        db_funcs: set = set()
        for row in cursor:
            db_funcs.add(row['name'])
            if row['c_file_path']:
                func_to_c_file[row['name']] = row['c_file_path']

        # For scoring, use the narrower decompiled+verified set
        decompiled_funcs = resolved_funcs - self._get_sdk_excluded_cache()

        known_types = self.db.get_all_known_types(min_confidence=0.3)
        if self.config.verbose and known_types:
            self._log(f"\n📚 Struct learning database: {len(known_types)} known types")

        def _score(func):
            dep_score = self.dep_analyzer.compute_type_aware_score(
                func['name'], decompiled_funcs, known_types,
                c_file_path=func.get('c_file_path')
            )
            simplicity = 100.0 * (1.0 - min(func['line_count'], 200) / 200.0)
            w = self.config.dependency_weight
            # Extra blocker bonus: reward functions that directly unblock many
            # callers, visible at the agent level independently of dep_score scaling.
            blocker_count = len(self.dep_analyzer.called_by.get(func['name'], set()))
            blocker_bonus = blocker_count * self.config.blocker_multiplier
            return (w * dep_score) + ((1 - w) * simplicity) + blocker_bonus

        if strict:
            # Partition: ready (all same-file callees resolved) vs waiting.
            # When --filter-cross-module is set, cross-file callees are also
            # checked, ensuring m2c never sees an unresolved cross-module callee.
            check_cross = self.config.filter_cross_module_unresolved
            ready_names = set(self.dep_analyzer.get_ready_functions(
                [f['name'] for f in functions], resolved_funcs, db_funcs,
                func_to_c_file=func_to_c_file,
                check_cross_file=check_cross,
            ))
            ready = [f for f in functions if f['name'] in ready_names]
            waiting = [f for f in functions if f['name'] not in ready_names]

            if self.config.verbose:
                label = "cross-module-filtered" if check_cross else "strict"
                self._log(
                    f"  {label.capitalize()} ordering: {len(ready)} ready, "
                    f"{len(waiting)} waiting on unresolved callees"
                )

            ready.sort(key=lambda f: -_score(f))

            if len(ready) >= self.config.batch_size:
                functions = ready[:self.config.batch_size]
            elif ready:
                if check_cross:
                    # Never mix in unresolved-cross-module functions
                    functions = ready
                    if self.config.verbose:
                        self._log(
                            f"  Cross-module filter: {len(ready)} fully-ready "
                            f"(not padding with unresolved)"
                        )
                else:
                    # Fill remainder with fewest-unresolved waiting functions
                    waiting.sort(key=lambda f: len(
                        self.dep_analyzer.get_unresolved_dependencies(f['name'], resolved_funcs)
                    ))
                    needed = self.config.batch_size - len(ready)
                    functions = ready + waiting[:needed]
                    if self.config.verbose:
                        self._log(
                            f"  Strict ordering: only {len(ready)} fully-ready; "
                            f"adding {needed} lowest-unresolved waiting functions"
                        )
            else:
                if check_cross:
                    # No cross-module-clean functions available — return empty so
                    # the main loop can try blocked/needs-refine queues instead.
                    if self.config.verbose:
                        self._log(
                            "  Cross-module filter: no fully-ready functions; "
                            "returning empty batch"
                        )
                    return []
                else:
                    # No ready functions — process fewest-unresolved first
                    waiting.sort(key=lambda f: len(
                        self.dep_analyzer.get_unresolved_dependencies(f['name'], resolved_funcs)
                    ))
                    functions = waiting[:self.config.batch_size]
                    if self.config.verbose:
                        self._log(
                            f"  Strict ordering: no fully-ready functions; "
                            f"using fewest-unresolved fallback"
                        )
        else:
            # Original soft-scoring path
            scored = [(f, _score(f)) for f in functions]
            scored.sort(key=lambda x: -x[1])

            if self.config.verbose and scored:
                self._log(f"\nTop candidates (dep_weight={self.config.dependency_weight:.1f}):")
                for func, combined in scored[:5]:
                    unresolved = len(self.dep_analyzer.get_unresolved_dependencies(
                        func['name'], decompiled_funcs))
                    self._log(
                        f"  {func['name']}: score={combined:.1f}, "
                        f"lines={func['line_count']}, unresolved={unresolved}"
                    )

            functions = [f for f, _ in scored[:self.config.batch_size]]

        return functions

    def _get_sdk_excluded_cache(self) -> set:
        """Return the current set of sdk_excluded function names (fresh per call)."""
        cursor = self.db.conn.cursor()
        cursor.execute("SELECT name FROM functions WHERE status='sdk_excluded'")
        return {row['name'] for row in cursor}

    def _llm_preanalyze(self, func: Dict) -> Optional[Dict]:
        """Use LLM to analyze function before decompilation."""
        if not self.llm:
            return None
        
        # Only analyze if function meets threshold
        if func['line_count'] < self.config.llm_analyze_threshold:
            return None
        
        self._log(f"  ⚙️  Analyzing with LLM ({func['line_count']} lines)...")
        
        try:
            analysis = self.llm.analyze_assembly(func['name'])
            if analysis:
                self.stats['functions_analyzed_by_llm'] += 1
                self._log(f"  📊 Complexity: {analysis['complexity']}, Summary: {analysis['summary'][:80]}...")
                
                # Store full analysis in database notes as JSON for later retrieval
                import json
                notes = json.dumps({
                    'llm_analysis': {
                        'complexity': analysis['complexity'],
                        'summary': analysis['summary'][:200],
                        'c_suggestion': analysis.get('c_suggestion', '')[:500] if analysis.get('c_suggestion') else None,
                        'patterns': analysis.get('patterns', [])
                    }
                })
                self.db.update_function_status(func['name'], func['status'], notes=notes)
                
                return analysis
        except Exception as e:
            self._log(f"  ⚠️  LLM analysis failed: {e}", "WARN")
        
        return None
    
    def _process_batch(self, functions: List[Dict]) -> Dict:
        """Process a batch of functions."""
        if not functions:
            return {'success': 0, 'failed': 0}
        
        # Use parallel processing if enabled.
        # LLM pre-analysis (--llm flag) cannot run inside parallel worker processes
        # (spawned without LLM runner to avoid concurrent Ollama calls and pickle
        # limitations). Force sequential execution when --llm is on so every function
        # gets the full pre-analysis + auto-fix pipeline.
        # Note: runner-level auto-fix (llm_auto_fix) already works sequentially inside
        # each worker via decompile.py, so no sequential forcing is needed for that.
        llm_preanalysis_active = self.config.use_llm and self.llm is not None
        if self.config.parallel_workers > 1 and not llm_preanalysis_active:
            return self._process_batch_parallel(functions)
        elif self.config.parallel_workers > 1 and llm_preanalysis_active:
            self._log("LLM pre-analysis enabled (--llm) — running sequentially to allow per-function LLM patching", "INFO")
        
        self._log(f"Processing batch of {len(functions)} functions", "INFO")
        
        success_count = 0
        failed_count = 0
        
        # Create one TypeErrorFixer for the whole batch if LLM auto-fix is on
        post_fixer = None
        if self.llm and self.config.llm_auto_fix and TYPE_FIXER_AVAILABLE:
            try:
                post_fixer = TypeErrorFixer(model=self.config.llm_model, verbose=self.config.verbose)
            except SystemExit:
                self._log("TypeErrorFixer: LLM not available, post-decompile error fixing disabled", "WARN")
        
        project_root = Path(__file__).parent.parent
        
        for i, func in enumerate(functions, 1):
            if self._check_stop_conditions():
                break
            
            func_name = func['name']
            self._log(f"[{i}/{len(functions)}] Decompiling {func_name} ({func['line_count']} lines)")
            
            try:
                # Run optional LLM pre-analysis (may store notes + increment counter)
                if self.llm:
                    try:
                        self._llm_preanalyze(func)
                    except Exception as e:
                        self._log(f"  ⚠️ LLM pre-analysis failed: {e}", "WARN")

                success = self.runner.decompile_function(func_name)
                
                if success:
                    success_count += 1
                    self.stats['functions_processed'] += 1
                    self.stats['last_progress_time'] = time.time()
                    
                    # Immediately check and fix compilation errors in this file,
                    # but ONLY if errors fall within the newly-decompiled function.
                    # Pre-existing errors in other functions in the same file are
                    # intentionally ignored here to avoid wasted LLM calls.
                    if post_fixer is not None:
                        c_file = func.get('c_file_path')
                        if c_file:
                            full_path = project_root / c_file
                            if full_path.exists():
                                try:
                                    errors = post_fixer.get_compilation_errors(str(full_path))
                                    if errors:
                                        # Find the line range of the newly-decompiled function
                                        all_funcs = post_fixer.discover_functions_in_file(str(full_path))
                                        func_range = next(
                                            ((s, e) for n, s, e in all_funcs if n == func_name), None)
                                        if func_range is not None:
                                            func_errors = [err for err in errors
                                                           if func_range[0] <= err['line'] <= func_range[1]]
                                        else:
                                            func_errors = []
                                        if func_errors:
                                            self._log(
                                                f"  {func_name}: {len(func_errors)} compilation error(s) after decompile, fixing...",
                                                "INFO"
                                            )
                                            n = post_fixer.fix_file(str(full_path), max_functions=1,
                                                                    max_passes=2, target_function=func_name)
                                            if n > 0:
                                                self._log(f"  {func_name}: fixed {n} function(s)", "INFO")
                                        else:
                                            self._log(
                                                f"  {func_name}: {len(errors)} pre-existing error(s) elsewhere in file, skipping LLM fix",
                                                "INFO"
                                            )
                                except Exception as e:
                                    self._log(f"  Post-decompile error fix failed for {func_name}: {e}", "WARN")
                else:
                    failed_count += 1
                    self.stats['functions_failed'] += 1
                    
            except Exception as e:
                self._log(f"Error processing {func_name}: {e}", "ERROR")
                self.stats['errors'].append({
                    'function': func_name,
                    'error': str(e),
                    'time': datetime.now().isoformat()
                })
                failed_count += 1
        
        self.stats['batches_completed'] += 1
        
        return {'success': success_count, 'failed': failed_count}
    
    def _process_batch_parallel(self, functions: List[Dict]) -> Dict:
        """Process a batch of functions in parallel, with per-file locking to prevent race conditions."""
        if not functions:
            return {'success': 0, 'failed': 0}
        
        # Group functions by C file
        functions_by_file = {}
        for func in functions:
            c_file = func.get('c_file_path', 'unknown')
            if c_file not in functions_by_file:
                functions_by_file[c_file] = []
            functions_by_file[c_file].append(func)
        
        num_workers = min(self.config.parallel_workers, len(functions), cpu_count())
        self._log(f"Processing {len(functions)} functions from {len(functions_by_file)} files with {num_workers} parallel workers (file-level locking enabled)", "INFO")
        
        # Create locks for each file using Manager (shared across processes)
        with Manager() as manager:
            file_locks = {c_file: manager.Lock() for c_file in functions_by_file.keys()}
            
            # Prepare data for workers
            project_root = Path(__file__).parent.parent
            worker_data = []
            for func in functions:
                c_file = func.get('c_file_path', 'unknown')
                worker_data.append({
                    'func': func,
                    'project_root': str(project_root),
                    'fix_structs': self.config.fix_structs,
                    'strict_validation': self.config.strict_validation,
                    'verbose': self.config.verbose,
                    'db_path': self.db.db_path,
                    'file_lock': file_locks.get(c_file),  # Pass lock for this file
                    'enable_llm_fixes': self.config.llm_auto_fix,  # Enable runner-level LLM auto-fix
                })
            
            # Run decompilations in parallel with file-level mutual exclusion
            with Pool(processes=num_workers) as pool:
                results = pool.map(_parallel_decompile_worker, worker_data)
        
        # Process results
        success_count = 0
        failed_count = 0
        needs_refine_count = 0
        
        for i, (func, result) in enumerate(zip(functions, results), 1):
            func_name = func['name']
            
            if result['success']:
                success_count += 1
                self.stats['functions_processed'] += 1
                self.stats['last_progress_time'] = time.time()
                self._log(f"[{i}/{len(functions)}] ✅ {func_name} ({func['line_count']} lines)", "INFO")
            else:
                failed_count += 1
                self.stats['functions_failed'] += 1
                error_msg = result.get('error', 'Unknown error')
                self._log(f"[{i}/{len(functions)}] ❌ {func_name}: {error_msg}", "WARN")
                self.stats['errors'].append({
                    'function': func_name,
                    'error': error_msg,
                    'time': datetime.now().isoformat()
                })
        
        # Post-parallel PSY-Q compile gate.
        # Workers run without build_verifier (the subprocess-based checker cannot be
        # pickled for inter-process use), so text-validated functions that contain
        # m2c artifacts that the text checks miss (e.g. `.unk-N`, `(?** )` casts,
        # `*(void*)lval =` writes) are caught here instead.
        # For each C file that had at least one success, compile the whole file once.
        # If it fails, downgrade every newly-decompiled function in that file to
        # 'decompiled_needs_refine' so the LLM fixer or manual review can handle them.
        files_with_successes: Dict[str, list] = {}
        for func, result in zip(functions, results):
            if result['success']:
                c_file = func.get('c_file_path')
                if c_file:
                    files_with_successes.setdefault(c_file, []).append(func['name'])

        if files_with_successes:
            self._log("Post-parallel PSY-Q compile check...", "INFO")
            for rel_path, func_names in files_with_successes.items():
                full_path = project_root / rel_path
                if not full_path.exists():
                    continue
                try:
                    compile_ok, compile_errors = self.verifier.check_file_compiles(full_path)
                except Exception as e:
                    self._log(f"  Compile check failed for {rel_path}: {e}", "WARN")
                    continue
                if not compile_ok:
                    errors_summary = "; ".join(
                        e['message'] for e in compile_errors[:2]
                    )
                    self._log(
                        f"  ⚠️  {rel_path}: PSY-Q errors after batch — {errors_summary}",
                        "WARN",
                    )
                    for fname in func_names:
                        entry = self.db.get_function(fname)
                        if entry and entry.get('status') == 'decompiled':
                            # Revert C body to INCLUDE_ASM so SHA1 stays passing
                            reverted = self._revert_function_to_include_asm(full_path, fname)
                            self.db.update_function_status(
                                fname,
                                'decompiled_needs_refine',
                                notes=f"Post-parallel compile check failed: {errors_summary}",
                            )
                            if reverted:
                                self._log(
                                    f"    {fname} → decompiled_needs_refine ↩️ reverted (compile errors)",
                                    "WARN",
                                )
                            else:
                                self._log(
                                    f"    {fname} → decompiled_needs_refine (compile errors; revert failed)",
                                    "WARN",
                                )
                            success_count -= 1
                            needs_refine_count += 1

        # LLM post-processing (kept sequential as requested)
        if self.llm and self.config.llm_auto_fix and TYPE_FIXER_AVAILABLE:
            self._log("Running LLM post-processing on decompiled functions (sequential)...", "INFO")
            try:
                post_fixer = TypeErrorFixer(model=self.config.llm_model, verbose=self.config.verbose)
                
                for func, result in zip(functions, results):
                    if not result['success']:
                        continue
                    
                    func_name = func['name']
                    c_file = func.get('c_file_path')
                    if not c_file:
                        continue
                    
                    full_path = project_root / c_file
                    if not full_path.exists():
                        continue
                    
                    try:
                        errors = post_fixer.get_compilation_errors(str(full_path))
                        if errors:
                            all_funcs = post_fixer.discover_functions_in_file(str(full_path))
                            func_range = next(
                                ((s, e) for n, s, e in all_funcs if n == func_name), None)
                            if func_range is not None:
                                func_errors = [err for err in errors
                                               if func_range[0] <= err['line'] <= func_range[1]]
                            else:
                                func_errors = []
                            
                            if func_errors:
                                self._log(
                                    f"  {func_name}: {len(func_errors)} compilation error(s), fixing...",
                                    "INFO"
                                )
                                n = post_fixer.fix_file(str(full_path), max_functions=1,
                                                        max_passes=2, target_function=func_name)
                                if n > 0:
                                    self._log(f"  {func_name}: fixed {n} function(s)", "INFO")
                    except Exception as e:
                        self._log(f"  Post-decompile fix failed for {func_name}: {e}", "WARN")
                        
            except SystemExit:
                self._log("TypeErrorFixer: LLM not available, post-decompile error fixing disabled", "WARN")
        
        self.stats['batches_completed'] += 1
        return {'success': success_count, 'failed': failed_count, 'needs_refine': needs_refine_count}
    
    def _refresh_decls(self):
        """Regenerate include/decomp_decls.h from current C source files.

        Called at startup and after each verification pass so that newly
        verified functions immediately become available as m2c context for
        all subsequent decompilations in the same session.
        """
        project_root = Path(__file__).parent.parent
        try:
            count = generate_decls(project_root=project_root, verbose=False)
            self._log(f"Refreshed decomp_decls.h + per-module decls: {count} declarations", "INFO")
        except Exception as e:
            self._log(f"generate_decls failed (non-fatal): {e}", "WARN")

    def _verify_batch(self):
        """Run verification on decompiled functions."""
        self._log("Running verification...", "INFO")
        
        try:
            results = self.verifier.verify_decompiled_functions()
            self.stats['functions_verified'] += results['verified']
            self._log(f"Verified {results['verified']} functions", "INFO")
        except Exception as e:
            self._log(f"Verification error: {e}", "ERROR")
            self.stats['errors'].append({
                'type': 'verification',
                'error': str(e),
                'time': datetime.now().isoformat()
            })

        # Refresh declarations so newly-verified functions are available as context
        self._refresh_decls()
    
    def _run_full_build(self) -> bool:
        """Run a full build to check for errors."""
        self._log("Running full build check...", "INFO")
        self.stats['builds_run'] += 1
        
        try:
            success = self.verifier.full_build_and_verify()
            if success:
                self._log("Build successful!", "INFO")
            else:
                self._log("Build failed, check output for errors", "WARN")
            # Refresh rank scores so the queue stays sorted easiest→hardest
            # as functions are removed from the asm/ directories.
            try:
                updated = populate_rank_scores(self.db)
                self._log(f"Rank scores refreshed ({updated} functions updated)", "INFO")
            except Exception as rank_err:
                self._log(f"Rank refresh failed (non-fatal): {rank_err}", "WARN")
            return success
        except Exception as e:
            self._log(f"Build error: {e}", "ERROR")
            self.stats['errors'].append({
                'type': 'build',
                'error': str(e),
                'time': datetime.now().isoformat()
            })
            return False
    
    def _fix_type_errors_with_llm(self) -> int:
        """
        Scan C files with decompiled functions for actual PSY-Q compilation errors
        and fix them with LLM via TypeErrorFixer.

        Returns:
            Number of functions fixed
        """
        if not self.llm or not TYPE_FIXER_AVAILABLE:
            return 0

        # Collect unique C files that have decompiled functions
        decompiled = self.db.get_functions_by_status('decompiled', limit=50)
        if not decompiled:
            return 0

        seen_files = set()
        c_files_to_check = []
        for func in decompiled:
            c_file = func.get('c_file_path')
            if c_file and c_file not in seen_files:
                seen_files.add(c_file)
                c_files_to_check.append(c_file)

        if not c_files_to_check:
            return 0

        self._log(f"Checking {len(c_files_to_check)} file(s) for PSY-Q compilation errors...", "INFO")

        fixed_count = 0
        try:
            fixer = TypeErrorFixer(model=self.config.llm_model, verbose=self.config.verbose)
        except SystemExit:
            self._log("TypeErrorFixer: LLM not available, skipping type error fixing", "WARN")
            return 0

        project_root = Path(__file__).parent.parent
        for c_file in c_files_to_check:
            if self._check_stop_conditions():
                break
            full_path = project_root / c_file
            if not full_path.exists():
                continue
            try:
                # Compile the file directly via PSY-Q to get real errors
                errors = fixer.get_compilation_errors(str(full_path))
                if not errors:
                    continue
                self._log(f"  {c_file}: {len(errors)} PSY-Q error(s) — attempting LLM fix", "INFO")
                n = fixer.fix_file(str(full_path), max_functions=5)
                fixed_count += n
            except Exception as e:
                self._log(f"  Error fixing {c_file}: {e}", "ERROR")

        if fixed_count > 0:
            self._log(f"🔧 Fixed type errors in {fixed_count} function(s) with LLM", "INFO")

        return fixed_count
    
    def _reset_stale_in_progress(self):
        """Reset functions stuck in 'in_progress' (from crashed runs) back to 'failed'.

        These occur when the agent is killed mid-decompilation.  Without this
        cleanup they never re-enter the retry queue.
        """
        cursor = self.db.conn.cursor()
        cursor.execute(
            "UPDATE functions SET status='failed', attempt_count=1, "
            "notes='Reset from stale in_progress (agent crash)' "
            "WHERE status='in_progress'"
        )
        count = cursor.rowcount
        self.db.conn.commit()
        if count:
            self._log(f"Reset {count} stale in_progress function(s) to failed", "INFO")

    def _reset_failed_attempt_counts(self):
        """Reset attempt_count=0 for all failed/blocked functions on agent startup.

        This gives every function a fresh chance on each new agent run, preventing
        permanent dead-ends when the decompilation system has been improved.
        The max_retries limit then controls per-session attempts only.
        """
        cursor = self.db.conn.cursor()
        cursor.execute(
            "UPDATE functions SET attempt_count=0 WHERE status IN ('failed', 'blocked')"
        )
        reset_count = cursor.rowcount
        self.db.conn.commit()
        if reset_count:
            self._log(f"Reset retry counters for {reset_count} failed/blocked function(s)", "INFO")

    def _retry_failed_functions(self):
        """Re-queue functions that previously failed (status='failed')."""
        failed = self.db.get_functions_by_status(
            'failed', module=self.config.module_filter)

        if not failed:
            return

        self._log(f"Retrying {len(failed)} failed functions...", "INFO")

        queued = 0
        skipped = 0
        infra_skip = 0
        for func in failed:
            if func['attempt_count'] >= self.config.max_retries:
                skipped += 1
                continue

            # Skip permanent infrastructure failures (missing ASM files, etc.)
            # These cannot succeed until the ASM extraction is done for that module
            notes = func.get('notes') or ''
            if 'Function not found in version' in notes or 'not found in version' in notes.lower():
                infra_skip += 1
                continue

            self.db.update_function_status(func['name'], 'todo',
                                           notes=f"Retry attempt {func['attempt_count'] + 1}")
            queued += 1

        if queued:
            self._log(f"  Queued {queued} function(s) for retry", "INFO")
        if skipped:
            self._log(f"  Skipped {skipped} function(s) (max retries this session)", "INFO")
        if infra_skip:
            self._log(f"  Skipped {infra_skip} function(s) (permanent infra failure - missing ASM)", "INFO")

    @staticmethod
    def _find_func_body_range(text: str, func_name: str):
        """Return (start_line, end_line) of the C body definition of func_name
        in `text` (1-indexed, inclusive), or None if not found.

        Looks for patterns like:
            return_type func_name(...) {
        where the line ends with '{' (body definition, not a prototype ending in ';').
        Walks forward counting braces to find the closing '}'.
        """
        import re as _re
        # Match a definition line: any return type, the function name, params, opening brace.
        # The line must contain '{' (not just a prototype).
        pattern = _re.compile(
            r'^[^\n]*\b' + _re.escape(func_name) + r'\s*\([^)]*\)[^;{]*\{',
            _re.MULTILINE,
        )
        m = pattern.search(text)
        if not m:
            return None

        start_line = text[:m.start()].count('\n') + 1
        # Walk from the '{' to find the matching '}'
        depth = 0
        for i, ch in enumerate(text[m.start():]):
            if ch == '{':
                depth += 1
            elif ch == '}':
                depth -= 1
                if depth == 0:
                    end_line = text[:m.start() + i].count('\n') + 1
                    return (start_line, end_line)
        return None

    def _retry_needs_refine(self):
        """Re-queue or promote decompiled_needs_refine functions whose code is clean.

        When a file has compile errors, we now check each function individually:
        - If no error line falls within the function's body range → the function's
          own code is fine; promote it directly to 'decompiled' so it can be verified.
        - If an error line overlaps the function's body → leave as
          decompiled_needs_refine for manual/LLM fixing.

        This handles the common case where a file has pre-existing errors (unrelated
        to the newly-decompiled functions) that would otherwise permanently block all
        decompiled_needs_refine functions in that file.
        """
        needs_refine = self.db.get_functions_by_status(
            'decompiled_needs_refine', module=self.config.module_filter)

        if not needs_refine:
            return

        self._log(f"Re-queuing {len(needs_refine)} decompiled_needs_refine functions for retry...", "INFO")

        project_root = Path(__file__).parent.parent

        # Group by C file so we only compile-check each file once
        file_to_funcs: dict = {}
        funcs_without_file = []
        for func in needs_refine:
            c_file = func.get('c_file_path')
            if c_file:
                file_to_funcs.setdefault(c_file, []).append(func)
            else:
                funcs_without_file.append(func)

        queued = 0
        promoted = 0
        left_pending = 0

        for rel_c_path, funcs_in_file in file_to_funcs.items():
            full_path = project_root / rel_c_path
            compile_ok = True
            error_lines = set()
            if full_path.exists():
                try:
                    compile_ok, compile_errors = self.verifier.check_file_compiles(full_path)
                    if not compile_ok:
                        error_lines = {e['line'] for e in compile_errors if e.get('line')}
                except Exception:
                    compile_ok = True  # Can't tell — give it a chance

            if compile_ok:
                for func in funcs_in_file:
                    self.db.update_function_status(func['name'], 'todo',
                                                   notes="Re-queued after fix (file now compiles)")
                    self._log(f"  Queued {func['name']} for retry", "INFO")
                    queued += 1
            else:
                # File still has errors — but check each function individually.
                try:
                    file_text = full_path.read_text(errors='replace')
                except OSError:
                    file_text = None

                for func in funcs_in_file:
                    fname = func['name']

                    # DB out of sync: function still has INCLUDE_ASM stub — reset to todo
                    if file_text and f', {fname})' in file_text and 'INCLUDE_ASM(' in file_text:
                        body_range_check = self._find_func_body_range(file_text, fname)
                        if body_range_check is None:
                            self.db.update_function_status(fname, 'todo',
                                                           notes="Reset from needs_refine: still has INCLUDE_ASM stub")
                            self._log(f"  Reset {fname} → todo (INCLUDE_ASM stub present)", "INFO")
                            queued += 1
                            continue

                    body_range = None
                    if file_text:
                        body_range = self._find_func_body_range(file_text, fname)

                    if body_range is not None:
                        start, end = body_range
                        overlapping = {ln for ln in error_lines if start <= ln <= end}
                        if not overlapping:
                            # No errors inside this function's body — promote to decompiled
                            self.db.update_function_status(
                                fname, 'decompiled',
                                notes="Promoted from needs_refine: no errors in function body (pre-existing errors elsewhere in file)"
                            )
                            self._log(f"  Promoted {fname} → decompiled (errors are outside its body)", "INFO")
                            promoted += 1
                            continue

                    left_pending += 1
                    if self.config.verbose:
                        self._log(f"  Skipped {fname} — file still has compile errors in/near its body", "INFO")

        # Functions with no recorded C file path: re-queue unconditionally
        for func in funcs_without_file:
            self.db.update_function_status(func['name'], 'todo',
                                           notes="Re-queued after manual fix")
            self._log(f"  Queued {func['name']} for retry (no C file path recorded)", "INFO")
            queued += 1

        if queued:
            self._log(f"  Queued {queued} function(s) for retry", "INFO")
        if promoted:
            self._log(f"  Promoted {promoted} function(s) → decompiled (errors outside their bodies)", "INFO")
        if left_pending:
            self._log(f"  Left {left_pending} function(s) as decompiled_needs_refine (file still has compile errors)", "INFO")

    def verify_needs_refine(self, module: str = None, verbose: bool = True):
        """Attempt to promote all decompiled_needs_refine functions to verified.

        For each function in the decompiled_needs_refine state:
          1. Re-run PSY-Q compile check on its source file.
          2. If the file now compiles cleanly, run check_function_match() and
             promote the function to 'verified' (matches) or 'decompiled'
             (compile ok but still needs objdiff confirmation).
          3. If the file still has errors but the error lines don't overlap the
             function body, promote to 'decompiled' so the normal verify path
             can handle it.
          4. Leave remaining functions as decompiled_needs_refine and report them.

        This is intended to be called after manually fixing compile errors outside
        the automation workflow (e.g. ``python agent.py --verify-needs-refine``).
        """
        project_root = Path(__file__).parent.parent
        needs_refine = self.db.get_functions_by_status('decompiled_needs_refine', module=module)

        if not needs_refine:
            print("No decompiled_needs_refine functions found.")
            return

        print(f"\n{'='*60}")
        print(f"VERIFY NEEDS-REFINE  ({len(needs_refine)} functions)")
        print(f"{'='*60}\n")

        # Group by C file so we only compile-check each file once
        file_to_funcs: dict = {}
        funcs_without_file = []
        for func in needs_refine:
            c_file = func.get('c_file_path')
            if c_file:
                file_to_funcs.setdefault(c_file, []).append(func)
            else:
                funcs_without_file.append(func)

        verified_count = 0
        promoted_count = 0
        still_broken_count = 0

        for rel_c_path, funcs_in_file in file_to_funcs.items():
            full_path = project_root / rel_c_path
            compile_ok = True
            error_lines: set = set()
            errors: list = []

            if full_path.exists():
                try:
                    compile_ok, errors = self.verifier.check_file_compiles(full_path)
                    if not compile_ok:
                        error_lines = {e['line'] for e in errors if e.get('line')}
                except Exception:
                    compile_ok = True  # Can't determine — give it a chance

            file_text = None
            if full_path.exists():
                try:
                    file_text = full_path.read_text(errors='replace')
                except OSError:
                    pass

            for func in funcs_in_file:
                fname = func['name']

                # If the file still has an INCLUDE_ASM stub for this function the
                # C body was never written (or was reverted) — the DB is out of sync.
                # Reset to todo so the function can be decompiled fresh.
                if file_text and f', {fname})' in file_text and 'INCLUDE_ASM(' in file_text:
                    body_range = self._find_func_body_range(file_text, fname)
                    if body_range is None:
                        self.db.update_function_status(fname, 'todo',
                                                       notes="Reset from needs_refine: still has INCLUDE_ASM stub (DB was out of sync)")
                        print(f"  🔄 {fname} → todo (INCLUDE_ASM stub present, DB was stale)")
                        promoted_count += 1
                        continue

                if compile_ok:
                    # File compiles — run match check then promote
                    matches, err_msg = self.verifier.check_function_match(fname)
                    if matches:
                        self.db.update_function_status(fname, 'verified',
                                                       notes="Promoted from needs_refine after compile errors resolved")
                        print(f"  ✅ {fname} → verified")
                        verified_count += 1
                    else:
                        # Compiled but objdiff says no match yet — push to decompiled
                        self.db.update_function_status(fname, 'decompiled',
                                                       notes=f"Promoted from needs_refine; needs objdiff: {err_msg}")
                        print(f"  ➡️  {fname} → decompiled (compile ok, pending objdiff)")
                        promoted_count += 1
                else:
                    # File still has errors — check whether they overlap this function
                    overlap_found = True  # default: assume overlap if we can't tell
                    if file_text:
                        body_range = self._find_func_body_range(file_text, fname)
                        if body_range is not None:
                            start, end = body_range
                            overlap_found = bool({ln for ln in error_lines if start <= ln <= end})

                    if not overlap_found:
                        # Errors are elsewhere in the file — safe to promote
                        self.db.update_function_status(fname, 'decompiled',
                                                       notes="Promoted from needs_refine: compile errors are outside function body")
                        print(f"  ➡️  {fname} → decompiled (errors outside its body)")
                        promoted_count += 1
                    else:
                        if verbose:
                            first_err = errors[0].get('message', '?') if errors else '?'
                            print(f"  ❌ {fname} still broken  — {first_err}")
                        still_broken_count += 1

        # Functions with no recorded C file — re-queue so the agent can retry
        for func in funcs_without_file:
            self.db.update_function_status(func['name'], 'todo',
                                           notes="Re-queued from needs_refine (no C file recorded)")
            print(f"  🔄 {func['name']} → todo (no C file path)")
            promoted_count += 1

        print(f"\n{'='*60}")
        print(f"Verified:          {verified_count}")
        print(f"Promoted→decompiled: {promoted_count}")
        print(f"Still needs fixing:  {still_broken_count}")
        print(f"{'='*60}\n")

    def _reset_false_verified(self):
        """Find verified/decompiled functions whose source file still contains
        INCLUDE_ASM (i.e. mako.sh exited 0 but never actually wrote C code) and
        reset them back to todo so they can be retried properly.
        """
        from pathlib import Path

        statuses = ('verified', 'decompiled')
        false_names = []

        for status in statuses:
            funcs = self.db.get_functions_by_status(status, module=self.config.module_filter)
            for func in funcs:
                c_file = func.get('c_file_path')
                if not c_file:
                    continue
                try:
                    content = Path(c_file).read_text()
                    if 'INCLUDE_ASM(' in content and f", {func['name']})".encode().decode() in content:
                        false_names.append(func['name'])
                except OSError:
                    pass

        if not false_names:
            self._log("No false-verified functions found", "INFO")
            return

        self._log(f"Found {len(false_names)} falsely-verified functions (INCLUDE_ASM still present) — resetting to todo", "WARN")
        for name in false_names:
            self.db.update_function_status(name, 'todo', notes="Reset: INCLUDE_ASM still present despite verified status")
            self.db.conn.execute("UPDATE functions SET attempt_count=0 WHERE name=?", (name,))
        self.db.conn.commit()
        self._log(f"  Reset {len(false_names)} functions to todo", "INFO")

    def _retry_blocked_functions(self):
        """Re-queue blocked functions so they can be re-attempted.

        In smart mode (dep_analyzer available, force_retry_blocked=False):
          Only re-queues blocked functions where a same-file callee has been
          resolved since last_attempt_at. This avoids burning the session
          re-trying functions stuck on structural /*?*/ type gaps.

        In force mode (force_retry_blocked=True or no dep_analyzer):
          Re-queues all eligible blocked functions regardless.
        """
        blocked = self.db.get_functions_by_status(
            'blocked', module=self.config.module_filter)

        eligible = [f for f in blocked if f['attempt_count'] < self.config.max_retries]

        if not eligible:
            self._log("No eligible blocked functions to retry (all at max retries)", "INFO")
            return

        # Smart filtering: only re-queue if a same-file callee was resolved since last attempt
        if self.dep_analyzer and not self.config.force_retry_blocked:
            cursor = self.db.conn.cursor()
            func_to_c_file = {
                row[0]: row[1] for row in cursor.execute(
                    "SELECT name, c_file_path FROM functions WHERE c_file_path IS NOT NULL"
                )
            }
            resolved_statuses = "('verified', 'decompiled', 'decompiled_needs_refine', 'sdk_excluded')"

            filtered = []
            for func in eligible:
                fname = func['name']
                last_attempt = func.get('last_attempt_at')
                c_file = func_to_c_file.get(fname)

                if not last_attempt or not c_file:
                    continue  # Skip if never attempted or c_file unknown

                # Only consider same-file callees (cross-file ordering has no benefit)
                callees = list(self.dep_analyzer.calls.get(fname, set()))
                same_file_callees = [c for c in callees if func_to_c_file.get(c) == c_file]

                if not same_file_callees:
                    continue  # Blocked on structural type issue — retry won't help

                # Re-queue only if a same-file callee was resolved after last attempt
                placeholders = ','.join('?' * len(same_file_callees))
                row = cursor.execute(
                    f"SELECT MAX(updated_at) FROM functions "
                    f"WHERE name IN ({placeholders}) AND status IN {resolved_statuses}",
                    same_file_callees
                ).fetchone()
                last_resolved = row[0] if row else None

                if last_resolved and last_resolved > last_attempt:
                    filtered.append(func)

            original_count = len(eligible)
            eligible = filtered

            if not eligible:
                self._log(
                    f"No blocked functions have newly-resolved same-file callees "
                    f"({original_count} checked); skipping retry", "INFO"
                )
                self._log("  Use --force-retry-blocked to override and retry all", "INFO")
                return

            self._log(
                f"Smart retry: {len(eligible)}/{original_count} blocked functions "
                f"have newly-resolved same-file callees", "INFO"
            )
        else:
            self._log(f"Re-queuing {len(eligible)}/{len(blocked)} blocked functions for retry...", "INFO")

        for func in eligible:
            self.db.update_function_status(func['name'], 'todo',
                                           notes=f"Retry after block (attempt {func['attempt_count'] + 1})")

        self._log(f"  Done — {len(eligible)} function(s) re-queued", "INFO")
    
    def _show_progress(self):
        """Display current progress."""
        stats = self.db.get_statistics()
        
        total = stats['total']
        verified = stats['by_status'].get('verified', 0)
        decompiled = stats['by_status'].get('decompiled', 0)
        needs_refine = stats['by_status'].get('decompiled_needs_refine', 0)
        failed = stats['by_status'].get('failed', 0)
        blocked = stats['by_status'].get('blocked', 0)
        todo = stats['by_status'].get('todo', 0)
        
        completed = verified + decompiled + needs_refine
        progress = (completed / total * 100) if total > 0 else 0
        
        elapsed = time.time() - self.stats['start_time']
        rate = self.stats['functions_processed'] / elapsed if elapsed > 0 else 0
        
        print("\n" + "="*60)
        print(f"AGENT PROGRESS - {datetime.now().strftime('%H:%M:%S')}")
        print("="*60)
        print(f"Processed: {self.stats['functions_processed']} functions")
        print(f"Verified:  {verified}/{total} ({verified/total*100:.1f}%)")
        print(f"Decompiled: {decompiled}")
        print(f"Needs Refine: {needs_refine}")
        print(f"Blocked:   {blocked}")
        print(f"Failed:    {failed}")
        print(f"Todo:      {todo}")
        print(f"Batches:   {self.stats['batches_completed']}")
        print(f"Builds:    {self.stats['builds_run']}")
        if self.llm:
            print(f"LLM analyzed: {self.stats['functions_analyzed_by_llm']}")
        print(f"Rate:      {rate:.2f} functions/second")
        print(f"Elapsed:   {timedelta(seconds=int(elapsed))}")
        
        if self.config.max_duration:
            remaining = self.config.max_duration - elapsed
            print(f"Time left: {timedelta(seconds=int(max(0, remaining)))}")
        
        print("="*60 + "\n")
    
    def _sync_main_func_aliases(self):
        """Ensure every `func_8XXXXXXX` jal target referenced in overlay ASM files
        that maps to a known main-segment symbol has a `func_XXXXXXXX = 0xXXXXXXXX;`
        alias in config/sym_export.us.txt.

        This prevents linker undefined-reference errors when overlay object files
        call PSX SDK / runtime functions by their raw address alias rather than
        their canonical name.  The alias file is shared by all overlay linker
        invocations, so a single addition here fixes every affected overlay.

        Safe to run repeatedly: existing entries are never touched.
        """
        project_root = Path(__file__).parent.parent
        sym_export_path = project_root / "config" / "sym_export.us.txt"
        main_syms_path  = project_root / "config" / "symbols.main.us.txt"
        asm_root        = project_root / "asm" / "us"

        if not sym_export_path.exists() or not main_syms_path.exists():
            return

        # --- 1. Load all addresses known to the main-segment symbol file ---
        main_addrs: set = set()
        addr_re = re.compile(r'\w+\s*=\s*(0x[0-9a-fA-F]+)\s*;')
        with open(main_syms_path) as f:
            for line in f:
                m = addr_re.search(line)
                if m:
                    main_addrs.add(int(m.group(1), 16))

        # --- 2. Load addresses already in sym_export.us.txt ---
        existing_names: set = set()
        existing_addrs: set = set()
        name_re = re.compile(r'^(\w+)\s*=\s*(0x[0-9a-fA-F]+)\s*;')
        with open(sym_export_path) as f:
            for line in f:
                m = name_re.match(line.strip())
                if m:
                    existing_names.add(m.group(1))
                    existing_addrs.add(int(m.group(2), 16))

        # --- 3. Scan overlay ASM files for `jal func_8XXXXXXX` references ---
        jal_func_re = re.compile(r'\bjal\s+(func_[0-9a-fA-F]{8})\b')
        referenced: set = set()
        if asm_root.is_dir():
            for asm_file in asm_root.rglob("*.s"):
                # Skip main/ — it defines these symbols, not references them
                try:
                    parts = asm_file.relative_to(asm_root).parts
                    if parts and parts[0] == "main":
                        continue
                except ValueError:
                    pass
                try:
                    text = asm_file.read_text(errors="replace")
                    for name in jal_func_re.findall(text):
                        referenced.add(name)
                except OSError:
                    pass

        # --- 4. Determine which are missing from sym_export and known to main ---
        to_add = []
        for sym_name in sorted(referenced):
            if sym_name in existing_names:
                continue
            addr = int(sym_name[len("func_"):], 16)
            if addr in main_addrs and addr not in existing_addrs:
                to_add.append((sym_name, addr))

        if not to_add:
            self._log("sym_export alias check: all overlay jal targets accounted for", "INFO")
            return

        # --- 5. Append missing aliases ---
        with open(sym_export_path, "a") as f:
            for sym_name, addr in to_add:
                f.write(f"{sym_name} = 0x{addr:08X};\n")

        self._log(
            f"sym_export alias check: added {len(to_add)} missing func_ alias(es) "
            f"to {sym_export_path.relative_to(project_root)}",
            "INFO",
        )
        if self.config.verbose:
            for sym_name, addr in to_add:
                self._log(f"  + {sym_name} = 0x{addr:08X};", "INFO")

    def _initial_audit(self):
        """
        At startup, compile each C file that contains 'decompiled' or 'verified'
        functions and flag any that fail PSY-Q compilation as 'decompiled_needs_refine'.

        This catches functions that were accepted by the old heuristic validator
        but actually produce type/declaration errors in the real compiler.  They
        are NOT automatically reverted here (too risky without the original ASM
        context), but they'll be picked up by the LLM post-fixer or can be
        manually reverted and re-queued.
        """
        self._log("Running initial compilation audit of decompiled functions...", "INFO")

        # Gather unique C files that contain 'decompiled' OR 'verified' functions
        # We include verified because old agent runs may have accepted bad code.
        decompiled = self.db.get_functions_by_status('decompiled', limit=None,
                                                      module=self.config.module_filter)
        verified = self.db.get_functions_by_status('verified', limit=None,
                                                    module=self.config.module_filter)
        all_funcs = (decompiled or []) + (verified or [])
        if not all_funcs:
            self._log("  No decompiled/verified functions to audit", "INFO")
            return

        # Build a map: c_file_path -> [func_name, ...]
        file_to_funcs = {}
        for func in all_funcs:
            cp = func.get('c_file_path')
            if cp:
                file_to_funcs.setdefault(cp, []).append(func['name'])

        project_root = Path(__file__).parent.parent
        flagged = 0

        for rel_c_path, func_names in file_to_funcs.items():
            full_path = project_root / rel_c_path
            if not full_path.exists():
                continue

            compile_ok, errors = self.verifier.check_file_compiles(full_path)
            if compile_ok:
                continue

            # Identify which lines are affected
            error_lines = {e['line'] for e in errors}
            if not error_lines:
                continue

            # Find function body ranges using precise brace-counting so we only
            # flag functions whose body actually contains an error line.
            try:
                file_text = full_path.read_text(errors='replace')
                for name in func_names:
                    body_range = self._find_func_body_range(file_text, name)
                    if body_range is None:
                        continue  # stub / prototype — skip
                    start, end = body_range
                    if any(start <= el <= end for el in error_lines):
                        self.db.update_function_status(
                            name, 'decompiled_needs_refine',
                            notes=f"Startup audit: PSY-Q compile errors in {rel_c_path}"
                        )
                        flagged += 1
                        if self.config.verbose:
                            self._log(f"  Flagged {name} ({rel_c_path})", "WARN")
            except Exception as e:
                self._log(f"  Audit error for {rel_c_path}: {e}", "WARN")

        if flagged:
            self._log(f"Startup audit: flagged {flagged} function(s) as decompiled_needs_refine", "WARN")
        else:
            self._log("Startup audit: all decompiled functions compile cleanly", "INFO")

        # Second pass: reset verified/decompiled_needs_refine functions that still
        # have INCLUDE_ASM in their C file back to 'todo'.  This happens when the
        # DB is reused after a branch reset (the C files were wiped but the DB
        # retained the old status).
        import re as _re
        reset_count = 0
        stale_statuses = ('verified', 'decompiled_needs_refine', 'decompiled')
        stale_funcs = []
        for status in stale_statuses:
            stale_funcs.extend(self.db.get_functions_by_status(status, limit=None,
                                                                module=self.config.module_filter) or [])
        for func in stale_funcs:
            c_file_rel = func.get('c_file_path')
            func_name = func['name']
            if not c_file_rel:
                continue
            c_file = project_root / c_file_rel
            if not c_file.exists():
                continue
            content = c_file.read_text(errors='replace')
            pat = rf'INCLUDE_ASM\s*\(\s*"[^"]*"\s*,\s*{_re.escape(func_name)}\s*\)'
            if _re.search(pat, content):
                self.db.update_function_status(func_name, 'todo',
                                               notes='Reset: C file reverted to INCLUDE_ASM')
                reset_count += 1
                if self.config.verbose:
                    self._log(f"  Reset {func_name} ({func['status']} → todo): C file has INCLUDE_ASM", "WARN")
        if reset_count:
            self._log(f"Startup audit: reset {reset_count} stale function(s) to 'todo'", "WARN")

    def _ensure_asm_splits(self):
        """
        Run splat for any module whose nonmatchings/ directory is missing or empty.

        This happens after `make clean`, which deletes the gitignored asm/ tree.
        The agent needs those .s files to decompile functions, so we regenerate them
        automatically rather than requiring the user to remember the splat step.
        """
        project_root = Path(__file__).parent.parent
        build_us = project_root / 'build' / 'us'
        asm_us = project_root / 'asm' / 'us'
        splat_bin = project_root / '.venv' / 'bin' / 'splat'

        if not splat_bin.exists():
            self._log("  splat not found in .venv; skipping ASM regeneration", "WARN")
            return

        # Which modules have todo functions that need ASM files?
        module_filter = self.config.module_filter
        cursor = self.db.conn.cursor()
        if module_filter:
            cursor.execute(
                "SELECT DISTINCT module FROM functions WHERE status='todo' AND module=?",
                (module_filter,)
            )
        else:
            cursor.execute("SELECT DISTINCT module FROM functions WHERE status='todo'")
        modules_needing_asm = {row['module'] for row in cursor}

        regenerated = []
        for module in sorted(modules_needing_asm):
            nonmatch_dir = asm_us / module / 'nonmatchings'
            if nonmatch_dir.is_dir() and any(nonmatch_dir.rglob('*.s')):
                continue  # Already present

            # Try to find the yaml for this module
            yaml_path = build_us / f'{module}.yaml'
            if not yaml_path.exists():
                # Some modules use a different yaml name (e.g. batini → battle.yaml covers it)
                continue

            self._log(f"  Re-running splat for {module} (nonmatchings/ missing or empty)...", "INFO")
            try:
                import subprocess as _sp
                result = _sp.run(
                    [str(splat_bin), 'split', str(yaml_path)],
                    capture_output=True, text=True,
                    cwd=str(project_root)
                )
                if result.returncode == 0:
                    count = sum(1 for _ in nonmatch_dir.rglob('*.s')) if nonmatch_dir.is_dir() else 0
                    self._log(f"    {module}: generated {count} nonmatching .s files", "INFO")
                    regenerated.append(module)
                else:
                    self._log(f"    {module}: splat failed: {result.stderr[:200]}", "WARN")
            except Exception as e:
                self._log(f"    {module}: splat error: {e}", "WARN")

        if not regenerated:
            self._log("  ASM splits up to date", "INFO")

        # Fix matchings/ directories: some C files use INCLUDE_ASM("…/matchings/…")
        # for functions that splat now emits to nonmatchings/.  Copy any missing ones.
        self._fix_matchings_from_nonmatchings()

    def _fix_matchings_from_nonmatchings(self):
        """
        After a splat run, copy .s files from nonmatchings/ to matchings/ for any
        INCLUDE_ASM references that point at matchings/ but whose file is missing.
        This keeps `make build` working after `make clean`.
        """
        import re as _re
        import shutil as _shutil
        project_root = Path(__file__).parent.parent
        src_root = project_root / 'src'
        asm_us = project_root / 'asm' / 'us'
        pattern = _re.compile(r'INCLUDE_ASM\s*\(\s*"(asm/us/[^"]+/matchings/[^"]+)"\s*,\s*([^);\s]+)')
        copied = 0
        for c_file in src_root.rglob('*.s'):
            pass  # not needed
        for c_file in src_root.rglob('*.c'):
            try:
                text = c_file.read_text(errors='replace')
            except Exception:
                continue
            for m in pattern.finditer(text):
                asm_hint, func_name = m.group(1), m.group(2)
                dst = project_root / asm_hint / f'{func_name}.s'
                if dst.exists():
                    continue
                # Look for the same file in nonmatchings/
                nonmatch_path = str(asm_hint).replace('/matchings/', '/nonmatchings/')
                src = project_root / nonmatch_path / f'{func_name}.s'
                if src.exists():
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    _shutil.copy2(str(src), str(dst))
                    copied += 1
        if copied:
            self._log(f"  Copied {copied} .s file(s) from nonmatchings → matchings to satisfy INCLUDE_ASM paths", "INFO")

    def _cleanup_failure_comments(self):
        """
        Remove 'Decompilation failure:' comment blocks from all C source files.

        These are left behind by m2c when it can't parse external declarations.
        They prevent subsequent decompilations from getting clean context and
        must be swept out before each run.
        """
        import re as _re
        src_dir = Path(__file__).parent.parent / 'src'
        pattern = _re.compile(r'/\*\s*Decompilation failure[^\n]*\n.*?\*/', _re.DOTALL)
        total_removed = 0

        for c_file in src_dir.rglob('*.c'):
            content = c_file.read_text()
            cleaned, n = pattern.subn('', content)
            if n:
                # Collapse runs of blank lines left by removal
                cleaned = _re.sub(r'\n\n\n+', '\n\n', cleaned)
                c_file.write_text(cleaned)
                total_removed += n

        if total_removed:
            self._log(f"Cleaned {total_removed} 'Decompilation failure' comment block(s) from src/", "INFO")

    def _hoist_externs_all_files(self):
        """
        Apply the extern-declaration hoisting pass to every C source file.

        extern declarations generated by mako.sh dec are placed inline beside
        each function body.  This pass gathers them all and moves them to a
        single block before the first INCLUDE_ASM / function definition so every
        file has a clean layout:
            #include ...
            typedefs (if any, hoisted before their first use)
            extern declarations   ← ALL here
            function bodies / INCLUDE_ASM stubs

        Uses apply_all_fixes (which runs extern-hoist then typedef-hoist) so
        that any typedef whose first use is now an extern in the header section
        also gets moved up correctly.
        """
        try:
            from intellisense_fixer import apply_all_fixes
        except ImportError:
            self._log("intellisense_fixer not available; skipping extern hoist", "WARN")
            return

        src_dir = Path(__file__).parent.parent / 'src'
        total_moved = 0
        files_changed = 0

        for c_file in src_dir.rglob('*.c'):
            try:
                content = c_file.read_text()
                fixed, changes = apply_all_fixes(content)
                extern_changes = [c for c in changes if 'extern' in c]
                if extern_changes:
                    c_file.write_text(fixed)
                    n = sum(int(c.split()[1]) for c in extern_changes if c.split()[1].isdigit())
                    total_moved += n
                    files_changed += 1
                    if self.config.verbose:
                        self._log(f"  {c_file.relative_to(Path(__file__).parent.parent)}: {', '.join(changes)}", "INFO")
            except Exception as e:
                self._log(f"  extern hoist error in {c_file.name}: {e}", "WARN")

        if total_moved:
            self._log(f"Hoisted {total_moved} extern declaration(s) across {files_changed} file(s) to file headers", "INFO")

    def _revert_function_to_include_asm(self, c_file: Path, func_name: str) -> bool:
        """
        Replace a decompiled function body with its INCLUDE_ASM stub.

        Walks the file to find the function definition by name, counts braces to
        locate the closing }, then replaces the whole definition with the stub.
        Returns True if the revert succeeded.
        """
        import re as _re
        content = c_file.read_text()
        lines = content.splitlines(keepends=True)

        # Derive asm directory from the file path:
        # src/{overlay}/{stem}.c  →  asm/us/{overlay}/nonmatchings/{stem}
        stem = c_file.stem
        overlay = c_file.parent.name
        asm_dir = f"asm/us/{overlay}/nonmatchings/{stem}"

        # Find the function definition start using a strict pattern that requires a
        # return type before the function name.  This prevents matching call sites
        # like: '} else if (func_name(-1) != 0) {' which the old loose search
        # mistakenly treated as a definition start.
        strict_def_pattern = _re.compile(
            rf'^(?:(?:const|static|inline)\s+)*'
            rf'(?:(?:unsigned|signed)\s+)*'
            rf'(?:void|char|int|u8|s8|u16|s16|u32|s32|long|short|\w+)\s+'
            rf'\*?\s*{_re.escape(func_name)}\s*\(',
            _re.MULTILINE,
        )
        # Fallback: implicit-int — function name at beginning of line with no type
        implicit_def_pattern = _re.compile(
            rf'^{_re.escape(func_name)}\s*\(',
            _re.MULTILINE,
        )
        m = strict_def_pattern.search(content)
        if m is None:
            m = implicit_def_pattern.search(content)
        if m is None:
            return False
        start_idx = content[:m.start()].count('\n')

        # Find the opening brace (may be on the same or next few lines)
        brace_start = None
        for i in range(start_idx, min(start_idx + 6, len(lines))):
            if '{' in lines[i]:
                brace_start = i
                break

        if brace_start is None:
            return False  # No opening brace found — probably just a declaration

        # Count braces to find the matching closing brace
        depth = 0
        end_idx = None
        for i in range(brace_start, len(lines)):
            depth += lines[i].count('{') - lines[i].count('}')
            if depth == 0:
                end_idx = i
                break

        if end_idx is None:
            return False

        # Build the replacement stub
        include_asm_line = f'INCLUDE_ASM("{asm_dir}", {func_name});\n'

        # Also strip m2c-emitted extern/forward-declaration lines that precede the
        # function body.  These are orphaned when a function is reverted and would
        # poison m2c context on the next run.
        # Pattern: blank lines OR lines ending with "// extern" OR lines starting
        # with "extern " (bare extern declarations for D_ globals / unknown funcs).
        _extern_or_blank = _re.compile(r'^\s*$|//\s*extern\s*$|^\s*extern\s')
        i = start_idx - 1
        while i >= 0 and _extern_or_blank.search(lines[i].rstrip('\n\r')):
            i -= 1
        trim_start = i + 1

        new_lines = lines[:trim_start] + [include_asm_line] + lines[end_idx + 1:]
        c_file.write_text(''.join(new_lines))
        return True

    def _cleanup_unknown_type_functions(self):
        """
        Scan C source files for decompiled functions that contain '?' type markers
        (from m2c's inability to determine types).  These poison m2c context for every
        other function in the same file.

        Any such function is reverted to its INCLUDE_ASM stub and marked 'blocked'.
        """
        import re as _re
        src_dir = Path(__file__).parent.parent / 'src'

        # Match ? used as a parameter type, e.g.: "(s32 arg0, ? arg2)" or "(? arg)"
        # Ternary operators like "x ? y : z" are NOT matched because ternary '?' is
        # preceded by an expression value, not by '(' or ','.
        unknown_param = _re.compile(r'[,(]\s*\?\s*(?:[,)]|\w)')
        # Also catch ? as return type at start of a function definition line
        unknown_return = _re.compile(r'^\s*\?\s+\w+\s*\(', _re.MULTILINE)
        # And ?* pointer type
        unknown_ptr = _re.compile(r'\?\s*\*\s+\w+')

        total_reverted = 0

        for c_file in src_dir.rglob('*.c'):
            content = c_file.read_text()

            # Quick check — does this file even have a '?' type marker?
            if not ('/*?*/' in content
                    or unknown_param.search(content)
                    or unknown_return.search(content)
                    or unknown_ptr.search(content)):
                continue

            # Identify the specific function(s) that contain '?' markers.
            # We look for a function signature containing '?' then revert it.
            func_sig_pattern = _re.compile(
                r'^(\w[\w\s*]*?)\s+(\w+)\s*\([^)]*(?:\([^)]*\)[^)]*|[^)])*\?\s*[^;{]*\)',
                _re.MULTILINE
            )

            reverted_in_file = set()
            for match in func_sig_pattern.finditer(content):
                func_name = match.group(2)
                if func_name in reverted_in_file:
                    continue

                # Skip if it's only a forward declaration (ends with ;)
                after = content[match.end():match.end() + 20].lstrip()
                if after.startswith(';'):
                    continue

                # Also skip INCLUDE_ASM lines
                line_start = content.rfind('\n', 0, match.start()) + 1
                line = content[line_start:match.start() + 100]
                if 'INCLUDE_ASM' in line:
                    continue

                reverted = self._revert_function_to_include_asm(c_file, func_name)
                if reverted:
                    reverted_in_file.add(func_name)
                    total_reverted += 1
                    # Re-read — file was modified
                    content = c_file.read_text()
                    # Mark as blocked in DB if it's tracked
                    try:
                        existing = self.db.get_function(func_name)
                        if existing and existing.get('status') in ('verified', 'decompiled',
                                                                    'decompiled_needs_refine'):
                            self.db.update_function_status(
                                func_name, 'blocked',
                                notes="Reverted: had unknown '?' type marker in signature"
                            )
                    except Exception:
                        pass
                    self._log(f"  Reverted {func_name} in {c_file.name} (? type marker)", "WARN")

            # Second pass: catch '?' in m2c-emitted extern blocks that precede a function
            # body.  These are lines ending with "// extern" that contain '?' types, or
            # "extern /*?*/" lines with NO valid type after the comment.  They won't
            # match func_sig_pattern (which looks at function signatures, not forward
            # declarations) but still poison the context.
            # Pattern 1: "extern /*?*/" where ? is the ONLY type (no valid C type follows)
            #   e.g. "extern /*?*/ some_sym;" — poisoned, type unknown
            #   NOT "extern /*?*/s32 D_800F9F3C;" — ok, s32 is the resolved type
            # Pattern 2: "? ... // extern" — unknown-type forward decl emitted by m2c
            extern_unknown = _re.compile(
                r'^extern\s+/\*\?\*/\s+\w+\s*;|\?[^*].*//\s*extern\s*$', _re.MULTILINE
            )
            func_def_pattern = _re.compile(
                r'^(?:const\s+)?(?:unsigned\s+|signed\s+)?'
                r'(?:void|u8|s8|u16|s16|u32|s32|int|char|short|long|unk_\w+|ff7s|\w+)\s*\*?\s+'
                r'(\w+)\s*\([^)]*\)\s*\{',
                _re.MULTILINE,
            )
            # Snapshot content once so all finditer positions remain valid, then
            # collect target function names before making any modifications.
            # Updating content inside the loop caused position drift: after each
            # revert the file shrinks, and old byte positions from the finditer
            # landed in wrong places, causing unrelated functions to be reverted.
            content = c_file.read_text()
            funcs_to_revert_by_extern = []
            for ext_match in extern_unknown.finditer(content):
                # Find the next concrete function definition after this bad extern line
                # (within a reasonable window — externs are adjacent to their function)
                search_start = ext_match.end()
                def_match = func_def_pattern.search(content, search_start,
                                                     search_start + 4000)
                if def_match is None:
                    continue
                fname = def_match.group(1)
                if fname not in reverted_in_file and fname not in funcs_to_revert_by_extern:
                    funcs_to_revert_by_extern.append(fname)

            for func_name in funcs_to_revert_by_extern:
                if func_name in reverted_in_file:
                    continue
                reverted = self._revert_function_to_include_asm(c_file, func_name)
                if reverted:
                    reverted_in_file.add(func_name)
                    total_reverted += 1
                    try:
                        existing = self.db.get_function(func_name)
                        if existing and existing.get('status') in ('verified', 'decompiled',
                                                                    'decompiled_needs_refine'):
                            self.db.update_function_status(
                                func_name, 'blocked',
                                notes="Reverted: had '?' type in preceding extern declarations"
                            )
                    except Exception:
                        pass
                    self._log(
                        f"  Reverted {func_name} in {c_file.name} (? in extern block)", "WARN"
                    )

        if total_reverted:
            self._log(f"Reverted {total_reverted} function(s) with '?' type markers to INCLUDE_ASM",
                      "WARN")

    def _cleanup_file_level_question_marks(self):
        """
        Remove bare file-scope lines that use '?' as a type — these are m2c
        artifacts for SDK/external functions whose type is unknown.  They are
        syntactically invalid C and cause build failures.

        Examples removed:
          ? SpuSetTransferCallback(?*);            // extern
          extern ?* jtbl_80034D14;
          s16 GetTPage(s32, ?, s16, s16);          // extern
          ? LoadImage(void*, s32);                 // extern
        """
        import re as _re

        # Line is at file scope if it starts at column 0 (no leading whitespace)
        # and contains ? used as a type (not inside a string or comment).
        # Patterns:
        #   1. Line starting with '?' — unknown-return-type extern
        #   2. Line starting with 'extern ?*' — extern pointer of unknown type
        #   3. Forward-declaration line (no leading brace) with '?' in parameter list
        bad_line = _re.compile(
            r'^(?:'
            r'\?\s+\w[\w\s*,]*\(.*\)\s*(?:;|//.*)?'   # ? ReturnType FuncName(...) or ? FuncName(...)
            r'|extern\s+\?\s*\*[^;]*;'                  # extern ?* sym;
            r'|(?:extern\s+)?(?:\w+\s+)?\w+\s*\([^)]*\?[^)]*\)\s*;(?:\s*//.*)?'  # Decl with ? param
            r')$',
            _re.MULTILINE,
        )

        src_dir = Path(__file__).parent.parent / 'src'
        total_removed = 0
        for c_file in src_dir.rglob('*.c'):
            content = c_file.read_text()
            if '?' not in content:
                continue
            cleaned = bad_line.sub('', content)
            # Remove any double blank lines left behind
            cleaned = _re.sub(r'\n{3,}', '\n\n', cleaned)
            if cleaned != content:
                c_file.write_text(cleaned)
                removed = content.count('\n') - cleaned.count('\n')
                total_removed += removed
                self._log(f"  Removed {removed} file-scope '?' line(s) from {c_file.name}", "INFO")
        if total_removed:
            self._log(f"Removed {total_removed} file-scope '?' type line(s) from source files", "INFO")

    def run(self):
        """Main agent loop."""
        self._log("="*60)
        self._log("STARTING DECOMPILATION AGENT")
        self._log("="*60)
        
        # Show configuration
        self._log(f"Batch size: {self.config.batch_size}")
        self._log(f"Verify interval: {self.config.verify_interval}")
        if self.config.module_filter:
            self._log(f"Module filter: {self.config.module_filter}")
        if self.config.max_duration:
            self._log(f"Duration limit: {self.config.max_duration}s")
        if self.config.target_count:
            self._log(f"Target count: {self.config.target_count}")
        
        self.stats['start_time'] = time.time()
        self.stats['last_progress_time'] = time.time()

        # Regenerate nonmatchings/ ASM files if wiped by make clean
        self._log("Checking ASM splits...", "INFO")
        self._ensure_asm_splits()

        # Regenerate cross-file forward declarations so m2c has full type context
        self._refresh_decls()

        # Sweep stale 'Decompilation failure:' comment blocks that block m2c context
        self._cleanup_failure_comments()

        # Hoist all scattered extern declarations to the file header section
        self._hoist_externs_all_files()

        # Revert functions with '?' type markers (left by old agent runs without the fix)
        self._cleanup_unknown_type_functions()

        # Remove bare file-scope lines with '?' types (e.g. "? SpuSetTransferCallback(?*);")
        self._cleanup_file_level_question_marks()

        # Reset false-verified functions before starting if requested
        if self.config.retry_verified:
            self._reset_false_verified()

        # Flag currently-decompiled functions whose file fails PSY-Q compilation
        self._initial_audit()

        # Ensure all overlay ASM jal targets have func_ aliases in sym_export.us.txt
        self._sync_main_func_aliases()

        # Reset functions that were left in_progress from a previous crash
        self._reset_stale_in_progress()

        # Give all previously-failed/blocked functions a fresh retry chance this session
        self._reset_failed_attempt_counts()

        # Initial status
        initial_stats = self.db.get_statistics()
        self._log(f"Initial state: {initial_stats['by_status'].get('verified', 0)}/{initial_stats['total']} verified")
        
        # Main loop
        functions_since_build = 0
        
        while not self._check_stop_conditions():
            # Get next batch
            batch = self._get_next_batch()
            
            if not batch:
                self._log("No more functions to process", "INFO")

                # Dynamic retry gate: if we've already done at least one retry round
                # and that round produced zero new decompilations or needs_refine
                # results, stop — further retries won't help.
                if self._retry_round > 0 and self._retry_progress_since_last_retry == 0:
                    self._log(
                        f"Retry round {self._retry_round} produced no progress — stopping retries",
                        "INFO",
                    )
                    break

                # Start a new retry round: reset progress counter.
                self._retry_round += 1
                self._retry_progress_since_last_retry = 0
                self._log(f"Starting retry round {self._retry_round}...", "INFO")

                # Try retrying failed functions (check DB, not just runtime counter)
                failed_in_db = self.db.get_functions_by_status(
                    'failed', limit=1, module=self.config.module_filter)
                if failed_in_db:
                    self._retry_failed_functions()
                    batch = self._get_next_batch()
                
                # Try re-queuing decompiled_needs_refine functions (may have been manually fixed)
                if not batch:
                    self._retry_needs_refine()
                    batch = self._get_next_batch()
                
                # Try retrying blocked functions if flag is set
                if not batch and self.config.retry_blocked:
                    self._retry_blocked_functions()
                    batch = self._get_next_batch()
                
                if not batch:
                    # Check if we have blocked functions
                    cursor = self.db.conn.cursor()
                    if self.config.module_filter:
                        cursor.execute("SELECT COUNT(*) FROM functions WHERE status='blocked' AND module=?", (self.config.module_filter,))
                    else:
                        cursor.execute("SELECT COUNT(*) FROM functions WHERE status='blocked'")
                    blocked_count = cursor.fetchone()[0]
                    if blocked_count:
                        self._log(f"Queue empty; {blocked_count} functions blocked (will retry on next run)", "INFO")
                    self._log("No functions available, stopping", "INFO")
                    break
            
            # Process batch
            result = self._process_batch(batch)
            functions_since_build += result['success']

            # Accumulate progress for retry-round tracking.
            # Count both true successes and needs_refine (compiled but with pre-existing
            # errors) as forward progress, since both represent real decompilation work.
            self._retry_progress_since_last_retry += result['success'] + result.get('needs_refine', 0)
            
            # Verify after every batch that produced at least one success so that
            # newly-decompiled signatures land in decomp_decls.h immediately and
            # are available as m2c context for the very next batch.
            if result['success'] > 0:
                self._verify_batch()
                
                # Fix type errors with LLM if enabled (after verification)
                if self.llm and self.config.llm_auto_fix:
                    self._fix_type_errors_with_llm()
            
            # Periodic full build
            if functions_since_build >= self.config.full_build_interval:
                self._run_full_build()
                functions_since_build = 0
            
            # Show progress
            self._show_progress()
            
            # Pause between batches
            if not self._check_stop_conditions():
                time.sleep(self.config.pause_between_batches)
        
        # Final verification (catches any successes from the last batch if it
        # was skipped above, e.g. if stop conditions fired before verify ran)
        self._log("Running final verification...", "INFO")
        self._verify_batch()

        # LLM fix pass: attempt to repair decompiled_needs_refine functions
        if self.llm:
            try:
                self._llm_fix_needs_refine()
            except Exception as e:
                self._log(f"LLM fix pass failed: {e}", "WARN")

        # Final stats
        self._show_final_report()
    
    def _show_final_report(self):
        """Show final statistics."""
        elapsed = time.time() - self.stats['start_time']
        
        print("\n" + "="*60)
        print("AGENT EXECUTION COMPLETE")
        print("="*60)
        print(f"Total time: {timedelta(seconds=int(elapsed))}")
        print(f"Functions processed: {self.stats['functions_processed']}")
        print(f"Functions verified: {self.stats['functions_verified']}")
        print(f"Functions failed: {self.stats['functions_failed']}")
        print(f"Batches completed: {self.stats['batches_completed']}")
        print(f"Builds run: {self.stats['builds_run']}")
        if self.llm:
            print(f"LLM analyzed: {self.stats['functions_analyzed_by_llm']}")
        
        if elapsed > 0:
            rate = self.stats['functions_processed'] / elapsed
            print(f"Average rate: {rate:.2f} functions/second")
        
        if self.stats['errors']:
            print(f"\nErrors encountered: {len(self.stats['errors'])}")
            print("Check agent.log for details")
        
        # Final database state
        final_stats = self.db.get_statistics()
        verified = final_stats['by_status'].get('verified', 0)
        total = final_stats['total']
        progress = (verified / total * 100) if total > 0 else 0
        
        print(f"\nFinal Progress: {verified}/{total} ({progress:.1f}%)")
        
        # Struct learning statistics
        try:
            struct_stats = self.db.get_struct_learning_stats()
            if struct_stats['total_patterns'] > 0:
                print(f"\n📚 Struct Learning:")
                print(f"  Total patterns learned: {struct_stats['total_patterns']}")
                print(f"  High confidence (≥70%): {struct_stats['high_confidence']}")
                print(f"  Medium confidence (30-70%): {struct_stats['medium_confidence']}")
                print(f"  Low confidence (<30%): {struct_stats['low_confidence']}")
        except Exception as e:
            if self.config.verbose:
                self._log(f"Could not get struct learning stats: {e}")
        
        print("="*60 + "\n")
        
        # Write summary to file
        summary = {
            'completed_at': datetime.now().isoformat(),
            'duration_seconds': int(elapsed),
            'functions_processed': self.stats['functions_processed'],
            'functions_verified': self.stats['functions_verified'],
            'functions_failed': self.stats['functions_failed'],
            'functions_analyzed_by_llm': self.stats['functions_analyzed_by_llm'],
            'final_progress_percent': progress,
            'llm_enabled': self.llm is not None,
            'errors': self.stats['errors']
        }
        
        try:
            with open('agent_summary.json', 'w') as f:
                json.dump(summary, f, indent=2)
            print("Summary saved to agent_summary.json")
        except Exception as e:
            print(f"Could not save summary: {e}")

    @staticmethod
    def _extract_c_function(content: str, func_name: str) -> Optional[str]:
        """Extract a complete C function body by brace-matching."""
        pattern = rf'\b{re.escape(func_name)}\s*\([^)]*\)\s*\{{'
        m = re.search(pattern, content, re.DOTALL)
        if not m:
            return None
        brace_start = m.end() - 1  # index of opening '{'
        depth = 0
        for i, ch in enumerate(content[brace_start:], brace_start):
            if ch == '{':
                depth += 1
            elif ch == '}':
                depth -= 1
                if depth == 0:
                    return content[m.start():i + 1]
        return None

    def _llm_fix_needs_refine(self):
        """
        For each decompiled_needs_refine function whose C code is already in the
        source file, use the LLM to fix PSY-Q compile errors, re-check compilation,
        and promote to 'decompiled' on success.
        """
        needs_refine = self.db.get_functions_by_status(
            'decompiled_needs_refine', module=self.config.module_filter)
        if not needs_refine:
            self._log("No decompiled_needs_refine functions to fix", "INFO")
            return

        self._log(f"LLM fix pass: {len(needs_refine)} decompiled_needs_refine function(s)", "INFO")
        project_root = Path(__file__).parent.parent

        # Group by C file so we only compile-check each file once per outer loop
        file_to_funcs: dict = {}
        for func in needs_refine:
            c_file = func.get('c_file_path')
            if c_file:
                file_to_funcs.setdefault(c_file, []).append(func)

        fixed_total = 0

        for rel_c_path, funcs_in_file in file_to_funcs.items():
            full_path = project_root / rel_c_path
            if not full_path.exists():
                continue

            compile_ok, errors = self.verifier.check_file_compiles(full_path)

            if compile_ok:
                # File already compiles without any changes — promote immediately
                for func in funcs_in_file:
                    self.db.update_function_status(
                        func['name'], 'decompiled',
                        notes="LLM fix pass: file compiles cleanly")
                    fixed_total += 1
                self._log(f"  {rel_c_path}: already compiles — promoted {len(funcs_in_file)} function(s)", "INFO")
                continue

            # Attempt to fix each function in this file with the LLM
            content = full_path.read_text()
            for func in funcs_in_file:
                func_name = func['name']
                error_summary = "; ".join(e['message'] for e in errors[:5])

                broken_code = self._extract_c_function(content, func_name)
                if not broken_code:
                    if self.config.verbose:
                        self._log(f"  {func_name}: function body not found in {rel_c_path}, skipping", "INFO")
                    continue

                self._log(f"  Fixing {func_name} (errors: {error_summary[:80]})", "INFO")

                try:
                    fix_result = self.llm.fix_m2c_error(func_name, broken_code, error_summary)
                except Exception as e:
                    self._log(f"  {func_name}: LLM call failed: {e}", "WARN")
                    continue

                if not fix_result or not fix_result.get('fixed_code') or fix_result.get('confidence') == 'low':
                    confidence = fix_result.get('confidence', 'none') if fix_result else 'none'
                    self._log(f"  {func_name}: no usable fix (confidence={confidence})", "INFO")
                    continue

                # Apply fix tentatively
                patched_content = content.replace(broken_code, fix_result['fixed_code'])
                full_path.write_text(patched_content)

                compile_ok_after, errors_after = self.verifier.check_file_compiles(full_path)

                if compile_ok_after:
                    explanation = (fix_result.get('explanation') or '')[:200]
                    self.db.update_function_status(
                        func_name, 'decompiled',
                        notes=f"LLM fix ({fix_result['confidence']} confidence): {explanation}")
                    self.stats['functions_analyzed_by_llm'] += 1
                    fixed_total += 1
                    content = patched_content  # carry forward for next func in same file
                    errors = errors_after       # update error list (may be empty now)
                    self._log(f"  ✅ {func_name}: fixed and promoted to decompiled", "INFO")

                    # Audit log
                    try:
                        audit_log = project_root / "automation" / "llm_fixes_audit.log"
                        with open(audit_log, 'a') as f:
                            from datetime import datetime as _dt
                            f.write(f"{_dt.now().isoformat()}\t{func_name}\t{rel_c_path}\t"
                                    f"{fix_result['confidence']}\t{error_summary[:120]}\t{explanation}\n")
                    except Exception:
                        pass
                else:
                    # Revert this function's change and keep looking
                    full_path.write_text(content)
                    self._log(f"  ❌ {func_name}: LLM fix didn't resolve errors, reverted", "INFO")

        self._log(f"LLM fix pass complete — fixed {fixed_total} function(s)", "INFO")


def main():
    parser = argparse.ArgumentParser(description="Autonomous decompilation agent")
    
    # Main commands
    parser.add_argument('--run', action='store_true',
                       help='Start the agent')
    parser.add_argument('--status', action='store_true',
                       help='Show agent statistics')
    
    # Configuration
    parser.add_argument('--batch', type=int, default=10,
                       help='Batch size (default: 10)')
    parser.add_argument('--parallel', type=int, default=1, metavar='N',
                       help='Decompile N functions in parallel (default: 1, max: cpu_count)')
    parser.add_argument('--duration', type=int,
                       help='Max duration in seconds')
    parser.add_argument('--target', type=int,
                       help='Stop after N functions')
    parser.add_argument('--target-percent', type=float,
                       help='Stop at completion percentage')
    parser.add_argument('--module', type=str,
                       help='Filter by module (battle, field, etc.)')
    parser.add_argument('--max-lines', type=int,
                       help='Skip functions larger than N lines')
    
    # Intervals
    parser.add_argument('--verify-interval', type=int, default=20,
                       help='Verify every N functions (default: 20)')
    parser.add_argument('--build-interval', type=int, default=100,
                       help='Full build every N functions (default: 100)')
    
    # LLM assistance
    parser.add_argument('--llm', action='store_true',
                       help='Enable LLM-assisted analysis (requires Ollama)')
    parser.add_argument('--llm-threshold', type=int, default=20,
                       help='Analyze functions >= N lines with LLM (default: 20)')
    parser.add_argument('--llm-model', type=str, default='qwen2.5-coder:7b',
                       help='LLM model to use (default: qwen2.5-coder:7b)')
    parser.add_argument('--llm-auto-fix', action='store_true',
                       help='Enable automatic LLM fixing of m2c errors')
    
    # Dependency analysis
    parser.add_argument('--no-dependency-analysis', action='store_true',
                       help='Disable dependency-aware function prioritization')
    parser.add_argument('--dependency-weight', type=float, default=0.5,
                       help='Weight for dependency score vs simplicity (0-1, default: 0.5)')
    parser.add_argument('--blocker-multiplier', type=float, default=2.0,
                       help='Per-dependent bonus added to agent score (default: 2.0; higher = more aggressive blocker prioritization)')
    parser.add_argument('--no-strict-ordering', action='store_true',
                       help='Disable strict bottom-up decompilation ordering (allow decompiling callers before callees)')
    parser.add_argument('--filter-cross-module', action='store_true',
                       help='Only attempt functions whose cross-module callees are already decompiled (eliminates /*?*/ markers)')

    # Retry / recovery
    parser.add_argument('--retry-blocked', action='store_true',
                       help='Re-queue blocked functions when todo queue is empty (smart: only if a same-file callee was recently resolved)')
    parser.add_argument('--force-retry-blocked', action='store_true',
                       help='Re-queue ALL blocked functions regardless of whether callees changed (use after adding type definitions to headers)')
    parser.add_argument('--retry-verified', action='store_true',
                       help='Scan verified/decompiled functions for false positives (INCLUDE_ASM still present) and reset them to todo')
    parser.add_argument('--verify-needs-refine', action='store_true',
                       help='Re-check all decompiled_needs_refine functions: promote to verified/decompiled those whose compile errors are now fixed')

    # Behavior
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output')
    parser.add_argument('--log-file', type=str, default='agent.log',
                       help='Log file path')
    parser.add_argument('--strict-validation', action='store_true',
                       help="Use only base soft errors (saved_reg_*, unaligned type); treat '?' type-unknown artifacts as hard rejects (revert to INCLUDE_ASM) instead of decompiled_needs_refine")
    
    args = parser.parse_args()
    
    if args.status:
        # Show current status
        db = DecompDatabase()
        stats = db.get_statistics()
        
        print("\n" + "="*60)
        print("AGENT STATUS")
        print("="*60)
        
        total = stats['total']
        by_status = stats['by_status']
        verified = by_status.get('verified', 0)
        decompiled = by_status.get('decompiled', 0)
        needs_refine = by_status.get('decompiled_needs_refine', 0)
        failed = by_status.get('failed', 0)
        blocked = by_status.get('blocked', 0)
        
        print(f"Total:      {total}")
        print(f"Verified:   {verified} ({verified/total*100:.1f}%)")
        print(f"Decompiled: {decompiled}")
        print(f"Needs Refine: {needs_refine}")
        print(f"Blocked:    {blocked}")
        print(f"Failed:     {failed}")
        print(f"Todo:       {by_status.get('todo', 0)}")
        print("="*60 + "\n")
        
        sys.exit(0)

    if args.verify_needs_refine:
        config = AgentConfig()
        config.verbose = args.verbose
        config.log_file = args.log_file
        config.module_filter = args.module
        agent = DecompilationAgent(config)
        agent.verify_needs_refine(module=args.module, verbose=args.verbose)
        sys.exit(0)

    if args.run:
        # Create configuration
        config = AgentConfig()
        config.batch_size = args.batch
        config.parallel_workers = max(1, min(args.parallel, cpu_count()))  # Clamp to valid range
        config.max_duration = args.duration
        config.target_count = args.target
        config.target_progress = args.target_percent
        config.module_filter = args.module
        config.max_function_lines = args.max_lines
        config.verify_interval = args.verify_interval
        config.full_build_interval = args.build_interval
        config.use_llm = args.llm
        config.llm_analyze_threshold = args.llm_threshold
        config.llm_model = args.llm_model
        config.llm_auto_fix = args.llm_auto_fix
        config.use_dependency_analysis = not args.no_dependency_analysis
        config.dependency_weight = args.dependency_weight
        config.blocker_multiplier = args.blocker_multiplier
        config.strict_dependency_ordering = not args.no_strict_ordering
        config.filter_cross_module_unresolved = args.filter_cross_module
        config.retry_blocked = args.retry_blocked or args.force_retry_blocked
        config.force_retry_blocked = args.force_retry_blocked
        config.retry_verified = args.retry_verified
        config.verbose = args.verbose
        config.log_file = args.log_file
        config.strict_validation = args.strict_validation

        # Clear the log file at the start of every manual run so it only
        # captures output from this invocation.
        if config.log_file:
            try:
                open(config.log_file, 'w').close()
            except OSError:
                pass

        # Tee stdout/stderr so every print() call also goes to the log file
        if config.log_file:
            tee = _TeeOutput(sys.stdout, config.log_file)
            sys.stdout = tee
            sys.stderr = _TeeOutput(sys.stderr, config.log_file)

        # Create and run agent
        agent = DecompilationAgent(config)
        
        try:
            agent.run()
        except KeyboardInterrupt:
            print("\n\n⚠️  Interrupted by user")
            agent._show_final_report()
            sys.exit(1)
        except Exception as e:
            print(f"\n\n❌ Fatal error: {e}")
            import traceback
            traceback.print_exc()
            sys.exit(1)
    
    else:
        parser.print_help()
        print("\nQuick examples:")
        print("  python agent.py --run")
        print("  python agent.py --run --duration 3600 --batch 20 --parallel 4")
        print("  python agent.py --run --target 100 --module battle")
        print("  python agent.py --run --target-percent 10.0")
        print("  python agent.py --run --retry-blocked --parallel 4")
        print("  python agent.py --status")


if __name__ == '__main__':
    main()
