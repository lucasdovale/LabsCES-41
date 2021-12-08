/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"

#define SHIFT 4

static int hash(char *key)
{
  int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

void st_insere ( char * nome, ExpType tipo, AuxType auxType, int lineno, int loc, int nivel )
{
  int h = hash(nome);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(nome, l->nome) != 0))
    l = l->prox;
  if (l == NULL)
  {
    l = (BucketList)malloc(sizeof(struct BucketListRec));
    l->nome = nome;
    l->linhas = (LineList)malloc(sizeof(struct LineListRec));
    l->linhas->lineno = lineno;
    l->loc = loc;
    l->tipo = tipo;
    l->escopo = nivel;
    l->aux = auxType;
    l->linhas->prox = NULL;
    l->prox = hashTable[h];
    hashTable[h] = l;
  }
  else
  {
    LineList t = l->linhas;
    while (t->prox != NULL)
      t = t->prox;
    t->prox = (LineList)malloc(sizeof(struct LineListRec));
    t->prox->lineno = lineno;
    t->prox->prox = NULL;
  }
}

int st_busca(char *nome)
{
  int h = hash(nome);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(nome, l->nome) != 0))
    l = l->prox;
  if (l == NULL)
    return -1;
  else
    return l->loc;
}

void st_elimina(int nivel)
{
  for (int i = 0; i < SIZE; i++) {
    if (hashTable[i] == NULL) continue;
    BucketList raizNova = hashTable[i];
    while (raizNova && raizNova->escopo == nivel) { raizNova = raizNova->prox; }
    BucketList bucketAnterior = (BucketList)malloc(sizeof(struct BucketListRec));
    bucketAnterior->prox = hashTable[i];
    while (hashTable[i] != NULL) {
      if (hashTable[i]->escopo == nivel) {
        BucketList table = hashTable[i];
        hashTable[i] = hashTable[i]->prox;
        bucketAnterior->prox = hashTable[i];
        free(table);
      }
      else {
        bucketAnterior = hashTable[i];
        hashTable[i] = hashTable[i]->prox;
      }
    }
    hashTable[i] = raizNova;
  }
}

int st_declarado(char *nome, int nivel)
{
  int h = hash(nome);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(nome, l->nome) != 0))
      l = l->prox;
  if (l->escopo != nivel || l == NULL)
      return 0;
  return 1;
}

BucketList st_obtem_atributos(char *nome)
{
  int h = hash(nome);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(nome, l->nome) != 0))
      l = l->prox;
  return l;
}

int st_seta_atributos(TreeNode *t, int nivel)
{
  BucketList b = st_obtem_atributos(t->child[1]->attr.name);
  if (b == NULL || t->child[1]->type != Integer)
    fprintf(listing, "ERRO SEMÂNTICO (5) na linha '%d', identificador '%s'\n", t->child[1]->lineno, t->child[1]->attr.name);
  if (st_busca(t->child[0]->attr.name) == -1) {
    fprintf(listing, "ERRO SEMÂNTICO (1) na linha '%d', identificador '%s'\n", t->child[0]->lineno, t->child[0]->attr.name);
  }
  else
  {
    BucketList b = st_obtem_atributos(t->child[0]->attr.name);
    int is_declared = st_declarado(t->child[0]->attr.name, b->escopo);
    if (b->escopo == 0 || is_declared)
      if (t->child[0]->type != t->child[1]->type)
        fprintf(listing, "ERRO SEMÂNTICO (2) na linha '%d', identificador '%s'\n", t->child[0]->lineno, t->child[0]->attr.name);
  }
}

void printSymTab(FILE *listing)
{
  int i;
  fprintf(listing, "Nome        Tipo       Escopo\n");
  fprintf(listing, "----        ----     ----------\n");
  for (i = 0; i < SIZE; ++i)
  {
    if (hashTable[i] != NULL)
    {
      BucketList l = hashTable[i];
      while (l != NULL)
      {
        LineList t = l->linhas;
        fprintf(listing, "%-14s", l->nome);
        fprintf(listing, "%-12d", l->tipo);
        fprintf(listing, "%-9d", l->escopo);
        fprintf(listing, "\n");
        l = l->prox;
      }
    }
  }
}
