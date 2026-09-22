package lint

import (
	"bufio"
	"fmt"
	"os"
	"regexp"

	"github.com/xeeynamo/ff7-decomp/tools/builder/symbols"
)

var addrNameRe = regexp.MustCompile(`^_?D_([0-9A-Fa-f]{8})$`)

// LoadSymbolTable builds a name -> address map from the given symbol_addrs files.
func LoadSymbolTable(paths []string) (map[string]uint32, error) {
	table := map[string]uint32{}
	for _, path := range paths {
		f, err := os.Open(path)
		if err != nil {
			return nil, err
		}
		scanner := bufio.NewScanner(f)
		for scanner.Scan() {
			line := scanner.Text()
			if line == "" {
				continue
			}
			name, addr, _, err := symbols.ParseLine(line)
			if err != nil {
				f.Close()
				return nil, fmt.Errorf("%s: %w", path, err)
			}
			table[name] = addr
		}
		err = scanner.Err()
		f.Close()
		if err != nil {
			return nil, err
		}
	}
	return table, nil
}

// LoadSymbolTableUnion merges every overlay's symbol_addrs files into one table, so a
// symbol defined by one overlay (Savemap, in main) resolves while linting the overlays
// that only declare neighbours of it. Each file is read once even when several overlays
// list it.
//
// Names are unique across the files today. A name that maps to two different addresses
// is an error rather than a silent first-wins, because the wrong address would invent
// overlaps that do not exist.
func LoadSymbolTableUnion(overlays []Overlay) (map[string]uint32, error) {
	union := map[string]uint32{}
	origin := map[string]string{}
	seen := map[string]bool{}
	for _, ovl := range overlays {
		for _, path := range ovl.SymbolAddrsPath {
			if seen[path] {
				continue
			}
			seen[path] = true

			table, err := LoadSymbolTable([]string{path})
			if err != nil {
				return nil, err
			}
			for name, addr := range table {
				if prev, ok := union[name]; ok && prev != addr {
					return nil, fmt.Errorf("symbol %s maps to 0x%08X in %s and 0x%08X in %s",
						name, prev, origin[name], addr, path)
				}
				union[name] = addr
				origin[name] = path
			}
		}
	}
	return union, nil
}

// resolveAddress finds the absolute address for a declared name: the symbol table first,
// then the D_XXXXXXXX / _D_XXXXXXXX address-in-name convention.
func resolveAddress(name string, table map[string]uint32) (uint32, bool) {
	if addr, ok := table[name]; ok {
		return addr, true
	}
	if m := addrNameRe.FindStringSubmatch(name); m != nil {
		var addr uint32
		if _, err := fmt.Sscanf(m[1], "%08X", &addr); err == nil {
			return addr, true
		}
	}
	return 0, false
}
