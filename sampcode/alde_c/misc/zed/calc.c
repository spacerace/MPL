#include <stdio.h>
#include <ctype.h>

/*
 * Calculator program - 9 May 1985
 * Please send corrections or remarks to:
 *	Bob Brodt
 *	34 Mehrhof Rd.
 *	Little Ferry, NJ 07643
 *	(201)-641-9582
 *
 * This is a simple integer arithmetic calculator program. It uses infix
 * notation, i.e. 1+2*3 as opposed to "reverse polish" notation: 1 2 3 * +.
 *
 * CONSTANTS:
 *	Numbers may be input as in C using 0x notation for hex, and a
 *	leading zero for octal, everything else is assumed to be decimal.
 * VARIABLES:
 *	Also available are 26 registers referenced by a single letter
 *	(a-z or A-Z, case is ignored).
 * OPERATORS:
 *	The following operators are supported (from highest precedence to
 *	lowest):
 *
 *	( )		associativity
 *	~		one's complement
 *	* / %		mutliply, divide and modulo
 *	+ -		unary and binary add & subtract
 *	<< >>		shift left and right
 *	&		bitwise AND
 *	^		bitwise exclusive OR
 *	|		bitwise inclusive OR
 *	=		assignment
 *	,		comma - seperates function arguments
 *
 *	All operators associate from left to right with the exception of
 *	the assignment (=) operator.
 * FUNCTIONS:
 *	The calculator also has built-in function capabilties which may be
 *	enhanced later:
 *	base n  - set the output conversion base to n (n = 8, 10 or 16)
 * ADDITIONAL FEATURES:
 *	help - displays a help screen containing the above information.
 *	? - displays all of the variables and their current values.
 */

/*
 * Tokens
 */
#define T_EOL		';'
#define T_CONST		'C'
#define T_SYMBOL	'S'
#define T_LPAR		'('
#define T_RPAR		')'
#define T_COMMA		','
#define T_ASSIGN	'='
#define T_POINT		'$'
#define T_MUL		'*'
#define T_DIV		'/'
#define T_MOD		'%'
#define T_ADD		'+'
#define T_SUB		'-'
#define T_NEG		'_'
#define T_NOT		'~'
#define T_SHL		'L'
#define T_SHR		'R'
#define T_AND		'&'
#define T_XOR		'^'
#define T_IOR		'|'
#define T_FUNC		'F'


/*
 * Operator/Operand stack, base ptr and top of stack ptr
 */
#define STAKSZ 128
struct {
	char o_token;
	int o_value;
} Opstk[ STAKSZ ];
int Opbase, Opsp;

/*
 * Value (working) stack, base ptr and top of stack ptr
 */
int Valstk[ STAKSZ ];
int Valbase, Valsp;

/*
 * Built-in Functions and jump table
 */
int f_sum(), f_base();
int (*Functab[ 2 ])();

/*
 * Symbol Table
 */
int Symbols[ 26 ];
	
/*
 * Miscellaneous
 */
#define MAXLEVEL 32
char Level;	/* current recursion level in calc() */
char Token;	/* current input token */
int Value;	/* and its value */
char Eol;	/* set when end of line encountered */
char *Lineptr;	/* points to next character in input line */
char *Ofmt;	/* current output format (set by "base" command) */
int Error;	/* set if parsing error occurred */
char Nohelp;	/* set if help was given already */

char *skipws();

main()
{
	char line[ 1024 ], *cp;

	initialize();

	for ( ;; )
	{
		Error = 0;
		/*
		 * read a line from stdin and try to parse it
		 */
		printf( "? " );
		if ( ! gets( line ) )
			break;
		cp = skipws( line );
		if ( *cp )
		{
			if ( *cp == '?' )
			{
				dumpvars();
				continue;
			}
			else if ( strcmp( cp, "quit" ) == 0 )
				break;
			else if ( strcmp( cp, "help" ) == 0 )
			{
				help();
				continue;
			}
			calc( cp );
			/*
			 * If no parsing errors were found, print the
			 * results of the evaluation of the expression.
			 */
			if ( Error && Nohelp )
			{
				printf( "do you need help ? " );
				gets( line );	
				cp = skipws( line );	
				if ( toupper( *cp ) == 'Y' )
					help();
			}
			else if ( ! Error )
			{
				printf( Ofmt, Value );
				putchar( '\n' );
			}
		}
	}
	printf( "bye bye\n" );
}

