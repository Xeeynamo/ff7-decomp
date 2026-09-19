package lint

import (
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"strconv"
	"strings"

	"github.com/xeeynamo/ff7-decomp/tools/builder/deps"
)

// Size is the byte size of one symbol as measured by the real compiler.
type Size struct {
	Bytes      uint32
	LowerBound bool // true when only sizeof(NAME[0]) could be measured
}

var commRe = regexp.MustCompile(`\.comm\s+__lint_sz_(\w+),\s*(\d+)`)
var stderrLineRe = regexp.MustCompile(`probe\.c:(\d+):`)

// probeSizes compiles a probe translation unit that #includes src and takes sizeof()
// of every decl, then reads back the emitted .comm directives to learn each symbol's
// real size exactly as the game's own compiler lays it out. cc1's exit code is
// ignored on purpose: real prototype conflicts in a few files make it exit 33 while
// still emitting full assembly, and verbose echoes its diagnostics either way.
func probeSizes(compiler, src string, decls []Decl, verbose bool) (map[string]Size, error) {
	if len(decls) == 0 {
		return map[string]Size{}, nil
	}

	absSrc, err := filepath.Abs(src)
	if err != nil {
		return nil, err
	}

	dir, err := os.MkdirTemp("", "ff7lint")
	if err != nil {
		return nil, err
	}
	defer os.RemoveAll(dir)

	probePath := filepath.Join(dir, "probe.c")
	var b strings.Builder
	fmt.Fprintf(&b, "#include \"%s\"\n", absSrc)
	// probeLine maps a 1-based line in this generated body (line N below is the Nth
	// probe emitted) back to the Decl it measures, for stderr-based drop-outs.
	probeLine := map[int]string{}
	lineNo := 1 // the #include line above
	for _, d := range decls {
		lineNo++
		if d.Unsized {
			fmt.Fprintf(&b, "char __lint_sz_%s[sizeof(%s[0])];\n", d.Name, d.Name)
		} else {
			fmt.Fprintf(&b, "char __lint_sz_%s[sizeof(%s)];\n", d.Name, d.Name)
		}
		probeLine[lineNo] = d.Name
	}
	if err := os.WriteFile(probePath, []byte(b.String()), 0644); err != nil {
		return nil, err
	}

	pre, err := deps.Cpp("-Iinclude", "-Iinclude/psxsdk", "-DFF7_STR", "-lang-c", "-undef", "-fno-builtin", probePath)
	if err != nil {
		return nil, err
	}
	converted, err := deps.Str(pre)
	if err != nil {
		return nil, err
	}
	stdout, stderr, _ := deps.Cc1(compiler, converted, "-quiet", "-O2", "-G0", "-mcpu=3000", "-mgas", "-o", "-")
	if verbose && len(stderr) > 0 {
		fmt.Fprintf(os.Stderr, "lint: %s:\n%s", src, stderr)
	}

	dropped := map[string]bool{}
	for _, m := range stderrLineRe.FindAllStringSubmatch(string(stderr), -1) {
		n, _ := strconv.Atoi(m[1])
		if name, ok := probeLine[n]; ok {
			dropped[name] = true
		}
	}

	sizes := map[string]Size{}
	for _, m := range commRe.FindAllStringSubmatch(string(stdout), -1) {
		name := m[1]
		if dropped[name] {
			continue
		}
		n, err := strconv.ParseUint(m[2], 10, 32)
		if err != nil {
			continue
		}
		if n == 0 {
			continue
		}
		byName := findDecl(decls, name)
		sizes[name] = Size{Bytes: uint32(n), LowerBound: byName.Unsized}
	}

	if len(sizes) == 0 && len(stdout) == 0 {
		return nil, fmt.Errorf("probe compile produced no output:\n%s", stderr)
	}
	return sizes, nil
}

func findDecl(decls []Decl, name string) Decl {
	for _, d := range decls {
		if d.Name == name {
			return d
		}
	}
	return Decl{}
}

// FieldLayout is one field's real offset and size inside its struct, as measured by
// the compiler rather than assumed from source order (padding, bitfields, alignment).
type FieldLayout struct {
	Offset uint32
	Size   uint32
}

var offRe = regexp.MustCompile(`\.comm\s+__lint_off_(\w+),\s*(\d+)`)

// probeFieldOffsets measures every field of the named struct type declared in src,
// using the same #include-and-sizeof trick as probeSizes but with
// (int)&((T*)0)->field for the offset (the +1 keeps a zero offset out of a
// zero-length .comm, which some assemblers drop).
func probeFieldOffsets(compiler, src, typeName string, def StructDef) (map[string]FieldLayout, error) {
	if len(def.Fields) == 0 {
		return map[string]FieldLayout{}, nil
	}

	absSrc, err := filepath.Abs(src)
	if err != nil {
		return nil, err
	}
	dir, err := os.MkdirTemp("", "ff7lint")
	if err != nil {
		return nil, err
	}
	defer os.RemoveAll(dir)

	probePath := filepath.Join(dir, "probe.c")
	var b strings.Builder
	fmt.Fprintf(&b, "#include \"%s\"\n", absSrc)
	for _, field := range def.Fields {
		tag := typeName + "__" + field
		fmt.Fprintf(&b, "char __lint_off_%s[(int)&((%s*)0)->%s + 1];\n", tag, typeName, field)
		fmt.Fprintf(&b, "char __lint_sz_%s[sizeof(((%s*)0)->%s)];\n", tag, typeName, field)
	}
	if err := os.WriteFile(probePath, []byte(b.String()), 0644); err != nil {
		return nil, err
	}

	pre, err := deps.Cpp("-Iinclude", "-Iinclude/psxsdk", "-DFF7_STR", "-lang-c", "-undef", "-fno-builtin", probePath)
	if err != nil {
		return nil, err
	}
	converted, err := deps.Str(pre)
	if err != nil {
		return nil, err
	}
	stdout, _, _ := deps.Cc1(compiler, converted, "-quiet", "-O2", "-G0", "-mcpu=3000", "-mgas", "-o", "-")

	offsets := map[string]uint32{}
	for _, m := range offRe.FindAllStringSubmatch(string(stdout), -1) {
		n, err := strconv.ParseUint(m[2], 10, 32)
		if err != nil || n == 0 {
			continue
		}
		offsets[m[1]] = uint32(n) - 1
	}
	sizes := map[string]uint32{}
	for _, m := range commRe.FindAllStringSubmatch(string(stdout), -1) {
		n, err := strconv.ParseUint(m[2], 10, 32)
		if err != nil {
			continue
		}
		sizes[m[1]] = uint32(n)
	}

	layout := map[string]FieldLayout{}
	for _, field := range def.Fields {
		tag := typeName + "__" + field
		off, hasOff := offsets[tag]
		sz, hasSz := sizes[tag]
		if !hasSz {
			continue
		}
		if !hasOff {
			off = 0
		}
		layout[field] = FieldLayout{Offset: off, Size: sz}
	}
	return layout, nil
}
