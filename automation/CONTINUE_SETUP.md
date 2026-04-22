# Continue + Qwen Setup Guide

## ✅ Setup Complete!

Your system is configured to use **Continue** with **Qwen2.5-Coder 7B** locally via Ollama.

## What's Installed

- ✅ **Ollama** v0.17.4 (`/usr/local/bin/ollama`)
- ✅ **qwen2.5-coder:7b** model (4.7 GB)
- ✅ **Continue config** at `~/.continue/config.json`

## Quick Start

### 1. Install Continue Extension (If Not Already)

If Continue isn't installed in VS Code:
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "Continue"
4. Install "Continue - Codestral, Claude, and more"
5. Reload VS Code

### 2. Start Ollama Service

Ollama needs to be running in the background:

```bash
# Start ollama service (if not already running)
ollama serve
```

Or better yet, run it in the background:

```bash
# Check if ollama is running
pgrep ollama || ollama serve &
```

### 3. Using Continue

Once Continue is installed and Ollama is running:

**Open Continue Panel:**
- Click the Continue icon in the left sidebar (or Ctrl+L)

**Chat with Qwen:**
- Type questions in the Continue chat
- Select code and ask questions about it
- Use @ to reference files/symbols

**Custom Commands (for decompilation):**

We've added special commands for your decompilation workflow:

1. **`/explain-asm`** - Analyze MIPS assembly
   - Select assembly code
   - Type `/explain-asm`
   - Get detailed explanation

2. **`/decompile-hint`** - Get decompilation suggestions
   - Select assembly function
   - Type `/decompile-hint`
   - Get C equivalent suggestions

3. **`/review-decomp`** - Review decompiled C code
   - Select your C code
   - Type `/review-decomp`
   - Get accuracy check

**Code Completion:**
- Just start typing - Qwen will suggest completions
- Press Tab to accept

## Example Workflow

### Decompile a Function with AI Help

1. **Find a function to decompile:**
   ```bash
   cd ~/GitHub/ff7-decomp-armstrca/automation
   python3 discover.py --list-todo | head -10
   ```

2. **Get the assembly:**
   Look at the corresponding .s file in `asm/us/`

3. **Ask Qwen for help:**
   - Open the .s file in VS Code
   - Select the assembly
   - Open Continue (Ctrl+L)
   - Type: `/decompile-hint`

4. **Implement the C code:**
   - Use Qwen's suggestions
   - Write C in the appropriate src/ file

5. **Run decompilation:**
   ```bash
   python3 decompile.py --function func_name
   ```

6. **Verify it matches:**
   ```bash
   make build
   # Check for matching
   ```

## Configuration Details

Your Continue config (`~/.continue/config.json`) includes:

- **Main model**: Qwen2.5-Coder 7B (32K context)
- **Autocomplete**: Optimized for fast suggestions
- **Temperature**: 0.2 (balanced creativity/accuracy)
- **Custom commands**: Decompilation-specific prompts

## Performance Expectations

With your RTX 2060 (6GB VRAM) and i5-9300H:

- **Chat responses**: ~20-40 tokens/sec
- **Autocomplete**: Nearly instant (<100ms)
- **Context window**: 32K tokens (~24K lines of code)
- **Model size**: 4.7 GB (fits in VRAM + RAM)

## Troubleshooting

### Ollama not responding:
```bash
# Restart ollama service
pkill ollama
ollama serve &
```

### Model not loading:
```bash
# Verify model is installed
ollama list

# Pull again if needed
ollama pull qwen2.5-coder:7b
```

### Continue can't connect:
- Check Ollama is running: `pgrep ollama`
- Verify port 11434 is open: `curl http://localhost:11434`
- Restart VS Code

### Slow responses:
- Close other applications to free RAM
- Use smaller batch sizes in automation
- Consider running only Ollama during heavy AI use

## Advanced: Running Ollama as a Service

To auto-start Ollama on boot:

```bash
# Create systemd service
sudo tee /etc/systemd/system/ollama.service > /dev/null <<EOF
[Unit]
Description=Ollama Service
After=network.target

[Service]
ExecStart=/usr/local/bin/ollama serve
User=$USER
Restart=always

[Install]
WantedBy=multi-user.target
EOF

# Enable and start
sudo systemctl enable ollama
sudo systemctl start ollama

# Check status
sudo systemctl status ollama
```

## Useful Commands

```bash
# List all models
ollama list

# Remove a model
ollama rm model-name

# Pull additional models (if you want variety)
ollama pull codellama:7b
ollama pull deepseek-coder:6.7b

# Check Ollama logs
journalctl -u ollama -f

# Test model directly
ollama run qwen2.5-coder:7b "Explain MIPS assembly: lui $v0, 0x8001"
```

## Integration with Automation

You can enhance the automation scripts to use Qwen:

**For Chunk 5 (LLM Integration)**, we'll add:
- Automatic assembly analysis before decompilation
- Refinement suggestions for non-matching functions
- Code quality checks on decompiled C

This will be in the next chunk!

## Resources

- [Continue Documentation](https://continue.dev/docs)
- [Ollama Models](https://ollama.ai/library)
- [Qwen2.5-Coder](https://huggingface.co/Qwen/Qwen2.5-Coder-7B-Instruct)
- [MIPS Assembly Reference](https://www.mips.com/products/architectures/mips32-2/)

---

**You're all set!** Continue is configured and ready to help with decompilation. Open VS Code and try it out!
