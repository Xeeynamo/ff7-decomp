package rank

import (
	"bufio"
	"fmt"
	"math"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/builder"
)

type entry struct {
	score float32
	name  string
}

func Rank(path string, minThreshold float32) error {
	path = strings.TrimPrefix(path, "src/")
	path = strings.TrimSuffix(path, ".c")
	if !strings.HasPrefix(path, "asm/") {
		path = filepath.Join("asm", builder.Version(), path)
	}
	if _, err := os.Stat(path); os.IsNotExist(err) {
		targetedPath := filepath.Join(filepath.Dir(path), "nonmatchings", filepath.Base(path))
		if _, err := os.Stat(targetedPath); !os.IsNotExist(err) {
			path = targetedPath
		}
	} else {
		targetedPath := filepath.Join(path, "nonmatchings")
		if _, err := os.Stat(targetedPath); !os.IsNotExist(err) {
			path = targetedPath
		}
	}
	var entries []entry
	if err := filepath.Walk(path, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !strings.Contains(path, "nonmatchings") {
			return nil
		}
		if !strings.HasSuffix(path, ".s") {
			return nil
		}
		score, err := rankFunction(path)
		if err != nil {
			return err
		}
		if score < minThreshold {
			return nil
		}
		entries = append(entries, entry{score, filepath.Base(path)})
		return nil
	}); err != nil {
		return err
	}
	sort.Slice(entries, func(i, j int) bool {
		return entries[i].score < entries[j].score
	})
	for _, entry := range entries {
		fmt.Printf("%.3f: %s\n", entry.score, entry.name)
	}
	return nil
}

// NOTE: decompilationDifficultyScore and rankFunction are directly converted
// from https://github.com/cdlewis/snowboardkids2-decomp/blob/main/CLAUDE.md
// all rights reserved to the original author https://github.com/cdlewis
// Read more in this article from the same author:
// https://blog.chrislewis.au/the-unexpected-effectiveness-of-one-shot-decompilation-with-claude/

// decompilationDifficultyScore calculates the ML-based difficulty score
// Based on the Python implementation's logistic regression model
func decompilationDifficultyScore(instructions, branches, jumps, labels int) float32 {
	// Standardization parameters (from training)
	means := []float64{34.27065527065527, 1.6666666666666667, 3.1880341880341883, 1.98005698005698}
	stds := []float64{24.763225638334454, 2.047860394102145, 3.200600790997309, 2.3803926026229827}

	// Model coefficients
	coefficients := []float64{2.499706543629367, -0.46648920346754463, -1.61606926820365, 0.4911494991317799}
	intercept := -0.5155412977000488

	// Calculate score
	features := []float64{float64(instructions), float64(branches), float64(jumps), float64(labels)}

	// Standardize features
	logit := intercept
	for i := 0; i < 4; i++ {
		featuresScaled := (features[i] - means[i]) / stds[i]
		logit += featuresScaled * coefficients[i]
	}

	// Sigmoid function
	difficulty := 1.0 / (1.0 + math.Exp(-logit))
	return float32(difficulty)
}

func rankFunction(path string) (float32, error) {
	// Extract function name from file
	filename := filepath.Base(path)
	funcName := strings.TrimSuffix(filename, ".s")

	// Skip non-code sections (data, bss, rodata, header)
	if strings.HasPrefix(funcName, "jtbl_") ||
		strings.HasPrefix(funcName, "D_") ||
		strings.HasSuffix(funcName, ".data") ||
		strings.HasSuffix(funcName, ".bss") ||
		strings.HasSuffix(funcName, ".rodata") ||
		funcName == "header" {
		return 0, nil
	}

	// Open and read file
	file, err := os.Open(path)
	if err != nil {
		return 0, err
	}
	defer file.Close()

	// Compile regex patterns
	branchPattern := regexp.MustCompile(`\b(beq|bne|bnez|beqz|blez|bgtz|bltz|bgez|blt|bgt|ble|bge|bltzal|bgezal)\b`)
	jumpJalPattern := regexp.MustCompile(`\bjal\b`)
	jumpJPattern := regexp.MustCompile(`\bj\b`)
	labelPattern := regexp.MustCompile(`^\s*\.L[0-9A-Fa-f_]+:`)
	instructionPattern := regexp.MustCompile(`/\*\s*[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s*\*/`)

	instructionCount := 0
	branchCount := 0
	jumpCount := 0
	labelCount := 0

	// Read file content to check for rodata-only files
	scanner := bufio.NewScanner(file)
	var content strings.Builder
	for scanner.Scan() {
		content.WriteString(scanner.Text())
		content.WriteString("\n")
	}
	if err := scanner.Err(); err != nil {
		return 0, err
	}

	contentStr := content.String()

	// Skip files that only contain rodata (no actual code)
	if strings.Contains(contentStr, ".section .rodata") && !strings.Contains(contentStr, "glabel") {
		return 0, nil
	}

	// Parse line by line
	scanner = bufio.NewScanner(strings.NewReader(contentStr))
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())

		// Skip empty lines, glabel, endlabel, and comments
		if line == "" ||
			strings.HasPrefix(line, "glabel") ||
			strings.HasPrefix(line, "endlabel") ||
			strings.HasPrefix(line, "nonmatching") {
			continue
		}

		// Count local labels
		if labelPattern.MatchString(line) {
			labelCount++
			continue
		}

		// Count instructions
		if instructionPattern.MatchString(line) {
			instructionCount++

			// Check for branches
			if branchPattern.MatchString(line) {
				branchCount++
			}

			// Check for jumps
			if jumpJalPattern.MatchString(line) || jumpJPattern.MatchString(line) {
				jumpCount++
			}
		}
	}

	if err := scanner.Err(); err != nil {
		return 0, err
	}

	// Skip functions with 0 instructions (data sections)
	if instructionCount == 0 {
		return 0, nil
	}

	// Calculate difficulty score
	score := decompilationDifficultyScore(instructionCount, branchCount, jumpCount, labelCount)
	return score, nil
}
