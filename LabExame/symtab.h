/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"
#define SIZE 211

typedef struct LineListRec
   { int lineno;
     struct LineListRec * prox;
   } * LineList;

typedef struct BucketListRec
   { char * nome;
     LineList linhas;
     int loc ;
     ExpType tipo;
     AuxType aux;
     int escopo;
     struct BucketListRec * prox;
   } * BucketList;

static BucketList hashTable[SIZE];


int st_busca ( char * name );
void st_elimina ( int nivel );
void st_insere ( char * nome, ExpType tipo, AuxType auxType, int lineno, int loc, int nivel );
int st_declarado ( char * nome, int nivel );
int st_seta_atributos ( TreeNode * t, int nivel );
BucketList st_obtem_atributos( char *nome );

void printSymTab(FILE * listing);

#endif
