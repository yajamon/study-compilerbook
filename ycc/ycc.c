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

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // 加算
    ND_SUB, // 減算
    ND_MUL, // 乗算
    ND_DIV, // 除算
    ND_NUM, // 正数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind;
    Node *lhs;      // 左辺
    Node *rhs;      // 右辺
    int val;        // kind が ND_NUM の場合、その数値
};

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// エラー箇所を報告する
void error_at(char* loc, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号の場合、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
bool expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        error_at(token->str, "'%c'ではありません", op);
    }
    token = token->next;
}


// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "数ではありません");
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

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, current, p);
    return head.next;
}

Node* new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *primary();

Node* expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+')) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume('-')) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node *node = primary();

    for (;;) {
        if (consume('*')) {
            node = new_node(ND_MUL, node, primary());
        } else if (consume('/')) {
            node = new_node(ND_DIV, node, primary());
        } else {
            return node;
        }
    }
}

Node* primary() {
    // 次のトークンが '(' なら、 "'(' expr ')'" のはず
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    // さもなければ数値のはず
    return new_node_num(expect_number());
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    char *p = argv[1];
    user_input = p;
    token = tokenize(p);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("\n");
    printf("main:\n");

    // 式の最初は数でなければならない。
    // それをチェックして最初のmov命令を出力。
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }

    printf("    ret\n");
    return 0;
}
