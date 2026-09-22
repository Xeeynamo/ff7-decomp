package cmd

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/spf13/cobra"
)

var assetEncoders = map[string]func([]byte) ([]byte, error){}

func init() {
	rootCmd.AddCommand(&cobra.Command{
		Use:           "asset <kind> <in> <out>",
		Short:         "Converts an extracted asset back into its binary form",
		Hidden:        true,
		Args:          cobra.ExactArgs(3),
		SilenceErrors: true,
		RunE: func(cmd *cobra.Command, args []string) error {
			kind, in, out := args[0], args[1], args[2]
			encode, ok := assetEncoders[kind]
			if !ok {
				return fmt.Errorf("unknown asset kind %q", kind)
			}
			data, err := os.ReadFile(in)
			if err != nil {
				return err
			}
			bin, err := encode(data)
			if err != nil {
				return fmt.Errorf("%s: %w", in, err)
			}
			if err := os.MkdirAll(filepath.Dir(out), 0755); err != nil {
				return err
			}
			return os.WriteFile(out, bin, 0644)
		},
	})
}
