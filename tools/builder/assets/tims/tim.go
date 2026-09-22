package tims

import (
	"fmt"
	"os"
	"path/filepath"
	"time"

	"github.com/xeeynamo/ff7-decomp/tools/builder/assets"
)

const magic = 0x10

type Tim struct{}

func assetPath(m assets.Metadata) string {
	return filepath.Join(m.AssetDir, m.Name+".tim")
}

func asmPath(m assets.Metadata) string {
	return filepath.Join(m.AsmDataDir, m.Symbol+".s")
}

func (Tim) Extract(m assets.Metadata) error {
	data := m.Data[m.Start:m.End]
	if len(data) < 4 || data[0] != magic {
		fmt.Printf("warning: tim %q at 0x%X does not start with TIM magic 0x%02X\n", m.Name, m.Start, magic)
	}
	if err := os.MkdirAll(m.AssetDir, 0755); err != nil {
		return err
	}
	if err := os.WriteFile(assetPath(m), data, 0644); err != nil {
		return err
	}
	if err := os.MkdirAll(m.AsmDataDir, 0755); err != nil {
		return err
	}
	contents := fmt.Sprintf(".include \"macro.inc\"\n\n.section .data, \"wa\"\n\nglabel %s\n.incbin \"%s\"\n", m.Symbol, assetPath(m))
	return os.WriteFile(asmPath(m), []byte(contents), 0644)
}

func (Tim) Timestamp(m assets.Metadata) time.Time {
	assetInfo, err := os.Stat(assetPath(m))
	if err != nil {
		return time.Time{}
	}
	asmInfo, err := os.Stat(asmPath(m))
	if err != nil {
		return time.Time{}
	}
	ts := assetInfo.ModTime()
	if asmInfo.ModTime().Before(ts) {
		ts = asmInfo.ModTime()
	}
	return ts
}

func (Tim) Build(m assets.Metadata) []assets.BuildStep {
	return nil
}

func (Tim) SplatEntry(m assets.Metadata) map[string]any {
	return map[string]any{
		"start":   m.Start,
		"type":    "databin",
		"kind":    "tim",
		"name":    m.Symbol,
		"extract": false,
	}
}
