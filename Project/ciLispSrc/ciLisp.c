#include "ciLisp.h"

void yyerror(char *s) {
    printf("\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}


SYMBOL_TABLE_NODE * findSymbol(SYMBOL_TABLE_NODE * symbol, const char * name)
{
    while (symbol != NULL) {
        if ( strcmp(name, symbol->ident) == 0 )
            return symbol;
        symbol = symbol->next;
    }
    return NULL;
}

SYMBOL_TABLE_NODE * let_list(SYMBOL_TABLE_NODE * list, SYMBOL_TABLE_NODE * elem)
{
    SYMBOL_TABLE_NODE * node = findSymbol(list, elem->ident);
    if (node != NULL) {
        yyerror("duplicate symbol");
        return list;
    }
    elem->next = list;
    return elem;
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

SYMBOL_TABLE_NODE * let_elem(char * type, char * name, AST_NODE * s_expr)
{
    SYMBOL_TABLE_NODE * p = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    p->val_type = resolveType(type);
    p->ident = name;
    p->val = s_expr;
    return p;
}

int resolveFunc(char *func)
{
    char *funcs[] = {
        "neg", "abs", "exp", "sqrt", "add", "sub", 
        "mult", "div", "remainder", "log", "pow", 
        "max", "min", "exp2", "cbrt", "hypot", 
        "print", ""
    };

    int i = 0;
    while (funcs[i][0] != '\0')
    {
        if (strcmp(funcs[i], func) == 0)
            return i;
        i++;
    }
    yyerror("invalid function");
    return INVALID_OPER;
}

AST_NODE *scope(SYMBOL_TABLE_NODE * scope, AST_NODE * s_expr)
{
    s_expr->scope = scope;
    return s_expr;
}

AST_NODE *makeNode(AST_NODE_TYPE type)
{
    AST_NODE * p = calloc(1, sizeof(AST_NODE));
    if (p == NULL)
        yyerror("out of memory");
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

AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p = makeNode(FUNC_TYPE);
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;
    if (p->data.function.op1 != NULL)
        p->data.function.op1->parent = p;
    if (p->data.function.op2 != NULL)
        p->data.function.op2->parent = p;
    return p;
}


void freeScope(SYMBOL_TABLE_NODE *p)
{
    if (p == NULL)
        return;
    free(p->ident);
    freeNode(p->val);
    freeScope(p->next);
    free(p);
}

void freeNode(AST_NODE *p)
{
    if (p == NULL)
        return;

    freeScope(p->scope);
    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }
    else if (p->type == SYM_TYPE)
    {
        free(p->data.symbol.name);
    }
    free(p);
}

double divide(double op1, double op2)
{
    if (op2 == 0.0) {
        yyerror("dividing by zero");
        return 0.0;
    }
    return op1 / op2;
}

double evalFunctionValue(OPER_TYPE func, double op1, double op2)
{
    switch( func )
    {
        case NEG:       return -op1;
        case ABS:       return fabs(op1);
        case EXP:       return exp(op1);
        case SQRT:      return sqrt(op1);
        case ADD:       return op1 + op2;
        case SUB:       return op1 - op2;
        case MULT:      return op1 * op2;
        case DIV:       return divide(op1, op2);
        case LOG:       return log10(op1);
        case POW:       return pow(op1, op2);
        case MAX:       return fmax(op1, op2);
        case MIN:       return fmin(op1, op2);
        case EXP2:      return exp2(op1);
        case CBRT:      return cbrt(op1);
        case HYPOT:     return hypot(op1, op2);
        case REMAINDER: return fmod(op1, op2);
        default:
            yyerror("undefined function");
    }
    return 0.0;
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

void print(RETURN_VALUE op) 
{
    if (op.type == INTEGER_TYPE)
        printf("=> %.0lf\n", op.value);
    else 
        printf("=> %.2lf\n", op.value);
}

RETURN_VALUE evalFunction(OPER_TYPE func, RETURN_VALUE op1, RETURN_VALUE op2)
{
    if (func == PRINT)
    {
        print(op1);
        return op1;
    }

    return (RETURN_VALUE){
        evalFunctionType(func, op1.type, op2.type), 
        evalFunctionValue(func, op1.value, op2.value) 
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
    if (p == NULL) {
        yyerror("Undefined symbol");
        return (RETURN_VALUE) { NO_TYPE, 0.0 };
    }

    SYMBOL_TABLE_NODE * node = findSymbol(p->scope, name);
    if (node != NULL)
        return evalSymbolCast(node, eval(node->val));
    return evalSymbol(p->parent, name);
}

RETURN_VALUE eval(AST_NODE *p)
{
    if (p == NULL)
        return (RETURN_VALUE) { NO_TYPE, 0.0 };

    switch( p->type )
    {
        case NUM_TYPE:  return (RETURN_VALUE) { NO_TYPE, p->data.number.value };
        case FUNC_TYPE: return evalFunction(resolveFunc(p->data.function.name), 
                            eval(p->data.function.op1), eval(p->data.function.op2));
        case SYM_TYPE:  return evalSymbol(p, p->data.symbol.name);
        default:
            yyerror("Unkown node type");
            return (RETURN_VALUE) { NO_TYPE, 0.0 };
    }
}  
