// Package saveicons handles the memory card icons embedded in SAVEMENU.MNU.
//
// Each icon lives in a slot of slotSize bytes: a 16x16 4bpp TIM, one unknown
// byte, then zero padding. The code references the icons through a label that
// points at the CLUT of the first TIM rather than at its header.
package saveicons

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/xeeynamo/ff7-decomp/tools/builder/assets"
)

const (
	slotSize   = 0x3F6
	timSize    = 0xC0
	clutOffset = 0x14
	clutSize   = 0x20
	imgOffset  = 0x40
	label      = "g_SaveIcons"
)

// timHeader holds the bytes shared by all icons that sit before the CLUT colors.
var timHeader = []byte{
	0x10, 0x00, 0x00, 0x00, // TIM magic
	0x08, 0x00, 0x00, 0x00, // 4bpp with CLUT
	0x2C, 0x00, 0x00, 0x00, // CLUT block size
	0x00, 0x01, 0xE0, 0x01, // CLUT position (0x100, 0x1E0)
	0x10, 0x00, 0x01, 0x00, // CLUT size (16, 1)
}

// imgHeader holds the bytes shared by all icons between the CLUT and the pixels.
var imgHeader = []byte{
	0x8C, 0x00, 0x00, 0x00, // image block size
	0xC0, 0x03, 0x00, 0x01, // image position (0x3C0, 0x100)
	0x04, 0x00, 0x10, 0x00, // image size in 16-bit units (4, 16)
}

type SaveIcons struct{}

func count(m assets.Metadata) (int, error) {
	if len(m.Args) < 1 {
		return 0, fmt.Errorf("missing icon count")
	}
	switch n := m.Args[0].(type) {
	case int:
		return n, nil
	case int64:
		return int(n), nil
	case uint64:
		return int(n), nil
	}
	return 0, fmt.Errorf("icon count %v is not an integer", m.Args[0])
}

func timPath(m assets.Metadata, i int) string {
	return filepath.Join(m.AssetDir, m.Name, fmt.Sprintf("%02d.tim", i))
}

func asmPath(m assets.Metadata) string {
	return filepath.Join(m.AsmDataDir, m.Symbol+".s")
}

func isZero(data []byte) bool {
	for _, b := range data {
		if b != 0 {
			return false
		}
	}
	return true
}

func (SaveIcons) Extract(m assets.Metadata) error {
	n, err := count(m)
	if err != nil {
		return err
	}
	data := m.Data[m.Start:m.End]
	if len(data) < n*slotSize {
		return fmt.Errorf("0x%X bytes cannot hold %d icons of 0x%X bytes", len(data), n, slotSize)
	}
	tail := data[n*slotSize:]
	if !isZero(tail) {
		return fmt.Errorf("unexpected non-zero data after the last icon")
	}
	if err := os.MkdirAll(filepath.Join(m.AssetDir, m.Name), 0755); err != nil {
		return err
	}

	var sb strings.Builder
	sb.WriteString(".include \"macro.inc\"\n\n.section .data, \"wa\"\n\n")
	for i := 0; i < n; i++ {
		slot := data[i*slotSize : (i+1)*slotSize]
		tim := slot[:timSize]
		if !bytes.Equal(tim[:clutOffset], timHeader) || !bytes.Equal(tim[clutOffset+clutSize:imgOffset], imgHeader) {
			return fmt.Errorf("icon %d has an unexpected TIM header", i)
		}
		if !isZero(slot[timSize+1:]) {
			return fmt.Errorf("icon %d has non-zero padding", i)
		}

		path := timPath(m, i)
		if err := os.WriteFile(path, tim, 0644); err != nil {
			return err
		}
		if i == 0 {
			fmt.Fprintf(&sb, ".incbin \"%s\", 0, 0x%X\nglabel %s\n.incbin \"%s\", 0x%X\n", path, clutOffset, label, path, clutOffset)
		} else {
			fmt.Fprintf(&sb, ".incbin \"%s\"\n", path)
		}
		fmt.Fprintf(&sb, ".byte 0x%02X\n.space 0x%X\n", slot[timSize], slotSize-timSize-1)
	}
	if len(tail) > 0 {
		fmt.Fprintf(&sb, ".space 0x%X\n", len(tail))
	}

	if err := os.MkdirAll(m.AsmDataDir, 0755); err != nil {
		return err
	}
	return os.WriteFile(asmPath(m), []byte(sb.String()), 0644)
}

func (SaveIcons) Build(m assets.Metadata) []assets.BuildStep {
	return nil
}

func (SaveIcons) Timestamp(m assets.Metadata) time.Time {
	n, err := count(m)
	if err != nil {
		return time.Time{}
	}
	paths := []string{asmPath(m)}
	for i := 0; i < n; i++ {
		paths = append(paths, timPath(m, i))
	}
	var ts time.Time
	for _, path := range paths {
		info, err := os.Stat(path)
		if err != nil {
			return time.Time{}
		}
		if ts.IsZero() || info.ModTime().Before(ts) {
			ts = info.ModTime()
		}
	}
	return ts
}

func (SaveIcons) SplatEntry(m assets.Metadata) map[string]any {
	return map[string]any{
		"start":   m.Start,
		"type":    "databin",
		"kind":    "saveicons",
		"name":    m.Symbol,
		"extract": false,
	}
}
