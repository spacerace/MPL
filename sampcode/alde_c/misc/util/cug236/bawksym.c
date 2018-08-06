/*
	HEADER:		CUG236;
	TITLE:		BAWK Support Module;
	DATE:		05/17/1987;
	VERSION:	1.1;
	FILENAME:	BAWKSYM.C;
	SEE-ALSO:	BAWK.C;
	AUTHORS:	W. C. Colley III, B. Brodt;
*/

/*
 * Bawk C actions builtin functions, variable declaration, and
 * stack management routines.
 */
#include <stdio.h>
#include "bawk.h"

#define MAXARGS		10	/* max # of arguments to a builtin func */
#define F_PRINTF	1
#define F_GETLINE	2
#define F_STRLEN	3
#define F_STRCPY	4
#define F_STRCMP	5
#define F_TOUPPER	6
#define F_TOLOWER	7
#define F_MATCH		8
#define F_NEXTFILE	9

/* Functions that are local to this module.	*/

VARIABLE *decl();

int isfunction( s )
char *s;
{
	/*
	 * Compare the string "s" to a list of builtin functions
	 * and return its (non-zero) token number.
	 * Return zero if "s" is not a function.
	 */
	if ( !strcmp( s, "printf" ) )
		return F_PRINTF;
	if ( !strcmp( s, "getline" ) )
		return F_GETLINE;
	if ( !strcmp( s, "strlen" ) )
		return F_STRLEN;
	if ( !strcmp( s, "strcpy" ) )
		return F_STRCPY;
	if ( !strcmp( s, "strcmp" ) )
		return F_STRCMP;
	if ( !strcmp( s, "toupper" ) )
		return F_TOUPPER;
	if ( !strcmp( s, "tolower" ) )
		return F_TOLOWER;
	if ( !strcmp( s, "match" ) )
		return F_MATCH;
	if ( !strcmp( s, "nextfile" ) )
		return F_NEXTFILE;
	return 0;
}

int iskeyword( s )
char *s;
{
	/*
	 * Compare the string "s" to a list of keywords and return its
	 * (non-zero) token number.  Return zero if "s" is not a keyword.
	 */
	if ( !strcmp( s, "char" ) )
		return T_CHAR;
	if ( !strcmp( s, "int" ) )
		return T_INT;
	if ( !strcmp( s, "if" ) )
		return T_IF;
	if ( !strcmp( s, "else" ) )
		return T_ELSE;
	if ( !strcmp( s, "while" ) )
		return T_WHILE;
	if ( !strcmp( s, "break" ) )
		return T_BREAK;

	if ( !strcmp( s, "NF" ) )
		return T_NF;
	if ( !strcmp( s, "NR" ) )
		return T_NR;
	if ( !strcmp( s, "FS" ) )
		return T_FS;
	if ( !strcmp( s, "RS" ) )
		return T_RS;
	if ( !strcmp( s, "FILENAME" ) )
		return T_FILENAME;
	if ( !strcmp( s, "BEGIN" ) )
		return T_BEGIN;
	if ( !strcmp( s, "END" ) )
		return T_END;
	return 0;
}

