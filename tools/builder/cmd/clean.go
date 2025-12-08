package cmd

import "github.com/spf13/cobra"
import "github.com/xeeynamo/ff7-decomp/tools/builder/builder"

var cleanCmd = &cobra.Command{
	Use:           "clean [version]",
	Short:         "Clean all generated build files",
	SilenceErrors: true,
	RunE: func(cmd *cobra.Command, args []string) error {
		var version string
		if len(args) > 0 {
			version = args[0]
		}
		if err := builder.Clean(version); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(cleanCmd)
}
