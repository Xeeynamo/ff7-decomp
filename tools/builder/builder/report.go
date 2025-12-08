package builder

import (
	"os"
	"path/filepath"

	"github.com/goccy/go-yaml"
	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
)

func Report(version string, outputFile string) error {
	data, _ := os.ReadFile(ConfigPath(version))
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
	_ = os.Setenv("FF7_PROGRESS_REPORT", "1")
	if err := deps.GenNinja(b.BuildPath); err != nil {
		return err
	}
	_ = os.Unsetenv("FF7_PROGRESS_REPORT")
	if err := deps.Ninja(); err != nil {
		return err
	}
	return deps.ObjdiffCLI("report", "generate", "-o", outputFile)
}