initialize()
{
	/*
	 * initialize
	 */
	Functab[0] = f_sum;
	Functab[1] = f_base;
		
	push( 10 );
	f_base();
	Nohelp = 1;
}

help()
{
	Nohelp = 0;

printf("This calculator uses infix notation i.e. 1+2*3 as opposed to\n");
printf("\"reverse polish\" notation: 1 2 3 * +.\n");
printf("CONSTANTS:\n");
printf(" Numbers may be entered using 0x notation for hex (like in C),\n");
printf(" and a leading zero for octal, everything else is assumed to be\n");
printf(" decimal.\n");
printf("VARIABLES:\n");
printf(" Also available are 26 registers referenced by a single letter\n");
printf(" (a-z or A-Z, case is ignored).\n");
printf("OPERATORS:\n");
printf(" The following operators are supported (from highest precedence\n");
printf(" to lowest):\n");
printf("\n");
printf(" ( )		associativity\n");
printf(" ~		one's complement\n");
printf(" * / %		mutliply, divide and modulo\n");
printf(" + -		unary and binary add & subtract\n");
printf(" << >>		shift left and right\n");
printf(" &		bitwise AND\n");
printf(" ^		bitwise exclusive OR\n");
printf(" |		bitwise inclusive OR\n");
printf("-MORE-" );
	getchar();
printf(" =		assignment\n");
printf(" ,		comma - seperates function arguments\n");
printf("\n");
printf(" All operators associate from left to right with the exception\n");
printf(" of the assignment (=) operator.\n");
printf("FUNCTIONS:\n");
printf(" The calculator also has built-in function capabilties (which\n");
printf(" may be enhanced in a later revision):\n");
printf("\n");
printf(" base n  - set the output conversion base to n (n = 8, 10 or 16)\n");
printf("\n");
printf("ADDITIONAL FEATURES:\n");
printf(" help - displays a help screen containing the above information.\n");
printf(" ? - displays all of the variables and their current values.\n");
}

dumpvars()
{
	int i;

	for ( i=0; i<26; ++i )
	{
		if ( !(i % 8) )
			printf( "\n" );
		printf( "%c=", i + 'A' );
		printf( Ofmt, Symbols[ i ] );
		printf( "\t" );
	}
	printf( "\n" );
}

/*************************************************************
*                 EXPRESSION EVALUATOR                       *
**************************************************************/
/*
 * NOTE: The comments make reference to "lvalues" and "rvalues". These
 * are attributes of <primaries> (primaries, for the layman, are things
 * like constants and variables, and parenthesized expressions. If you
 * don't know what an expression is, you shouldn't be a reading this!).
 * If a <primary> is an "lvalue", it means that it can usually be found on
 * LEFT-HAND side of an assignment operator. "rvalues" can only be found
 * on the RIGHT-HAND side of an assignment. Simply stated, only things like
 * variables can be used as both "lvalues" and "rvalues", whereas things
 * like constants and parenthesized expressions can only be "rvalues" since
 * it wouldn't make sense to say: 12 = 5.
 */


