#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
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

// 現在着目しているトークン
Token *token;

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM) {
        error("数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

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
        if (isspace(*p)) {
            p += 1;
            continue;
        }

        if (*p == '+' || *p == '-') {
            current = new_token(TK_RESERVED, current, p);
            p += 1;
            continue;
        }

        if (isdigit(*p)) {
            current = new_token(TK_NUM, current, p);
            current->val = strtol(p, &p, 10);
            continue;
        }

        error("トークナイズできません");
    }

    new_token(TK_EOF, current, p);
    return head.next;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    char *p = argv[1];
    token = tokenize(p);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("\n");
    printf("main:\n");

    // 式の最初は数でなければならない。
    // それをチェックして最初のmov命令を出力。
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (token->kind == TK_RESERVED && token->str[0] == '+') {
            token = token->next;
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        if (token->kind == TK_RESERVED && token->str[0] == '-') {
            token = token->next;
            printf("    sub rax, %d\n", expect_number());
            continue;
        }

        error("予期しないトークンです");
    }

    printf("    ret\n");
    return 0;
}
