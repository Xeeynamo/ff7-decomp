package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/lint"
)

var (
	lintVerbose     bool
	lintMinOverlaps int
)

var lintCmd = &cobra.Command{
	Use:   "lint [overlay...]",
	Short: "Detect symbols whose memory ranges overlap inside an overlay",
	Long: "Compiles a small probe translation unit per .c file with the game's own " +
		"cpp | cc1-psx pipeline and reads the sizes of every file-scope symbol back from " +
		"the emitted .comm directives, then checks for symbols in the same overlay whose " +
		"address ranges intersect. Addresses are compared per overlay, since several " +
		"overlays share the same vram_start and reuse the same D_XXXXXXXX names for " +
		"unrelated data. Add `// lint:ignore-overlap` to a declaration's source line to " +
		"suppress a finding that is accepted for now.",
	SilenceErrors: true,
	SilenceUsage:  true,
	RunE: func(cmd *cobra.Command, args []string) error {
		return lint.Lint(lint.Options{Only: args, Verbose: lintVerbose, MinOverlaps: lintMinOverlaps})
	},
}

func init() {
	lintCmd.Flags().BoolVar(&lintVerbose, "verbose", false, "echo compiler diagnostics from probed .c files")
	lintCmd.Flags().IntVar(&lintMinOverlaps, "min-overlaps", 0, "fail only when the number of overlapping pairs exceeds this count")
	rootCmd.AddCommand(lintCmd)
}
