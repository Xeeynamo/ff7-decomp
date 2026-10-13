package lint

import (
	"bufio"
	"bytes"
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"runtime"
	"sort"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
	"golang.org/x/sync/errgroup"
)

var tidyDiagnostic = regexp.MustCompile(`^(\S+):(\d+):(\d+): (warning|error|fatal error): (.*) \[([^\]]+)\]$`)

// builtinIncludeDir asks the host compiler for its stddef.h and friends, which the static clang-tidy does not ship.
func builtinIncludeDir() string {
	out, err := exec.Command("cc", "-print-file-name=include").Output()
	if err != nil {
		return ""
	}
	return strings.TrimSpace(string(out))
}

func Tidy() error {
	data, err := os.ReadFile("compile_commands.json")
	if err != nil {
		return fmt.Errorf("clang-tidy: %w: run `make build` first", err)
	}
	var db []struct {
		File string `json:"file"`
	}
	if err := json.Unmarshal(data, &db); err != nil {
		return fmt.Errorf("clang-tidy: compile_commands.json: %w", err)
	}
	args := []string{"-p", ".", "--quiet"}
	if dir := builtinIncludeDir(); dir != "" {
		args = append(args, "--extra-arg=-isystem"+dir)
	}

	cwd, err := os.Getwd()
	if err != nil {
		return err
	}
	if _, err := deps.ClangTidy("--version"); err != nil {
		return fmt.Errorf("clang-tidy: %w", err)
	}
	outputs := make([][]byte, len(db))
	var eg errgroup.Group
	eg.SetLimit(runtime.NumCPU())
	for i, entry := range db {
		eg.Go(func() error {
			out, err := deps.ClangTidy(append(args, entry.File)...)
			outputs[i] = out
			return err
		})
	}
	if err := eg.Wait(); err != nil {
		return fmt.Errorf("clang-tidy: %w", err)
	}

	findings := map[string]bool{}
	for i, out := range outputs {
		scanner := bufio.NewScanner(bytes.NewReader(out))
		for scanner.Scan() {
			m := tidyDiagnostic.FindStringSubmatch(scanner.Text())
			if m == nil {
				continue
			}
			if m[4] == "fatal error" || strings.HasSuffix(m[5], "file not found") {
				return fmt.Errorf("clang-tidy: %s: %s", db[i].File, scanner.Text())
			}
			if strings.HasPrefix(m[6], "clang-diagnostic-") {
				continue
			}
			path := m[1]
			if rel, err := filepath.Rel(cwd, path); err == nil && !strings.HasPrefix(rel, "..") {
				path = rel
			}
			if strings.HasPrefix(path, "tools/") {
				continue
			}
			findings[fmt.Sprintf("%s:%s:%s: %s [%s]", path, m[2], m[3], m[5], m[6])] = true
		}
	}
	if len(findings) == 0 {
		return nil
	}
	lines := make([]string, 0, len(findings))
	for line := range findings {
		lines = append(lines, line)
	}
	sort.Strings(lines)
	for _, line := range lines {
		fmt.Println(line)
	}
	return fmt.Errorf("clang-tidy: %d findings", len(lines))
}
