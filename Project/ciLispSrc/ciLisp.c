/**
* Name: Keith Skinner
* Lab: Final Project ciLisp
* Date: 12/04/2018
**/

#include "ciLisp.h"
#include <limits.h>

void yyerror(char *s) {
    printf("\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

int resolveFunc(char *func)
{
    char *funcs[] = {
            "neg",  "abs",  "exp",
            "sqrt", "add",  "sub",
            "mult", "div",  "remainder",
            "log",  "pow",  "max",
            "min",  "exp2", "cbrt",
            "hypot", "print", "equal",
            "smaller", "larger", ""
    };

    int i = 0;
    while (funcs[i][0] != '\0')
    {
        if (strcmp(funcs[i], func) == 0)
            return i;
        i++;
    }
    return INVALID_OPER;
}


DATA_TYPE resolveType(char * type)
{
    if (type == NULL)
        return NO_TYPE;
    if (strcmp(type, "integer") == 0)
        return INTEGER_TYPE;
    if (strcmp(type, "real") == 0)
        return REAL_TYPE;

    yyerror("unknown type");
    return NO_TYPE;
}


SYMBOL_TABLE_NODE * findSymbolScope(SYMBOL_TABLE_NODE * symbol, const char * name)
{
    while (symbol != NULL) {
        if ( strcmp(name, symbol->ident) == 0 )
            return symbol;
        symbol = symbol->next;
    }
    return NULL;
}
SYMBOL_TABLE_NODE * findSymbol(AST_NODE * p, const char * name)
{
    if (p == NULL)
        return NULL;
    if (p->scope != NULL) {
        SYMBOL_TABLE_NODE *node = findSymbolScope(p->scope, name);
        if (node != NULL)
            return node;
    }
    return findSymbol(p->parent, name);
}


SYMBOL_TABLE_NODE * let_list(SYMBOL_TABLE_NODE * list, SYMBOL_TABLE_NODE * elem)
{
    SYMBOL_TABLE_NODE * node = findSymbolScope(list, elem->ident);
    if (node != NULL) {
        yyerror("duplicate symbol");
        return list;
    }
    node = list;
    while (node->next != NULL)
        node = node->next;
    node->next = elem;
    return list;
}

SYMBOL_TABLE_NODE * let_var_elem(char * type, char * name, AST_NODE * val)
{
    SYMBOL_TABLE_NODE * p = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    p->type = VARIABLE_TYPE;
    p->val_type = resolveType(type);
    p->ident = name;
    p->val = val;
    return p;
}

SYMBOL_TABLE_NODE * let_arg_elem(char * name)
{
    char *funcs[] = {
        "neg",  "abs",  "exp",
        "sqrt", "add",  "sub",
        "mult", "div",  "remainder",
        "log",  "pow",  "max",
        "min",  "exp2", "cbrt",
        "hypot", "print", "equal",
        "smaller", "larger", ""
    };
    SYMBOL_TABLE_NODE * node = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    node->type = ARG_TYPE;
    node->val_type = NO_TYPE;
    node->ident = name;
    return node;
}


SYMBOL_TABLE_NODE * let_func_elem(char * type, char * symbol, SYMBOL_TABLE_NODE * arg_list, AST_NODE * s_expr) {
    SYMBOL_TABLE_NODE * node = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    node->type = LAMBDA_TYPE;
    node->val_type = resolveType(type);
    node->ident = symbol;
    node->next = arg_list;
    node->val = s_expr;
    node->val->scope = node->next;
    return node;
}

AST_NODE *s_expr_list(AST_NODE *s_expr_list, AST_NODE * s_expr)
{
    AST_NODE * head = s_expr_list;
    while(head->next != NULL)
        head = head->next;
    head->next = s_expr;
    return s_expr_list;
}

AST_NODE *scope(SYMBOL_TABLE_NODE * scope, AST_NODE * s_expr)
{

    s_expr->scope = scope;
    return s_expr;
}

AST_NODE *makeNode(AST_NODE_TYPE type)
{
    AST_NODE * p = calloc(1, sizeof(AST_NODE));
    if (p == NULL) {
        yyerror("out of memory");
        return NULL;
    }
    p->type = type;
    return p;
}

AST_NODE * symbol(char * name)
{
    AST_NODE * p = makeNode(SYM_TYPE);
    p->data.symbol.name = name;
    return p;
}

AST_NODE *number(double value)
{
    AST_NODE *p = makeNode(NUM_TYPE);
    p->data.number.value = value;
    return p;
}

void setParent(AST_NODE * p, AST_NODE * child)
{
    if (child != NULL)
        child->parent = p;
}

void functionParent(AST_NODE * p)
{
    AST_NODE * op = p->data.function.opList;
    while (op != NULL) {
        setParent(p, op);
        op = op->next;
    }
}

AST_NODE * read()
{
    char * line = NULL;
    size_t size = 0;
    printf("read := ");
    getline(&line, &size, stdin);
    double value = strtod(line, NULL);
    return number(value);
}

AST_NODE *function(char *funcName, AST_NODE *opList)
{
    if (strcmp(funcName, "rand") == 0) {
        return number(rand()); //NOLINT
    }

    if (strcmp(funcName, "read") == 0) {
        return read();
    }

    AST_NODE *p = makeNode(FUNC_TYPE);
    p->data.function.name = funcName;
    p->data.function.opList = opList;
    functionParent(p);
    return p;
}

AST_NODE *condition(AST_NODE * cond, AST_NODE * nonzero, AST_NODE * zero)
{
    AST_NODE *p = makeNode(COND_TYPE);
    p->data.condition.cond = cond;
    p->data.condition.zero = zero;
    p->data.condition.nonzero = nonzero;
    setParent(p, p->data.condition.cond);
    setParent(p, p->data.condition.zero);
    setParent(p, p->data.condition.nonzero);
    return p;
}

void freeScope(SYMBOL_TABLE_NODE * p)
{
    if (p == NULL)
        return;

    freeScope(p->next);

    free(p->ident);

    switch(p->type)
    {
        case ARG_TYPE:
            break;
        case VARIABLE_TYPE:
            freeNode(p->val);
            break;
        case LAMBDA_TYPE:
            if (p->val != NULL)
                p->val->scope = NULL;
            freeNode(p->val);
    }
}

void freeData(AST_NODE * p)
{
    if (p == NULL)
        return;

    switch (p->type)
    {
        case NUM_TYPE:
            break;
        case SYM_TYPE:
            free(p->data.symbol.name);
            break;
        case COND_TYPE:
            freeNode(p->data.condition.cond);
            freeNode(p->data.condition.nonzero);
            freeNode(p->data.condition.zero);
            break;
        case FUNC_TYPE:
            free(p->data.function.name);
            freeNode(p->data.function.opList);
    }
}

void freeNode(AST_NODE *p)
{
    if (p == NULL)
        return;
    freeNode(p->next);
    freeData(p);
    freeScope(p->scope);
    free(p);
}

DATA_TYPE evalFunctionType(OPER_TYPE func, DATA_TYPE op1, DATA_TYPE op2)
{
    if (op1 == INTEGER_TYPE)
    {
        if (func == NEG)
            return INTEGER_TYPE;

        if (func == ADD || func == SUB || func == MULT)
            if (op2 == INTEGER_TYPE)
                return INTEGER_TYPE;
    }
    return REAL_TYPE;
}

double divide(double op1, double op2)
{
    if (op2 == 0.0) {
        yyerror("dividing by zero");
        return 0.0;
    }
    return op1 / op2;
}

RETURN_VALUE print(AST_NODE * opList)
{
    RETURN_VALUE value = { NO_TYPE, 0.0};
    if (opList != NULL) {
        printf("=>");
        while (opList != NULL) {
            value = eval(opList);
            if (value.type == INTEGER_TYPE)
                printf(" %.0lf", value.value);
            else
                printf(" %.2lf", value.value);
            opList = opList->next;
        }
        printf("\n");
    }
    return value;
}

double evalFunctionValue(OPER_TYPE func, double op1, double op2)
{
    switch( func )
    {
        case EQUAL:     return op1 == op2;
        case SMALLER:   return op1 < op2;
        case LARGER:    return op1 > op2;
        case NEG:       return -op1;
        case ABS:       return fabs(op1);
        case EXP:       return exp(op1);
        case SQRT:      return sqrt(op1);
        case LOG:       return log10(op1);
        case EXP2:      return exp2(op1);
        case CBRT:      return cbrt(op1);
        case SUB:       return op1 - op2;
        case DIV:       return divide(op1, op2);
        case POW:       return pow(op1, op2);
        case MAX:       return fmax(op1, op2);
        case MIN:       return fmin(op1, op2);
        case HYPOT:     return hypot(op1, op2);
        case REMAINDER: return fmod(op1, op2);
        default:
            yyerror("invalid function");
    }
    return 0.0;
}

int countParameters(SYMBOL_TABLE_NODE * p)
{
    int count = 0;
    while (p != NULL && p->type == ARG_TYPE) {
        ++count;
        p = p->next;
    }
    return count;
}

int countArguments(AST_NODE * opList)
{
    int count = 0;
    while (opList != NULL) {
        ++count;
        opList = opList->next;
    }
    return count;
}

int maxParameters(OPER_TYPE func)
{
    switch( func )
    {
        //unary functions
        case NEG:
        case ABS:
        case EXP:
        case SQRT:
        case LOG:
        case CBRT:
            return 1;

        //binary functions
        case SUB:
        case DIV:
        case POW:
        case MAX:
        case MIN:
        case HYPOT:
        case REMAINDER:
        case EXP2:
        case EQUAL:
        case SMALLER:
        case LARGER:
            return 2;

        //N-ary functions
        case PRINT:
        case ADD:
        case MULT:
            return INT_MAX;

        default:
            yyerror("undefined function");
    }
    return 0;
}

int minParameters(OPER_TYPE func)
{
    switch( func )
    {
        case PRINT:
        case NEG:
        case ABS:
        case EXP:
        case SQRT:
        case LOG:
        case CBRT:
            return 1;

        case ADD:
        case MULT:
        case SUB:
        case DIV:
        case POW:
        case MAX:
        case MIN:
        case HYPOT:
        case REMAINDER:
        case EXP2:
        case EQUAL:
        case SMALLER:
        case LARGER:
            return 2;

        default:
            yyerror("undefined function");
    }
    return 0;
}

bool isValidCall(char * funcName, int arguments, int min, int max)
{
    if (arguments < min) {
        printf("ERROR: too few parameters for the function %s", funcName);
        return false;
    }
    if (arguments > max) {
        printf("WARNING: too many parameters for the function %s", funcName);
    }
    return true;
}

bool evalFunctionNumParameters(char * funcName, OPER_TYPE func, AST_NODE * opList)
{
    return isValidCall(funcName, countArguments(opList), minParameters(func), maxParameters(func));
}


RETURN_VALUE add(AST_NODE * opList)
{
    if (opList == NULL)
        return (RETURN_VALUE){ NO_TYPE, 0.0 };
    if (opList->next == NULL)
        return eval(opList);

    RETURN_VALUE rest = add(opList->next);
    RETURN_VALUE this = eval(opList);
    return (RETURN_VALUE) {
            evalFunctionType(ADD, this.type, rest.type),
            this.value + rest.value
    };
}

RETURN_VALUE mult(AST_NODE * opList)
{
    if (opList == NULL)
        return (RETURN_VALUE) { NO_TYPE, 1.0 };

    RETURN_VALUE rest = mult(opList->next);
    RETURN_VALUE this = eval(opList);
    return (RETURN_VALUE) {
            evalFunctionType(MULT, this.type, rest.type),
            this.value * rest.value
    };
}

void evalFunctionPop(SYMBOL_TABLE_NODE *node, SYMBOL_TABLE_NODE *paramIter) {
    paramIter = node->next;
    while (paramIter != NULL && paramIter->type == ARG_TYPE) {
        STACK_NODE * temp = paramIter->stack;
        paramIter->stack = paramIter->stack->next;
        free(temp);
        paramIter = paramIter->next;
    }
}

void evalFunctionPush(AST_NODE *argIter, SYMBOL_TABLE_NODE *paramIter) {
    while (paramIter != NULL && paramIter->type == ARG_TYPE) {
        STACK_NODE * temp = paramIter->stack;
        paramIter->stack = malloc(sizeof(STACK_NODE));
        paramIter->stack->next = temp;
        paramIter->stack->val = argIter;

        argIter = argIter->next;
        paramIter = paramIter->next;
    }
}

RETURN_VALUE evalFunctionLambda(AST_NODE *p) {
    SYMBOL_TABLE_NODE *node = findSymbol(p, p->data.function.name);
    if (node == NULL) {
        yyerror("invalid function");
        return (RETURN_VALUE) { NO_TYPE, 0.0 };
    }

    int params = countParameters(node->next);
    if (!isValidCall(node->ident, countArguments(p->data.function.opList), params, params))
        return (RETURN_VALUE) { NO_TYPE, 0.0 };

    evalFunctionPush(p->data.function.opList, node->next);
    RETURN_VALUE value = eval(node->val);
    evalFunctionPop(node, node->next);

    return value;
}

RETURN_VALUE evalFunction(AST_NODE * p)
{
    OPER_TYPE func = resolveFunc(p->data.function.name);
    //user defined functions
    if (func == INVALID_OPER)
        return evalFunctionLambda(p);

    //can I call this function
    if (!evalFunctionNumParameters(p->data.function.name, func, p->data.function.opList))
        return (RETURN_VALUE){ NO_TYPE, 0.0 };

    //"special" n-ary functions
    if (func == PRINT)
        return print(p->data.function.opList);
    if (func == ADD)
        return add(p->data.function.opList);
    if (func == MULT)
        return mult(p->data.function.opList);
    //other built in functions
    RETURN_VALUE v1 = eval(p->data.function.opList);
    RETURN_VALUE v2 = eval(p->data.function.opList->next);
    return (RETURN_VALUE) {
            evalFunctionType(func, v1.type, v2.type),
            evalFunctionValue(func, v1.value, v2.value)
    };
}

RETURN_VALUE evalSymbolCast(SYMBOL_TABLE_NODE * node, RETURN_VALUE value)
{   
    if (node->val_type == INTEGER_TYPE && value.type == REAL_TYPE) {
        printf("WARNING: incompatible type assignment for variables %s\n", node->ident);
        return (RETURN_VALUE){ INTEGER_TYPE, round(value.value) };
    }
    if (node->val_type == INTEGER_TYPE)
        return (RETURN_VALUE){ INTEGER_TYPE, round(value.value) };

    if (node->val_type == REAL_TYPE)
        return (RETURN_VALUE){ REAL_TYPE, value.value };

    return value;
}

RETURN_VALUE evalSymbol(AST_NODE * p, char * name)
{
    SYMBOL_TABLE_NODE * node = findSymbol(p, name);
    if (node == NULL) {
        yyerror("Undefined symbol");
        return (RETURN_VALUE) { NO_TYPE, 0.0 };
    }
    if (node->type == ARG_TYPE)
        return evalSymbolCast(node, eval(node->stack->val));
    return evalSymbolCast(node, eval(node->val));
}

RETURN_VALUE evalCondition(AST_NODE *p)
{
    return eval(p->data.condition.cond).value ?
            eval(p->data.condition.nonzero) :
            eval(p->data.condition.zero);
}

RETURN_VALUE eval(AST_NODE *p)
{

    if (p == NULL)
        return (RETURN_VALUE) { NO_TYPE, 0.0 };

    switch( p->type )
    {
        case NUM_TYPE:  return (RETURN_VALUE) { NO_TYPE, p->data.number.value };
        case FUNC_TYPE: return evalFunction(p);
        case SYM_TYPE:  return evalSymbol(p, p->data.symbol.name);
        case COND_TYPE: return evalCondition(p);
        default:
            yyerror("Unkown node type");
            return (RETURN_VALUE) { NO_TYPE, 0.0 };
    }
}
