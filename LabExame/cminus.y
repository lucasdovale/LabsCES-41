/****************************************************/
/* File: CMINUS.y                                   */
/* The CMINUS Yacc/Bison specification file         */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

// #define YYDEBUG 1

#define YYSTYPE TreeNode *
static int savedInt;
static int savedType;
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *s);
%}

// código original da prof
%token IF ELSE ERROR
%token ID NUM
%token ASSIGN EQ LT PLUS MINUS TIMES OVER LPAREN RPAREN SEMI

// adicionando o do lab antigo
%token DIF LTE RT RTE
%token INT RETURN VOID WHILE
%token LCURBR RCURBR LBRCKS RBRCKS COL


%% /* Grammar for CMINUS */

programa            : declaracao_lista 
                      { savedTree = $1; }
                    ;

declaracao_lista    : declaracao_lista declaracao
                      { 
                        YYSTYPE t = $1;
                        if (t != NULL)
                        { while (t->sibling != NULL)
                              t = t->sibling;
                          t->sibling = $2;
                          $$ = $1; }
                          else { $$ = $2; }
                      }
                    | declaracao 
                      { $$ = $1; }
                    ;

declaracao          : var_declaracao
                      { $$ = $1; }
                    | fun_declaracao
                      { $$ = $1; }
                    | error  { $$ = NULL; }
                    ; 

identificador       : ID { savedName = copyString(tokenString);
                      savedLineNo = lineno; }
                    ;

numero              : NUM { savedInt = atoi(tokenString);
                      savedLineNo = lineno; }
                    ;

tipo_especificador  : INT { $$ = newStmtNode(IntK);
                          savedType = Integer; }
                    | VOID { $$ = newStmtNode(VoidK);
                          savedType = Void; }
                    ;

var_declaracao      : tipo_especificador
                      identificador
                      { $$ = $1;
                        $$->child[0] = newExpNode(IdK);
                        $$->child[0]->attr.name = savedName;
                        $$->child[0]->type = savedType;
                      }
                      SEMI 
                    | tipo_especificador
                      identificador
                      {
                        $$ = $1;
                        $$->child[0] = newExpNode(IdK);
                        $$->child[0]->attr.name = savedName;
                        $$->child[0]->type = savedType;
                      }
                      LBRCKS 
                      { $$ = $1;
                        $$->child[0]->child[0] = newExpNode(OpK);
                        $$->child[0]->child[0]->attr.op = LBRCKS;
                      }     
                      numero 
                      { $$ = $1;
                        $$->child[0]->child[1] = newExpNode(ConstK);;
                        $$->child[0]->child[1]->attr.val = savedInt;
                      }
                      RBRCKS
                      { $$ = $1;
                        $$->child[0]->child[2] = newExpNode(OpK);
                        $$->child[0]->child[2]->attr.op = RBRCKS;
                      }     
                      SEMI
                    ;

fun_declaracao      : tipo_especificador
                      identificador
                      {
                        $$ = $1;
                        $$->child[0] = newExpNode(IdK);
                        $$->child[0]->attr.name = savedName;
                        $$->child[0]->type = savedType;
                      }
                      LPAREN params RPAREN composto_decl 
                      { $$ = $3;
                        $$->child[0]->child[0] = $5;
                        $$->child[0]->child[1] = $7;
                      }
                    ;

params              : param_lista 
                      { $$ = $1; }
                    | VOID 
                      { $$ = NULL; }
                    ;

param_lista         : param_lista COL param 
                      { 
                        YYSTYPE t = $1;
                        if (t != NULL)
                        { while (t->sibling != NULL)
                              t = t->sibling;
                          t->sibling = $3;
                          $$ = $1; }
                          else { $$ = $3; }
                      }
                    | param { $$ = $1; }
                    ;

param               : tipo_especificador
                      identificador
                      {
                        $$ = $1;
                        $$->child[0] = newExpNode(IdK);
                        $$->child[0]->attr.name = savedName;
                        $$->child[0]->type = savedType;
                      }
                    | tipo_especificador
                      identificador
                      {
                        $$ = $1;
                        $$->child[0] = newExpNode(IdK);
                        $$->child[0]->attr.name = savedName;
                        $$->child[0]->type = savedType;
                      }
                      LBRCKS
                      { $$ = $1;
                        $$->child[0]->child[0] = newExpNode(OpK);
                        $$->child[0]->child[0]->attr.op = LBRCKS;
                      } 
                      RBRCKS
                      { $$ = $1;
                        $$->child[0]->child[1] = newExpNode(OpK);
                        $$->child[0]->child[1]->attr.op = RBRCKS;
                      }          
                    ;

composto_decl       : LCURBR local_declaracoes statement_lista RCURBR 
                       {  YYSTYPE t = $2;
                        if (t != NULL)
                        { while (t->sibling != NULL)
                              t = t->sibling;
                          t->sibling = $3;
                          $$ = $2; }
                          else $$ = $3;
                      }
                    ;

