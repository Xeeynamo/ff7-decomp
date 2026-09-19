package lint

import (
	"bufio"
	"bytes"
	"regexp"
	"strconv"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
)

// Decl is one file-scope declaration seen while scanning a preprocessed translation unit.
type Decl struct {
	Name     string
	TypeName string
	Dims     []string // one entry per [..] pair, in source order
	Unsized  bool     // true when the first dimension was empty ([])
	File     string
	Line     int
	Static   bool
}

// StructDef is a typedef struct {...} Name; body, used only to report field paths.
// File is the .c translation unit it was seen in, so a later field-offset probe can
// #include the same file the type was actually visible from.
type StructDef struct {
	Name   string
	Fields []string
	File   string
}

var lineMarkerRe = regexp.MustCompile(`^# (\d+) "([^"]*)"`)

// declRe matches one single-line file-scope declaration or definition, terminated by
// the same line it starts on. Function declarations and pointers-to-function are
// filtered out by the caller because they contain '(' before the terminator.
var declRe = regexp.MustCompile(
	`^(extern\s+)?(static\s+)?(const\s+)?(struct\s+\w+|\w+)\s*(\**)\s*(_?[A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)\s*(=|;)`)

// preprocess runs the cross preprocessor over src with the flags gen.py uses for the
// psx-cc rule, minus -DUSE_INCLUDE_ASM and -Wall so INCLUDE_ASM(...) expands to nothing
// and warnings do not clutter probe output.
func preprocess(src string) ([]byte, error) {
	return deps.Cpp("-Iinclude", "-Iinclude/psxsdk", "-DFF7_STR", "-lang-c", "-undef", "-fno-builtin", src)
}

// scanDecls walks preprocessed text, tracking # LINE "file" markers and brace depth,
// and collects every file-scope (depth 0) declaration plus every typedef struct body.
// tuPath is the .c file that was preprocessed, stamped onto each StructDef so a later
// field-offset probe can #include the same file the type was actually visible from.
func scanDecls(pre []byte, tuPath string) ([]Decl, []StructDef) {
	var decls []Decl
	var structs []StructDef
	byName := map[string]int{} // name -> index into decls, to merge duplicates in one TU

	file := ""
	line := 0
	depth := 0

	// struct-body tracking: when we enter a `typedef struct {` at depth 0, we record
	// its opening depth and collect field names declared at exactly one level deeper.
	inTypedefStruct := false
	structOpenDepth := 0
	var curFields []string

	scanner := bufio.NewScanner(bytes.NewReader(pre))
	scanner.Buffer(make([]byte, 0, 64*1024), 8*1024*1024)
	for scanner.Scan() {
		raw := scanner.Text()
		if m := lineMarkerRe.FindStringSubmatch(raw); m != nil {
			n, _ := strconv.Atoi(m[1])
			line = n - 1 // the marker announces the *next* line's number
			file = m[2]
			continue
		}
		line++

		trimmed := strings.TrimSpace(raw)

		if depth == 0 && strings.HasPrefix(trimmed, "typedef struct") && strings.Contains(trimmed, "{") {
			inTypedefStruct = true
			structOpenDepth = depth
			curFields = nil
		} else if inTypedefStruct && depth == structOpenDepth+1 {
			if fm := structFieldRe.FindStringSubmatch(trimmed); fm != nil {
				curFields = append(curFields, fm[1])
			}
		}

		openCount := strings.Count(raw, "{")
		closeCount := strings.Count(raw, "}")

		if inTypedefStruct && closeCount > 0 && depth+openCount-closeCount <= structOpenDepth {
			if nm := structTypedefNameRe.FindStringSubmatch(trimmed); nm != nil {
				structs = append(structs, StructDef{Name: nm[1], Fields: curFields, File: tuPath})
			}
			inTypedefStruct = false
		}

		depth += openCount - closeCount

		if depth != 0 {
			continue
		}
		if strings.Contains(trimmed, "(") {
			continue // function declarations/definitions and function-pointer arrays
		}
		if strings.HasPrefix(trimmed, "typedef") || strings.HasPrefix(trimmed, "return") {
			continue
		}
		m := declRe.FindStringSubmatch(trimmed)
		if m == nil {
			continue
		}
		isStatic := m[2] != ""
		typeName := strings.TrimSpace(m[4])
		name := m[6]
		dimsStr := m[7]

		dims, unsized := parseDims(dimsStr)

		d := Decl{
			Name:     name,
			TypeName: typeName,
			Dims:     dims,
			Unsized:  unsized,
			File:     file,
			Line:     line,
			Static:   isStatic,
		}
		if idx, ok := byName[name]; ok {
			decls[idx] = d
		} else {
			byName[name] = len(decls)
			decls = append(decls, d)
		}
	}
	return decls, structs
}

