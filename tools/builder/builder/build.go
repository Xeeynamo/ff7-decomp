package builder

import (
	"bytes"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/goccy/go-yaml"
	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
)

func Build(version string) error {
	data, _ := os.ReadFile(ConfigPath(version))
	var b BuildConfig
	if err := yaml.Unmarshal(data, &b); err != nil {
		panic(err)
	}
	if err := writeObjdiffConfig(b); err != nil {
		return err
	}
	if err := os.MkdirAll(b.BuildPath, 0755); err != nil {
		return err
	}
	if err := writeSplatConfigs(b); err != nil {
		return err
	}
	if err := writeSha1Check(b); err != nil {
		return err
	}
	if err := deps.GenNinja(b.BuildPath); err != nil {
		return err
	}
	if err := deps.Ninja(); err != nil {
		return err
	}
	if err := generateExpected(); err != nil {
		return err
	}
	return nil
}

func writeSplatConfigs(b BuildConfig) error {
	for _, o := range b.Overlays {
		expectedFingerprint := o.Fingerprint()
		actualFingerprint, _ := os.ReadFile(fmt.Sprintf("%s/%s.fingerprint", b.BuildPath, o.Name))
		if actualFingerprint != nil && bytes.Equal(expectedFingerprint, actualFingerprint) {
			continue
		}
		splatConfig, err := makeSplatConfig(b, o)
		if err != nil {
			return err
		}
		data, err := yaml.Marshal(&splatConfig)
		if err != nil {
			return err
		}
		if err := os.WriteFile(fmt.Sprintf("%s/%s.yaml", b.BuildPath, o.Name), data, 0644); err != nil {
			return err
		}
		if err := os.WriteFile(fmt.Sprintf("%s/%s.fingerprint", b.BuildPath, o.Name), expectedFingerprint, 0644); err != nil {
			return err
		}
	}
	return nil
}

func writeSha1Check(b BuildConfig) error {
	var sb strings.Builder
	for _, o := range b.Overlays {
		sb.WriteString(fmt.Sprintf("%s  %s\n", o.Sha1, filepath.Join(b.BuildPath, o.Name+".exe")))
	}
	return os.WriteFile(filepath.Join(b.BuildPath, "check.sha1"), []byte(sb.String()), 0644)
}

func generateExpected() error {
	if err := os.MkdirAll("expected/build", 0o755); err != nil {
		return fmt.Errorf("mkdir: %w", err)
	}
	if err := os.RemoveAll("expected/build"); err != nil {
		return fmt.Errorf("remove: %w", err)
	}
	cmd := exec.Command("cp", "-r", "build", "expected/")
	if err := cmd.Run(); err != nil {
		return fmt.Errorf("%s: %w", strings.Join(cmd.Args, " "), err)
	}
	return nil
}