calc( line )
char *line;
{
	/*
	 * Parse and calculate an arithmetic expression pointed to
	 * by "line". This routine is fully re-entrant - a feature
	 * that is not currently used, but may come in handy later
	 * (for instance if a variable points to a character string
	 * that contains an expression to be evaluated).
	 */
	char *savLineptr;
	int savValbase, savValsp, savOpbase, savOpsp;

	if ( ++Level > MAXLEVEL )
	{
		err( "recursion" );
	}
	else
	{
		/*
		 * Save all global variables that may change if calc()
		 * is called recursively.
		 */
		savLineptr = Lineptr;
		savValbase = Valbase;
		savValsp = Valsp;
		savOpbase = Opbase;
		savOpsp = Opsp;

		/*
		 * Set up stack base ptrs and input line ptr
		 */
		Valbase = Valsp;
		Opbase = Opsp;
		Lineptr = line;

		/*
		 * Get the first token from input line and parse an
		 * expression and then evaluate it.
		 */
		Eol = 0;

		getoken();
		if ( ! Eol )
		{
			expression();
			if ( Error <= 0 )
				Value = evaluate();
		}
		/*
		 * Restore previous values of globals
		 */
		Lineptr = savLineptr;
		Valbase = savValbase;
		Valsp = savValsp;
		Opbase = savOpbase;
		Opsp = savOpsp;
	}

	--Level;
}

evaluate()
{
	/*
	 * Evaluate an expression by popping operators and operands
	 * from the Operator/Operand stack and performing each indicated
	 * operation. Only the operators starting from current base ptr
	 * (Opbase) to top of stack (Opsp) are evaluated, so that evaluate()
	 * may be called by any generation of calc().
	 */
	int opsp, val, *ip;
	char op;

#define TOS (Valstk[Valsp-1])	/* top of stack macro */

	for ( opsp=Opbase; opsp<Opsp; ++opsp )
	{
		op = Opstk[ opsp ].o_token;

		if ( op == T_CONST )
			push( Opstk[ opsp ].o_value );
		else if ( op == T_SYMBOL )
			/*
			 * Push the address of a variable
			 */
			push( &Symbols[ Opstk[ opsp ].o_value ] );
		else if ( op == T_POINT )
		{
			/*
			 * Get the value of the integer pointed to by the
			 * address on top of the stack. This usually follows
			 * a T_SYMBOL when the symbol is not being used
			 * as an "lvalue".
			 */
			ip = pop();
			push( *ip );
		}
		else if ( op == T_ASSIGN )
		{
			/*
			 * Assignment operator: The item on top of stack is
			 * the "rvalue", second on stack is the "lvalue"
			 * (an address where to store the "rvalue"). The
			 * "rvalue" gets pushed back on top of the stack.
			 */
			val = pop();
			ip = pop();
			push( *ip = val );
		}
		else if ( op == T_ADD )
		{
			val = pop();
			TOS += val;
		}
		else if ( op == T_SUB )
		{
			val = pop();
			TOS -= val;
		}
		else if ( op == T_NOT )
			TOS = ~TOS;
		else if ( op == T_NEG )
			TOS = -TOS;
		else if ( op == T_MUL )
		{
			val = pop();
			TOS *= val;
		}
		else if ( op == T_DIV )
		{
			val = pop();
			TOS /= val;
		}
		else if ( op == T_MOD )
		{
			val = pop();
			TOS %= val;
		}
		else if ( op == T_SHL )
		{
			val = pop();
			TOS <<= val;
		}
		else if ( op == T_SHR )
		{
			val = pop();
			TOS >>= val;
		}
		else if ( op == T_AND )
		{
			val = pop();
			TOS &= val;
		}
		else if ( op == T_XOR )
		{
			val = pop();
			TOS ^= val;
		}
		else if ( op == T_IOR )
		{
			val = pop();
			TOS |= val;
		}
		else if ( op == T_COMMA )
			;
		else if ( op == T_FUNC )
			push( (*Functab[ Opstk[ opsp ].o_value ])() );
		else
			err( "bad operator in stack: 0x%x (%c)", op, op );
	}
	return Valstk[ Valbase ];
}

push( val )
{
	if ( Valsp >= STAKSZ )
		err( "stack overflow" );
	return Valstk[ Valsp++ ] = val;
}

