package builder

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/assets"
	"github.com/xeeynamo/ff7-decomp/tools/builder/assets/saveicons"
	"github.com/xeeynamo/ff7-decomp/tools/builder/assets/tims"
)

var assetHandlers = map[string]assets.Handler{
	"tim":       tims.Tim{},
	"saveicons": saveicons.SaveIcons{},
}

type SplatOptions struct {
	Platform                       string   `yaml:"platform"`
	Basename                       string   `yaml:"basename"`
	BasePath                       string   `yaml:"base_path"`
	BuildPath                      string   `yaml:"build_path"`
	TargetPath                     string   `yaml:"target_path"`
	AsmPath                        string   `yaml:"asm_path"`
	AssetPath                      string   `yaml:"asset_path"`
	SrcPath                        string   `yaml:"src_path"`
	LdScriptPath                   string   `yaml:"ld_script_path"`
	Compiler                       string   `yaml:"compiler"`
	SymbolAddrsPath                []string `yaml:"symbol_addrs_path"`
	CreateUndefinedFuncsAuto       bool     `yaml:"create_undefined_funcs_auto"`
	UndefinedFuncsAutoPath         string   `yaml:"undefined_funcs_auto_path"`
	CreateUndefinedSymsAuto        bool     `yaml:"create_undefined_syms_auto"`
	UndefinedSymsAutoPath          string   `yaml:"undefined_syms_auto_path"`
	FindFileBoundaries             bool     `yaml:"find_file_boundaries"`
	UseLegacyIncludeAsm            bool     `yaml:"use_legacy_include_asm"`
	AsmJtblLabelMacro              string   `yaml:"asm_jtbl_label_macro,omitempty"`
	MigrateRodataToFunctions       bool     `yaml:"migrate_rodata_to_functions"`
	DisassembleAll                 bool     `yaml:"disassemble_all"`
	GlobalVramStart                int64    `yaml:"global_vram_start"`
	GPValue                        int64    `yaml:"gp_value,omitempty"`
	SectionOrder                   []string `yaml:"section_order"`
	LdGenerateSymbolPerDataSegment bool     `yaml:"ld_generate_symbol_per_data_segment"`
	LdBssIsNoLoad                  bool     `yaml:"ld_bss_is_noload"`
	HasmInSrcPath                  bool     `yaml:"hasm_in_src_path"`
}

type SplatSegment struct {
	Name        string `yaml:"name"`
	Type        string `yaml:"type"`
	Start       int    `yaml:"start"`
	Vram        int64  `yaml:"vram"`
	BssSize     int64  `yaml:"bss_size,omitempty"`
	Align       int    `yaml:"align"`
	Subalign    int    `yaml:"subalign"`
	Subsegments []any  `yaml:"subsegments"`
}

type SplatConfig struct {
	Options  SplatOptions `yaml:"options"`
	Sha1     string       `yaml:"sha1"`
	Segments []any        `yaml:"segments"`
}

func asInt(v any) (int, bool) {
	switch n := v.(type) {
	case int:
		return n, true
	case int64:
		return int(n), true
	case uint64:
		return int(n), true
	}
	return 0, false
}

type assetMatch struct {
	index   int
	kind    string
	handler assets.Handler
	meta    assets.Metadata
}

func findAssetMatches(b BuildConfig, o Overlay) ([]assetMatch, error) {
	var raw []byte
	var matches []assetMatch
	for i, sub := range o.Segments {
		if len(sub) < 3 {
			continue
		}
		kind, ok := sub[1].(string)
		if !ok {
			continue
		}
		handler, ok := assetHandlers[kind]
		if !ok {
			continue
		}
		start, ok := asInt(sub[0])
		if !ok {
			return nil, fmt.Errorf("overlay %s: %s subsegment has non-integer start %v", o.Name, kind, sub[0])
		}
		name, symbol := kind, kind
		if s, ok := sub[2].(string); ok {
			name, symbol = s, s+"_"+kind
		}
		if i+1 >= len(o.Segments) {
			return nil, fmt.Errorf("overlay %s: %s subsegment %q has no following subsegment to bound its end", o.Name, kind, name)
		}
		end, ok := asInt(o.Segments[i+1][0])
		if !ok {
			return nil, fmt.Errorf("overlay %s: %s subsegment %q cannot determine end offset", o.Name, kind, name)
		}

		if raw == nil {
			var err error
			raw, err = os.ReadFile(o.DiskPath)
			if err != nil {
				return nil, err
			}
		}
		if end > len(raw) {
			return nil, fmt.Errorf("overlay %s: %s subsegment %q end 0x%X exceeds disk image size 0x%X", o.Name, kind, name, end, len(raw))
		}

		m := assets.Metadata{
			Data:       raw,
			Start:      start,
			End:        end,
			Name:       name,
			Symbol:     symbol,
			Args:       sub[2:],
			AssetDir:   filepath.Join(b.AssetPath, o.BasePath),
			AsmDataDir: filepath.Join(b.AsmPath, o.BasePath, "data"),
			BuildDir:   filepath.Join(b.BuildPath, b.AssetPath, o.BasePath),
		}
		matches = append(matches, assetMatch{index: i, kind: kind, handler: handler, meta: m})
	}
	return matches, nil
}

