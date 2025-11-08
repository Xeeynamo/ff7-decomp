package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"

	"github.com/goccy/go-yaml"
)

func report(configPath string, outputFile string) error {
	objdiffCli := fmt.Sprintf("bin/objdiff-cli-%s-%s", runtime.GOOS, "x86_64")
	if runtime.GOOS == "windows" {
		objdiffCli += ".exe"
	}
	if err := downloadFromGithubIfNotExists("encounter/objdiff", "v3.3.1", filepath.Base(objdiffCli), objdiffCli); err != nil {
		return err
	}
	data, _ := os.ReadFile(configPath)
	var b BuildConfig
	if err := yaml.Unmarshal(data, &b); err != nil {
		panic(err)
	}
	b.BuildPath = filepath.Join("report", b.BuildPath)
	if err := os.MkdirAll(b.BuildPath, 0755); err != nil {
		return err
	}
	if err := writeObjdiffConfig(b); err != nil {
		return err
	}
	if err := writeSplatConfigs(b); err != nil {
		return err
	}
	if err := writeSha1Check(b); err != nil {
		return err
	}
	if err := generateNinjaBuild(b.BuildPath, "GENERATE_REPORT=1"); err != nil {
		return err
	}
	if err := execNinja(); err != nil {
		return err
	}
	return (&exec.Cmd{
		Path:   objdiffCli,
		Args:   []string{objdiffCli, "report", "generate", "-o", outputFile},
		Env:    os.Environ(),
		Stdout: os.Stdout,
		Stderr: os.Stderr,
	}).Run()
}

func downloadFromGithubIfNotExists(repo, tag, name, destination string) error {
	url := fmt.Sprintf("https://github.com/%s/releases/download/%s/%s", repo, tag, name)
	if err := downloadIfNotExists(url, destination); err != nil {
		return fmt.Errorf("failed to download at %s", url)
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
	return os.Chmod(filename, 0755)
}
