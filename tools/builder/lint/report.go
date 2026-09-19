package lint

import (
	"fmt"
	"io"
	"path/filepath"
	"sort"
)

// pairKey identifies a finding by its unordered pair of names, used to deduplicate
// the same overlap reported from several overlays that happen to share addresses.
func pairKey(a, b string) string {
	if a > b {
		a, b = b, a
	}
	return a + "\x00" + b
}

type reportEntry struct {
	a, b     Symbol
	overlays []string
}

// StructResolver looks up field layouts for report.go's struct-path explanations.
// One instance is built per overlay; each struct type is probed from the .c file it
// was actually seen in (StructDef.File), since the same type name can mean
// different things across overlays and not every TU in an overlay sees every type.
type StructResolver struct {
	compilerFor func(src string) string
	defs        map[string]StructDef
	cache       map[string]map[string]FieldLayout
}

func NewStructResolver(defs map[string]StructDef) *StructResolver {
	return &StructResolver{compilerFor: compilerFor, defs: defs, cache: map[string]map[string]FieldLayout{}}
}

func (r *StructResolver) fields(typeName string) map[string]FieldLayout {
	if r == nil {
		return nil
	}
	if layout, ok := r.cache[typeName]; ok {
		return layout
	}
	def, ok := r.defs[typeName]
	if !ok {
		return nil
	}
	layout, err := probeFieldOffsets(r.compilerFor(def.File), def.File, typeName, def)
	if err != nil {
		layout = map[string]FieldLayout{}
	}
	r.cache[typeName] = layout
	return layout
}

// Report renders findings (grouped and deduplicated across overlays) to w, then a
// one-line summary to stderr. It returns the number of distinct overlapping pairs.
func Report(w, errw io.Writer, findingsByOverlay map[string][]Finding, resolvers map[string]*StructResolver) int {
	byPair := map[string]*reportEntry{}
	var order []string
	for overlay, findings := range findingsByOverlay {
		for _, f := range findings {
			key := pairKey(f.A.Name, f.B.Name)
			e, ok := byPair[key]
			if !ok {
				a, b := f.A, f.B
				if a.Addr > b.Addr || (a.Addr == b.Addr && a.Name > b.Name) {
					a, b = b, a
				}
				e = &reportEntry{a: a, b: b}
				byPair[key] = e
				order = append(order, key)
			}
			e.overlays = append(e.overlays, overlay)
		}
	}
	sort.Slice(order, func(i, j int) bool {
		ei, ej := byPair[order[i]], byPair[order[j]]
		if ei.a.Addr != ej.a.Addr {
			return ei.a.Addr < ej.a.Addr
		}
		return ei.a.Name < ej.a.Name
	})

	overlayCount := map[string]bool{}

	// First pass: render each finding's cells and measure column widths, so the
	// two rows per finding and the header all line up in a single table.
	type findingRows struct {
		declA, nameA, rngA string
		declB, nameB, rngB string
		relation           string
	}
	rows := make([]findingRows, 0, len(order))
	w1, w2 := len("DECL"), len("SYMBOL")
	maxw := func(cur, s int) int {
		if s > cur {
			return s
		}
		return cur
	}
	for _, key := range order {
		e := byPair[key]
		sort.Strings(e.overlays)
		var resolver *StructResolver
		if len(e.overlays) > 0 {
			resolver = resolvers[e.overlays[0]]
		}
		for _, o := range e.overlays {
			overlayCount[o] = true
		}
		fr := findingRows{
			declA:    shortLoc(e.a),
			nameA:    e.a.Name,
			rngA:     rangePlain(e.a),
			declB:    shortLoc(e.b),
			nameB:    e.b.Name,
			rngB:     rangePlain(e.b),
			relation: relationStr(*e, resolver),
		}
		w1 = maxw(w1, maxw(len(fr.declA), len(fr.declB)))
		w2 = maxw(w2, maxw(len(fr.nameA), len(fr.nameB)))
		rows = append(rows, fr)
	}

	if len(rows) > 0 {
		fmt.Fprintf(w, "%-*s  %-*s  %s\n", w1, "DECL", w2, "SYMBOL", "RANGE")
		for i, fr := range rows {
			if i > 0 {
				fmt.Fprintln(w)
			}
			fmt.Fprintf(w, "%-*s  %-*s  %s\n", w1, fr.declA, w2, fr.nameA, fr.rngA)
			last := fmt.Sprintf("%-*s  %-*s  %s", w1, fr.declB, w2, fr.nameB, fr.rngB)
			if fr.relation != "" {
				last += "   " + fr.relation
			}
			fmt.Fprintln(w, last)
		}
	}

	fmt.Fprintf(errw, "lint: %d overlapping symbol pairs in %d overlays\n", len(order), len(overlayCount))
	return len(order)
}