local_declaracoes   : local_declaracoes var_declaracao
                      { YYSTYPE t = $1;
                        if (t != NULL)
                        { while (t->sibling != NULL)
                            t = t->sibling;
                          t->sibling = $2;
                          $$ = $1; }
                        else $$ = $2;
                      }
                    | { $$ = NULL; }
                    ;

statement_lista     : statement_lista statement 
                      { YYSTYPE t = $1;
                        if (t != NULL)
                        { while (t->sibling != NULL)
                            t = t->sibling;
                          t->sibling = $2;
                          $$ = $1; }
                        else $$ = $2;
                      }
                    | { $$ = NULL; }
                    ;         

statement           : expressao_decl { $$ = $1; }
                    | composto_decl { $$ = $1; }
                    | selecao_decl { $$ = $1; }
                    | iteracao_decl { $$ = $1; }
                    | retorno_decl { $$ = $1; }
                    ;

expressao_decl      : expressao SEMI
                      { $$ = $1; }
                    | SEMI { $$ = NULL; }
        
selecao_decl        : IF LPAREN expressao RPAREN statement 
                      { $$ = newStmtNode(IfK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                      }     
                    | IF LPAREN expressao RPAREN statement ELSE statement 
                      { $$ = newStmtNode(IfK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                        $$->child[2] = $7;
                      } 
                    ;

iteracao_decl       : WHILE LPAREN expressao RPAREN statement
                      { $$ = newStmtNode(WhileK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                      }
                    ;

retorno_decl        : RETURN SEMI 
                      { $$ = newStmtNode(ReturnK);}

                    | RETURN expressao SEMI
                      { $$ = newStmtNode(ReturnK);
                        $$->child[0] = $2;
                      }
                    ;

expressao           : var ASSIGN expressao
                      { $$ = newExpNode(OpK);
                        $$->attr.op = ASSIGN;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                      }
                    | simples_expressao 
                      { $$ = $1; }
                    ;

var                 : identificador
                      { $$ = newExpNode(IdK);
                        $$->attr.name = savedName;
                      }
                    | identificador
                      { $$ = newExpNode(IdK);
                        $$->attr.name = savedName;
                      }
                      LBRCKS 
                      { $$ = $2;
                        $$->child[0] = newExpNode(OpK);
                        $$->child[0]->attr.op = LBRCKS;
                      }     
                      expressao 
                      { $$ = $2;
                        $$->child[1] = $5;
                      }
                      RBRCKS
                      { $$ = $2;
                        $$->child[2] = newExpNode(OpK);
                        $$->child[2]->attr.op = RBRCKS;
                      }  
                      ;

simples_expressao   : soma_expressao LT soma_expressao 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = LT;
                      }
                    | soma_expressao LTE soma_expressao 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = LTE;
                      } 
                    |  soma_expressao RT soma_expressao 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = RT;
                      }
                    | soma_expressao RTE soma_expressao 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = RTE;
                      } 
                    | soma_expressao EQ soma_expressao 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = EQ;
                      }
                    | soma_expressao DIF soma_expressao 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = DIF;
                      } 
                    | soma_expressao { $$ = $1; }
                    ;

soma_expressao      : soma_expressao PLUS termo 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = PLUS;
                      }
                    | soma_expressao MINUS termo 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = MINUS;
                      } 
                    | termo { $$ = $1; }
                    ;

termo               : termo TIMES fator 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = TIMES;
                      }
                    | termo OVER fator 
                      { $$ = newExpNode(OpK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op = OVER;
                      }
                    | fator { $$ = $1; }
                    ; 

fator               : LPAREN expressao RPAREN
                      { $$ = $2; }
                    | var { $$ = $1; }   
                    | ativacao { $$ = $1; }
                    | numero
                        { $$ = newExpNode(ConstK);
                          $$->attr.val = savedInt;
                        }
                    | error { $$ = NULL; }
                    ;

ativacao            : identificador
                      { $$ = newExpNode(IdK);
                        $$->attr.name = savedName;
                      }
                      LPAREN args RPAREN
                      { $$ = $2;
                        $$->child[0] = $4;
                      }
                    ;

args                : arg_lista { $$ = $1; }
                    | { $$ = NULL; }
                    ;

arg_lista           : arg_lista COL expressao 
                      { YYSTYPE t = $1;
                        if (t != NULL)
                        { while (t->sibling != NULL)
                              t = t->sibling;
                          t->sibling = $3;
                          $$ = $1; }
                          else $$ = $3;
                      }
                    | expressao { $$ = $1; }
                    ;
%%

int yyerror(char * message)
{ fprintf(listing,"ERRO SINTÁTICO na linha '%d', token '%s'\n",lineno,tokenString);
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the CMINUS scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
