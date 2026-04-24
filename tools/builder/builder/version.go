package builder

import "os"

func Version() string {
	version := os.Getenv("VERSION")
	if version == "" {
		return "us"
	}
	return version
}
