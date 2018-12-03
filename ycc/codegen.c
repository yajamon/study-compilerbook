#include "ycc.h"

void gen_lvalue(Node *node) {
    if (node->type_code == ND_IDENT) {
        printf("	mov rax, rbp\n");
        printf("	sub rax, %d\n",
                ('z' - node->name + 1) * 8);
        printf("	push rax\n");
        return;
    }
    error("代入の左辺値が変数ではありません", "");
}

void gen(Node *node) {
    if (node->type_code == ND_NUM) {
        printf("	push %d\n", node->value);
        return;
    }

    if (node->type_code == ND_IDENT) {
        gen_lvalue(node);
        printf("	pop rax\n");
        printf("	mov rax, [rax]\n");
        printf("	push rax\n");
        return;
    }

    if (node->type_code == '=') {
        // 変数のアドレスも、右辺値もとりあえずスタックに積む
        gen_lvalue(node->lhs);
        gen(node->rhs);

        // rax(左辺値)のアドレスにrdi(右辺値)を書き込む
        printf("	pop rdi\n");
        printf("	pop rax\n");
        printf("	mov [rax], rdi\n");
        // 右辺値としての値を返してあげないと、a=b=1が成立しなくなる
        printf("	push rdi\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("	pop rdi\n");
    printf("	pop rax\n");

    switch (node->type_code) {
        case '+':
            printf("	add rax, rdi\n");
            break;
        case '-':
            printf("	sub rax, rdi\n");
            break;
        case '*':
            printf("	mul rdi\n");
            break;
        case '/':
            printf("	mov rdx, 0\n");
            printf("	div rdi\n");
            break;
    }
    printf("	push rax\n");
}
