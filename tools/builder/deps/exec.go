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
