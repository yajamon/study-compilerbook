#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TK_RESERVED,    // 記号
    TK_NUM,         // 数値
    TK_EOF,         // 終端
} TokenKind;

struct Token {
    TokenKind kind;
    int val;            // kind が TK_NUM の場合、その数値
    char* str;           // Tokenの文字列
};

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("\n");
    printf("main:\n");
    printf("    mov rax, %ld\n", strtol(p, &p, 10));

    while (*p) {
        if (*p == '+') {
            p++;
            printf("    add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        if (*p == '-') {
            p++;
            printf("    sub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "予期しない文字です: '%c'\n", *p);
        return 1;
    }

    printf("    ret\n");
    return 0;
}
