%union
{
	char   *tval;
	int		ival;
	float	fval;
    long    lval;
}

%term <tval> TCON
%term <ival> ICON
%term <fval> FCON

%type <tval> text
%type <ival> integer
%type <fval> real

%%

dialog:		/* empty */ | dialog command;

command:	text integer real = {
			printf("command: text = %s, integer = %d, real = %f\n",$1,$2,$3);
			};

text:		TCON = {$<tval>$ = save(yytext);printf("YACC/TCON: %s\n", $<tval>$);};

integer:	ICON = {$<ival>$ = yyival;printf("YACC/ICON: %d\n", $<ival>$);};

real:		FCON = {$<fval>$ = yyfval;printf("YACC/FCON: %f\n", $<fval>$);};

%%

#include <stdio.h>

extern char yytext[];
extern int yyival;
extern float yyfval;

main()
{
	if(yyparse())
	    printf("Fail\n");
	else
	    printf("Success\n");
}

yyunion(to, from)
YYSTYPE *to, *from;
{
	to->lval = from->lval;
}

yyerror(s)
char *s;
{
	extern int yyline;

	if (yyline)
		fprintf(stderr, "%d: ", yyline);
	fprintf(stderr, "%s\n", s);
}

save(s)
char *s;
{
	char *t;
	if(t = malloc(strlen(s)+1)) {
	    strcpy(t, s);
	    return(t);
	} else {
	    printf("Out of Memory in save\n");
	    exit();
	}
}
