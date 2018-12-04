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
    PRINT,
<<<<<<< HEAD
    RAND,
=======
>>>>>>> 7215257... Done with task 6
    EQUAL,
    SMALLER,
    LARGER,
    INVALID_OPER=255
} OPER_TYPE;

typedef enum
{
    VARIABLE_TYPE,
    LAMBDA_TYPE,
    ARG_TYPE
} SYMBOL_TYPE;

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

typedef struct stack_node {
    struct ast_node *val;
    struct stack_node *next;
} STACK_NODE;


typedef enum {
    NUM_TYPE,
    FUNC_TYPE,
    SYM_TYPE,
    COND_TYPE
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
    struct ast_node *opList;
} FUNCTION_AST_NODE;

typedef struct {
    struct ast_node *cond;
    struct ast_node *zero;
    struct ast_node *nonzero;
} COND_AST_NODE;

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
        COND_AST_NODE condition;

    } data;
    struct ast_node * next;
} AST_NODE;

typedef struct stack_node {
    struct ast_node *val;
    struct stack_node *next;
} STACK_NODE;

typedef struct symbol_table_node 
{
    SYMBOL_TYPE type;
    DATA_TYPE val_type;
    char *ident;
    struct STACK_NODE * stack;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;


AST_NODE *symbol(char * name);
AST_NODE *number(double value);
AST_NODE *function(char *name, AST_NODE *opList);
AST_NODE *condition(AST_NODE * cond, AST_NODE * nonzero, AST_NODE * zero);

AST_NODE *scope(SYMBOL_TABLE_NODE * scope, AST_NODE * s_expr);
AST_NODE *s_expr_list(AST_NODE *s_expr, AST_NODE * s_expr_list);

SYMBOL_TABLE_NODE * let_elem(char * type, char *name, AST_NODE *s_expr);
SYMBOL_TABLE_NODE * let_list(SYMBOL_TABLE_NODE * list, SYMBOL_TABLE_NODE * elem);

void freeNode(AST_NODE *p);

RETURN_VALUE eval(AST_NODE * p);

#endif
