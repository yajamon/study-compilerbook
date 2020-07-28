#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED, // 記号
  TK_NUM,      // 数値
  TK_EOF,      // 終端
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;   // kind が TK_NUM の場合、その数値
  char *str; // Tokenの文字列
  int len;   // Tokenの長さ
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD, // 加算
  ND_SUB, // 減算
  ND_MUL, // 乗算
  ND_NUM, // 整数
  ND_DIV, // 除算
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;
  Node *lhs; // 左辺
  Node *rhs; // 右辺
  int val;   // kind が ND_NUM の場合、その数値
};

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
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
bool consume(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) ||
      memcmp(token->str, op, token->len) != 0) {

    return false;
  }
  token = token->next;
  return true;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
bool expect(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) ||
      memcmp(token->str, op, token->len) != 0) {
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

bool at_eof() { return token->kind == TK_EOF; }

Token *new_token(TokenKind kind, Token *current, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  current->next = tok;
  return tok;
}

bool has_prefix(const char *str, const char *prefix) {
  return memcmp(str, prefix, strlen(prefix)) == 0;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *current = &head;

  while (*p) {
    if (isspace(*p)) {
      p += 1;
      continue;
    }

    // 複数文字の演算子
    if (has_prefix(p, "==") || has_prefix(p, "!=") || has_prefix(p, "<=")) {
      current = new_token(TK_RESERVED, current, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<", *p)) {
      current = new_token(TK_RESERVED, current, p, 1);
      p += 1;
      continue;
    }

    if (isdigit(*p)) {
      current = new_token(TK_NUM, current, p, 0);
      char *q = p;
      current->val = strtol(p, &p, 10);
      // ポインタが進行したので、アドレスから長さを算出できる
      current->len = p - q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, current, p, 0);
  return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr() { return equality(); }

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }

  return primary();
}

Node *primary() {
  // "("があれば中に式があるはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  // さもなければ数値のはず
  return new_node_num(expect_number());
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("    push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("    pop rdi\n");
  printf("    pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("    add rax, rdi\n");
    break;
  case ND_SUB:
    printf("    sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("    imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("    cqo\n");
    printf("    idiv rax, rdi\n");
    break;
  case ND_EQ:
    printf("    cmp rax, rdi\n");
    printf("    sete al\n");
    printf("    movzb rax, al\n");
    break;
  case ND_NE:
    printf("    cmp rax, rdi\n");
    printf("    setne al\n");
    printf("    movzb rax, al\n");
    break;
  case ND_LT:
    printf("    cmp rax, rdi\n");
    printf("    setl al\n");
    printf("    movzb rax, al\n");
    break;
  case ND_LE:
    printf("    cmp rax, rdi\n");
    printf("    setle al\n");
    printf("    movzb rax, al\n");
    break;
  }

  printf("    push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");
  printf("main:\n");

  gen(node);

  printf("    pop rax\n");
  printf("    ret\n");
  return 0;
}
