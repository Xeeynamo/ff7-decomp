package builder

import (
	"encoding/json"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
)

type compileDBEntry struct {
	Directory string   `json:"directory"`
	Arguments []string `json:"arguments"`
	File      string   `json:"file"`
}

func writeCompileCommands(b BuildConfig) error {
	cwd, err := os.Getwd()
	if err != nil {
		return err
	}

	var db []compileDBEntry
	err = filepath.WalkDir(b.SrcPath, func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if d.IsDir() || !strings.HasSuffix(path, ".c") {
			return nil
		}
		args := []string{
			"clang", "-std=gnu11", "-DSKIP_ASM=1",
			"-Iinclude", "-Iinclude/psxsdk",
			// The source's own dir, so quoted "*_private.h" includes resolve.
			"-I" + filepath.Dir(path),
			"-c", path,
		}
		db = append(db, compileDBEntry{
			Directory: cwd,
			Arguments: args,
			File:      filepath.Join(cwd, path),
		})
		return nil
	})
	if err != nil {
		return err
	}

	data, err := json.MarshalIndent(db, "", "  ")
	if err != nil {
		return err
	}
	return os.WriteFile("compile_commands.json", data, 0644)
}
