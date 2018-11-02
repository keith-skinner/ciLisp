#ifndef __cilisp_h_
#define __cilisp_h_

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

typedef enum oper { // must be in sync with funcs in resolveFunc()
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

typedef enum {
    NUM_TYPE, FUNC_TYPE
} AST_NODE_TYPE;

typedef struct {
    double value;
} NUMBER_AST_NODE;

typedef struct {
    char *name;
    struct ast_node *op1;
    struct ast_node *op2;
} FUNCTION_AST_NODE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    union {
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
    } data;
} AST_NODE;

AST_NODE *number(double value);

AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2);

void freeNode(AST_NODE *p);

double eval(AST_NODE *ast);

#endif