var structFieldRe = regexp.MustCompile(`^\s*(?:const\s+)?[A-Za-z_]\w*\s*\**\s*([A-Za-z_]\w*)\s*(?:\[[^\]]*\])*\s*;`)
var structTypedefNameRe = regexp.MustCompile(`\}\s*([A-Za-z_]\w*)\s*;`)

// parseDims splits a run of [..] groups into their raw contents. The first group empty
// (as in T x[];) marks the declaration as unsized (element size only, a lower bound).
func parseDims(s string) (dims []string, unsized bool) {
	for _, part := range regexp.MustCompile(`\[([^\]]*)\]`).FindAllStringSubmatch(s, -1) {
		dims = append(dims, strings.TrimSpace(part[1]))
	}
	if len(dims) > 0 && dims[0] == "" {
		unsized = true
	}
	return dims, unsized
}

// evalDim evaluates a preprocessed array-dimension expression, which after macro
// expansion is a parenthesized sum/product of integer literals such as
// "(((3) + 1) + (6))". Returns ok=false for anything it cannot evaluate (an
// identifier that did not expand, for instance), in which case the caller should
// treat the array as unsized rather than guess.
func evalDim(expr string) (int64, bool) {
	expr = strings.TrimSpace(expr)
	if expr == "" {
		return 0, false
	}
	toks := dimTokenRe.FindAllString(expr, -1)
	if strings.Join(toks, "") != strings.ReplaceAll(strings.ReplaceAll(expr, " ", ""), "\t", "") {
		return 0, false
	}
	p := &dimParser{toks: toks}
	v, ok := p.parseExpr()
	if !ok || p.pos != len(p.toks) {
		return 0, false
	}
	return v, true
}

var dimTokenRe = regexp.MustCompile(`0[xX][0-9A-Fa-f]+|\d+|[()+\-*]`)

type dimParser struct {
	toks []string
	pos  int
}

func (p *dimParser) peek() string {
	if p.pos >= len(p.toks) {
		return ""
	}
	return p.toks[p.pos]
}

func (p *dimParser) parseExpr() (int64, bool) {
	v, ok := p.parseTerm()
	if !ok {
		return 0, false
	}
	for p.peek() == "+" || p.peek() == "-" {
		op := p.toks[p.pos]
		p.pos++
		rhs, ok := p.parseTerm()
		if !ok {
			return 0, false
		}
		if op == "+" {
			v += rhs
		} else {
			v -= rhs
		}
	}
	return v, true
}

func (p *dimParser) parseTerm() (int64, bool) {
	v, ok := p.parseFactor()
	if !ok {
		return 0, false
	}
	for p.peek() == "*" {
		p.pos++
		rhs, ok := p.parseFactor()
		if !ok {
			return 0, false
		}
		v *= rhs
	}
	return v, true
}

func (p *dimParser) parseFactor() (int64, bool) {
	tok := p.peek()
	if tok == "(" {
		p.pos++
		v, ok := p.parseExpr()
		if !ok || p.peek() != ")" {
			return 0, false
		}
		p.pos++
		return v, true
	}
	if tok == "" {
		return 0, false
	}
	p.pos++
	v, err := strconv.ParseInt(tok, 0, 64)
	if err != nil {
		return 0, false
	}
	return v, true
}
