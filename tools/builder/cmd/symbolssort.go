package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/symbols"
)

var symbolsSortCmd = &cobra.Command{
	Use:   "sort <symbols_path>",
	Short: "Sort symbols",
	Args:  cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		if err := symbols.Sort(args[0]); err != nil {
			panic(err)
		}
	},
}

func init() {
	symbolsCmd.AddCommand(symbolsAddCmd)
}
