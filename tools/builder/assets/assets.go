package assets

import "time"

type Metadata struct {
	Data       []byte
	Start      int
	End        int
	Name       string
	Symbol     string
	Args       []any
	AssetDir   string
	AsmDataDir string
	BuildDir   string
}

type BuildStep struct {
	Command string   `yaml:"command"`
	Inputs  []string `yaml:"inputs"`
	Outputs []string `yaml:"outputs"`
}

type Handler interface {
	Extract(m Metadata) error
	SplatEntry(m Metadata) map[string]any

	// Build lists the conversions Ninja needs to run before assembling the
	// asset, or nil when the extracted files are embedded as they are.
	Build(m Metadata) []BuildStep

	// Timestamp reports the oldest modification time among the files this
	// handler extracts, or the zero time.Time to always mark it as dirty.
	Timestamp(m Metadata) time.Time
}
