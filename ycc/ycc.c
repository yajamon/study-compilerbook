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


Node *code[100] = {NULL};




