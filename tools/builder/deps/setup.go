package deps

import (
	"compress/gzip"
	"fmt"
	"io"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func downloadFromGithubIfNotExists(repo, tag, name, destination string) error {
	tagPath := destination + ".tag"
	if tagData, err := os.ReadFile(tagPath); err == nil && string(tagData) == tag {
		if _, err := os.Stat(destination); !os.IsNotExist(err) {
			return nil
		}
	}
	_ = os.Remove(destination)
	url := fmt.Sprintf("https://github.com/%s/releases/download/%s/%s", repo, tag, name)
	downloadFileName := destination
	if strings.HasSuffix(url, ".gz") {
		downloadFileName += ".gz"
	}
	defer os.Remove(downloadFileName)
	if err := downloadIfNotExists(url, downloadFileName); err != nil {
		return fmt.Errorf("failed to download at %s: %w", url, err)
	}
	if strings.HasSuffix(downloadFileName, ".gz") {
		if err := ungz(destination, downloadFileName); err != nil {
			return err
		}
	}
	if err := os.Chmod(destination, 0755); err != nil {
		return err
	}
	return os.WriteFile(tagPath, []byte(tag), 0o644)
}

func ungz(dstPath, srcPath string) error {
	f, err := os.Open(srcPath)
	if err != nil {
		return err
	}
	defer f.Close()

	gz, err := gzip.NewReader(f)
	if err != nil {
		return err
	}
	defer gz.Close()

	out, err := os.Create(dstPath)
	if err != nil {
		return err
	}
	defer out.Close()

	if _, err := io.Copy(out, gz); err != nil {
		return err
	}
	return nil
}

func downloadIfNotExists(url string, filename string) error {
	if _, err := os.Stat(filename); os.IsNotExist(err) {
		return download(url, filename)
	}
	return nil
}

func download(url string, filename string) error {
	resp, err := http.Get(url)
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("unexpected HTTP status %d for %s", resp.StatusCode, url)
	}
	out, err := os.Create(filename)
	if err != nil {
		return err
	}
	if _, err := io.Copy(out, resp.Body); err != nil {
		return err
	}
	if err := out.Close(); err != nil {
		return err
	}
	if err := verify(filename); err != nil {
		return err
	}
	if filepath.Dir(filename) == "bin" {
		if err := os.Chmod(filename, 0755); err != nil {
			return err
		}
	}
	return nil
}

func venvPython() (string, error) {
	const bin = ".venv/bin/python3"
	if _, err := os.Stat(bin); err != nil {
		if !os.IsNotExist(err) {
			return "", fmt.Errorf("fetch python venv: %w", err)
		}
		binPath, err := exec.LookPath("python3")
		if err != nil {
			return "", err
		}
		if err := (&exec.Cmd{
			Path:   binPath,
			Args:   []string{binPath, "-m", "venv", ".venv"},
			Stdin:  os.Stdin,
			Stdout: os.Stdout,
			Stderr: os.Stderr,
		}).Run(); err != nil {
			return "", fmt.Errorf("init python venv: %w", err)
		}
	}
	return bin, nil
}

func pip(args ...string) error {
	binPath, err := venvPython()
	if err != nil {
		return err
	}
	cmd := exec.Cmd{
		Path:   binPath,
		Args:   append([]string{binPath, "-m", "pip"}, args...),
		Stdin:  os.Stdin,
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}
	return cmd.Run()
}
