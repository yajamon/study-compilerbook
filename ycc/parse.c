#include "ycc.h"

// プロトタイプ宣言
void program();
Node* assign();
Node* expr();
Node* mul();
Node* term();

int pos = 0; // Tokenの位置指定に使用
int codePos = 0; // code[]における位置指定に使用

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
