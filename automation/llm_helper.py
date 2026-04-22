#!/usr/bin/env python3
"""
LLM Integration for Decompilation Assistance

Interfaces with Ollama/Qwen to provide AI-assisted decompilation:
1. Analyze MIPS assembly before decompilation
2. Suggest C code improvements
3. Help refine non-matching functions
4. Provide context and explanations

Usage:
    python llm_helper.py --analyze-asm function_name
    python llm_helper.py --suggest-c function_name
    python llm_helper.py --explain asm/us/battle/file.s
"""

import subprocess
import argparse
import json
import re
from pathlib import Path
from typing import Optional, Dict, List, Tuple
from database import DecompDatabase

# Project root
PROJECT_ROOT = Path(__file__).parent.parent
ASM_DIR = PROJECT_ROOT / "asm" / "us"

# Ollama configuration
OLLAMA_MODEL = "qwen2.5-coder:7b"
OLLAMA_ENDPOINT = "http://localhost:11434"

# Shared FF7 project context injected into all LLM prompts
FF7_CONTEXT = """
## Project: Final Fantasy VII (PS1 USA) Decompilation
Goal: byte-accurate recompilation — the output C must compile to identical machine code as the original binary.

### PSX SDK Types (always use these — never int/short/char/long)
```c
typedef signed char s8;      typedef unsigned char u8;
typedef signed short s16;    typedef unsigned short u16;
typedef signed int s32;      typedef unsigned int u32;
typedef u8 unk_data;         typedef unsigned int* unk_ptr;
```

### Naming Conventions
- Unknown functions: `func_800XXXXX` (address-based, do not rename)
- Decompiled functions: verb-first (e.g., `InitBattle`, `LoadScene`, `DrawSprite`), prefer PSX SDK names
- Unknown globals: `D_800XXXXX`
- Known game state globals: `g_` prefix (e.g., `g_BattleState`)
- Unknown struct fields: `unkXX` (offset in hex, e.g., `unk10` for offset 0x10)

### String Encoding
FF7 uses custom encoding — strings must use the `_S()` macro: `const char* msg = _S("Save game?");`

### Decompilation Checklist — output MUST satisfy ALL of these
- No `/*?*/` unknown types — infer from register usage and context
- No `void*` parameters that should be typed struct pointers
- No pointer arithmetic for struct field access — use `->` or `.` operators
- Array elements accessed with `arr[i]`, not `*(arr + i * sizeof(...))`
- `goto loop_XX` patterns converted to `while` / `do-while` loops
- `goto block_XX` inside switch statements inlined (reverse compiler optimization)
- Struct sizes and field alignments match assembly access patterns (4-byte aligned)

### Compiler Notes (PSYQ 3.3 / cc1-psx)
- GP (global pointer) = `0x80062D44` in main overlay — variables near this use gp-relative addressing
- Two compilers: `cc1-psx-26` (PSYQ 2.6) and `cc1-psx-272` (PSYQ 2.72); check `config/us.yaml`
- Calling convention: `$a0`–`$a3` args, `$v0`–`$v1` return values
- Type affects load instruction: `s8` → `lb`, `u8` → `lbu`, `s16` → `lh`, `u16` → `lhu`

### Common Matching Pitfalls
- Merged calls: compiler merges common code after if/else branches; duplicate the call inside each branch to match
- Struct vs variables: use proper structs for stack-allocated data — compiler may optimize away isolated local vars
- Sign extension: `sll`/`sra` pairs appear in both branches of original — do not merge them
"""


