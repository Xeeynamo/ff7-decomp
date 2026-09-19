package lint

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/goccy/go-yaml"
	"github.com/xeeynamo/ff7-decomp/tools/builder/builder"
)

// Overlay is the subset of builder.Overlay that lint needs, plus the resolved
// list of .c source paths that belong to it.
type Overlay struct {
	Name            string
	Sources         []string
	SymbolAddrsPath []string
	VramStart       uint32
}

// LoadOverlays reads config/us.yaml and resolves each overlay's .c sources.
func LoadOverlays() ([]Overlay, error) {
	data, err := os.ReadFile(builder.ConfigPath(""))
	if err != nil {
		return nil, err
	}
	var cfg builder.BuildConfig
	if err := yaml.Unmarshal(data, &cfg); err != nil {
		return nil, err
	}

	overlays := make([]Overlay, 0, len(cfg.Overlays))
	for _, ovl := range cfg.Overlays {
		var sources []string
		for _, seg := range ovl.Segments {
			if len(seg) != 3 {
				continue
			}
			kind, ok := seg[1].(string)
			if !ok || kind != "c" {
				continue
			}
			name, ok := seg[2].(string)
			if !ok {
				continue
			}
			sources = append(sources, filepath.Join("src", ovl.BasePath, name+".c"))
		}
		overlays = append(overlays, Overlay{
			Name:            ovl.Name,
			Sources:         sources,
			SymbolAddrsPath: ovl.SymbolAddrsPath,
			VramStart:       uint32(ovl.VramStart),
		})
	}
	return overlays, nil
}

// compilerFor picks the cc1-psx binary a source file must be probed with,
// mirroring tools/ninja/gen.py's get_compiler_params/parse_compiler_params
// restricted to the CC1/PSYQ keys that select the compiler.
func compilerFor(path string) string {
	f, err := os.Open(path)
	if err != nil {
		return "cc1-psx-272"
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)
	for i := 0; i < 10 && scanner.Scan(); i++ {
		line := scanner.Text()
		if !strings.HasPrefix(line, "//!") {
			continue
		}
		for _, param := range strings.Fields(strings.TrimPrefix(line, "//!")) {
			key, value, _ := strings.Cut(param, "=")
			switch key {
			case "CC1":
				if value == "2.6.3" {
					return "cc1-psx-26"
				}
				return "cc1-psx-272"
			case "PSYQ":
				if value == "3.3" || value == "3.5" {
					return "cc1-psx-26"
				}
				return "cc1-psx-272"
			}
		}
		return "cc1-psx-272"
	}
	return "cc1-psx-272"
}

func filterOverlays(overlays []Overlay, only []string) ([]Overlay, error) {
	if len(only) == 0 {
		return overlays, nil
	}
	wanted := map[string]bool{}
	for _, name := range only {
		wanted[name] = true
	}
	var filtered []Overlay
	for _, ovl := range overlays {
		if wanted[ovl.Name] {
			filtered = append(filtered, ovl)
			delete(wanted, ovl.Name)
		}
	}
	if len(wanted) > 0 {
		var missing []string
		for name := range wanted {
			missing = append(missing, name)
		}
		return nil, fmt.Errorf("unknown overlay(s): %s", strings.Join(missing, ", "))
	}
	return filtered, nil
}
