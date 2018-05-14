#ifndef __TOKEN_HEADER__
#define __TOKEN_HEADER__
#include <stdio.h>
typedef enum{
  INT_VAL_TOKEN,//int
  IDENT_TOKEN,//id
  STRING_LITERAL_TOKEN,//string
  EQ_TOKEN,//==
  NE_TOKEN,//!=
  GE_TOKEN,//>=
  LE_TOKEN,//<=
  ADD_TOKEN,//+
  SUB_TOKEN,//-
  MUL_TOKEN,//*
  DIV_TOKEN,// /
  ASSIGN_TOKEN,//=
  GT_TOKEN,//>
  LT_TOKEN,//<
  LPAREN_TOKEN,//(
  RPAREN_TOKEN,//)
  LBRACE_TOKEN,//{
  RBRACE_TOKEN,//}
  COMMA_TOKEN,//,
  SEMICOLON_TOKEN,//;
  IF_TOKEN,//if
  ELSE_TOKEN,//else
  WHILE_TOKEN,//while
  GOTO_TOKEN,//goto
  GOSUB_TOKEN,//gosub
  RETURN_TOKEN,//return
  PRINT_TOKEN,//print
  END_OF_FILE_TOKEN//EOF
}TokenKind
typedef struct{
  TokenKind kind;
  union{
    int int_val;
    char *string;
    char ident;
  }u;
}Token;
void lex_initialize(FILE *fp);
Token lex_get_token(void);
int lex_get_line_number(void);
extern char *g_string_pointer_pool[];
#endif
