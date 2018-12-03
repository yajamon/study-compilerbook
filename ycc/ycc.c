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
    TK_IDENT,       // 識別子
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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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

        if ('a' <= *p && *p <= 'z') {
            tokens[i].type_code = TK_IDENT;
            tokens[i].input = p;
            i++;
            p++;
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

// 抽象構文木
// ノードの型を表す値
enum {
    ND_NUM = 256,
};
// ノードの型
typedef struct Node {
    int type_code;      // 整数かND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int value;          // type_codeがND_NUMの場合にのみ使う
} Node;

Node *code[100] = {NULL};

Node* new_node(int type_code, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->type_code = type_code;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int value) {
    Node *node = malloc(sizeof(Node));
    node->type_code = ND_NUM;
    node->value = value;
    return node;
}
// Parser
int pos = 0; // Tokenの位置指定に使用
// プロトタイプ宣言
Node* expr();
Node* mul();
Node* term();
// bnf
//  expr    : mul expr'
//  expr'   : ε | '+' expr | '-' expr
Node* expr() {
    Node *lhs = mul();
    if (tokens[pos].type_code == '+') {
        pos++;
        return new_node('+', lhs, expr());
    }
    if (tokens[pos].type_code == '-') {
        pos++;
        return new_node('-', lhs, expr());
    }
    return lhs;
}
// bnf
//  mul : term mul'
//  mul': ε | '*' term | '/' term
Node* mul() {
    Node *lhs = term();
    if (tokens[pos].type_code == '*') {
        pos++;
        return new_node('*', lhs, mul());
    }
    if (tokens[pos].type_code == '/') {
        pos++;
        return new_node('/', lhs, mul());
    }
    return lhs;
}
// bnf
//  term    : number | '(' expr ')'
Node* term() {
    if (tokens[pos].type_code == TK_NUM) {
        Node *num = new_node_num(tokens[pos].value);
        pos++;
        return num;
    }
    if (tokens[pos].type_code == '(') {
        pos++;
        Node *node = expr();
        if (tokens[pos].type_code != ')') {
            error("開きカッコに対応する閉じカッコがありません: %s\n", tokens[pos].input);
        }
        pos++;
        return node;
    }
    error("数字でも開きカッコでもないトークンです: %s\n", tokens[pos].input);
}

void gen(Node *node) {
    if (node->type_code == ND_NUM) {
        printf("	push %d\n", node->value);
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

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の数が正しくありません\n");
        return 1;
    }

    tokenize(argv[1]);
    Node *rootNode = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main, _main\n");
    printf("\n");
    printf("main:\n");

    // 抽象構文木を下りながらコード生成
    gen(rootNode);

    // スタックトップに式全体の値があるはずなので
    // それをraxにロードする
    printf("	pop rax\n");

    printf("	ret\n");
    printf("\n");
    printf("_main:\n");
    printf("	call main\n");
    printf("	ret\n");

    return 0;
}
