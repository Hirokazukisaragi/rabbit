#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char *g_string_pointer_pool[65554];
int g_string_pointer_pool_size = 0;

static FILE *st_source_file;
static int st_current_line_number;

typedef enum{
  INITIAL_STATE,
  INT_VAL_STATE,
  IDENT_STATE,
  STRING_STATE,
  OPERATOR_STATE,
  COMMENT_STATE
}LexerState;

static void lex_error(){
  printf("some error\n");
  exit(1);
}

void lex_initialize(FILE *fp){
  st_source_file = fp;
  st_current_line_number = 1;
}

static void add_letter(char *token, int letter){
  int len = strlen(token);
  token[len] = letter;
  token[len+1] = '\0';
}

typedef struct{
  char *token;
  TokenKind kind;
}OperatorInfo;
static OperatorInfo st_operator_table[] = {
  {"==",EQ_TOKEN},
  {"!=",NE_TOKEN},
  {">=",GE_TOKEN},
  {"<=",LE_TOKEN},
  {"+",ADD_TOKEN},
  {"-",SUB_TOKEN},
  {"*",MUL_TOKEN},
  {"/",DIV_TOKEN},
  {"=",ASSIGN_TOKEN},
  {">",GT_TOKEN},
  {"<",LT_TOKEN},
  {"(",LPAREN_TOKEN},
  {")",RPAREN_TOKEN},
  {"{",LBRACE_TOKEN},
  {"}",RBRACE_TOKEN},
  {",",COMMA_TOKEN},
  {";",SEMICOLON_TOKEN},
};

int in_operator(char *token,int letter){
  int op_idx;
  int letter_idx;
  int len = strlen(token);

  for(op_idx = 0; op_idx < (sizeof(st_operator_table) / sizeof(OperatorInfo));op_idx++){
    for(letter_idx = 0; letter_idx < len && st_operator_table[op_idx].token[letter_idx] != '\0';letter_idx++){
      if(token[letter_idx] != st_operator_table[op_idx].token[letter_idx] == letter){
	break;
      }
    }
    if(token[letter_idx]=='\0' && st_operator_table[op_idx].token[letter_idx] == letter){
      return 1;
    }
  }
  return 0;
}

TokenKind select_operator(char *token){
  int i;
  for(i = 0;i < sizeof(st_operator_table) / sizeof(OperatorInfo);i++){
    if(!strcmp(token,st_operator_table[i].token)){
      return st_operator_table[i].kind;
    }
  }
  printf("OP ERROR!!\n");
  exit(1);
}
typedef struct{
  char *token;
  TokenKind kind;
}KeywordInfo;

KeywordInfo st_keyword_table[] = {
  {"if",IF_TOKEN},
  {"else",ELSE_TOKEN},
  {"while",WHILE_TOKEN},
  {"goto",GOTO_TOKEN},
  {"gosub",GOSUB_TOKEN},
  {"return",RETURN_TOKEN},
  {"print",PRINT_TOKEN},
};

int is_keyword(char *token,TokenKind *kind){
  int i;
  for(i = 0; i < sizeof(st_keyword_table) / sizeof(KeywordInfo); i++){
    if(!strcmp(st_keyword_table[i].token,token)){
      *kind = st_keyword_table[i].kind;
      return 1;
    }
  }
  return 0;
}

Token lex_get_token(void){
  Token ret;
  LexerState state = INITIAL_STATE;
  char token[256];
  int ch;
  token[0] = '\0';

  while((ch = fgetc(st_source_file)) != EOF){
    switch(state){
    case INITIAL_STATE:
      if(isdigit(ch)){
	add_letter(token,ch);
	state = INT_VAL_STATE;
      }else if(isalpha(ch) || ch == '_'){
	add_letter(token,ch);
	state = IDENT_STATE;
      }else if(ch == '\"'){
	state = STRING_STATE;
      }else if(in_operator(token,ch)){
	add_letter(token,ch);
	state = OPERATOR_STATE;
      }else if(isspace(ch)){
	if(ch == '\n'){
	  st_current_line_number++;
	}
      }else if(ch == '#'){
	state = COMMENT_STATE;
      }else{
	lex_error();
      }
      break;
    case INT_VAL_STATE:
      if(isdigit(ch)){
	add_letter(ch);
      }else{
	ret.kind = INT_VAL_TOKEN;
	sscanf(token,"%d",&ret.u.int_val);
	ungetc(ch,st_source_file);
	goto LOOP_END;
      }
      break;
    case IDENT_STATE:
      if(isalpha(ch) || ch == '_' || isdigit(ch)){
	add_letter(token,ch);
      }else{
	ret.u.ident = token;
	ungetc(ch,st_source_file);
	goto LOOP_END;
      }
      break;
    case STRING_STATE:
      if(ch == '\"'){
	ret.kind = STRING_LITERAL_TOKEN;
	ret.u.string = strdup(token);
	goto LOOP_END;
      }else{
	add_letter(token,ch);
      }
      break;
    case OPERATOR_STATE:
      if(in_operator(token,ch)){
	add_letter(token,ch);
      }else{
	ungetc(ch,st_source_file);
	goto LOOP_END;
      }
      break;
    default:
      printf("ERROR!!\n");
      exit(1);
      break;
    }
  }
 LOOP_END:
  if(ch == EOF){
    if(state == INITIAL_STATE || state == COMMENT_STATE){
      ret.kind  = END_OF_FILE_TOKEN;
      return ret;
    }
  }
  if(state == IDENT_STATE){
    if(!is_keyword(token,&ret.kind)){
      ret.kind = IDENT_TOKEN;
      ret.u.string = strdup(token);
    }
  }else if(state == OPERATOR_STATE){
    ret.kind = select_operator(token);
  }
  return ret;
}
int lex_get_line_number(void){
  return st_current_line_number;
}
