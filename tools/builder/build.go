package main

import (
	"bytes"
	"fmt"
	"os"

	"github.com/goccy/go-yaml"
)

func build(configPath string) error {
	data, _ := os.ReadFile(configPath)
	var b BuildConfig
	if err := yaml.Unmarshal(data, &b); err != nil {
		panic(err)
	}
	_ = os.MkdirAll(b.BuildPath, 0755)
	for _, o := range b.Overlays {
		expectedFingerprint := o.Fingerprint()
		actualFingerprint, _ := os.ReadFile(fmt.Sprintf("%s/%s.fingerprint", b.BuildPath, o.Name))
		if actualFingerprint != nil && bytes.Equal(expectedFingerprint, actualFingerprint) {
			continue
		}
		splatConfig, err := MakeSplatConfig(b, o)
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
