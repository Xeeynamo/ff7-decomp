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
	Use:           "lint [overlay...]",
	Short:         "Detect symbols whose memory ranges overlap",
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
