#include "ycc.h"

// エラー報告関数
void error(char* format, char* param) {
    if (param != NULL) {
        fprintf(stderr, format, param);
    } else {
        fprintf(stderr, format);
    }
    exit(1);
}

// トークナイズした結果のトークン列を保存する配列
// 100個以上のトークンはこないものとする
Token tokens[100];
Node *code[100] = {NULL};