void function(funcnum)
int funcnum;
{
	int argc;
	char lpar;
	DATUM args[MAXARGS];

	argc = 0;
	if (Token == T_LPAREN) {
		lpar = 1;  getoken();
	}
	else lpar = 0;
	/*
	 * If there are any arguments, evaluate them and copy their values
	 * to a local array.
	 */
	if (Token != T_RPAREN && Token != T_EOF) {
		for (;;) {
			expression();
			if (argc < MAXARGS) args[argc++].ival = popint();
			else popint();
			if (Token == T_COMMA) getoken();
			else break;
		}
	}
	if (lpar && Token != T_RPAREN) error( "missing ')'", ACT_ERROR );
	else if (Token == T_RPAREN) getoken();

	switch (funcnum) {
	       case F_PRINTF:  /* just like the real printf() function */
		    pushint(printf(args[0].dptr,args[1].ival,args[2].ival,
			args[3].ival,args[4].ival,args[5].ival, args[6].ival,
			args[7].ival,args[8].ival,args[9].ival));
		    break;
	       case F_GETLINE:
		/*
		 * Get the next line of input from the current input file
		 * and parse according to the current field seperator.
		 * Don't forget to free up the previous line's words first...
		 */
		    while (Fieldcount) free(Fields[--Fieldcount]);
		    pushint(getline());
		    Fieldcount = parse(Linebuf,Fields,Fldsep);
		    break;
	       case F_STRLEN:  /* calculate length of string argument */
		    pushint(strlen(args[0].dptr));
		    break;
	       case F_STRCPY:  /* copy second string to first string */
		    pushint((int)strcpy(args[0].dptr,args[1].dptr));
		    break;
	       case F_STRCMP:  /* compare two strings */
		    pushint(strcmp(args[0].dptr,args[1].dptr));
		    break;
	       case F_TOUPPER: /* convert character argument to upper case */
		    pushint(toupper(args[0].ival));
		    break;
	       case F_TOLOWER: /* convert character argument to lower case */
		    pushint(tolower(args[0].ival));
		    break;
	       case F_MATCH:   /* match string argument to regular expr. */
		    pushint(match(args[0].dptr,args[1].dptr));
		    break;
	       case F_NEXTFILE:/* close current input file, process next */
		    endfile();	pushint(0);
		    break;
	       default:	       /* oops! */
		    error("bad function call",ACT_ERROR);
	}
}

VARIABLE *findvar( s )
char *s;
{
	/*
	 * Search the symbol table for a variable whose name is "s".
	 */
	VARIABLE *pvar;
	int i;
	char name[ MAXVARLEN ];

	i = 0;
	while ( i < MAXVARLEN && alphanum( *s ) )
		name[i++] = *s++;
	if ( i<MAXVARLEN )
		name[i] = 0;

	for ( pvar = Vartab; pvar<Nextvar; ++pvar )
	{
		if ( !strncmp( pvar->vname, name, MAXVARLEN ) )
			return pvar;
	}
	return NULL;
}

VARIABLE *addvar( name )
char *name;
{
	/*
	 * Add a new variable to symbol table and assign it default
	 * attributes (int name;)
	 */
	int i;

	if ( Nextvar <= Vartab + MAXVARTABSZ )
	{
		i = 0;
		while ( i<MAXVARLEN && alphanum( *name ) )
			Nextvar->vname[i++] = *name++;
		if ( i<MAXVARLEN )
			Nextvar->vname[i] = 0;

		Nextvar->vclass = 0;
		Nextvar->vsize = WORD;
		Nextvar->vlen = 0;
		/*
		 * Allocate some new room
		 */
		Nextvar->vptr = getmem( WORD );
		fillmem( Nextvar->vptr, WORD, 0 );
	}
	else
		error( "symbol table overflow", MEM_ERROR );

	return Nextvar++;
}

void declist()
{
	/*
	 * Parse a "char" or "int" statement.
	 */
	char type;

	type = Token;
	getoken();
	decl( type );
	while ( Token==T_COMMA )
	{
		getoken();
		decl( type );
	}
	if ( Token==T_SEMICOLON )
		getoken();
}

