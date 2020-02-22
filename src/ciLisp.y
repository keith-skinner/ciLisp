/**
* Name: Keith Skinner
* Lab: Final Project ciLisp
* Date: 12/04/2018
**/

%{
    #include "ciLisp.h"
    #define ylog(r, p) fprintf(stderr, "BISON: %s ::= %s \n", r, p)
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symNode;
};

%token LPAREN RPAREN EOL QUIT LET COND LAMBDA TEOF
%token <sval> FUNC SYMBOL TYPE
%token <dval> NUMBER
%type <astNode> s_expr f_expr s_expr_list
%type <symNode> scope let_list let_elem arg_list

%%

program:
    s_expr EOL { 
        ylog("program", "s_expr EOL");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
            YYACCEPT;
        }
    }
    | s_expr TEOF {
        ylog("program", "s_expr TEOF");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
        exit(EXIT_SUCCESS);
    }
    | TEOF {
        exit(EXIT_SUCCESS);
    };

s_expr:
    f_expr {
        ylog("s_expr", "f_expr");
        $$ = $1;
    }
    | NUMBER {
        ylog("s_expr", "s_expr");
        $$ = number($1);
    }
    | SYMBOL {
        ylog("s_expr", "SYMBOL");
        $$ = symbol($1);
    }
    | LPAREN scope s_expr RPAREN {
        ylog("s_expr", "LPAREN scope s_expr RPAREN");
        $$ = scope($2, $3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {
        ylog("s_expr", "LPAREN COND s_expr s_expr s_expr RPAREN");
        $$ = condition($3, $4, $5);
    }
    | LPAREN SYMBOL s_expr_list RPAREN {
        ylog("s_expr", "LPAREN SYMBOL s_expr_list RPAREN");
        $$ = function($2, $3);
    }
    | QUIT {
        ylog("s_expr", "QUIT");
        exit(EXIT_SUCCESS);
    };

f_expr:
    LPAREN FUNC s_expr_list RPAREN {
        ylog("f_expr", "LPAREN FUNC s_expr_list RPAREN");
        $$ = function($2, $3);
    };

s_expr_list:
    %empty {
        ylog("s_expr_list", "<empty>");
        $$ = NULL;
    }
    | s_expr {
        ylog("s_expr_list", "s_expr");
        $$ = $1;
    }
    | s_expr_list s_expr {
        ylog("s_expr_list", "s_expr_list s_expr");
        $$ = s_expr_list($1, $2);
    };

scope:
    %empty {
        ylog("scope", "<empty>");
        $$ = NULL;
    }
    | LPAREN LET let_list RPAREN {
        ylog("scope", "LPAREN LET let_list RPAREN");
        $$ = $3;
    };

let_list:
    let_elem {
        ylog("let_list", "let_elem");
        $$ = $1;
    }
    | let_list let_elem {
        ylog("let_list", "let_list let_elem");
        $$ = let_list($1, $2);
    };

let_elem:
    LPAREN TYPE SYMBOL s_expr RPAREN {
        ylog("let_elem", "LPAREN TYPE SYMBOL s_expr RPAREN");
        $$ = let_var_elem($2, $3, $4);
    }
    | LPAREN SYMBOL s_expr RPAREN {
        ylog("let_elem", "LPAREN SYMBOL s_expr RPAREN");
        $$ = let_var_elem(NULL, $2, $3);
    }
    | LPAREN TYPE SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
        ylog("let_elem", "LPAREN TYPE SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN");
        $$ = let_func_elem($2, $3, $6, $8);
    }
    | LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
        ylog("let_elem", "LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN");
        $$ = let_func_elem(NULL, $2, $5, $7);
    };

arg_list:
    %empty {
        ylog("arg_list", "<empty>");
        $$ = NULL;
    }
    | SYMBOL {
        ylog("arg_list", "SYMBOL");
        $$ = let_arg_elem($1);
    }
    | arg_list SYMBOL {
        ylog("arg_list", "arg_list SYMBOL");
        $$ = let_list($1, let_arg_elem($2));
    };
%%
