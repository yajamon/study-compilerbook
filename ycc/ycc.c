#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// エラー報告関数
void error(char* format, char* param) {
    if (param != NULL) {
        fprintf(stderr, format, param);
    } else {
        fprintf(stderr, format);
    }
    exit(1);
}

// トークンの型を表す値
enum {
    TK_NUM = 256,   // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
    int type_code;  // トークンの型を表す値
    int value;      // type_codeがTK_NUMの場合、その数値
    char *input;    // トークン文字列 (エラーメッセージ)
} Token;

// トークナイズした結果のトークン列を保存する配列
// 100個以上のトークンはこないものとする
Token tokens[100];

// pが指している文字列をトークンに分割してtokensに保存する
void tokenize(char *p) {
    int i = 0;
    while (*p) {
        // トークン可の検証用処理
        // fprintf(stderr, "target: %s\n", p);

        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].type_code = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].type_code = TK_NUM;
            tokens[i].input = p;
            tokens[i].value = strtol(p, &p, 10);
            i++;
            continue;
        }

        error("トークナイズできません: %s\n", p);
    }

    tokens[i].type_code = TK_EOF;
    tokens[i].input = p;
}

// エラー報告関数
void token_error(int i) {
    error("予期しないトークンを検出しました: '%s'\n", tokens[i].input);
}

// ノードの型を表す値
enum {
    ND_NUM = 256,
};
// ノードの型
typedef struct Node {
    int type_code;      // 整数かND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // type_codeがND_NUMの場合にのみ使う
} Node;

Node* new_node(int type_code, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->type_code = type_code;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->type_code = ND_NUM;
    node->val = val;
    return node;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の数が正しくありません\n");
        return 1;
    }

    tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main, _main\n");
    printf("\n");
    printf("main:\n");

    // 式の最初は数である必要がある
    if (tokens[0].type_code != TK_NUM) {
        token_error(0);
    }
    printf("	mov rax, %d\n", tokens[0].value);

    // `+ <数>`, `- <数>`というトークンの並びを消費してアセンブリ出力
    int i = 1;
    while (tokens[i].type_code != TK_EOF) {
        if (tokens[i].type_code == '+') {
            i++;
            if (tokens[i].type_code != TK_NUM) {
                fprintf(stderr, "Add \n");
                token_error(i);
            }
            printf("	add rax, %d\n", tokens[i].value);
            i++;
            continue;
        }

        if (tokens[i].type_code == '-') {
            i++;
            if (tokens[i].type_code != TK_NUM) {
                token_error(i);
            }
            printf("	sub rax, %d\n", tokens[i].value);
            i++;
            continue;
        }

        token_error(i);
    }

    printf("	ret\n");
    printf("\n");
    printf("_main:\n");
    printf("	call main\n");
    printf("	ret\n");

    return 0;
}
