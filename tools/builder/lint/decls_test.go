package lint

import "testing"

func declByName(decls []Decl, name string) (Decl, bool) {
	for _, d := range decls {
		if d.Name == name {
			return d, true
		}
	}
	return Decl{}, false
}

func TestScanDeclsShapes(t *testing.T) {
	src := `# 1 "src/battle/batini.c"
extern s8 D_80062FFC = 0;
extern struct FieldRain g_FieldRain[];
extern DB* g_cDb;
extern void (*D_x[4])(s32);
extern u8 D_800F6934[0x40][8];
extern s16 D_80075D08[];
u8 D_80062DFC = 0x40;
static s8 _D_80062DFE;
`
	decls, _ := scanDecls([]byte(src), "src/battle/batini.c")

	cases := []struct {
		name     string
		wantType string
		wantDims []string
		unsized  bool
		static   bool
	}{
		{"D_80062FFC", "s8", nil, false, false},
		{"g_FieldRain", "struct FieldRain", []string{""}, true, false},
		{"g_cDb", "DB", nil, false, false},
		{"D_800F6934", "u8", []string{"0x40", "8"}, false, false},
		{"D_80075D08", "s16", []string{""}, true, false},
		{"D_80062DFC", "u8", nil, false, false},
		{"_D_80062DFE", "s8", nil, false, true},
	}
	for _, c := range cases {
		d, ok := declByName(decls, c.name)
		if !ok {
			t.Errorf("%s: not found among decls", c.name)
			continue
		}
		if d.TypeName != c.wantType {
			t.Errorf("%s: type = %q, want %q", c.name, d.TypeName, c.wantType)
		}
		if len(d.Dims) != len(c.wantDims) {
			t.Errorf("%s: dims = %v, want %v", c.name, d.Dims, c.wantDims)
		}
		if d.Unsized != c.unsized {
			t.Errorf("%s: unsized = %v, want %v", c.name, d.Unsized, c.unsized)
		}
		if d.Static != c.static {
			t.Errorf("%s: static = %v, want %v", c.name, d.Static, c.static)
		}
	}

	if _, ok := declByName(decls, "D_x"); ok {
		t.Errorf("function-pointer array D_x should have been skipped")
	}
}

func TestScanDeclsLineTracking(t *testing.T) {
	src := `# 10 "src/battle/battle.h"
extern u8 D_800F6934[0x40][8];
extern s8 D_800F6936[0x40][8];
# 6 "src/battle/batini.c"
extern u8 D_800F6934[0x40][8];
`
	decls, _ := scanDecls([]byte(src), "src/battle/batini.c")

	d, ok := declByName(decls, "D_800F6936")
	if !ok {
		t.Fatalf("D_800F6936 not found")
	}
	if d.File != "src/battle/battle.h" || d.Line != 11 {
		t.Errorf("D_800F6936: got %s:%d, want src/battle/battle.h:11", d.File, d.Line)
	}

	// D_800F6934 is redeclared from a later # marker; the later occurrence must win.
	d, ok = declByName(decls, "D_800F6934")
	if !ok {
		t.Fatalf("D_800F6934 not found")
	}
	if d.File != "src/battle/batini.c" || d.Line != 6 {
		t.Errorf("D_800F6934: got %s:%d, want src/battle/batini.c:6", d.File, d.Line)
	}
}

func TestScanDeclsStructBody(t *testing.T) {
	src := `# 1 "src/battle/battle.h"
typedef struct {
    s8 D_801636B8;
    u8 D_801636B9;
    s16 D_801636BE;
} Unk801636B8;
`
	_, structs := scanDecls([]byte(src), "src/battle/batini.c")
	if len(structs) != 1 {
		t.Fatalf("expected one struct def, got %d", len(structs))
	}
	def := structs[0]
	if def.Name != "Unk801636B8" {
		t.Errorf("struct name = %q, want Unk801636B8", def.Name)
	}
	want := []string{"D_801636B8", "D_801636B9", "D_801636BE"}
	if len(def.Fields) != len(want) {
		t.Fatalf("fields = %v, want %v", def.Fields, want)
	}
	for i, f := range want {
		if def.Fields[i] != f {
			t.Errorf("field %d = %q, want %q", i, def.Fields[i], f)
		}
	}
	if def.File != "src/battle/batini.c" {
		t.Errorf("struct File = %q, want the TU path", def.File)
	}
}

func TestEvalDim(t *testing.T) {
	cases := []struct {
		expr string
		want int64
		ok   bool
	}{
		{"8", 8, true},
		{"0x40", 0x40, true},
		{"(((3) + 1) + (6))", 10, true},
		{"NUM_BATTLE_ACTOR", 0, false},
		{"", 0, false},
	}
	for _, c := range cases {
		got, ok := evalDim(c.expr)
		if ok != c.ok {
			t.Errorf("evalDim(%q) ok = %v, want %v", c.expr, ok, c.ok)
			continue
		}
		if ok && got != c.want {
			t.Errorf("evalDim(%q) = %d, want %d", c.expr, got, c.want)
		}
	}
}

// A qualifier run must not be mistaken for the type. Before this was handled,
// "extern volatile s32 D_8009D268[];" scanned as a symbol literally named "s32",
// so the real symbol never got an address and its overlap went unreported.
func TestScanDeclsQualifiedDecls(t *testing.T) {
	src := `# 1 "include/game.h"
extern volatile s32 D_8009D268[];
extern const volatile u8 D_8009D269;
extern const u16 D_8009D26A[4];
`
	decls, _ := scanDecls([]byte(src), "include/game.h")

	for _, c := range []struct {
		name     string
		typeName string
		unsized  bool
	}{
		{"D_8009D268", "s32", true},
		{"D_8009D269", "u8", false},
		{"D_8009D26A", "u16", false},
	} {
		d, ok := declByName(decls, c.name)
		if !ok {
			t.Errorf("%s was not scanned", c.name)
			continue
		}
		if d.TypeName != c.typeName {
			t.Errorf("%s TypeName = %q, want %q", c.name, d.TypeName, c.typeName)
		}
		if d.Unsized != c.unsized {
			t.Errorf("%s Unsized = %v, want %v", c.name, d.Unsized, c.unsized)
		}
	}
	if _, ok := declByName(decls, "s32"); ok {
		t.Error("the qualifier run was parsed as the type, leaving a decl named s32")
	}
}
