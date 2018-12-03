#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ノードの型
typedef struct Node {
    int type_code;      // 整数かND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int value;          // type_codeがND_NUMの場合にのみ使う
    char name;          // type_codeがND_IDENTの場合にのみ使う
} Node;

// プロトタイプ宣言
void tokenize(char *p);
void program();
void gen(Node *node);

// グローバル変数
extern Node *code[];
