# Quick Start: Type Learning System

## First Run

The type learning system works automatically. Just run the agent as usual:

```bash
cd automation

# Standard run with type learning enabled
python3 agent.py --run --llm --batch 20 --verbose

# Long run (8 hours)
python3 agent.py --run --llm --duration 28800 --batch 64 --parallel 8
```

## What's Different?

### Before
```
[2026-03-29 10:00:00] Processing func_800A1234...
[2026-03-29 10:00:05] ❌ PSY-Q compilation failed: void* pointer access
[2026-03-29 10:00:05] Reverting to INCLUDE_ASM
```

### After
```
[2026-03-29 10:00:00] Processing func_800A1234...
[2026-03-29 10:00:01] 🔍 Found 3 extern symbols, inferring types...
[2026-03-29 10:00:08] 🤖 Inferring types for 2 unknown symbols...
[2026-03-29 10:00:15] ✅ Generated type definitions in include/auto_types.h
[2026-03-29 10:00:20] ✅ Decompilation successful!
[2026-03-29 10:00:20] 📈 Increased confidence for D_800F83D0 pattern

📚 Struct learning database: 12 known types
```

## Checking Progress

### View Learned Types

```bash
python3 -c "
from database import DecompDatabase
db = DecompDatabase()

# Stats
stats = db.get_struct_learning_stats()
print(f'📚 Struct Learning Stats:')
print(f'  Total patterns: {stats[\"total_patterns\"]}')
print(f'  High confidence (≥70%%): {stats[\"high_confidence\"]}')
print(f'  Medium confidence (30-70%%): {stats[\"medium_confidence\"]}')
print(f'  Low confidence (<30%%): {stats[\"low_confidence\"]}')
print()

# List known types
print('Known Types:')
known = db.get_all_known_types(min_confidence=0.3)
for symbol, info in sorted(known.items(), key=lambda x: -x[1]['confidence'])[:10]:
    print(f'  {symbol:20} → {info[\"likely_type\"]:20} (confidence: {info[\"confidence\"]:.2f}, seen: {info[\"seen_count\"]}x)')
"
```

### Check Which Functions Use a Type

```bash
python3 -c "
from database import DecompDatabase
db = DecompDatabase()

symbol = 'D_800F83D0'  # Change this
funcs = db.get_functions_using_symbol(symbol)
print(f'Functions using {symbol}:')
for func in funcs:
    print(f'  - {func}')
"
```

## Database Migration

The new tables are created automatically on first run. No manual steps needed.

If you want to reset the learning database:

```bash
# Backup first
cp automation/functions.db automation/functions.db.backup

# Reset struct learning (keeps function data)
python3 -c "
from database import DecompDatabase
db = DecompDatabase()
db.conn.execute('DELETE FROM struct_patterns')
db.conn.execute('DELETE FROM symbol_usage')
db.conn.commit()
print('✅ Struct learning reset')
"
```

## Monitoring During Runs

Watch the logs for:

### Good Signs ✅
```
✅ All extern symbols have known types
📈 Increased confidence for D_800F83D0 pattern
types=3/3 ✅
```

### Needs Attention ⚠️
```
⚠️ Type inference failed: timeout
types=0/5 ⚠️
```

### Expected First Few Functions
```
🤖 Inferring types for 12 unknown symbols...  ← Normal, learning phase
```

### After 50+ Functions
```
✅ All extern symbols have known types  ← Database is learning!
```

## Performance Notes

### Initial Slowdown
- First 20-30 functions: +10 sec each (LLM type inference)
- After that: Most types known → no inference needed

### Long-term Speedup
- Fewer reversions = more successful decomps
- More successful decomps = less wasted work
- Net: ~30% faster once types are learned

## Troubleshooting

### "LLM not available"

Make sure Ollama is running:
```bash
ollama serve &
ollama list  # Should show qwen2.5-coder:7b
```

### "Type inference timeout"

Increase timeout in `llm_helper.py::infer_types_from_assembly()`:
```python
response = self._call_ollama(prompt, temperature=0.1, max_tokens=2048, timeout=120)  # Was 90
```

### Wrong Types Learned

Remove bad entries:
```bash
python3 -c "
from database import DecompDatabase
db = DecompDatabase()
db.conn.execute('DELETE FROM struct_patterns WHERE symbol_name = ?', ('D_BADTYPE',))
db.conn.commit()
"
```

### No Auto-Generated Header

Check if type inference is enabled:
```bash
# Should see LLM initialization
python3 agent.py --run --llm --verbose 2>&1 | grep -i llm
```

## Best Practices

1. **Start with verbose:** First few runs should use `--verbose` to see learning in action
2. **Check stats periodically:** Run the stats check every 50 functions
3. **Review inferred types:** Check `include/auto_types.h` occasionally, move confirmed types to proper headers
4. **Batch by module:** Run `--module battle` first to learn battle types, then move to other modules

## Example Session

```bash
# Day 1: Bootstrap battle module
python3 agent.py --run --llm --module battle --batch 30 --verbose

# Check what was learned
python3 -c "from database import DecompDatabase; db = DecompDatabase(); print(db.get_struct_learning_stats())"

# Day 2: Use learned types for world module
python3 agent.py --run --llm --module world --batch 50

# Day 3: Full run with accumulated knowledge
python3 agent.py --run --llm --batch 100 --parallel 8 --duration 28800
```

## Next Steps

1. Run with `--llm --verbose` to see the system in action
2. After 50 functions, check stats to see learned types
3. Review `include/auto_types.h` and promote good types to proper headers
4. Continue running - the system gets smarter over time!

For detailed information, see [TYPE_LEARNING.md](TYPE_LEARNING.md)
