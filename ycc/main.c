#include "ycc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の数が正しくありません\n");
        return 1;
    }

    tokenize(argv[1]);
    // パース。結果は code に保存される
    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main, _main\n");
    printf("\n");
    printf("main:\n");

    // プロローグ
    // 変数26個分の領域を確保する
    printf("	push rbp\n");
    printf("	mov rbp, rsp\n");
    printf("	sub rsp, 208\n"); // 8 * 26 = 208

    // 先頭の式から順にコード生成
    for (int i=0; code[i]; i++){
        gen(code[i]);

        // 式の評価結果としてスタックに1つの値が残る
        // これが積み重なって溢れないよう、ポップしておく
        printf("	pop rax\n");
    }

    // エピローグ
    // 最後の式の結果がraxに残っているので、それが返り値となる
    printf("	mov rsp, rbp\n");
    printf("	pop rbp\n");

    printf("	ret\n");
    printf("\n");
    printf("_main:\n");
    printf("	call main\n");
    printf("	ret\n");

    return 0;
}
