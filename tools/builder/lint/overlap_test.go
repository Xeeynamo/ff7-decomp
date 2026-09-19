package lint

import "testing"

func sym(name string, addr, size uint32) Symbol {
	return Symbol{Name: name, Addr: addr, Size: size, Decls: []Decl{{Name: name, File: "test.c", Line: 1}}}
}

func TestFindOverlapsAdjacentRangesDoNotOverlap(t *testing.T) {
	// D_8016360C (0xAC bytes) ends exactly where D_801636B8 (0xA0 bytes) begins.
	syms := []Symbol{
		sym("D_8016360C", 0x8016360C, 0xAC),
		sym("D_801636B8", 0x801636B8, 0xA0),
	}
	if got := findOverlaps(syms); len(got) != 0 {
		t.Fatalf("expected no overlaps for adjacent ranges, got %v", got)
	}
}

func TestFindOverlapsTwoByteOffset(t *testing.T) {
	// D_800F6934[0x40][8] and D_800F6936[0x40][8], two bytes apart, both 0x200 bytes.
	syms := []Symbol{
		sym("D_800F6934", 0x800F6934, 0x200),
		sym("D_800F6936", 0x800F6936, 0x200),
	}
	got := findOverlaps(syms)
	if len(got) != 1 {
		t.Fatalf("expected exactly one overlap, got %v", got)
	}
	if got[0].A.Name != "D_800F6934" || got[0].B.Name != "D_800F6936" {
		t.Fatalf("unexpected pair: %+v", got[0])
	}
}

func TestFindOverlapsSameAddressAlias(t *testing.T) {
	syms := []Symbol{
		sym("D_800F83AC", 0x800F83AC, 0x444),
		sym("g_BattleState", 0x800F83AC, 0x444),
	}
	got := findOverlaps(syms)
	if len(got) != 1 {
		t.Fatalf("expected exactly one overlap for a same-address alias, got %v", got)
	}
}

func TestFindOverlapsZeroSizeNeverMatches(t *testing.T) {
	syms := []Symbol{
		sym("D_80000000", 0x80000000, 0),
		sym("D_80000000_other", 0x80000000, 0x10),
	}
	if got := findOverlaps(syms); len(got) != 0 {
		t.Fatalf("expected zero-size symbols to never match, got %v", got)
	}
}

func TestFindOverlapsSkipsSameName(t *testing.T) {
	// The same declaration appearing twice at the same address (merged upstream, but
	// exercised here directly) must never be reported against itself.
	syms := []Symbol{
		sym("D_80000000", 0x80000000, 0x10),
		sym("D_80000000", 0x80000000, 0x10),
	}
	if got := findOverlaps(syms); len(got) != 0 {
		t.Fatalf("expected no self-overlap, got %v", got)
	}
}

func TestMergeSymbolsPrefersCompleteSizeOverLowerBound(t *testing.T) {
	perTU1 := []Symbol{{Name: "D_80075D08", Addr: 0x80075D08, Size: 4, LowerBound: true}}
	perTU2 := []Symbol{{Name: "D_80075D08", Addr: 0x80075D08, Size: 0x40, LowerBound: false}}

	merged := mergeSymbols(perTU1, perTU2)
	if len(merged) != 1 {
		t.Fatalf("expected one merged symbol, got %d", len(merged))
	}
	if merged[0].Size != 0x40 || merged[0].LowerBound {
		t.Fatalf("expected the complete size to win, got %+v", merged[0])
	}
}

func TestMergeSymbolsKeepsLargestLowerBoundWhenNoCompleteSize(t *testing.T) {
	perTU1 := []Symbol{{Name: "D_80075D08", Addr: 0x80075D08, Size: 4, LowerBound: true}}
	perTU2 := []Symbol{{Name: "D_80075D08", Addr: 0x80075D08, Size: 8, LowerBound: true}}

	merged := mergeSymbols(perTU1, perTU2)
	if len(merged) != 1 {
		t.Fatalf("expected one merged symbol, got %d", len(merged))
	}
	if merged[0].Size != 8 || !merged[0].LowerBound {
		t.Fatalf("expected the largest lower bound to win, got %+v", merged[0])
	}
}
