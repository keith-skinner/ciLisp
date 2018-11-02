#include "ciLisp.h"

void yyerror(char *s) {
    printf("\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

//
// find out which function it is
//
int resolveFunc(char *func)
{
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min", ""};

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

//
// create a node for a number
//
AST_NODE *number(double value)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}



//
// create a node for a function
//
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;

    return p;
}

//
// free a node
//
void freeNode(AST_NODE *p)
{
    if (!p)
        return;

    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }

    free(p);
}

double evalFunction(AST_NODE *p)
{
    switch( resolveFunc(p->data.function.name) )
    {
        case NEG:   return -eval(p->data.function.op1);
        case ABS:   return fabs(eval(p->data.function.op1));
        case EXP:   return exp(eval(p->data.function.op1));
        case SQRT:  return sqrt(eval(p->data.function.op1));
        case ADD:   return eval(p->data.function.op1) + eval(p->data.function.op2);
        case SUB:   return eval(p->data.function.op1) - eval(p->data.function.op1);
        case MULT:  return eval(p->data.function.op1) * eval(p->data.function.op1);
        case DIV:   return eval(p->data.function.op1) / eval(p->data.function.op1);
        case LOG:   return log10(eval(p->data.function.op1));
        case POW:   return pow(eval(p->data.function.op1), eval(p->data.function.op2));
        case MAX:   return fmax(eval(p->data.function.op1), eval(p->data.function.op2));
        case MIN:   return fmin(eval(p->data.function.op1), eval(p->data.function.op2));
        case EXP2:  return exp2(eval(p->data.function.op1));
        case CBRT:  return cbrt(eval(p->data.function.op1));
        case HYPOT: return hypot(eval(p->data.function.op1), eval(p->data.function.op2));
        case REMAINDER: return fmod(eval(p->data.function.op1), eval(p->data.function.op2));
        default:
            fprintf(stderr, "ERROR evalFunction %d", resolveFunc(p->data.function.name));
            return 0.0;
    }
}

double eval(AST_NODE *p)
{
    if (!p)
        return 0.0;

    double value = 0.0;
    switch( p->type )
    {
        case NUM_TYPE:
            value = p->data.number.value;
            break;
        case FUNC_TYPE:
            value = evalFunction(p);
            break;
    }
    return value;
}  

