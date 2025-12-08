package cmd

import (
	"github.com/spf13/cobra"
)

var symbolsCmd = &cobra.Command{
	Use:           "symbols",
	Short:         "Manage project symbols",
	SilenceErrors: true,
	RunE: func(cmd *cobra.Command, args []string) error {
		return nil
	},
}

func init() {
	rootCmd.AddCommand(symbolsCmd)
}