class LLMHelper:
    """Helper for LLM-assisted decompilation."""
    
    def __init__(self, model: str = OLLAMA_MODEL, verbose: bool = False):
        self.model = model
        self.verbose = verbose
        self.available = self._check_ollama()
    
    def _check_ollama(self) -> bool:
        """Check if Ollama is available."""
        try:
            result = subprocess.run(
                ["ollama", "list"],
                capture_output=True,
                text=True,
                timeout=5
            )
            return result.returncode == 0 and self.model.split(':')[0] in result.stdout
        except Exception as e:
            if self.verbose:
                print(f"Warning: Ollama not available: {e}")
            return False
    
    def _call_ollama(self, prompt: str, temperature: float = 0.2, max_tokens: int = 2048, timeout: int = 60) -> Optional[str]:
        """Call Ollama with a prompt."""
        if not self.available:
            return None
        
        try:
            # Use ollama run command
            result = subprocess.run(
                ["ollama", "run", self.model, prompt],
                capture_output=True,
                text=True,
                timeout=timeout
            )
            
            if result.returncode == 0:
                return result.stdout.strip()
            else:
                if self.verbose:
                    print(f"Ollama error: {result.stderr}")
                return None
                
        except subprocess.TimeoutExpired:
            if self.verbose:
                print(f"Ollama request timed out after {timeout}s")
            return None
        except Exception as e:
            if self.verbose:
                print(f"Error calling Ollama: {e}")
            return None
    
    def get_assembly_code(self, function_name: str) -> Optional[str]:
        """Get assembly code for a function from the ASM directory."""
        db = DecompDatabase()
        func = db.get_function(function_name)
        
        if not func or not func['asm_file_path']:
            return None
        
        asm_path = PROJECT_ROOT / func['asm_file_path']
        
        if not asm_path.exists():
            return None
        
        try:
            content = asm_path.read_text()
            
            # Extract just this function's assembly
            # Look for function label and get code until next label or end
            pattern = rf'^glabel\s+{re.escape(function_name)}\s*$(.+?)(?=^glabel|\Z)'
            match = re.search(pattern, content, re.MULTILINE | re.DOTALL)
            
            if match:
                return match.group(1).strip()
            else:
                # Fallback: just return a reasonable chunk
                lines = content.split('\n')
                for i, line in enumerate(lines):
                    if function_name in line:
                        # Get next 50 lines as approximation
                        return '\n'.join(lines[i:i+50])
                return None
                
        except Exception as e:
            if self.verbose:
                print(f"Error reading assembly: {e}")
            return None
    
    def analyze_assembly(self, function_name: str) -> Optional[Dict]:
        """
        Analyze MIPS assembly and provide insights.
        
        Returns dict with:
            - summary: Brief description of what function does
            - complexity: Estimated difficulty (easy/medium/hard)
            - patterns: Detected patterns (loops, conditionals, etc.)
            - suggestions: C code suggestions
        """
        if not self.available:
            return None
        
        asm_code = self.get_assembly_code(function_name)
        if not asm_code:
            return None
        
        prompt = f"""{FF7_CONTEXT}
Analyze this MIPS R3000 assembly function.

Function: {function_name}

Assembly:
{asm_code}

Provide a concise analysis:
1. What does this function do? (1-2 sentences)
2. Complexity: easy/medium/hard
3. Key patterns: (loops, conditionals, function calls, memory access, struct usage)
4. Suggested C equivalent — use PSX SDK types (s32/u32/s16/u16/s8/u8), apply decompilation checklist rules (goto→while, array indices, struct `->` access)

Format your response as:
SUMMARY: <description>
COMPLEXITY: <easy|medium|hard>
PATTERNS: <list>
C_SUGGESTION:
<pseudocode>
"""
        
        if self.verbose:
            print(f"Analyzing {function_name} with LLM...")
        
        response = self._call_ollama(prompt, temperature=0.2)
        
        if not response:
            return None
        
        # Parse response
        result = {
            'function': function_name,
            'raw_response': response,
            'summary': None,
            'complexity': 'medium',
            'patterns': [],
            'c_suggestion': None
        }
        
        # Extract structured data
        if 'SUMMARY:' in response:
            summary_match = re.search(r'SUMMARY:\s*(.+?)(?=\n[A-Z]+:|$)', response, re.DOTALL)
            if summary_match:
                result['summary'] = summary_match.group(1).strip()
        
        if 'COMPLEXITY:' in response:
            complexity_match = re.search(r'COMPLEXITY:\s*(\w+)', response, re.IGNORECASE)
            if complexity_match:
                result['complexity'] = complexity_match.group(1).lower()
        
        if 'PATTERNS:' in response:
            patterns_match = re.search(r'PATTERNS:\s*(.+?)(?=\n[A-Z]+:|$)', response, re.DOTALL)
            if patterns_match:
                result['patterns'] = [p.strip() for p in patterns_match.group(1).strip().split(',')]
        
        if 'C_SUGGESTION:' in response:
            c_match = re.search(r'C_SUGGESTION:\s*(.+?)$', response, re.DOTALL)
            if c_match:
                result['c_suggestion'] = c_match.group(1).strip()
        
        return result
    
    def suggest_c_code(self, function_name: str, context: Optional[str] = None) -> Optional[str]:
        """
        Suggest C code for a function based on its assembly.
        
        Args:
            function_name: Name of function
            context: Optional additional context (nearby functions, structs, etc.)
        """
        if not self.available:
            return None
        
        asm_code = self.get_assembly_code(function_name)
        if not asm_code:
            return None
        
        prompt = f"""{FF7_CONTEXT}
Decompile this MIPS R3000 function to C code that will compile to identical assembly.

Function: {function_name}

Assembly:
{asm_code}
"""
        
        if context:
            prompt += f"\nAdditional Context:\n{context}\n"
        
        prompt += """
Generate C code following these rules:
- Use PSX SDK types exclusively: s8/u8/s16/u16/s32/u32
- Apply decompilation checklist: no /*?*/, no void* for struct args, use ->/.  for struct fields, array[i] not pointer arithmetic, goto loop_XX → while loops, goto block_XX in switch → inline the branch
- $a0–$a3 = arguments, $v0–$v1 = return values
- Type determines load instruction: s8→lb, u8→lbu, s16→lh, u16→lhu — pick types to match
- Duplicate common calls inside each branch of if/else to avoid compiler merging
- Use proper struct types for stack vars — not isolated s16 locals

Provide ONLY the C function code, no explanation:
"""
        
        if self.verbose:
            print(f"Generating C suggestion for {function_name}...")
        
        response = self._call_ollama(prompt, temperature=0.3, max_tokens=1024)
        
        if response:
            # Clean up response - extract just code if wrapped in markdown
            code_match = re.search(r'```c?\n(.*?)\n```', response, re.DOTALL)
            if code_match:
                return code_match.group(1).strip()
            return response.strip()
        
        return None
    
    def infer_types_from_assembly(self, function_name: str, extern_symbols: List[str]) -> Optional[Dict]:
        """
        Infer struct types for extern symbols before decompilation.
        
        Analyzes assembly to determine:
        - Field offsets accessed for each symbol
        - Field sizes (byte/half/word)
        - Likely struct definitions
        
        Args:
            function_name: Function to analyze
            extern_symbols: List of extern symbols (D_800XXXXX, etc.) used
            
        Returns:
            Dict mapping symbol_name -> {
                'typedef': suggested typedef string,
                'fields': {offset: (name, type)},
                'confidence': 0.0-1.0
            }
        """
        if not self.available or not extern_symbols:
            return None
        
        asm_code = self.get_assembly_code(function_name)
        if not asm_code:
            return None
        
        symbols_str = ', '.join(extern_symbols)
        
        prompt = f"""{FF7_CONTEXT}
Analyze this MIPS R3000 assembly to infer struct types for extern symbols.

Function: {function_name}
Extern Symbols to Analyze: {symbols_str}

Assembly:
{asm_code}

For each symbol, identify:
1. All memory offset accesses (from lw/lh/lb/sw/sh/sb instructions)
2. Access size determines field type:
   - lb/sb (signed byte) → s8
   - lbu (unsigned byte) → u8
   - lh/sh (signed half) → s16
   - lhu (unsigned half) → u16
   - lw/sw (word) → s32/u32/pointer

3. Generate typedef with fields at detected offsets

Example: If you see:
  lui $v0, %hi(D_800F83D0)
  lhu $v1, %lo(D_800F83D0)($v0)    # offset 0x0, unsigned half
  lhu $a0, 0x2($v0)                # offset 0x2, unsigned half

Generate:
  typedef struct {{
      u16 unk0;  // offset 0x0
      u16 unk2;  // offset 0x2
  }} Unk800F83D0;

Format response as JSON:
{{
  "symbol_name": {{
    "typedef": "typedef struct {{ ... }} TypeName;",
    "fields": {{
      "0x0": ["unk0", "u16"],
      "0x2": ["unk2", "u16"]
    }},
    "confidence": 0.8,
    "notes": "brief reason for confidence level"
  }}
}}

Provide ONLY valid JSON, no markdown or explanation.
"""
        
        if self.verbose:
            print(f"Inferring types for {function_name} ({len(extern_symbols)} symbols)...")
        
        response = self._call_ollama(prompt, temperature=0.1, max_tokens=2048, timeout=90)
        
        if not response:
            return None
        
        try:
            # Try to extract JSON from response (may have markdown wrapping)
            json_match = re.search(r'```json\s*(\{.+?\})\s*```', response, re.DOTALL)
            if json_match:
                response = json_match.group(1)
            elif '```' in response:
                # Try without json tag
                json_match = re.search(r'```\s*(\{.+?\})\s*```', response, re.DOTALL)
                if json_match:
                    response = json_match.group(1)
            
            # Parse JSON
            inferred = json.loads(response)
            
            if self.verbose:
                for symbol, info in inferred.items():
                    print(f"  {symbol}: confidence={info.get('confidence', 0.0):.2f}, fields={len(info.get('fields', {}))}")
            
            return inferred
            
        except json.JSONDecodeError as e:
            if self.verbose:
                print(f"Failed to parse LLM response as JSON: {e}")
                print(f"Response was: {response[:200]}...")
            return None
    
    def generate_type_header(self, inferred_types: Dict, output_path: Optional[Path] = None) -> str:
        """
        Generate a C header file from inferred types.
        
        Args:
            inferred_types: Dict from infer_types_from_assembly
            output_path: Optional path to write header file
            
        Returns:
            Header file content as string
        """
        header = """/* Auto-generated type definitions */
#ifndef AUTO_TYPES_H
#define AUTO_TYPES_H

#include "common.h"

"""
        
        for symbol, type_info in inferred_types.items():
            typedef = type_info.get('typedef', '')
            confidence = type_info.get('confidence', 0.0)
            notes = type_info.get('notes', '')
            
            header += f"/* {symbol} - confidence: {confidence:.2f} */\n"
            if notes:
                header += f"/* {notes} */\n"
            header += f"{typedef}\n\n"
            
            # Also add extern declaration
            # Extract type name from typedef
            type_match = re.search(r'\}\s*(\w+);', typedef)
            if type_match:
                type_name = type_match.group(1)
                header += f"extern {type_name} {symbol};\n\n"
        
        header += "#endif /* AUTO_TYPES_H */\n"
        
        if output_path:
            output_path.write_text(header)
            if self.verbose:
                print(f"Wrote type header to {output_path}")
        
        return header
    
    def explain_assembly_section(self, asm_code: str) -> Optional[str]:
        """Explain what a section of assembly does."""
        if not self.available:
            return None
        
        prompt = f"""Explain this MIPS R3000 assembly code in plain English:

{asm_code}

Provide a clear, concise explanation of what this code does.
"""
        
        return self._call_ollama(prompt, temperature=0.2)
    
    def suggest_refinement(self, function_name: str, current_c_code: str, error_message: Optional[str] = None) -> Optional[Dict]:
        """
        Suggest refinements for C code that doesn't match.
        
        Args:
            function_name: Function name
            current_c_code: Current C implementation
            error_message: Optional error/mismatch message
        
        Returns dict with:
            - suggestions: List of suggested changes
            - revised_code: Optional revised C code
        """
        if not self.available:
            return None
        
        asm_code = self.get_assembly_code(function_name)
        if not asm_code:
            return None
        
        prompt = f"""{FF7_CONTEXT}
Fix this decompiled function to match the original binary exactly.

Function: {function_name}

Original Assembly:
{asm_code}

Current C Code (does not match):
{current_c_code}
"""
        
        if error_message:
            prompt += f"\nError/Mismatch info:\n{error_message}\n"
        
        prompt += """
Matching workflow — work through these in order:
1. **Size**: Count instructions in assembly vs compiled output. If size is wrong, fix structure first.
2. **Types**: `s8`→`lb`, `u8`→`lbu`, `s16`→`lh`, `u16`→`lhu` — change types to match load/store instructions.
3. **Merged calls**: If a call appears once after if/else in the C but twice in asm, duplicate it inside each branch.
4. **Struct vs locals**: Replace isolated `s16 a, b;` stack vars with a proper `RECT` or similar struct so the compiler stores all fields.
5. **goto patterns**: `goto loop_XX` → `while` loop; `goto block_XX` in switch → inline branch.
6. **Redundant instructions**: Original may have duplicate `sll`/`sra` pairs in both branches — reproduce the duplication.

Provide:
SUGGESTIONS:
- <list specific changes>

REVISED_CODE:
<corrected C code, PSX SDK types only>
"""
        
        if self.verbose:
            print(f"Getting refinement suggestions for {function_name}...")
        
        response = self._call_ollama(prompt, temperature=0.3)
        
        if not response:
            return None
        
        result = {
            'function': function_name,
            'raw_response': response,
            'suggestions': [],
            'revised_code': None
        }
        
        # Extract suggestions
        if 'SUGGESTIONS:' in response:
            sugg_match = re.search(r'SUGGESTIONS:\s*(.+?)(?=\nREVISED_CODE:|$)', response, re.DOTALL)
            if sugg_match:
                suggestions_text = sugg_match.group(1).strip()
                result['suggestions'] = [s.strip('- ').strip() for s in suggestions_text.split('\n') if s.strip().startswith('-')]
        
        # Extract revised code
        if 'REVISED_CODE:' in response:
            code_match = re.search(r'REVISED_CODE:\s*(.+?)$', response, re.DOTALL)
            if code_match:
                code = code_match.group(1).strip()
                # Clean markdown if present
                code_clean = re.search(r'```c?\n(.*?)\n```', code, re.DOTALL)
                if code_clean:
                    result['revised_code'] = code_clean.group(1).strip()
                else:
                    result['revised_code'] = code
        
        return result
    
    def fix_m2c_error(self, function_name: str, broken_c_code: str, error_type: str, 
                     pre_analysis: Optional[Dict] = None) -> Optional[Dict]:
        """
        Attempt to automatically fix common m2c decompilation errors.
        
        Args:
            function_name: Function name
            broken_c_code: The function body with m2c errors
            error_type: Type of error detected (e.g., "unaligned cast", "incomplete type")
            pre_analysis: Optional pre-decompilation analysis with c_suggestion
        
        Returns dict with:
            - fixed_code: Corrected C code
            - explanation: What was fixed
            - confidence: "high", "medium", or "low"
        """
        if not self.available:
            return None
        
        # Get assembly for reference
        asm_code = self.get_assembly_code(function_name)
        
        prompt = f"""{FF7_CONTEXT}
Fix a decompilation error from m2c (MIPS → C decompiler) in a Final Fantasy VII function.

Function: {function_name}
Error Type: {error_type}

Broken C Code (from m2c):
{broken_c_code}
"""
        
        if asm_code:
            prompt += f"""
Original Assembly (authoritative — your fix must match this):
{asm_code}
"""
        
        if pre_analysis and pre_analysis.get('c_suggestion'):
            prompt += f"""
Pre-analysis C suggestion (use as structural guide):
{pre_analysis['c_suggestion']}
"""
        
        prompt += """
Common m2c errors and how to fix them:

1. `(unaligned s32)` / `(unaligned TYPE)` casts — unaligned memory access:
   Fix: `memcpy(&dst, &src, sizeof(s32));`

2. `/*?*/` unknown type markers — m2c couldn't infer the type:
   Fix: Infer from register use ($a0=first arg→likely pointer), struct offset, or context.
   Apply decompilation checklist: no `void*` for struct args, use proper PSX SDK types.

3. `void*` variable or parameter used with `->` (struct type inference failure):
   This is the most common battle-module error. m2c left a variable as `void*` because it
   couldn't determine which struct it points to.
   Fix: Look at which fields are accessed via `->` (e.g., `->unk4`, `->unk10`, `->unkC`) and
   define a local anonymous struct or use explicit byte-offset casts:
     Option A (struct): `typedef struct { s32 unk0; s32 unk4; s16 unkC; } SomeStruct;`
                        then change `void* var` → `SomeStruct* var`
     Option B (cast):   Replace `var->unk4` with `*(s32*)((u8*)var + 4)`
                        Replace `var->unkC` with `*(s16*)((u8*)var + 0xC)`
   Check the assembly: the register holding the pointer is usually $a0/$a1 for args,
   or $s0/$v0 for locals. The `lw/lh/lb` offset in the LW instruction = the field offset.

4. `Contains unknown parameter type '?'` — function call with `?` in signature:
   The extern declaration uses `?` as a placeholder because m2c saw an opaque argument.
   Fix: Look at the call in the assembly. The register(s) passed to the call determine
   the type. `$a0`=s32/ptr, `$a1`=s32, `$a2`=s32, `$a3`=s32. Change `/*?*/ void func(?)`
   to `void func(s32 arg0)` (or appropriate type). If truly unknown, use `s32`.

5. `primitive pointer used with ->unk` — e.g. `s32*` accessed as struct:
   Same as case 3 — m2c guessed `s32*` instead of `void*`. Use explicit offset casts.

6. Missing struct member / bad offset access:
   Fix: Derive field name from byte offset (e.g., offset 0x10 → `unk10`), or cast: `*(s16*)((u8*)ptr + 0x10)`

7. `Syntax error when parsing C context` / bad extern declarations:
   Fix: Use `void*` for truly unknown pointer params, or add proper forward declarations with known types.

8. `M2C_ERROR(...)` or decompilation failure comments — m2c gave up:
   Fix: Hand-write C from the assembly. For stale/unset registers (`$a1` never set before call), use `0`.
   E.g.: `func(ptr, 0 /* $a1 stale */);`

9. `goto loop_XX` patterns:
   Fix: Convert to `while` loop — `goto loop_4:` with label at top → `while (cond) { ... }`

10. `goto block_XX` in switch statements:
    Fix: Inline the branch body — reverse the compiler's jump-table optimization.

Output format:
CONFIDENCE: high/medium/low
EXPLANATION: <what you fixed>
FIXED_CODE:
<corrected C function, PSX SDK types only, no markdown>
"""
        
        if self.verbose:
            print(f"Attempting to fix m2c error in {function_name}...")
        
        response = self._call_ollama(prompt, temperature=0.2, max_tokens=1024)
        
        if not response:
            return None
        
        result = {
            'function': function_name,
            'error_type': error_type,
            'raw_response': response,
            'fixed_code': None,
            'explanation': None,
            'confidence': 'low'
        }
        
        # Extract confidence
        conf_match = re.search(r'CONFIDENCE:\s*(high|medium|low)', response, re.IGNORECASE)
        if conf_match:
            result['confidence'] = conf_match.group(1).lower()
        
        # Extract explanation
        exp_match = re.search(r'EXPLANATION:\s*(.+?)(?=\nFIXED_CODE:|$)', response, re.DOTALL)
        if exp_match:
            result['explanation'] = exp_match.group(1).strip()
        
        # Extract fixed code
        if 'FIXED_CODE:' in response:
            code_match = re.search(r'FIXED_CODE:\s*(.+?)$', response, re.DOTALL)
            if code_match:
                code = code_match.group(1).strip()
                # Clean markdown if present
                code_clean = re.search(r'```c?\n(.*?)\n```', code, re.DOTALL)
                if code_clean:
                    result['fixed_code'] = code_clean.group(1).strip()
                else:
                    result['fixed_code'] = code.strip()
        
        return result
    
    def batch_analyze(self, function_names: List[str]) -> Dict[str, Dict]:
        """Analyze multiple functions in batch."""
        results = {}
        
        for func_name in function_names:
            if self.verbose:
                print(f"Analyzing {func_name}...")
            
            analysis = self.analyze_assembly(func_name)
            if analysis:
                results[func_name] = analysis
        
        return results


