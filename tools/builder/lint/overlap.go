package lint

import (
	"sort"
)

// Symbol is one resolved, sized symbol inside an overlay's address space.
type Symbol struct {
	Name       string
	Addr       uint32
	Size       uint32
	LowerBound bool
	Decls      []Decl
}

// Finding is a pair of symbols whose byte ranges intersect.
type Finding struct {
	A, B Symbol
}

// mergeSymbols combines the per-TU symbol lists of one overlay into one list, keyed
// by name. The same name from several translation units is merged: its address must
// agree (kept as-is otherwise, first-seen wins) and its size is the largest complete
// size seen, falling back to the largest lower bound when no complete size exists.
func mergeSymbols(perTU ...[]Symbol) []Symbol {
	order := make([]string, 0)
	byName := map[string]*Symbol{}
	for _, syms := range perTU {
		for _, s := range syms {
			existing, ok := byName[s.Name]
			if !ok {
				sc := s
				byName[s.Name] = &sc
				order = append(order, s.Name)
				continue
			}
			existing.Decls = append(existing.Decls, s.Decls...)
			betterSize := (existing.LowerBound && !s.LowerBound) ||
				(existing.LowerBound == s.LowerBound && s.Size > existing.Size)
			if betterSize {
				existing.Size = s.Size
				existing.LowerBound = s.LowerBound
			}
		}
	}
	merged := make([]Symbol, 0, len(order))
	for _, name := range order {
		merged = append(merged, *byName[name])
	}
	return merged
}

// findOverlaps sweeps symbols sorted by address and reports every pair of distinct
// names whose half-open byte ranges [Addr, Addr+Size) intersect. Zero-size symbols
// never match anything.
func findOverlaps(syms []Symbol) []Finding {
	active := make([]Symbol, 0, len(syms))
	sorted := make([]Symbol, len(syms))
	copy(sorted, syms)
	sort.Slice(sorted, func(i, j int) bool { return sorted[i].Addr < sorted[j].Addr })

	var findings []Finding
	for _, s := range sorted {
		if s.Size == 0 {
			continue
		}
		kept := active[:0]
		for _, a := range active {
			if a.Addr+a.Size > s.Addr {
				kept = append(kept, a)
			}
		}
		active = kept

		for _, a := range active {
			if a.Name == s.Name {
				continue
			}
			findings = append(findings, Finding{A: a, B: s})
		}
		active = append(active, s)
	}
	return findings
}
