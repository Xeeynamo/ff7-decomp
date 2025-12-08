package symbols

import (
	"fmt"
	"os"
)

func Add(path string, symbol string, offset uint32, size int) error {
	f, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE, 0644)
	if err != nil {
		return err
	}
	defer f.Close()
	line := fmt.Sprintf("%s = %08X;", symbol, offset)
	if size > 0 {
		line += fmt.Sprintf("// size:0x%X", size)
	}
	if _, err := f.WriteString(line + "\n"); err != nil {
		return err
	}
	_ = f.Close()
	return Sort(path)
}
