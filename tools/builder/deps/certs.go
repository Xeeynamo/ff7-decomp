package deps

import (
	"crypto/sha256"
	"fmt"
	"os"
	"path/filepath"
)

var certs = map[string]string{
	"clang-format.gz":          "f8301e7086576800c8fb5527548fc74a85a079fb3d033c73b20b19c57b20b2eb",
	"clang-tidy.gz":            "2b36332c71e5f11841b9346905b4082471ccb6f7d88b4bbbe04ddf610fad7237",
	"objdiff-cli-linux-x86_64": "44f9a2bfc80319cfe07a09c0380bdd30d0af98d97cbe2bed3fa19a618cd8ca82",
}

func verify(path string) error {
	base := filepath.Base(path)
	expectedSum, ok := certs[base]
	if !ok {
		return fmt.Errorf("verification failed for %s: %s", base, "not found")
	}
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}
	actualSum := fmt.Sprintf("%x", sha256.Sum256(data))
	if actualSum != expectedSum {
		return fmt.Errorf("checksum mismatch for %s: %s", base, actualSum)
	}
	return nil
}
