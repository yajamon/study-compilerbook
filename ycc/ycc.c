#include <stdio.h>
#include <stdlib.h>

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

    printf("	ret\n");
    printf("\n");
    printf("_main:\n");
    printf("	call main\n");
    printf("	ret\n");

    return 0;
}
