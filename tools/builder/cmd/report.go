package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/builder"
)

var reportCmd = &cobra.Command{
	Use:           "report <version> <report.json>",
	Short:         "Generates a progress report",
	SilenceErrors: true,
	Args:          cobra.ExactArgs(2),
	RunE: func(cmd *cobra.Command, args []string) error {
		if err := builder.Report(args[0], args[1]); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(reportCmd)
}
