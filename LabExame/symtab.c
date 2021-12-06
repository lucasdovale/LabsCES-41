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

#define SIZE 211
#define SHIFT 4

static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

typedef struct LineListRec
   { int lineno;
     struct LineListRec * prox;
   } * LineList;


typedef struct BucketListRec
   { char * nome;
     LineList linhas;
     int loc ;
     ExpType tipo;
     char* escopo;
     struct BucketListRec * prox;
   } * BucketList;

static BucketList hashTable[SIZE];


void st_insert( char * nome, ExpType tipo, int lineno, int loc )
{ int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(nome,l->nome) != 0))
    l = l->prox;
  if (l == NULL) 
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->nome = nome;
    l->linhas = (LineList) malloc(sizeof(struct LineListRec));
    l->linhas->lineno = lineno;
    l->loc = loc;
    l->tipo = tipo;
    l->escopo = "global";
    l->linhas->prox = NULL;
    l->prox = hashTable[h];
    hashTable[h] = l; }
  else 
  { LineList t = l->linhas;
    while (t->prox != NULL) t = t->prox;
    t->prox = (LineList) malloc(sizeof(struct LineListRec));
    t->prox->lineno = lineno;
    t->prox->prox = NULL;
  }
} 


int st_lookup ( char * nome )
{ int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(nome,l->nome) != 0))
    l = l->prox;
  if (l == NULL) return -1;
  else return l->loc;
}

// A FAZER
int st_elimina ( char * nome )
{  
}

// A FAZER
int st_declarado ( char * nome )
{  
}

// A FAZER
int st_seta_atributos ( char * nome )
{  
}

// A FAZER
int st_obtem_atributos ( char * nome )
{  
}


void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Nome        Tipo       Escopo      Loc              Linhas\n");
  fprintf(listing,"----        ----     ----------    ---   ----------------------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->linhas;
        fprintf(listing,"%-14s",l->nome);
        fprintf(listing,"%-10d",l->tipo);
        fprintf(listing,"%-12s",l->escopo);
        fprintf(listing,"%2d   ",l->loc);
        while (t != NULL)
        { fprintf(listing," %2d ->",t->lineno);
          t = t->prox;
        }
        fprintf(listing,"\n");
        l = l->prox;
      }
    }
  }
}
