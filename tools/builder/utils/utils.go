package utils

import (
	"fmt"
	"strconv"
	"strings"
)

// ParseDigit parses a decimal or hexadecimal string into an int64
func ParseDigit(s string) (int64, error) {
	base := 10
	s = strings.TrimSpace(s)
	if strings.HasPrefix(s, "0x") {
		base = 16
		s = s[2:]
	}
	v, err := strconv.ParseInt(s, base, 64)
	if err != nil {
		return 0, fmt.Errorf("invalid value %q: %w", s, err)
	}
	return v, nil
}
