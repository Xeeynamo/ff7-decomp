# Coding Style
FF7-decomp follows several basic coding conventions.

## Functions

Function names use **PascalCase**.

```c
void InitializeBattle(void);
void LoadCharacterModel(void);
```

## Globals

Global variables are prefixed with `g_`.

```c
s32 g_BattleState;
u8 g_CurrentDisc;
```

## Formatting and Linting

The project uses light formatting and linting tools.

Before submitting changes, please run:

```sh
./mako.sh format
```

Alternatively, you may use:

```sh
make submit
```

Please ensure that your code is properly formatted before opening a Pull Request.



## C Language Guidelines

### `volatile` and `register`

The use of the `volatile` and `register` keywords is discouraged.

These keywords should generally only be used when there is a specific and well-understood reason for doing so.

In particular, contributors should avoid using them as a general attempt to influence compiler output or achieve an assembly match.

### Comments

Traditional block comments are discouraged:

```c
/* This style is generally discouraged. */
```

Prefer C99-style comments instead:

```c
// This style is preferred.
```

Block comments may still be appropriate when required for documentation systems such as Doxygen.