/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

static int loc = 0;
static int nivel = 0;

static void traverse(TreeNode *t,
                     void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *))
{
  if (t != NULL)
  {
    if (t->auxType == ChaveK || t->kind.stmt == WhileK || t->kind.stmt == IfK)
    {
      nivel++;
    }
    preProc(t);
    {
      int i;
      for (i = 0; i < MAXCHILDREN; i++)
        traverse(t->child[i], preProc, postProc);
    }
    postProc(t);
    if (t->auxType == ChaveK || t->kind.stmt == WhileK || t->kind.stmt == IfK)
    {
      // st_elimina(nivel);
      nivel--;
    }
    traverse(t->sibling, preProc, postProc);
  }
}

static void nullProc(TreeNode *t)
{
  if (t == NULL)
    return;
  else
    return;
}

static void insertNode(TreeNode *t)
{
  switch (t->nodekind)
  {
  case StmtK:
    switch (t->kind.stmt)
    {
    case AssignK:
      // if (st_busca(t->attr.name) == -1)
      //   st_insere(t->attr.name,t->type,t->auxType,t->lineno,loc++,nivel);
      // else
      //   {
      //   st_insere(t->attr.name,t->type,t->auxType,t->lineno,0,nivel);
      //   }
      break;
    // case IntK:
    // case VoidK:
    // case WhileK:
    // case ReturnK:
    default:
      break;
    }
    break;
  case ExpK:
    switch (t->kind.exp)
    {
    case IdK:
      if (st_busca(t->attr.name) == -1) {
        if (t->auxType == VarK && t->type == Void) 
          fprintf(listing, "ERRO SEMÂNTICO (3) na linha '%d', identificador '%s'\n", t->child[0]->lineno, t->child[0]->attr.name);
        else
          st_insere(t->attr.name, t->type, t->auxType, t->lineno, loc++, nivel);
      }
      else {
        BucketList b = st_obtem_atributos(t->attr.name);
        if (b != NULL) {
          if (b->aux == FunK)
            fprintf(listing, "ERRO SEMÂNTICO (7) na linha '%d', identificador '%s'\n", t->lineno, t->attr.name);
          else  
            fprintf(listing, "ERRO SEMÂNTICO (4) na linha '%d', identificador '%s'\n", t->lineno, t->attr.name);
        } 
        else
          st_insere(t->attr.name, t->type, t->auxType, t->lineno, 0, nivel);
      }
      break;
    case OpK:
      if (t->attr.op == ASSIGN)
        st_seta_atributos(t, nivel);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void buildSymtab(TreeNode *syntaxTree)
{
  traverse(syntaxTree, insertNode, nullProc);
  if (TraceAnalyze)
  {
    BucketList b = st_obtem_atributos("main");
    if (b == NULL || (b != NULL && b->aux != FunK))
    {
      fprintf(listing, "ERRO SEMÂNTICO (6): Função 'main' não declarada\n");
    }
    fprintf(listing, "\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode *t, char *mensagem)
{
  fprintf(listing, "Type error at line %d: %s\n", t->lineno, mensagem);
  Error = TRUE;
}

static void checkNode(TreeNode *t)
{
  switch (t->nodekind)
  {
  case ExpK:
    switch (t->kind.exp)
    {
    case OpK:
      if ((t->child[0]->type != Integer) ||
          (t->child[1]->type != Integer))
        typeError(t, "Op applied to non-integer");
      if ((t->attr.op == EQ) || (t->attr.op == LT))
        t->type = Boolean;
      else
        t->type = Integer;
      break;
    case ConstK:
    case IdK:
      t->type = Integer;
      break;
    default:
      break;
    }
    break;
  case StmtK:
    switch (t->kind.stmt)
    {
    case IfK:
      if (t->child[0]->type == Integer)
        typeError(t->child[0], "if test is not Boolean");
      break;
    case AssignK:
      if (t->child[0]->type != Integer)
        typeError(t->child[0], "assignment of non-integer value");
      break;
    case WriteK:
      if (t->child[0]->type != Integer)
        typeError(t->child[0], "write of non-integer value");
      break;
    case RepeatK:
      if (t->child[1]->type == Integer)
        typeError(t->child[1], "repeat test is not Boolean");
      break;
    case IntK:
      t->type = Integer;
      break;
    case VoidK:
      t->type = Void;
      break;
    case WhileK:
      t->type = Boolean;
      break;
    case ReturnK:
      t->type = Boolean;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void typeCheck(TreeNode *syntaxTree)
{
  traverse(syntaxTree, nullProc, checkNode);
}
