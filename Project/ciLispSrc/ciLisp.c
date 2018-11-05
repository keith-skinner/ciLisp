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

double evalFunction(OPER_TYPE type, double op1, double op2)
{
    switch( type )
    {
        case NEG: return -op1;
        case ABS: return fabs(op1);
        case EXP: return exp(op1);
        case SQRT: return sqrt(op1);
        case ADD: return op1 + op2;
        case SUB: return op1 - op2;
        case MULT: return op1 * op2;
        case DIV: return op1 / op2;
        case LOG: return log10(op1);
        case POW: return pow(op1, op2);
        case MAX: return fmax(op1, op2);
        case MIN: return fmin(op1, op2);
        case EXP2: return exp2(op1);
        case CBRT: return cbrt(op1);
        case HYPOT: return hypot(op1, op2);
        case REMAINDER: return fmod(op1, op2);
        default:
            fprintf(stderr, "ERROR invalid function: %d", type);
            return 0.0;
    }
}

double eval(AST_NODE *p)
{
    if (!p)
        return 0.0;
    switch( p->type )
    {
        case NUM_TYPE:
            return p->data.number.value;
        case FUNC_TYPE:
            return evalFunction(
                resolveFunc(p->data.function.name),
                eval(p->data.function.op1),
                eval(p->data.function.op2) 
            );
        default:
            printf("ERROR invalid node type: %d\n", p->type);
            return 0.0;
    }
}  
