// str.c
// part of Final Fantasy VII Decomp
// converts a C-style string into a FF7-style string
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char map_ascii[0x80];
const char* map_utf8[] = {
    "\xB4"
    "‘",
    "\xB5"
    "’",
    "\xB4"
    "❛",
    "\xB5"
    "❜",
};

void init() {
    unsigned char ch;
    for (ch = ' '; ch <= '~'; ch++) {
        map_ascii[ch] = ch - 0x20;
    }
}

const char* remap_string(FILE* out, int padding, const char* str) {
    fputs("{", out);
    int first = 1;
    while (*str) {
        unsigned char ch = (unsigned char)*str++;
        padding--;
        if (!first) {
            fputs(", ", out);
        }
        first = 0;
        if (ch == '\\') {
            fprintf(out, "0x%02X", (unsigned char)*str++);
        } else if (ch == '"') {
            // FF7-style string terminator
            fputs("0xFF", out);
            break;
        } else if (ch >= 0x20 && ch < 0x80) {
            fprintf(out, "0x%02X", map_ascii[ch]);
        } else {
            size_t len;
            unsigned char re = 0;
            for (int i = 0; i < sizeof(map_utf8) / sizeof(void*); i++) {
                len = strlen(&map_utf8[i][1]);
                if (strncmp(str - 1, &map_utf8[i][1], len) == 0) {
                    re = map_utf8[i][0];
                    break;
                }
            }
            if (!re) {
                fprintf(stderr, "WARNING: unrecognized byte %02X\n", ch);
                break;
            } else {
                str += len - 1;
                fprintf(out, "0x%02X", re);
            }
        }
    }
    while (--padding > 0) {
        if (!first) {
            fputs(", ", out);
        }
        first = 0;
        fputs("0", out);
    }
    fputc('}', out);
    return str + 1; // skip the ')' character
}

void rewrite_line(FILE* out, const char* line) {
    // checks for all the `_S(x)` and `_S(len, x)` in the current line
    while (*line) {
        const char* begin = strstr(line, "_S");
        if (begin) {
            const char* next = begin;
            int padding = 0;
            if (strncmp(next, "_S(\"", 4) == 0) {
                // it seems to be a _S("")
                next += 4;
                padding = -1;
            } else if (strncmp(next, "_SL(", 4) == 0) {
                // it seems to be a _S(len, "")
                next += 4;
                if (next[0] == '0' && next[1] == 'x') {
                    next += 2;
                    padding = (int)strtol(next, NULL, 16);
                } else {
                    padding = (int)strtol(next, NULL, 10);
                }
                if (padding) {
                    next = strchr(next, ',') + 1;
                    if (next) {
                        next = strchr(next, '"') + 1;
                    }
                }
            }
            if (padding) {
                // padding is non-zero, we probably have a valid macro
                fwrite(line, 1, begin - line, out);
                line = remap_string(out, padding, next);
                continue; // catch more macro at the same line
            }
        }
        // write the rest of the string and returns
        fputs(line, out);
        return;
    }
}

int rewrite_file(FILE* out, FILE* in) {
    char buf[0x2000];
    while (fgets(buf, sizeof(buf), in)) {
        rewrite_line(out, buf);
    }
    return 0;
}

int preview(const char* file_name, const char* str_offset) {
    if (!file_name || !str_offset || !*str_offset) {
        return 1;
    }
    if (str_offset[0] == '0' && str_offset[1] == 'x') {
        str_offset += 2;
    }

    FILE* f = fopen(file_name, "rb");
    if (!f) {
        fprintf(stderr, "failed to open %s\n", file_name);
        return 1;
    }
    fseek(f, strtol(str_offset, NULL, 16), SEEK_SET);
    while (1) {
        int ch = fgetc(f);
        if (ch == -1) {
            fprintf(stderr, "EOF\n");
            break;
        }
        if (ch == 0xFF) {
            break;
        }
        if (ch >= 0x60) {
            fprintf(stderr, "unrecognized byte %02X\n", ch);
            break;
        } else {
            fputc(ch + 0x20, stdout);
        }
    }
    fputc('\n', stdout);
    fclose(f);
    return 0;
}

int test_str(const char* str, const char* expected) {
    char actual[0x2000];
    FILE* stream = fmemopen(actual, sizeof(actual), "w");
    if (!stream) {
        perror("fmemopen failed");
        return -1;
    }
    rewrite_line(stream, str);
    fclose(stream);
    if (strcmp(actual, expected) != 0) {
        printf("[FAIL] %s\n", str);
        printf("       expected: %s\n", expected);
        printf("       actual  : %s\n", actual);
        return 1;
    }
    printf("[PASS] %s\n", str);
    return 0;
}

int test() {
    int r = 0;
    r |= test_str("ignore me", "ignore me");
    r |= test_str("char str[] = _S(\"Hello FF7\");",
                  "char str[] = {0x28, 0x45, 0x4C, 0x4C, 0x4F, 0x00, "
                  "0x26, 0x26, 0x17, 0xFF};");
    r |= test_str("char str[] = _SL(10, \"test\");",
                  "char str[] = {0x54, 0x45, 0x53, 0x54, 0xFF, 0, 0, 0, 0};");
    r |= test_str("char str[] = _SL(0xA, \"test\");",
                  "char str[] = {0x54, 0x45, 0x53, 0x54, 0xFF, 0, 0, 0, 0};");
    r |= test_str("_S(99, \"invalid\")", "_S(99, \"invalid\")");
    r |= test_str("_SL(0, \"invalid\")", "_SL(0, \"invalid\")");
    r |= test_str("_SL(\"invalid\")", "_SL(\"invalid\")");
    r |= test_str("_SL(1, \"ok\")", "{0x4F, 0x4B, 0xFF}");
    r |= test_str("_S(\"aa'bb\")", "{0x41, 0x41, 0x07, 0x42, 0x42, 0xFF}");
    r |= test_str("_S(\"‘’❛❜\")", "{0xB4, 0xB5, 0xB4, 0xB5, 0xFF}");
    return r;
}

int main(int argc, char* argv[]) {
    init();
    if (argc == 1) {
        // TTY-mode
        return rewrite_file(stdout, stdin);
    } else if (argc == 3) {
        return preview(argv[1], argv[2]);
    } else if (argc == 2 && !strcmp(argv[1], "--test")) {
        return test();
    } else {
        printf("Final Fantasy VII string conversion tool\n");
        printf("Usage\n");
        printf("    cat <in> | bin/str > <out>\n");
        printf("    bin/str <binary> 0x<offset>\n");
        return 0;
    }
}
