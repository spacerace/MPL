%term CKB LKB SKB OBJ DBJ SLT FAC PTI PTR PTX GTI GTR GTX
%term AMI API AGI AMR APR AGR PRO PRS EXT

%union
{
	int	ival;
	float	fval;
	char   *tval;
}

%term <tval> TEXT
%term <ival> ICON
%term <fval> FCON

%type <tval> name, object, type, generalizations, groups, 
%type <tval> slot, datatype, facet, text
%type <ival> integer, size origin, index
%type <fval> real

%{
char kbname[30] = "";
%}
%%
load:	  load command | command;

command:	  createkb
		| loadkb
		| storekb
		| createobject
		| deleteobject
		| createslot
		| createfacet
		| putfacett
		| putfaceti
		| putfacetr
		| getfacett
		| getfaceti
		| getfacetr
		| makearrayi
		| putarrayi
		| getarrayi
		| makearrayr
		| putarrayr
		| getarrayr
		| printobject
		| printslot
		| exit;

createkb:	CKB name = {
			if(createkb($2)) {
			    printf("KB %s created\n", $2);
			    strcpy(kbname, $2);
			} else
			    printf("ERROR: KB %s not created\n", $2);
		};

loadkb:		LKB name = {
			if(loadkb($2)) {
			    printf("KB %s loaded\n", $2);
			    strcpy(kbname, $2);
			} else
			    printf("ERROR: KB %s not loaded\n", $2);
		};

storekb:	SKB name = {
			if(storekb($2, 0))
			    printf("KB %s stored\n", $2);
			else
			    printf("ERROR: KB %s not stored\n", $2);
		};

createobject:	OBJ object type generalizations groups = {
			if(ccreateobject($2, $3, $4, $5))
			    printf("Object %s created\n", $2);
			else
			    printf("ERROR: Object %s not created\n", $2);
		};

deleteobject:	DBJ object = {
			if(cdeleteobject($2, 1))
			    printf("Object %s and progeny deleted\n", $2);
			else
			    printf("ERROR: Object %s not deleted\n", $2);
		};

createslot:	SLT object slot datatype = {
			if(ccreateslot($2, $3, $4))
			    printf("Slot %s created\n", $3);
			else
			    printf("ERROR: Slot %s not created\n", $3);
		};

createfacet:	FAC object slot facet = {
			if(ccreatefacet($2, $3, $4))
			    printf("Facet %s created\n", $4);
			else
			    printf("ERROR: Facet %s not created\n", $4);
		};

putfaceti:	PTI object slot facet integer = {
			cputfaceti($2, $3, $4, $5);
		};

putfacetr:	PTR object slot facet real = {
			cputfacetr($2, $3, $4, $5);
		};

putfacett:	PTX object slot facet text = {
			cputfacett($2, $3, $4, $5);
		};

getfaceti:	GTI object slot facet = {int ival;
			ival = cgetfaceti($2, $3, $4);
			printf("value = %d\n", ival);
		};

getfacetr:	GTR object slot facet = {float fval;
			fval = cgetfacetr($2, $3, $4);
			printf("value = %f\n", fval);
		};

getfacett:	GTX object slot facet = {char *tval;
			cgetfacett($2, $3, $4);
			printf("value = %s\n", tval);
		};

makearrayi:	AMI object slot facet size origin = {
			cmakearrayi($2, $3, $4, $5, $6);
		};

putarrayi:	API object slot facet index integer = {
			cputarrayi($2, $3, $4, $5, $6);
		};

getarrayi:	AGI object slot facet index = {int ival;
			cgetarrayi($2, $3, $4, $5);
			printf("value = %d\n", ival);
		};

makearrayr:	AMR object slot facet size origin = {
			cmakearrayr($2, $3, $4, $5, $6);
		};

putarrayr:	APR object slot facet index integer = {
			cputarrayr($2, $3, $4, $5, $6);
		};

getarrayr:	AGR object slot facet index = {float fval;
			fval = cgetarrayr($2, $3, $4, $5);
			printf("value = %f\n", fval);
		};

printobject:	PRO object = {
			cprintobject($2, 1);
		};

printslot:	PRS object slot  = {
			cprintslot($2, $3, 1);
		};

exit:		EXT = {
			exit();
		};

name:		TEXT = {$<tval>$ = save(yytext);};
object:		TEXT = {static char *lastobject;
			if(yytext[0] != '"')
			    $<tval>$ = lastobject = save(yytext);
			else
			    $<tval>$ = lastobject;
			};
type:		TEXT = {$<tval>$ = save(yytext);};
generalizations:TEXT = {$<tval>$ = save(yytext);};
groups:		TEXT = {$<tval>$ = save(yytext);};
slot:		TEXT = {static char *lastslot;
			if(yytext[0] != '"')
			    $<tval>$ = lastslot = save(yytext);
			else
			    $<tval>$ = lastslot;
			};
facet:		TEXT = {static char *lastfacet;
			if(yytext[0] != '"')
			    $<tval>$ = lastfacet = save(yytext);
			else
			    $<tval>$ = lastfacet;
			};
datatype:	TEXT = {$<tval>$ = save(yytext);};
text:		TEXT = {$<tval>$ = save(yytext);};
integer:	ICON = {$<ival>$ = yyival;};
size:		ICON = {$<ival>$ = yyival;};
origin:		ICON = {$<ival>$ = yyival;};
index:		ICON = {$<ival>$ = yyival;};
real:		FCON = {$<fval>$ = yyfval;};

%%

#include <stdio.h>
#include "spikelex.c"
#include "centry.c"

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
