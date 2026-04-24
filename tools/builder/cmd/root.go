package cmd

import "github.com/spf13/cobra"

var rootCmd = &cobra.Command{
	Short: "FF7 Decomp orchestrator",
	Long:  "Wraps all the tooling with a clear and simple interface",
}

func init() {
	cobra.OnInitialize(func() {})
}

func Execute() error {
	return rootCmd.Execute()
}