VARIABLE *decl( type )
int type;
{
	/*
	 * Parse an element of a "char" or "int" declaration list.
	 * The function stmt_compile() has already entered the variable
	 * into the symbol table as an integer, this routine simply changes
	 * the symbol's class, size or length according to the declaraction.
	 * WARNING: The interpreter depends on the fact that pointers are
	 * the same length as int's.  If your machine uses long's for
	 * pointers either change the code or #define int long (or whatever).
	 */
	char class, size;
	int len;
	unsigned oldsize, newsize;
	VARIABLE *pvar;

	if ( Token==T_MUL )
	{
		/*
		 * it's a pointer
		 */
		getoken();
		pvar = decl( type );
		++pvar->vclass;
	}
	else if ( Token==T_VARIABLE )
	{
		/*
		 * Simple variable so far.  The token value (in the global
		 * "Value" variable) is a pointer to the variable's symbol
		 * table entry.
		 */
		pvar = (VARIABLE *)Value.dptr;
		getoken();
		class = 0;
		/*
		 * Compute its length
		 */
		if ( Token==T_LBRACKET )
		{
			/*
			 * It's an array.
			 */
			getoken();
			++class;
			/*
			 * Compute the dimension
			 */
			expression();
			if ( Token!=T_RBRACKET )
				error( "missing ']'", ACT_ERROR );
			getoken();
			len = popint();
		}
		else
			/*
			 * It's a simple variable - array length is zero.
			 */
			len = 0;

		size = (type==T_CHAR) ? BYTE : WORD;

		newsize = (len ? len : 1) * size;
		oldsize = (pvar->vlen ? pvar->vlen : 1) * pvar->vsize;
		if ( newsize != oldsize )
		{
			/*
			 * The amount of storage needed for the variable
			 * has changed - free up memory allocated initially
			 * and reallocate for new size.
			 */
			free( pvar->vptr );
			pvar->vptr = getmem( newsize );
		}
		/*
		 * Now change the variable's attributes.
		 */
		pvar->vclass = class;
		pvar->vsize = size;
		pvar->vlen = len;
	}
	else
		syntaxerror();

	return pvar;
}

void assignment()
{
	/*
	 * Perform an assignment
	 */
	int ival;

	ival = popint();
	/*
	 * make sure we've got an lvalue
	 */
	if ( Stackptr->lvalue )
	{
		if ( Stackptr->class )
			movemem( &ival, Stackptr->value.dptr, WORD );
		else
			movemem(&ival, Stackptr->value.dptr, Stackptr->size);
		pop();
		pushint( ival );
	}
	else
		error( "'=' needs an lvalue", ACT_ERROR );
}

int pop()
{
	/*
	 * Pop the stack and return the integer value
	 */
	if ( Stackptr >= Stackbtm )
		return (Stackptr--)->value.ival;
	error( "stack underflow", ACT_ERROR );
	return 0;
}

void push( pclass, plvalue, psize, pdatum )
char pclass, plvalue, psize;
DATUM *pdatum;
{
	/*
	 * Push item parts onto the stack
	 */
	if ( ++Stackptr <= Stacktop )
	{
		Stackptr->lvalue = plvalue;
		Stackptr->size = psize;
		if ( !(Stackptr->class = pclass)  &&  !plvalue )
			Stackptr->value.ival = pdatum->ival;
		else
			Stackptr->value.dptr = pdatum->dptr;
	}
	else
		error( "stack overflow", MEM_ERROR );
}

void pushint( intvalue )
int intvalue;
{
	/*
	 * push an integer onto the stack
	 */
	if ( ++Stackptr <= Stacktop )
	{
		Stackptr->lvalue =
		Stackptr->class = 0;
		Stackptr->size = WORD;
		Stackptr->value.ival = intvalue;
	}
	else
		error( "stack overflow", MEM_ERROR );
}

int popint()
{
	/*
	 * Resolve the item on the top of the stack and return it
	 */
	int intvalue;

	if ( Stackptr->lvalue )
	{
		/*
		 * if it's a byte indirect, sign extend it
		 */
		if ( Stackptr->size == BYTE && !Stackptr->class )
			intvalue = *Stackptr->value.dptr;
		else
		{
			/*
			 * otherwise, it's an unsigned int
			 */
			intvalue = *(int *)Stackptr->value.dptr;
		}
		pop();
		return intvalue;
	}
	else
	{
		/*
		 * else it's an ACTUAL, just pop it
		 */
		return pop();
	}
}
