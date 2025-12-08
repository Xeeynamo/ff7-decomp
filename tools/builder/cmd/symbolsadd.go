package cmd

import (
	"fmt"
	"strings"

	"github.com/spf13/cobra"
	"github.com/xeeynamo/ff7-decomp/tools/builder/builder"
	"github.com/xeeynamo/ff7-decomp/tools/builder/symbols"
	"github.com/xeeynamo/ff7-decomp/tools/builder/utils"
)

var (
	rebuild bool
)

var symbolsAddCmd = &cobra.Command{
	Use:   "add <symbols_path> <symbol> 0x<offset> [size]",
	Short: "Add a new symbol",
	Args: func(cmd *cobra.Command, args []string) error {
		if len(args) < 3 {
			return fmt.Errorf("requires at least 3 arguments: <path> <symbol> 0x<offset>")
		}
		if !strings.HasPrefix(args[2], "0x") {
			return fmt.Errorf("offset must be hexadecimal and start with 0x")
		}
		return nil
	},
	RunE: func(cmd *cobra.Command, args []string) error {
		offset, err := utils.ParseDigit(args[2])
		if err != nil {
			return err
		}
		size, err := utils.ParseDigit(args[3])
		if err != nil {
			return err
		}
		if err := symbols.Add(args[0], args[1], uint32(offset), int(size)); err != nil {
			return fmt.Errorf("failed adding symbol: %w", err)
		}
		if rebuild {
			if err := builder.Clean(""); err != nil {
				return fmt.Errorf("clean: %w", err)
			}
			if err := builder.Build(""); err != nil {
				return fmt.Errorf("build: %w", err)
			}
		}
		return nil
	},
}

func init() {
	symbolsAddCmd.Flags().BoolVar(&rebuild, "rebuild", false,
		"Run build clean + build after adding symbol")
	symbolsCmd.AddCommand(symbolsAddCmd)
}
