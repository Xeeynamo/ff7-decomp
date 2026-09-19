package deps

import (
	"bytes"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
)

func ObjdiffCLI(args ...string) error {
	binPath := "bin/objdiff-cli-linux-x86_64"
	if err := downloadFromGithubIfNotExists("encounter/objdiff", "v3.3.1", filepath.Base(binPath), binPath); err != nil {
		return err
	}
	return (&exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath}, args...),
		Stdin:  os.Stdin,
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}).Run()
}

func ObjdiffGUI(args ...string) error {
	binPath := "bin/objdiff-linux-x86_64"
	if err := downloadFromGithubIfNotExists("encounter/objdiff", "v3.3.1", filepath.Base(binPath), binPath); err != nil {
		return err
	}
	cmd := &exec.Cmd{
		Path: binPath,
		Args: []string{binPath, "--project-dir", "."},
	}
	cmdSetDetached(cmd)
	return cmd.Start()
}

func Git(args ...string) error {
	binPath, err := exec.LookPath("git")
	if err != nil {
		return err
	}
	stderr := bytes.NewBuffer(nil)
	if err := (&exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath}, args...),
		Stderr: stderr,
	}).Run(); err != nil {
		return fmt.Errorf("git: %v\n%s", err, stderr.Bytes())
	}
	return nil
}

func Decompile(args ...string) error {
	binPath, err := venvPython()
	if err != nil {
		return err
	}
	return (&exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath, "tools/decompile.py"}, args...),
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}).Run()
}

func GenNinja(args ...string) error {
	binPath, err := venvPython()
	if err != nil {
		return err
	}
	return (&exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath, "tools/ninja/gen.py"}, args...),
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}).Run()
}

func Ninja(args ...string) error {
	binPath, err := exec.LookPath("ninja")
	if err != nil {
		return err
	}
	return (&exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath}, args...),
		Stdin:  os.Stdin,
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}).Run()
}

func Black(args ...string) error {
	binPath, err := venvPython()
	if err != nil {
		return err
	}
	blackPath := filepath.Join(filepath.Dir(binPath), "black")
	if _, err := os.Stat(blackPath); os.IsNotExist(err) {
		if err := pip("install", "black"); err != nil {
			return err
		}
	}
	return (&exec.Cmd{
		Path: blackPath,
		Args: append([]string{binPath, "tools/ninja/gen.py"}, args...),
	}).Run()
}

func ClangFormat(args ...string) error {
	binPath := "bin/clang-format"
	if err := downloadFromGithubIfNotExists("Xeeynamo/ff7-decomp", "init", filepath.Base(binPath)+".gz", binPath); err != nil {
		return err
	}
	return (&exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath}, args...),
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}).Run()
}

// Cpp runs the cross preprocessor and returns its stdout.
func Cpp(args ...string) ([]byte, error) {
	binPath, err := exec.LookPath("mipsel-linux-gnu-cpp")
	if err != nil {
		return nil, fmt.Errorf("mipsel-linux-gnu-cpp not found in PATH: install the cross toolchain: %w", err)
	}
	stdout := bytes.NewBuffer(nil)
	stderr := bytes.NewBuffer(nil)
	cmd := &exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath}, args...),
		Stdout: stdout,
		Stderr: stderr,
	}
	if err := cmd.Run(); err != nil {
		return nil, fmt.Errorf("cpp: %v\n%s", err, stderr.Bytes())
	}
	return stdout.Bytes(), nil
}

// Str runs bin/str followed by an iconv UTF-8 -> Shift-JIS pass, mirroring the ninja psx-cc rule.
func Str(stdin []byte) ([]byte, error) {
	strPath := "bin/str"
	if _, err := os.Stat(strPath); err != nil {
		return nil, fmt.Errorf("%s not found: run `make bin/str` first", strPath)
	}
	strOut := bytes.NewBuffer(nil)
	strErr := bytes.NewBuffer(nil)
	strCmd := &exec.Cmd{
		Path:   strPath,
		Args:   []string{strPath},
		Stdin:  bytes.NewReader(stdin),
		Stdout: strOut,
		Stderr: strErr,
	}
	if err := strCmd.Run(); err != nil {
		return nil, fmt.Errorf("str: %v\n%s", err, strErr.Bytes())
	}

	iconvPath, err := exec.LookPath("iconv")
	if err != nil {
		return nil, fmt.Errorf("iconv not found in PATH: %w", err)
	}
	iconvOut := bytes.NewBuffer(nil)
	iconvErr := bytes.NewBuffer(nil)
	iconvCmd := &exec.Cmd{
		Path:   iconvPath,
		Args:   []string{iconvPath, "--from-code=UTF-8", "--to-code=Shift-JIS"},
		Stdin:  strOut,
		Stdout: iconvOut,
		Stderr: iconvErr,
	}
	if err := iconvCmd.Run(); err != nil {
		return nil, fmt.Errorf("iconv: %v\n%s", err, iconvErr.Bytes())
	}
	return iconvOut.Bytes(), nil
}

// Cc1 runs the given cc1-psx binary over stdin and returns stdout and stderr
// separately. Unlike clang-format, bin/cc1-psx-* is fetched by the Makefile's bin/%
// rule (gzip plus a checked-in .sha256), not by downloadFromGithubIfNotExists, so
// this only checks it is present and tells the caller to run `make` if not. The exit
// code is returned but not treated as fatal by callers: cc1 can exit 33 on real
// prototype conflicts while still emitting full output.
func Cc1(name string, stdin []byte, args ...string) (stdout, stderr []byte, exitErr error) {
	binPath := filepath.Join("bin", name)
	if _, err := os.Stat(binPath); err != nil {
		return nil, nil, fmt.Errorf("%s not found: run `make %s` first", binPath, binPath)
	}
	outBuf := bytes.NewBuffer(nil)
	errBuf := bytes.NewBuffer(nil)
	cmd := &exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath}, args...),
		Stdin:  bytes.NewReader(stdin),
		Stdout: outBuf,
		Stderr: errBuf,
	}
	exitErr = cmd.Run()
	return outBuf.Bytes(), errBuf.Bytes(), exitErr
}