def main():
    parser = argparse.ArgumentParser(description="LLM-assisted decompilation helper")
    
    parser.add_argument('--analyze-asm', type=str, metavar='FUNCTION',
                       help='Analyze assembly for a function')
    parser.add_argument('--suggest-c', type=str, metavar='FUNCTION',
                       help='Suggest C code for a function')
    parser.add_argument('--explain', type=str, metavar='ASM_FILE',
                       help='Explain assembly in a file')
    parser.add_argument('--refine', type=str, metavar='FUNCTION',
                       help='Suggest refinements for a function')
    parser.add_argument('--batch', nargs='+', metavar='FUNCTION',
                       help='Analyze multiple functions')
    parser.add_argument('--model', type=str, default=OLLAMA_MODEL,
                       help=f'Ollama model to use (default: {OLLAMA_MODEL})')
    parser.add_argument('--check', action='store_true',
                       help='Check if LLM is available')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output')
    
    args = parser.parse_args()
    
    helper = LLMHelper(model=args.model, verbose=args.verbose)
    
    if args.check:
        if helper.available:
            print(f"✅ LLM available: {args.model}")
            print(f"Endpoint: {OLLAMA_ENDPOINT}")
        else:
            print(f"❌ LLM not available")
            print("Make sure Ollama is running: ollama serve")
            print(f"And model is installed: ollama pull {args.model}")
        return
    
    if not helper.available:
        print("❌ LLM not available. Run with --check for details.")
        return
    
    if args.analyze_asm:
        print(f"\n{'='*60}")
        print(f"ANALYZING: {args.analyze_asm}")
        print(f"{'='*60}\n")
        
        result = helper.analyze_assembly(args.analyze_asm)
        
        if result:
            print(f"Summary: {result['summary']}")
            print(f"Complexity: {result['complexity']}")
            if result['patterns']:
                print(f"Patterns: {', '.join(result['patterns'])}")
            if result['c_suggestion']:
                print(f"\nSuggested C Structure:\n{result['c_suggestion']}")
        else:
            print("Could not analyze function")
    
    elif args.suggest_c:
        print(f"\n{'='*60}")
        print(f"C CODE SUGGESTION: {args.suggest_c}")
        print(f"{'='*60}\n")
        
        suggestion = helper.suggest_c_code(args.suggest_c)
        
        if suggestion:
            print(suggestion)
        else:
            print("Could not generate suggestion")
    
    elif args.explain:
        asm_path = Path(args.explain)
        if asm_path.exists():
            asm_code = asm_path.read_text()[:2000]  # Limit size
            
            print(f"\n{'='*60}")
            print(f"EXPLAINING: {args.explain}")
            print(f"{'='*60}\n")
            
            explanation = helper.explain_assembly_section(asm_code)
            if explanation:
                print(explanation)
            else:
                print("Could not explain assembly")
        else:
            print(f"File not found: {args.explain}")
    
    elif args.refine:
        print(f"\n{'='*60}")
        print(f"REFINEMENT SUGGESTIONS: {args.refine}")
        print(f"{'='*60}\n")
        
        # Would need to get current C code from the source file
        print("Note: Refinement mode requires current C code as input")
        print("This feature is best used programmatically from other scripts")
    
    elif args.batch:
        print(f"\n{'='*60}")
        print(f"BATCH ANALYSIS: {len(args.batch)} functions")
        print(f"{'='*60}\n")
        
        results = helper.batch_analyze(args.batch)
        
        for func_name, result in results.items():
            print(f"\n{func_name}:")
            print(f"  Complexity: {result['complexity']}")
            print(f"  Summary: {result['summary']}")
    
    else:
        parser.print_help()
        print("\nQuick examples:")
        print("  python llm_helper.py --check")
        print("  python llm_helper.py --analyze-asm func_800A1234")
        print("  python llm_helper.py --suggest-c AverageSZ4")
        print("  python llm_helper.py --batch func_800A1234 func_800A5678")


if __name__ == '__main__':
    main()
