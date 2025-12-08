package cmd

import "github.com/spf13/cobra"
import "github.com/xeeynamo/ff7-decomp/tools/builder/builder"

var buildCmd = &cobra.Command{
	Use:           "build [version]",
	Short:         "Builds the game binaries with the support of the original game files",
	SilenceErrors: true,
	RunE: func(cmd *cobra.Command, args []string) error {
		var version string
		if len(args) > 0 {
			version = args[0]
		}
		if err := builder.Build(version); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(buildCmd)
}
