package format

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
	"github.com/xeeynamo/ff7-decomp/tools/builder/symbols"
	"golang.org/x/sync/errgroup"
)

func Format() error {
	var eg errgroup.Group
	eg.Go(clangFormat)
	eg.Go(symbolsFormat)
	eg.Go(pythonFormat)
	if err := eg.Wait(); err != nil {
		return fmt.Errorf("format: %v", err)
	}
	return nil
}

func clangFormat() error {
	var paths []string
	entries, err := os.ReadDir("include")
	if err != nil {
		return err
	}
	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}
		if !strings.HasSuffix(entry.Name(), ".h") {
			continue
		}
		paths = append(paths, filepath.Join("include", entry.Name()))
	}
	err = filepath.Walk("src", func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() {
			return nil
		}
		paths = append(paths, path)
		return nil
	})
	if err != nil {
		return err
	}
	args := append([]string{"-i"}, paths...)
	return deps.ClangFormat(args...)
}

func symbolsFormat() error {
	entries, err := os.ReadDir("config")
	if err != nil {
		return err
	}
	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}
		if !strings.HasSuffix(entry.Name(), ".txt") {
			continue
		}
		if !strings.HasPrefix(entry.Name(), "symbols.") {
			continue
		}
		if err := symbols.Sort(filepath.Join("config", entry.Name())); err != nil {
			return err
		}
	}
	return nil
}

func pythonFormat() error {
	for _, path := range []string{
		"tools/decompile.py",
		"tools/import_data.py",
		"tools/symbols.py",
		"tools/ninja/gen.py",
	} {
		if err := deps.Black(path); err != nil {
			return err
		}
	}
	return nil
}
