package symbols

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/utils"
)

type symbolEntry struct {
	offset  uint32
	name    string
	comment string
}

// ParseLine parses one "name = 0xADDR; comment" symbol_addrs line.
func ParseLine(line string) (name string, addr uint32, comment string, err error) {
	split := strings.SplitN(line, "=", 2)
	if len(split) != 2 {
		return "", 0, "", fmt.Errorf("invalid symbol entry: %s", line)
	}
	split2 := strings.SplitN(split[1], ";", 2)
	if len(split2) != 2 {
		return "", 0, "", fmt.Errorf("invalid symbol entry: %s", line)
	}
	offset, err := utils.ParseDigit(split2[0])
	if err != nil {
		return "", 0, "", fmt.Errorf("invalid symbol offset at %s: %w", line, err)
	}
	return strings.TrimSpace(split[0]), uint32(offset), strings.TrimSpace(split2[1]), nil
}

func Sort(path string) error {
	f, err := os.Open(path)
	if err != nil {
		return err
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)
	symbols := map[uint32]symbolEntry{}
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		name, offset, comment, err := ParseLine(line)
		if err != nil {
			return err
		}
		symbols[offset] = symbolEntry{
			offset:  offset,
			name:    name,
			comment: comment,
		}
	}
	if err := scanner.Err(); err != nil {
		return err
	}
	offsets := make([]uint32, 0, len(symbols))
	for offset := range symbols {
		offsets = append(offsets, offset)
	}
	sort.Slice(offsets, func(i, j int) bool {
		return offsets[i] < offsets[j]
	})

	_ = f.Close()
	f, err = os.OpenFile(path, os.O_WRONLY|os.O_TRUNC|os.O_CREATE, 0644)
	if err != nil {
		return err
	}
	defer f.Close()

	writer := bufio.NewWriter(f)
	for _, offset := range offsets {
		entry := symbols[offset]
		var line string
		if entry.comment != "" {
			line = fmt.Sprintf("%s = 0x%08X; %s\n", entry.name, entry.offset, entry.comment)
		} else {
			line = fmt.Sprintf("%s = 0x%08X;\n", entry.name, entry.offset)
		}
		if _, err := writer.WriteString(line); err != nil {
			return err
		}
	}
	return writer.Flush()
}
