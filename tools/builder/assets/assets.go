package assets

import "time"

type Metadata struct {
	Data       []byte
	Start      int
	End        int
	Name       string
	Symbol     string
	AssetDir   string
	AsmDataDir string
}

type Handler interface {
	Extract(m Metadata) error
	SplatEntry(m Metadata) map[string]any
	// Timestamp reports the oldest modification time among the files this
	// handler extracts, or the zero time.Time to always mark it as dirty.
	Timestamp(m Metadata) time.Time
}