pop()
{
	if ( --Valsp < 0 )
		Valsp = 0;
	return Valstk[ Valsp ];
}

/*************************************************************
*                   EXPRESSION PARSER                        *
**************************************************************/
expression()
{
	/*
	 * Parse an expression. Expressions have the following syntax:
	 *    <expression> := <primary> <operator> <primary>
	 * so the first thing to look for is a primary.
	 */
	int lvalue;

	if ( !(lvalue = primary()) )
		err( "bad expression" );
	else if ( Eol )
	{
		if ( lvalue < 0 )
			generate( T_POINT, 0 );
	}
	else
		op_prim( 0, lvalue );
}

op_prim( precedence, lvalue )
int precedence;	/* precedence of current <operator> */
int lvalue;	/* type of current <primary>: -1 => lvalue */
		/*                             0 => no <primary> (error) */
		/*                             1 => rvalue */
{
	/*
	 * Parse the <operator> <primary> part of an expression.
	 * "precedence" is the PREVIOUS <operator>'s precedence level
	 * (0=low, +n=high).
	 */
	char tkn;
	int pr, lv;

	while ( ! Eol )
	{
		/*
		 * Get the precedence level of current <operator> ("pr").
		 * If it is greater than previous operator ("precedence"),
		 * get the next <primary> and do another <operator> <primary>
		 * NOTE: For left-to-right associativity, the condition
		 *     pr > precedence
		 * must be true. for right-to-left associativity,
		 *     pr >= precedence
		 * must be true (assignment operator only).
		 */

		pr = binop( Token );

		if (
			(pr>precedence && pr>0) ||
			(Token==T_ASSIGN && pr>=precedence)
		)
		{
			if ( Token == T_ASSIGN )
			{
				if ( lvalue > 0 )
					err( "= needs and lvalue" );
			}
			else if ( lvalue < 0 )
				generate( T_POINT, 0 );

			/*
			 * Save the operator token and do a primary.
			 */
			tkn = Token;
			getoken();
			if ( ! (lv = primary()) )
				err( "missing an operand" );
			/*
			 * Now look at the next operator. If its precedence
			 * is greater than this one ("tkn" above), generate
			 * code for it BEFORE this one.
			 */
			lvalue = op_prim( pr, lv );

			if ( Token != T_ASSIGN && lvalue < 0 )
			{
				/*
				 * Next operator is not the assignment op.
				 * and the current <primary> is an lvalue,
				 * therefore generate a "load from address
				 * on top of stack" instruction.
				 */
				generate( T_POINT, 0 );
				/*
				 * This makes it an rvalue now.
				 */
				lvalue = 1;
			}
			else if ( tkn!=T_ASSIGN && Token==T_ASSIGN )
			{
				/*
				 * YEECH! this is the only way I know of to
				 * detect errors like: a+b=c
				 */
				err( "= needs an lvalue" );
			}

			/*
			 * Generate the instruction for the current operator.
			 */
			generate( tkn, 0 );
		}
		else
			break;
	}

	return lvalue;
}

