package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
)

var decompileCmd = &cobra.Command{
	Use:           "dec <function_name>",
	Short:         "Replaces INCLUDE_ASM with approximated decompiled function",
	SilenceErrors: true,
	RunE: func(cmd *cobra.Command, args []string) error {
		if err := deps.Decompile(args...); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(decompileCmd)
}
