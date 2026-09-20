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

// savemapSymbols is the real layout: Savemap with a complete size, plus the raw
// address aliases that decompiled code declares as neighbours of it. The array
// decls only ever probe to sizeof(x[0]), so most of them are one-byte lower bounds.
func savemapSymbols() []Symbol {
	return []Symbol{
		{Name: "Savemap", Addr: 0x8009C6E4, Size: 0x10F4},
		{Name: "D_8009C75A", Addr: 0x8009C75A, Size: 2, LowerBound: true},
		{Name: "D_8009C778", Addr: 0x8009C778, Size: 1, LowerBound: true},
		{Name: "D_8009C798", Addr: 0x8009C798, Size: 1, LowerBound: true},
		{Name: "D_8009CBDC", Addr: 0x8009CBDC, Size: 1, LowerBound: true},
		{Name: "D_8009CBE0", Addr: 0x8009CBE0, Size: 2, LowerBound: true},
		{Name: "D_8009CE60", Addr: 0x8009CE60, Size: 4, LowerBound: true},
		{Name: "D_8009D268", Addr: 0x8009D268, Size: 4, LowerBound: true},
		{Name: "D_8009D2A6", Addr: 0x8009D2A6, Size: 2},
		{Name: "D_8009D58A", Addr: 0x8009D58A, Size: 1, LowerBound: true},
		{Name: "D_8009D5E8", Addr: 0x8009D5E8, Size: 1},
		{Name: "D_8009D78A", Addr: 0x8009D78A, Size: 1, LowerBound: true},
	}
}

func TestFindOverlapsCatchesEverySavemapAlias(t *testing.T) {
	syms := savemapSymbols()
	got := findOverlaps(syms)
	if len(got) != len(syms)-1 {
		t.Fatalf("expected %d overlaps against Savemap, got %d", len(syms)-1, len(got))
	}
	seen := map[string]bool{}
	for _, f := range got {
		if f.A.Name != "Savemap" {
			t.Fatalf("expected every pair to be against Savemap, got %s vs %s", f.A.Name, f.B.Name)
		}
		seen[f.B.Name] = true
	}
	for _, s := range syms[1:] {
		if !seen[s.Name] {
			t.Errorf("%s was not reported as overlapping Savemap", s.Name)
		}
	}
}

func TestSplitByRegionKeepsOverlayLocalsApart(t *testing.T) {
	const sharedEnd = 0x800A0000
	syms := []Symbol{
		{Name: "Savemap", Addr: 0x8009C6E4, Size: 0x10F4},
		{Name: "D_801D3834", Addr: 0x801D3834, Size: 0x5C},
	}
	local, shared := splitByRegion(syms, sharedEnd)
	if len(shared) != 1 || shared[0].Name != "Savemap" {
		t.Fatalf("expected only Savemap in the shared region, got %+v", shared)
	}
	if len(local) != 1 || local[0].Name != "D_801D3834" {
		t.Fatalf("expected only the overlay-local symbol, got %+v", local)
	}
}

// Two overlays that load at the same vram_start are never resident together, so
// their identical addresses must not be pooled into one comparison.
func TestSplitByRegionAliasingOverlaysNeverPool(t *testing.T) {
	const sharedEnd = 0x800A0000
	itemmenu := []Symbol{{Name: "D_801D3834", Addr: 0x801D3834, Size: 0x40}}
	bginmenu := []Symbol{{Name: "D_801D3840", Addr: 0x801D3840, Size: 0x40}}

	_, sharedA := splitByRegion(itemmenu, sharedEnd)
	_, sharedB := splitByRegion(bginmenu, sharedEnd)
	if len(sharedA) != 0 || len(sharedB) != 0 {
		t.Fatalf("overlay-local symbols must never reach the shared pool")
	}
	if got := findOverlaps(append(sharedA, sharedB...)); len(got) != 0 {
		t.Fatalf("expected no cross-overlay finding for aliasing overlays, got %v", got)
	}
}

func TestSharedRegionEndIsLowestNonMainVram(t *testing.T) {
	overlays := []Overlay{
		{Name: "main", VramStart: 0x80010000, BssSize: 0x3C930},
		{Name: "field", VramStart: 0x800A0000},
		{Name: "batres", VramStart: 0x801B0000},
		{Name: "itemmenu", VramStart: 0x801D0000},
	}
	if got := SharedRegionEnd(overlays); got != 0x800A0000 {
		t.Fatalf("expected 0x800A0000, got 0x%08X", got)
	}
}
