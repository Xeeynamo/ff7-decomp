package lint

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func writeSymFile(t *testing.T, dir, name, body string) string {
	t.Helper()
	path := filepath.Join(dir, name)
	if err := os.WriteFile(path, []byte(body), 0644); err != nil {
		t.Fatal(err)
	}
	return path
}

func TestLoadSymbolTableUnionMergesOverlays(t *testing.T) {
	dir := t.TempDir()
	mainSyms := writeSymFile(t, dir, "main.txt", "Savemap = 0x8009C6E4; // size:0x10F4\n")
	itemSyms := writeSymFile(t, dir, "itemmenu.txt", "g_MateriaPriority = 0x801D3834; // size:0x5C\n")

	overlays := []Overlay{
		{Name: "main", SymbolAddrsPath: []string{mainSyms}},
		// itemmenu lists main's file too, which must not trip the conflict check.
		{Name: "itemmenu", SymbolAddrsPath: []string{itemSyms, mainSyms}},
	}
	table, err := LoadSymbolTableUnion(overlays)
	if err != nil {
		t.Fatal(err)
	}
	if got, ok := table["Savemap"]; !ok || got != 0x8009C6E4 {
		t.Errorf("Savemap = 0x%08X (ok=%v), want 0x8009C6E4", got, ok)
	}
	if got, ok := table["g_MateriaPriority"]; !ok || got != 0x801D3834 {
		t.Errorf("g_MateriaPriority = 0x%08X (ok=%v), want 0x801D3834", got, ok)
	}
}

func TestLoadSymbolTableUnionRejectsConflict(t *testing.T) {
	dir := t.TempDir()
	a := writeSymFile(t, dir, "a.txt", "Savemap = 0x8009C6E4;\n")
	b := writeSymFile(t, dir, "b.txt", "Savemap = 0x8009C6D8;\n")

	overlays := []Overlay{
		{Name: "main", SymbolAddrsPath: []string{a}},
		{Name: "world", SymbolAddrsPath: []string{b}},
	}
	_, err := LoadSymbolTableUnion(overlays)
	if err == nil {
		t.Fatal("expected an error for one name at two addresses")
	}
	if !strings.Contains(err.Error(), "Savemap") {
		t.Errorf("error should name the symbol, got %v", err)
	}
}
