#ifndef __cilisp_h_
#define __cilisp_h_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

typedef enum oper 
{ // must be in sync with funcs in resolveFunc()
    NEG,
    ABS,
    EXP,
    SQRT,
    ADD,
    SUB,
    MULT,
    DIV,
    REMAINDER,
    LOG,
    POW,
    MAX,
    MIN,
    EXP2,
    CBRT,
    HYPOT,
    INVALID_OPER=255
} OPER_TYPE;

typedef enum 
{ 
    NO_TYPE, 
    INTEGER_TYPE, 
    REAL_TYPE 
} DATA_TYPE;

typedef struct return_value 
{
  DATA_TYPE type;
  double value;
} RETURN_VALUE;


typedef enum {
    NUM_TYPE, FUNC_TYPE, SYM_TYPE
} AST_NODE_TYPE;


typedef struct 
{
    char * name;
} SYMBOL_AST_NODE;

typedef struct 
{
    double value;
} NUMBER_AST_NODE;

typedef struct 
{
    char *name;
    struct ast_node *op1;
    struct ast_node *op2;
} FUNCTION_AST_NODE;

typedef struct ast_node 
{
    struct symbol_table_node * scope;
    struct ast_node * parent;
    AST_NODE_TYPE type;
    union 
    {
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
        SYMBOL_AST_NODE symbol;
    } data;
} AST_NODE;

typedef struct symbol_table_node 
{
   DATA_TYPE val_type;
   char *ident;
   struct ast_node *val;
   struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;


AST_NODE *symbol(char * name);
AST_NODE *number(double value);
AST_NODE *function(char *name, AST_NODE *op1, AST_NODE *op2);

AST_NODE *scope(SYMBOL_TABLE_NODE * scope, AST_NODE * s_expr);

SYMBOL_TABLE_NODE * let_elem(char * type, char *name, AST_NODE *s_expr);
SYMBOL_TABLE_NODE * let_list(SYMBOL_TABLE_NODE * list, SYMBOL_TABLE_NODE * elem);

void freeTable(SYMBOL_TABLE_NODE *p);
void freeNode(AST_NODE *p);

RETURN_VALUE symbolEval(AST_NODE * p, char * name);
RETURN_VALUE functionEval(AST_NODE * p);
RETURN_VALUE eval(AST_NODE * p);

#endif
