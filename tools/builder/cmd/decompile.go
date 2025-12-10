package cmd

import (
	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
)

var (
	fixStructs bool
)

var decompileCmd = &cobra.Command{
	Use:           "dec <function_name>",
	Short:         "Replaces INCLUDE_ASM with approximated decompiled function",
	SilenceErrors: true,
	RunE: func(cmd *cobra.Command, args []string) error {
		if fixStructs {
			args = append(args, "--fix-structs")
		}
		if err := deps.Decompile(args...); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	decompileCmd.Flags().BoolVar(&fixStructs, "fix-structs", false, "Replace D_8009XXXX symbols with Savemap.field_name references")
	rootCmd.AddCommand(decompileCmd)
}
