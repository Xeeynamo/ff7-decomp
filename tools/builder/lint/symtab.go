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
