#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
extern Node *code[];
