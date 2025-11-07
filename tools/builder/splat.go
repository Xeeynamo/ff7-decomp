package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

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
	AsmJtblLabelMacro              string   `yaml:"asm_jtbl_label_macro"`
	MigrateRodataToFunctions       bool     `yaml:"migrate_rodata_to_functions"`
	DisassembleAll                 bool     `yaml:"disassemble_all"`
	GlobalVramStart                int64    `yaml:"global_vram_start"`
	SectionOrder                   []string `yaml:"section_order"`
	LdGenerateSymbolPerDataSegment bool     `yaml:"ld_generate_symbol_per_data_segment"`
}

type SplatSegment struct {
	Name        string          `yaml:"name"`
	Type        string          `yaml:"type"`
	Start       int             `yaml:"start"`
	Vram        int64           `yaml:"vram"`
	BssSize     int64           `yaml:"bss_size"`
	Align       int             `yaml:"align"`
	Subalign    int             `yaml:"subalign"`
	Subsegments [][]interface{} `yaml:"subsegments"`
}

type SplatConfig struct {
	Options  SplatOptions  `yaml:"options"`
	Sha1     string        `yaml:"sha1"`
	Segments []interface{} `yaml:"segments"`
}

func MakeSplatConfig(b BuildConfig, o Overlay) (SplatConfig, error) {
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
			SymbolAddrsPath:                o.SymbolAddrsPath,
			CreateUndefinedFuncsAuto:       true,
			UndefinedFuncsAutoPath:         filepath.Join(b.GeneratedSymPath, fmt.Sprintf("undefined_funcs.%s.txt", o.Name)),
			CreateUndefinedSymsAuto:        true,
			UndefinedSymsAutoPath:          filepath.Join(b.GeneratedSymPath, fmt.Sprintf("undefined_syms.%s.txt", o.Name)),
			FindFileBoundaries:             false,
			UseLegacyIncludeAsm:            false,
			AsmJtblLabelMacro:              "jlabel",
			MigrateRodataToFunctions:       o.MigrateRodataToFunctions,
			DisassembleAll:                 false,
			GlobalVramStart:                o.VramStart,
			SectionOrder:                   []string{".rodata", ".text", ".data", ".bss"},
			LdGenerateSymbolPerDataSegment: true,
		},
		Segments: []interface{}{
			SplatSegment{
				Name:        o.Name,
				Type:        "code",
				Start:       0,
				Vram:        o.VramStart,
				BssSize:     o.BssSize,
				Align:       b.Align,
				Subalign:    b.Align,
				Subsegments: o.Segments,
			},
			[]int64{stat.Size()},
		},
	}, nil
}