func makeAssetSubsegments(b BuildConfig, o Overlay) ([]any, error) {
	subsegments := make([]any, len(o.Segments))
	for i, sub := range o.Segments {
		subsegments[i] = sub
	}
	matches, err := findAssetMatches(b, o)
	if err != nil {
		return nil, err
	}
	for _, mt := range matches {
		entry := mt.handler.SplatEntry(mt.meta)
		if steps := mt.handler.Build(mt.meta); len(steps) > 0 {
			entry["build"] = steps
		}
		subsegments[mt.index] = entry
	}
	return subsegments, nil
}

func makeSplatConfig(b BuildConfig, o Overlay) (SplatConfig, error) {
	overlayExists := func(name string) bool {
		for _, other := range b.Overlays {
			if other.Name == name {
				return true
			}
		}
		return false
	}
	for _, imp := range o.Imports {
		if imp == o.Name {
			return SplatConfig{}, fmt.Errorf("overlay %s cannot import itself", o.Name)
		}
		if !overlayExists(imp) {
			return SplatConfig{}, fmt.Errorf("overlay %s imports unknown overlay %s", o.Name, imp)
		}
	}
	getRootDir := func(path string) string {
		if path == "" {
			return "."
		}
		parts := strings.Split(filepath.Clean(path), string(filepath.Separator))
		var ups []string
		for _, p := range parts {
			if p != "" {
				ups = append(ups, "..")
			}
		}
		if len(ups) == 0 {
			return "."
		}
		return filepath.Join(ups...)
	}
	stat, err := os.Stat(o.DiskPath)
	if err != nil {
		return SplatConfig{}, err
	}
	subsegments, err := makeAssetSubsegments(b, o)
	if err != nil {
		return SplatConfig{}, err
	}
	start := 0
	var segments []any
	if o.Name == "main" {
		start = 0x800
		header := []any{0x800, "header"}
		segments = append(segments, header)
	}
	seg := SplatSegment{
		Name:        o.Name,
		Type:        "code",
		Start:       start,
		Vram:        o.VramStart,
		BssSize:     o.BssSize,
		Align:       b.Align,
		Subalign:    b.Align,
		Subsegments: subsegments,
	}
	segments = append(segments, seg)
	segments = append(segments, []int64{stat.Size()})
	symbolAddrsPath := o.SymbolAddrsPath
	if len(o.Imports) > 0 {
		symbolAddrsPath = append(append([]string{}, o.SymbolAddrsPath...), filepath.Join(b.BuildPath, o.Name+".imports.txt"))
	}
	return SplatConfig{
		Sha1: o.Sha1,
		Options: SplatOptions{
			Platform:                       "psx",
			Compiler:                       "GCC",
			Basename:                       o.Name,
			BasePath:                       getRootDir(b.BuildPath),
			BuildPath:                      b.BuildPath,
			TargetPath:                     o.DiskPath,
			AsmPath:                        filepath.Join(b.AsmPath, o.BasePath),
			AssetPath:                      filepath.Join(b.AssetPath, o.BasePath),
			SrcPath:                        filepath.Join(b.SrcPath, o.BasePath),
			LdScriptPath:                   filepath.Join(b.LdScriptPath, fmt.Sprintf("%s.ld", o.Name)),
			SymbolAddrsPath:                symbolAddrsPath,
			CreateUndefinedFuncsAuto:       true,
			UndefinedFuncsAutoPath:         filepath.Join(b.GeneratedSymPath, fmt.Sprintf("undefined_funcs.%s.txt", o.Name)),
			CreateUndefinedSymsAuto:        true,
			UndefinedSymsAutoPath:          filepath.Join(b.GeneratedSymPath, fmt.Sprintf("undefined_syms.%s.txt", o.Name)),
			FindFileBoundaries:             false,
			UseLegacyIncludeAsm:            false,
			MigrateRodataToFunctions:       o.MigrateRodataToFunctions,
			DisassembleAll:                 o.Name == "main", // for some reason, `main` doesn't build without
			GlobalVramStart:                o.VramStart,
			GPValue:                        o.GPValue,
			SectionOrder:                   []string{".rodata", ".text", ".data", ".sdata", ".sbss", ".bss"},
			LdGenerateSymbolPerDataSegment: true,
			LdBssIsNoLoad:                  o.Name != "main" && o.BssSize > 0,
			HasmInSrcPath:                  true,
		},
		Segments: segments,
	}, nil
}
