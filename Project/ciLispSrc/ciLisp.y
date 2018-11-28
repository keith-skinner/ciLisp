%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
};

%token <sval> FUNC
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT

%type <astNode> s_expr f_expr

%%

program:
    s_expr EOL {
        fprintf(stderr, "[ s_expr EOL ] => program\n");
        if ($1) {
            printf("%lf", eval($1));
            freeNode($1);
        }
    };

s_expr:
    NUMBER {
        fprintf(stderr, "[ NUMBER ] => s_expr\n");
        $$ = number($1);
    }
    | f_expr {
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "[ QUIT ] => s_expr\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "[ error ] => s_expr\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "[ LPAREN FUNC expr RPAREN ] => f_expr\n");
        $$ = function($2, $3, 0);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "[ LPAREN FUNC expr expr RPAREN ] => f_expr\n");
        $$ = function($2, $3, $4);
    };
%%

