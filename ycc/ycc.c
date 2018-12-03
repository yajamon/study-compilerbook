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
    char *input;    // トークン文字列
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

        switch (*p) {
            case '+':
            case '-':
            case '*':
            case '/':
            case '(':
            case ')':
            case '=':
            case ';':
                tokens[i].type_code = *p;
                tokens[i].input = p;
                i++;
                p++;
                continue;
            default:
                break;
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
    ND_IDENT,
};
// ノードの型
typedef struct Node {
    int type_code;      // 整数かND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int value;          // type_codeがND_NUMの場合にのみ使う
    char name;          // type_codeがND_IDENTの場合にのみ使う
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

Node* new_node_ident(char name) {
    Node *node = malloc(sizeof(Node));
    node->type_code = ND_IDENT;
    node->name = name;
    return node;
}
// Parser
int pos = 0; // Tokenの位置指定に使用
int codePos = 0; // code[]における位置指定に使用
// プロトタイプ宣言
void program();
Node* assign();
Node* expr();
Node* mul();
Node* term();
// program
// bnf
//  program : assign program'
//  program': ε | program'
void program() {
    while (tokens[pos].type_code != TK_EOF) {
        code[codePos] = assign();
        codePos++;
    }
}
// assign
// bnf
//  assign  : expr assign' ";"
//  assign' : ε | "=" assign'
Node* assign() {
    Node *lhs = expr();
    if (tokens[pos].type_code == '=') {
        pos++;
        return new_node('=', lhs, assign());
    }
    if (tokens[pos].type_code != ';') {
        error("文の終わりを示す;がありません: %s\n", tokens[pos].input);
    }
    pos++;
    return lhs;
}
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
//  term    : number | ident | '(' expr ')'
Node* term() {
    if (tokens[pos].type_code == TK_NUM) {
        Node *num = new_node_num(tokens[pos].value);
        pos++;
        return num;
    }
    if (tokens[pos].type_code == TK_IDENT) {
        Node *ident = new_node_ident(*(tokens[pos].input));
        pos++;
        return ident;
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
