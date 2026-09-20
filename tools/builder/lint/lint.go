package lint

import (
	"fmt"
	"os"

	"golang.org/x/sync/errgroup"
)

// Options controls one Lint run.
type Options struct {
	Only        []string
	Verbose     bool
	MinOverlaps int
}

// Lint scans the requested overlays (all of them when Only is empty) for symbols
// whose real, compiler-measured byte ranges overlap, and prints one line per finding.
// It returns an error when the number of overlapping pairs exceeds opts.MinOverlaps.
func Lint(opts Options) error {
	all, err := LoadOverlays()
	if err != nil {
		return err
	}
	// The shared-region boundary and the symbol table come from every overlay, even
	// when only a few are being linted, so that -only runs resolve the same addresses.
	sharedEnd := SharedRegionEnd(all)
	table, err := LoadSymbolTableUnion(all)
	if err != nil {
		return err
	}

	overlays, err := filterOverlays(all, opts.Only)
	if err != nil {
		return err
	}

	type overlayResult struct {
		name     string
		symbols  []Symbol
		resolver *StructResolver
	}
	results := make([]overlayResult, len(overlays))

	var eg errgroup.Group
	for i, ovl := range overlays {
		i, ovl := i, ovl
		eg.Go(func() error {
			symbols, resolver, err := lintOverlay(ovl, table, opts.Verbose)
			if err != nil {
				return fmt.Errorf("%s: %w", ovl.Name, err)
			}
			results[i] = overlayResult{name: ovl.Name, symbols: symbols, resolver: resolver}
			return nil
		})
	}
	if err := eg.Wait(); err != nil {
		return err
	}

	findingsByOverlay := map[string][]Finding{}
	resolvers := map[string]*StructResolver{}

	// Overlay-local symbols are only compared within their own overlay: overlays that
	// share a vram_start are never resident together, so comparing them would invent
	// overlaps. Shared-region symbols are pooled and compared once, across everything.
	var shared []Symbol
	for _, r := range results {
		resolvers[r.name] = r.resolver
		local, sh := splitByRegion(r.symbols, sharedEnd)
		findingsByOverlay[r.name] = findOverlaps(local)
		shared = append(shared, sh...)
	}
	if len(shared) > 0 {
		findingsByOverlay[SharedScope] = findOverlaps(mergeSymbols(shared))
		rs := make([]*StructResolver, 0, len(results))
		for _, r := range results {
			rs = append(rs, r.resolver)
		}
		resolvers[SharedScope] = NewStructResolverUnion(rs)
	}

	n := Report(os.Stdout, os.Stderr, findingsByOverlay, resolvers)
	if n > opts.MinOverlaps {
		return fmt.Errorf("lint: %d overlapping symbol pairs found, limit is %d", n, opts.MinOverlaps)
	}
	return nil
}

// SharedScope is the reporting bucket for findings in the always-resident region,
// which belong to no single overlay.
const SharedScope = "(shared)"

// splitByRegion separates an overlay's symbols into the ones private to it and the
// ones in the always-resident region below sharedEnd.
func splitByRegion(syms []Symbol, sharedEnd uint32) (local, shared []Symbol) {
	for _, s := range syms {
		if sharedEnd != 0 && s.Addr < sharedEnd {
			shared = append(shared, s)
		} else {
			local = append(local, s)
		}
	}
	return local, shared
}

func lintOverlay(ovl Overlay, table map[string]uint32, verbose bool) ([]Symbol, *StructResolver, error) {
	type tuResult struct {
		symbols []Symbol
		structs map[string]StructDef
	}
	tuResults := make([]tuResult, len(ovl.Sources))

	var eg errgroup.Group
	for i, src := range ovl.Sources {
		i, src := i, src
		eg.Go(func() error {
			pre, err := preprocess(src)
			if err != nil {
				return fmt.Errorf("%s: %w", src, err)
			}
			decls, structs := scanDecls(pre, src)

			var resolvable []Decl
			for _, d := range decls {
				if _, ok := resolveAddress(d.Name, table); ok {
					resolvable = append(resolvable, d)
				}
			}

			compiler := compilerFor(src)
			sizes, err := probeSizes(compiler, src, resolvable, verbose)
			if err != nil {
				return fmt.Errorf("%s: %w", src, err)
			}

			structByName := map[string]StructDef{}
			for _, s := range structs {
				structByName[s.Name] = s
			}

			symbols := make([]Symbol, 0, len(resolvable))
			for _, d := range resolvable {
				size, ok := sizes[d.Name]
				if !ok {
					continue
				}
				addr, _ := resolveAddress(d.Name, table)
				symbols = append(symbols, Symbol{
					Name:       d.Name,
					Addr:       addr,
					Size:       size.Bytes,
					LowerBound: size.LowerBound,
					Decls:      []Decl{d},
				})
			}
			tuResults[i] = tuResult{symbols: symbols, structs: structByName}
			return nil
		})
	}
	if err := eg.Wait(); err != nil {
		return nil, nil, err
	}

	perTU := make([][]Symbol, len(tuResults))
	allStructs := map[string]StructDef{}
	for i, r := range tuResults {
		perTU[i] = r.symbols
		for name, def := range r.structs {
			allStructs[name] = def
		}
	}

	resolver := NewStructResolver(allStructs)
	return mergeSymbols(perTU...), resolver, nil
}
