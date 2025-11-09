package main

import (
	"crypto/md5"
	"fmt"
)

type Overlay struct {
	Name                     string   `yaml:"name"`
	DiskPath                 string   `yaml:"disk_path"`
	Compression              string   `yaml:"compression"`
	Sha1                     string   `yaml:"sha1"`
	Sha1Decompressed         string   `yaml:"sha1_decompressed"`
	BasePath                 string   `yaml:"base_path"`
	SymbolAddrsPath          []string `yaml:"symbol_addrs_path"`
	MigrateRodataToFunctions bool     `yaml:"migrate_rodata_to_functions"`
	VramStart                int64    `yaml:"vram_start"`
	GPValue                  int64    `yaml:"gp_value"`
	BssSize                  int64    `yaml:"bss_size"`
	Segments                 [][]any  `yaml:"segments"`
}

type BuildConfig struct {
	Version          string    `yaml:"version"`
	BuildPath        string    `yaml:"build_path"`
	AsmPath          string    `yaml:"asm_path"`
	AssetPath        string    `yaml:"asset_path"`
	SrcPath          string    `yaml:"src_path"`
	LdScriptPath     string    `yaml:"ld_script_path"`
	GeneratedSymPath string    `yaml:"generated_sym_path"`
	Align            int       `yaml:"align"`
	Overlays         []Overlay `yaml:"overlays"`
}

func (o Overlay) Fingerprint() []byte {
	sum := md5.Sum([]byte(fmt.Sprintf("%v", o)))
	return sum[:]
}
