#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TK_RESERVED,    // 記号
    TK_NUM,         // 数値
    TK_EOF,         // 終端
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token* next;
    int val;            // kind が TK_NUM の場合、その数値
    char* str;           // Tokenの文字列
};

Token* new_token(TokenKind kind, Token* current, char* str) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    current->next = tok;
    return tok;
}

Token* tokenize(char* p) {
    Token head;
    head.next = NULL;
    Token* current = &head;

    while (*p) {
        // 全部無視
        p++;
    }

    new_token(TK_EOF, current, p);
    return NULL;
}

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
