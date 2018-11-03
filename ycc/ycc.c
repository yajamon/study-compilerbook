#include <stdio.h>
#include <stdlib.h>

// トークンの型を表す値
enum {
    TK_NUM = 256,   // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン
};

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の数が正しくありません\n");
        return 1;
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".global main, _main\n");
    printf("\n");
    printf("main:\n");

    printf("	mov rax, %ld\n", strtol(p, &p, 10));

    while (*p) {
        if (*p == '+') {
            p++;
            printf("	add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        if (*p == '-') {
            p++;
            printf("	sub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "予期しない文字を検出しました: '%c'\n", *p);
        return 1;
    }

    printf("	ret\n");
    printf("\n");
    printf("_main:\n");
    printf("	call main\n");
    printf("	ret\n");

    return 0;
}
