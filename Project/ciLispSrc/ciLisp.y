%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symNode;
};

%token <sval> FUNC SYMBOL TYPE
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT LET

%type <astNode> s_expr f_expr
%type <symNode> scope let_list let_elem

%%

program:
    s_expr EOL {
        fprintf(stderr, "[ s_expr EOL ] => program\n");
        if ($1) {
            printf("%lf", eval($1).value);
            freeNode($1);
        }
    };

s_expr:
    f_expr {
        fprintf(stderr, "[ f_expr ] => s_expr\n");
        $$ = $1;
    }
    | NUMBER {
        fprintf(stderr, "[ NUMBER ] => s_expr\n");
        $$ = number($1);
    }
    | SYMBOL {
        fprintf(stderr, "[ SYMBOL ] => s_expr\n");
        $$ = symbol($1);
    }
    | LPAREN scope s_expr RPAREN {
        fprintf(stderr, "[ LPAREN scope s_expr RPAREN ] => s_expr\n");
        $$ = scope($2, $3);
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

scope:
    /* EMPTY */ {
        fprintf(stderr, "[ nothing ] => scope\n");
        $$ = NULL;
    }
    | LPAREN LET let_list RPAREN {
        fprintf(stderr, "LPAREN LET let_list RPAREN\n");
        $$ = $3;
    };

let_list:
    let_elem {
        fprintf(stderr, "[ let_elem ] => let_list\n");
        $$ = $1
    }
    | let_list let_elem {
        fprintf(stderr, "[ let_list let_elem ] => let_list\n");
        $$ = let_list($1, $2);
    };

let_elem:
    LPAREN TYPE SYMBOL s_expr RPAREN {
        fprintf(stderr, "[ LPAREN SYMBOL s_expr RPAREN ] => let_elem\n");
        $$ = let_elem($2, $3, $4);
    }
    | LPAREN SYMBOL s_expr RPAREN {
        fprintf(stderr, "[ LPAREN SYMBOL s_expr RPAREN ] => let_elem\n");
        $$ = let_elem(NULL, $2, $3);
    };

%%
