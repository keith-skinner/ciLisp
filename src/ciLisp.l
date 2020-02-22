/**
* Name: Keith Skinner
* Lab: Final Project ciLisp
* Date: 12/04/2018
**/

%option noyywrap
%option nounput
%option noinput

%{
    #include "ciLisp.h"
    #define llog(token) fprintf(stderr, "LEX: %s ( %s )\n", #token, yytext[0] == '\n' ? "EOL" : yytext)
    #define return_token(id)                        \
        {                                           \
            llog(id);                               \
            return id;                              \
        }
    #define return_string(id)                       \
        {                                           \
            llog(id);                               \
            yylval.sval = malloc((unsigned long long)yyleng+1ULL);         \
            strcpy(yylval.sval, yytext);            \
            return id;                              \
        }
    #define return_number(id)                       \
        {                                           \
            llog(id);                               \
            yylval.dval = strtod(yytext, NULL);     \
            return id;                              \
        }
%}

digit   [0-9]
num     {digit}+(\.{digit}+)?

letter  [a-zA-Z]
symbol  {letter}+

type    "integer"|"real"
func    "neg"|"abs"|"exp"|"sqrt"|"add"|"sub"|"mult"|"div"|"remainder"|"log"|"pow"|"max"|"min"|"exp2"|"cbrt"|"hypot"|"print"|"rand"|"read"|"equal"|"smaller"|"larger"

%%

"("         return_token(LPAREN)
")"         return_token(RPAREN)
"let"       return_token(LET)
"quit"      return_token(QUIT)
"cond"      return_token(COND)
"lambda"    return_token(LAMBDA)
"\n"        return_token(EOL)

{type}      return_string(TYPE)
{func}      return_string(FUNC)
{symbol}    return_string(SYMBOL)

{num}       return_number(NUMBER)

<<EOF>>     return_token(TEOF)
[[:space:]] ; /* skip whitespace */
.           { printf("ERROR: invalid character: >>%s<<\n", yytext); }

%%

int main(void) 
{
    // except for this line that can be uncommented to throw away debug printouts
    // freopen("/dev/null", "w", stderr);
    while(1) {
        printf("> ");
        if (yyparse()) {
            YY_FLUSH_BUFFER;
        }
    }
}