primary()
{
	/*
	 * Parse a primary. Primaries have the following syntax:
	 *    <primary> := <constant> |
	 *                 '(' <expression> ')' |
	 *                 <unary op> <primary> |
	 *                 <function> <primary>
	 */
	int rtn;
	int val;

	if ( Eol )
		return 0;

	/*
	 * Return value:
	 *   -1   => the <primary> is an lvalue
	 *    0   => not a <primary> (usually end of line or a syntax error)
	 *    1   => the <primary> is an rvalue
	 */
	rtn = 1;

	switch ( Token )
	{
	case T_SYMBOL:	/* a symbol */
		rtn = -1;
	case T_CONST:	/* a constant */
		generate( Token, Value );
		getoken();
		break;
	case T_LPAR:	/* a parenthesized expression */
		getoken();
		expression();
		if ( Token != T_RPAR )
		{
			err( "missing ')'" );
			rtn = 0;
		}
		else
			getoken();
		break;
	case T_SUB:	/* unary - */
		/*
		 * The lexical analyzer is not smart enough to recognize
		 * unary operators (+ and -), that's why we have to do
		 * it here
		 */
		getoken();
		expression();
		generate( T_NEG, 0 );
		break;
	case T_NOT:	/* unary ~ */
		getoken();
		expression();
		generate( T_NOT, 0 );
		break;
	case T_ADD:	/* unary + */
		getoken();
		expression();
		break;
	case T_FUNC:	/* built-in function */
		val = Value;
		getoken();
		expression();
		generate( T_FUNC, val );
		break;
	default:
		/*
		 * Not a primary
		 */
		rtn = 0;
	}
	return rtn;
}

binop( op )
char op;
{
	/*
	 * Determine if "op" is a binary operator and return its
	 * precedence level if so. If not, return 0.
	 */
	switch ( op )
	{
	case T_COMMA:
		return 1;
	case T_ASSIGN:
		return 2;
	case T_IOR:
		return 3;
	case T_XOR:
		return 4;
	case T_AND:
		return 5;
	case T_SHL:
	case T_SHR:
		return 6;
	case T_ADD:
	case T_SUB:
		return 7;
	case T_MUL:
	case T_DIV:
	case T_MOD:
		return 8;
	case T_NOT:
		return 9;
	}
	return 0;
}

generate( tkn, val )
char tkn;
{
	/*
	 * Push the given token and value onto the Operator/Operand stack.
	 */
	if ( Opsp < STAKSZ )
	{
		Opstk[ Opsp ].o_token = tkn;
		Opstk[ Opsp ].o_value = val;
		++Opsp;
	}
	else
		err( "expression too complex" );
}

/*************************************************************
*                     LEXICAL ANALYZER                       *
*************************************************************/
getoken()
{
	/*
	 * Lexical Analyzer. Gets next token from the input line
	 * pointed to by "Lineptr" and advances "Lineptr" to next
	 * character. If end of input line is encountered, the
	 * "Eol" flag is set.
	 */
	char *cp, buf[ 128 ];
	int i;

	/*
	 * skip white space
	 */
	Lineptr = skipws( Lineptr );

	if ( ! *Lineptr )
	{
		Eol = 1;
		Token = T_EOL;
	}
	else if ( *Lineptr == '0' )
	{
		/*
		 * Check if it's a hex or octal constant
		 */
		Token = T_CONST;
		++Lineptr;
		if ( toupper( *Lineptr ) == 'X' )
		{
			++Lineptr;
			for ( cp = buf; ishexdigit( *Lineptr ); )
				*cp++ = *Lineptr++;
			*cp = 0;
			sscanf( buf, "%x", &Value );
		}
		else if ( isdigit( *Lineptr ) )
		{
			for ( cp = buf; isoctdigit( *Lineptr ); )
				*cp++ = *Lineptr++;
			*cp = 0;
			sscanf( buf, "%o", &Value );
		}
		else
			Value = 0;
	}
	else if ( isdigit( *Lineptr ) )
	{
		/*
		 * It's a numeric constant, "Value" will be its value.
		 */
		Token = T_CONST;
		for ( cp = buf; isdigit( *Lineptr ); )
			*cp++ = *Lineptr++;
		*cp = 0;
		Value = atoi( buf );
	}
	else if ( Value = isfunc( &Lineptr ) )
	{
		/*
		 * It's a built-in function, "Value" will be the index
		 * into the function jump table.
		 */
		Token = T_FUNC;
		--Value;
	}
	else if ( Token = isbinop( &Lineptr ) )
	{
		/*
		 * It's a binary operator
		 */
		;
	}
	else if ( isalpha( *Lineptr ) )
	{
		Token = T_SYMBOL;
		Value = toupper( *Lineptr ) - 'A';
		++Lineptr;
	}
	else
	{
		/*
		 * Bad character in input line
		 */
		err( "bad syntax: %s", Lineptr );
		++Lineptr;
	}

	return Token;
}

