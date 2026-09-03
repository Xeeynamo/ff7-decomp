# Coding Style

```sh
# ensure codebase is formatted before submitting
./mako.sh format
```

## Coding convention

```c
#define SOME_MACRO

enum EnumName {
    /* 0x00 */ FIRST,
    /* 0x01 */ SECOND,
    /* 0x10 */ SUDDEN_JUMP = 0x10,
};

typedef struct {
    /* 0x1234 */ fieldOneTwoThree;
} SomeStruct; // size:0x5678

typedef mytype s32;

// Comments like /* this */ are only used to mark struct offsets or enum values. In addition, extern values must be
// declared by their offset in the .map file they come from.
extern s32 g_PublicVarFromSomewhereElse;
s32 g_PublicVarFromThisFile;
static mytype private_var; // short comment

static void FunctionLocalOnlyToTheSourcFile(s32 param) {
    int localVar;
    ...
}
void FunctionAccessibleWithinTheOverlay(s32 param1, u8* param2) {
    int localVar;
    ...
}
void MODULENAME_FunctionAccessibleOutsideTheOverlay(void) {
    ...
}
```

## Prohibited C keywords

* `register` use is strictly prohibited. It is just an assembly artifact, not true C code.
* `volatile` is generally discouraged, unless it's used on tight `while` loops


## Style patterns

The following rules must be followed, unless:
- Applying them provide worst matching code
- Codegen would diverge from matching the PC port

```c
// when you see this
if (var_s0 != 0) {
    do {
        ...
        var_s0++;
    } while (var_s0 < N);
}

// always attempt to rewrite it as a for loop
for (i = 0; i < N; i++) {
    ...
}
```

```c
// prefer to write comparison with zero value
if (ptr == 0) {
    ...
}
if (ptr != 0) {
    ...
}

// with this style instead
if (!ptr) {
    ...
}
if (ptr) {
    ...
}
```

```c
// when you see return in a switch, they're most likely not real
void Func(void) {
    switch (value) {
    case 0:
        return;
    case 1:
        return;
    case 2:
        return;
    }
}

// you should use break instead
void Func(void) {
    switch (value) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    }
}
```

```c
// when you see a return at the end of a function
void Func(void) {
    ...
    return;
}

// you must remove it
void Func(void) {
    ...
}
```

```c
// avoid big nested functions
void Func(void) {
    if (X >= 0) {
        if (Y != 2) {
            ...
        }
    }
}

// instead, evaluate if early returns are possible
void Func(void) {
    if (X < 0) {
        return;
    }
    if (Y == 2) {
        return;
    }
    ...
}
```

```c
// raw decompiled code can look hard to read
if ((u32)D_80163C7C - 4 < 2) {
    ...
}

// it's important to always attempt reconstructing the true author's intention
if (D_80163C7C > 3 && D_80163C7C < 6) {
    ...
}
```

```c
// overlapping data or arrays of 1 element are decompiler artifacts
extern s16 D_8000B006[];
extern s32 D_8000B008;
void func_80123456(s32 arg0) {
    if (*D_8000B006) {
        D_8000B008 = arg0;
    }
}

// consider instead using a struct, and remember to update symbols.{OVERLAY}.txt to reflect the new struct size on a
// symbol with D_8000B004 = 0x8000B004; // size:0x08
typedef struct {
    /* 0x00 */ s16 unk0;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s32 unk4;
} Unk8000B004; // size:0x08

Unk8000B004 D_8000B004;
void func_80123456(s32 arg0) {
    if (D_8000B004.unk2) {
        D_8000B004.unk4 = arg0;
    }
}
```
