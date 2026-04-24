package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/rank"
)

var rankCmd = &cobra.Command{
	Use:           "rank <source_path>",
	Short:         "Rank non-decompiled functions from easiest to hardest",
	SilenceErrors: true,
	Args:          cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		if err := rank.Rank(args[0], 0); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(rankCmd)
}
