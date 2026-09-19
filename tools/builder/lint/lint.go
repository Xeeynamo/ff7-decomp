package lint

import (
	"fmt"
	"os"

	"golang.org/x/sync/errgroup"
)

// Options controls one Lint run.
type Options struct {
	Only    []string
	Verbose bool
	MinOverlaps int
}

// Lint scans the requested overlays (all of them when Only is empty) for symbols
// whose real, compiler-measured byte ranges overlap, and prints one line per finding.
// It returns an error when the number of overlapping pairs exceeds opts.MinOverlaps.
func Lint(opts Options) error {
	overlays, err := LoadOverlays()
	if err != nil {
		return err
	}
	overlays, err = filterOverlays(overlays, opts.Only)
	if err != nil {
		return err
	}

	type overlayResult struct {
		name     string
		findings []Finding
		resolver *StructResolver
	}
	results := make([]overlayResult, len(overlays))

	var eg errgroup.Group
	for i, ovl := range overlays {
		i, ovl := i, ovl
		eg.Go(func() error {
			findings, resolver, err := lintOverlay(ovl, opts.Verbose)
			if err != nil {
				return fmt.Errorf("%s: %w", ovl.Name, err)
			}
			results[i] = overlayResult{name: ovl.Name, findings: findings, resolver: resolver}
			return nil
		})
	}
	if err := eg.Wait(); err != nil {
		return err
	}

	findingsByOverlay := map[string][]Finding{}
	resolvers := map[string]*StructResolver{}
	for _, r := range results {
		findingsByOverlay[r.name] = r.findings
		resolvers[r.name] = r.resolver
	}

	n := Report(os.Stdout, os.Stderr, findingsByOverlay, resolvers)
	if n > opts.MinOverlaps {
		return fmt.Errorf("lint: %d overlapping symbol pairs found, limit is %d", n, opts.MinOverlaps)
	}
	return nil
}

func lintOverlay(ovl Overlay, verbose bool) ([]Finding, *StructResolver, error) {
	table, err := LoadSymbolTable(ovl.SymbolAddrsPath)
	if err != nil {
		return nil, nil, err
	}

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

	merged := mergeSymbols(perTU...)
	findings := findOverlaps(merged)

	resolver := NewStructResolver(allStructs)
	return findings, resolver, nil
}
