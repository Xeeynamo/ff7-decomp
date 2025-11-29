package main

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"
)

func main() {
	rootCmd := &cobra.Command{
		Short: "FF7 Decomp orchestrator",
		Long:  "Wraps all the tooling with a clear and simple interface",
	}

	rootCmd.AddCommand(&cobra.Command{
		Use:           "build <config.yaml>",
		Short:         "Builds the game binaries with the support of the original game files",
		SilenceErrors: true,
		Args:          cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			if err := build(args[0]); err != nil {
				panic(err)
			}
			return nil
		},
	})

	rootCmd.AddCommand(&cobra.Command{
		Use:           "report <config.yaml> <report.json>",
		Short:         "Generates a progress report",
		SilenceErrors: true,
		Args:          cobra.ExactArgs(2),
		RunE: func(cmd *cobra.Command, args []string) error {
			if err := report(args[0], args[1]); err != nil {
				return err
			}
			return nil
		},
	})

	if err := rootCmd.Execute(); err != nil {
		_, _ = fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
