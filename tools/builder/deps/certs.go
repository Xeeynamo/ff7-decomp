package deps

import (
	"crypto/sha256"
	"fmt"
	"path/filepath"
)

var certs = map[string]string{
	"clang-format.gz":          "8d81075ea13dcfc5c1b1223cf141f4bc534fd52eb64c27d9c96aa7a85053a57c",
	"objdiff-cli-linux-x86_64": "6317ffdc0145709955ebb2dff6beca1b54752d4598829418881b44b1666059a8",
}

func verify(path string) error {
	base := filepath.Base(path)
	expectedSum, ok := certs[base]
	if !ok {
		return fmt.Errorf("verification failed for %s: %s", base, "not found")
	}
	actualSum := fmt.Sprintf("%x", sha256.Sum256([]byte(path)))
	if actualSum != expectedSum {
		return fmt.Errorf("checksum mismatch: %s", actualSum)
	}
	return nil
}