char *
skipws( cp )
char *cp;
{
	while ( *cp==' ' || *cp=='\t' )
		++cp;
	return cp;
}

isfunc( cpp )
char **cpp;
{
	/*
	 * Check if *cpp is the name of a built-function, return the
	 * function jump table index+1 if so and bump *cpp to next
	 * character. Return 0 if not a function.
	 */
	char *cp, *bp, buf[ 80 ];
	int funcno;

	/*
	 * copy the name from input line buffer to a local buffer so
	 * we can use it to make a proper comparison to function names.
	 */
	for ( cp=*cpp, bp=buf; isalpha( *cp ); )
		*bp++ = *cp++;
	*bp = 0;

	/*
	 * compare it to all of the function names we know about.
	 */
	if ( strcmp( buf, "sum" ) == 0 )
		funcno = 1;
	else if ( strcmp( buf, "base" ) == 0 )
		funcno = 2;
	else
		/*
		 * not a built-in function
		 */
		funcno = 0;

	if ( funcno )
		*cpp = cp;

	return funcno;
}

isbinop( cpp )
char **cpp;
{
	/*
	 * Check if *cpp is a binary operator, return its token value
	 * and bump *cpp to next character.
	 */
	int tkn;
	char c;

	c = **cpp;
	if ( c == ',' )
		tkn = T_COMMA;
	else if ( c == '=' )
		tkn = T_ASSIGN;
	else if ( c == '<' )
	{
		if ( *(++(*cpp)) == '<' )
			tkn = T_SHL;
	}
	else if ( c == '>' )
	{
		if ( *(++(*cpp)) == '>' )
			tkn = T_SHR;
	}
	else if ( c == '(' )
		tkn = T_LPAR;
	else if ( c == ')' )
		tkn = T_RPAR;
	else if ( c == '*' )
		tkn = T_MUL;
	else if ( c == '/' )
		tkn = T_DIV;
	else if ( c == '%' )
		tkn = T_MOD;
	else if ( c == '+' )
		tkn = T_ADD;
	else if ( c == '-' )
		tkn = T_SUB;
	else if ( c == '^' )
		tkn = T_XOR;
	else if ( c == '&' )
		tkn = T_AND;
	else if ( c == '|' )
		tkn = T_IOR;
	else if ( c == '~' )
		tkn = T_NOT;
	else
		tkn = 0;

	if ( tkn )
		++(*cpp);

	return tkn;
}

/*************************************************************
*                   BUILT-IN FUNCTIONS                       *
**************************************************************/
f_sum()
{
	/*
	 * usage: sum( a, b )
	 *    Sum all the numbers between a and b
	 */
	int i, a, b;

	b = pop();
	a = pop();
	for ( i=a+1; i<=b; ++i )
		a += i;

	push( a );
}

f_base()
{
	/*
	 * usage: base( n )
	 *    Set output number base
	 */
	switch ( pop() )
	{
	case 8:
		Ofmt = "0%o";
		break;
	case 16:
		Ofmt = "0x%x";
		break;
	case 10:
	default:
		Ofmt = "%d";
		break;
	}
}


/*************************************************************
*                       MISCELLANEOUS                        *
**************************************************************/
err( a, b, c, d, e, f )
{
	if ( ! Error )
	{
		printf( a, b, c, d, e, f );
		putchar( '\n' );
		Error = 1;
	}
}

ishexdigit( c )
char c;
{
	return instr( c, "0123456789abcdefABCDEF" );
}

isoctdigit( c )
char c;
{
	return instr( c, "01234567" );
}

instr( c, s )
char c, *s;
{
	while ( *s )
		if ( c == *s++ )
			return 1;

	return 0;
}
