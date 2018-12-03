#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

// プロトタイプ宣言
void error(char* format, char* param);
void tokenize(char *p);
void program();
void gen(Node *node);

// グローバル変数
extern Token tokens[];
extern Node *code[];
