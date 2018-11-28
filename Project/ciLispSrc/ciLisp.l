%option noyywrap
%option nounput
%option noinput

%{
    #include "ciLisp.h"

    #define GET_MACRO(_1, _2, NAME, ...) \
        NAME
    
    #define ldebug2(token, value) \
        fprintf(stderr, #token "(" #value ")\n" )

    #define ldebug1(token) \
        fprintf(stderr, #token "\n")

    #define ldebug(...) GET_MACRO(__VA_ARGS__, ldebug2, ldebug1)(__VA_ARGS__)
%}

digit   [0-9]
num     {digit}+(\.{digit}+)?
func    "neg"|"abs"|"exp"|"sqrt"|"add"|"sub"|"mult"|"div"|"remainder"|"log"|"pow"|"max"|"min"

%%

{num}   {
            ldebug(NUMBER, yylval);
            fprintf(stderr, "lex: NUMBER dval = %lf\n", yylval.dval);
            yylval.dval = strtod(yytext, NULL);
            return NUMBER;
        }

"quit"  {
            ldebug(QUIT);
            return QUIT;
        }

{func}  {
            ldebug(FUNC, yylval.sval);
            fprintf(stderr, "lex: FUNC sval = %s\n", yylval.sval);
            yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
            strcpy(yylval.sval, yytext);
            return FUNC;
        }

"("     {
            fprintf(stderr, "lex: LPAREN\n");
            return LPAREN;
        }

")"     {
            fprintf(stderr, "lex: RPAREN\n");
            return RPAREN;
        }

[\n]    {
            fprintf(stderr, "lex: EOL\n");
            YY_FLUSH_BUFFER;
            return EOL;
        }


[[:space:]] ; /* skip whitespace */
.       { printf("ERROR: invalid character: >>%s<<\n", yytext); }

%%

ssize_t getdelim(char **buf, size_t *bufsiz, int delimiter, FILE *fp)
{
	char *ptr, *eptr;


	if (*buf == NULL || *bufsiz == 0) {
		*bufsiz = BUFSIZ;
		if ((*buf = malloc(*bufsiz)) == NULL)
			return -1;
	}

	for (ptr = *buf, eptr = *buf + *bufsiz;;) {
		int c = fgetc(fp);
		if (c == -1) {
			if (feof(fp))
				return ptr == *buf ? -1 : ptr - *buf;
			else
				return -1;
		}
		*ptr++ = c;
		if (c == delimiter) {
			*ptr = '\0';
			return ptr - *buf;
		}
		if (ptr + 2 >= eptr) {
			char *nbuf;
			size_t nbufsiz = *bufsiz * 2;
			ssize_t d = ptr - *buf;
			if ((nbuf = realloc(*buf, nbufsiz)) == NULL)
				return -1;
			*buf = nbuf;
			*bufsiz = nbufsiz;
			eptr = nbuf + nbufsiz;
			ptr = nbuf + d;
		}
	}
}

ssize_t
getline(char **buf, size_t *bufsiz, FILE *fp)
{
	return getdelim(buf, bufsiz, '\n', fp);
}

/*
 * DO NOT CHANGE THE FOLLOWING CODE CRITICAL FOR REPL!
 */
int main(void) {

    //   freopen("/dev/null", "w", stderr); // except for this line that can be uncommented to throw away debug printouts

    char *s_expr_str = NULL;
    size_t s_expr_str_len = 0;
    YY_BUFFER_STATE buffer;
    while (true) {
        printf("\n> ");
        getline(&s_expr_str, &s_expr_str_len, stdin);
        s_expr_str[s_expr_str_len++] = '\0';
        s_expr_str[s_expr_str_len++] = '\0';
        buffer = yy_scan_buffer(s_expr_str, s_expr_str_len);
        yyparse();
        yy_delete_buffer(buffer);
    }
}
