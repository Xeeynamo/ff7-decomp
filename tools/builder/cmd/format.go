package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/format"
)

var formatCmd = &cobra.Command{
	Use:           "format",
	Short:         "Format all code base",
	SilenceErrors: true,
	RunE: func(cmd *cobra.Command, args []string) error {
		if err := format.Format(); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(formatCmd)
}