// relationStr expresses B's address relative to A, as "== A + 0xN" or, when the
// delta lands inside a known struct field of A, "== A.field" / "== A[i].field".
func relationStr(e reportEntry, resolver *StructResolver) string {
	delta := e.b.Addr - e.a.Addr
	if path, ok := fieldPath(e.a, delta, resolver); ok {
		return fmt.Sprintf("== %s", path)
	}
	return fmt.Sprintf("== %s + 0x%X", e.a.Name, delta)
}

// shortLoc renders a symbol's declaration site as "file:line" using just the
// file's basename, to keep the DECL column narrow; source lines are unambiguous
// within an overlay.
func shortLoc(s Symbol) string {
	if len(s.Decls) == 0 {
		return "?"
	}
	return fmt.Sprintf("%s:%d", filepath.Base(s.Decls[0].File), s.Decls[0].Line)
}

// rangePlain renders a symbol's byte range without the [..) brackets used in the
// prose format, keeping the "(at least)" suffix for lower-bound symbols.
func rangePlain(s Symbol) string {
	suffix := ""
	if s.LowerBound {
		suffix = " (at least)"
	}
	return fmt.Sprintf("0x%08X..0x%08X%s", s.Addr, s.Addr+s.Size, suffix)
}

// fieldPath tries to express delta as a path into a's struct type, walking nested
// fields until none of them account for any more of the remaining delta:
//   - "name" when delta is 0
//   - "name.field" when a is a scalar struct
//   - "name[index].field" when a is an array of a known struct, indexed by
//     delta / elemSize
func fieldPath(a Symbol, delta uint32, resolver *StructResolver) (string, bool) {
	if delta == 0 {
		return a.Name, true
	}
	if len(a.Decls) == 0 || resolver == nil {
		return "", false
	}
	decl := a.Decls[0]
	typeName := decl.TypeName

	base := a.Name
	remaining := delta
	if len(decl.Dims) > 0 {
		elemSize := a.Size
		if n, ok := evalDim(decl.Dims[0]); ok && n > 0 {
			elemSize = a.Size / uint32(n)
		}
		if elemSize == 0 {
			return "", false
		}
		index := remaining / elemSize
		base = fmt.Sprintf("%s[%d]", a.Name, index)
		remaining %= elemSize
	}

	path, ok := matchField(typeName, remaining, resolver)
	if !ok {
		if remaining == 0 {
			return base, true
		}
		return fmt.Sprintf("%s + 0x%X", base, remaining), true
	}
	return base + path, true
}

// matchField finds the field of typeName whose byte range contains offset and
// reports it as ".field" (or ".field + 0xN" for the remainder within that field,
// e.g. into a nested anonymous member this tool does not probe separately).
func matchField(typeName string, offset uint32, resolver *StructResolver) (string, bool) {
	fields := resolver.fields(typeName)
	if len(fields) == 0 {
		return "", false
	}
	var bestName string
	var bestLayout FieldLayout
	found := false
	for name, layout := range fields {
		if offset >= layout.Offset && offset < layout.Offset+layout.Size {
			if !found || layout.Offset > bestLayout.Offset {
				bestName, bestLayout, found = name, layout, true
			}
		}
	}
	if !found {
		return "", false
	}
	inner := offset - bestLayout.Offset
	if inner == 0 {
		return "." + bestName, true
	}
	return fmt.Sprintf(".%s + 0x%X", bestName, inner), true
}
