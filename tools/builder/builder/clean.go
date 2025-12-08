package builder

import (
	"os"
	"path/filepath"

	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
	"golang.org/x/sync/errgroup"
)

func Clean(version string) error {
	if version == "" {
		version = Version()
	}
	var eg errgroup.Group
	eg.Go(func() error {
		return deps.Git(
			"clean", "-Xfd",
			filepath.Join("asm", version),
			filepath.Join("build", version),
			"config")
	})
	eg.Go(func() error {
		for _, path := range []string{
			".ninja_deps",
			".ninja_log",
			"build.ninja",
			"objdiff.json",
		} {
			if err := os.RemoveAll(path); err != nil {
				return err
			}
		}
		return nil
	})
	return eg.Wait()
}
