/*
	HEADER:		CUG236;
	TITLE:		BAWK Text Pattern/Action Tool (Brod86);
	DATE:		05/17/1987;
	DESCRIPTION:	"BAWK scans text files for regular expression patterns
			and executes a user-defined action (C code fragment)
			for each specified pattern found.  Descended from the
			DECUS version of the same program.";
	VERSION:	1.1;
	KEYWORDS:	Text Filter;
	FILENAME:	BAWK.C;
	SEE-ALSO:	BAWK.H, BAWK.DOC, BAWKACT.C, BAWKDO.C, BAWKPAT.C,
			BAWKSYM.C;
	COMPILERS:	vanilla;
	AUTHORS:	W. C. Colley III, B. Brodt;
	WARNINGS:	"Program runs out of memory under CP/M Eco-C and dies
			without a trace.  Aztec C II is OK.  Won't compile
			yet under ECO-C88 -- module bawkdo.c gives a strange
			code generator error.  REQUIRES that sizeof(int) ==
			sizeof(char *)!!!!!  Therefore, the MSDOS small model
			is OK, but the MSDOS large model is no good.";
*/

/*
 * Bawk main program
 */
#define MAIN 1
#include <stdio.h>
#include "bawk.h"

/* Functions local to this module.	*/

void compile(), newfile(), process(), usage();

/*
 * Main program
 */
int main( argc, argv )
int argc;
char **argv;
{
	char gotrules, didfile, getstdin;

	getstdin =
	didfile =
	gotrules = 0;

	/*
	 * Initialize global variables:
	 */
	Stackptr = Stackbtm - 1;
	Stacktop = Stackbtm + MAXSTACKSZ;
	Nextvar = Vartab;

	strcpy( Fldsep, " \t" );
	strcpy( Rcrdsep, "\n" );

	/*
	 * Parse command line
	 */
	while ( --argc )
	{
		if ( **(++argv) == '-' )
		{
			/*
			 * Process dash options.
			 */
			switch (*++*argv)
			{
#ifdef DEBUG
			case 'D':
			case 'd':
				++Debug;
				break;
#endif
			case 0:
				++getstdin;
				--argv;
				goto dosomething;
				break;
			default: usage();
			}
		}
		else
		{
dosomething:
			if ( gotrules )
			{
				/*
				 * Already read rules file - assume this is
				 * is a text file for processing.
				 */
				if ( ++didfile == 1 && Beginact )
					doaction( Beginact );
				if ( getstdin )
				{
					--getstdin;
					newfile( 0 );
				}
				else
					newfile( *argv );
				process();
			}
			else
			{
				/*
				 * First file name argument on command line
				 * is assumed to be a rules file - attempt to
				 * compile it.
				 */
				if ( getstdin )
				{
					--getstdin;
					newfile( 0 );
				}
				else
					newfile( *argv );
				compile();
				gotrules = 1;
			}
		}
	}
	if ( !gotrules )
		usage();

	if ( ! didfile )
	{
		/*
		 * Didn't process any files yet - process stdin.
		 */
		newfile( 0 );
		if ( Beginact )
			doaction( Beginact );
		process();
	}
	if ( Endact )
		doaction( Endact );
	return 0;
}

/*
 * Regular expression/action file compilation routines.
 */
void compile()
{
	/*
	 * Compile regular expressions and C actions into Rules struct,
	 * reading from current input file "Fileptr".
	 */
	int c, len;

#ifdef DEBUG
	if ( Debug )
		error( "compiling...", 0 );
#endif

	while ( (c = getcharacter()) != -1 )
	{
		if ( c==' ' || c=='\t' || c=='\n' )
			/* swallow whitespace */
			;
		else if ( c=='#' )
		{
			/*
			 * Swallow comments
			 */
			while ( (c=getcharacter()) != -1 && c!='\n' )
				;
		}
		else if ( c=='{' )
		{
#ifdef DEBUG
			if ( Debug )
				error( "action", 0 );
#endif
			/*
			 * Compile (tokenize) the action string into our
			 * global work buffer, then allocate some memory
			 * for it and copy it over.
			 */
			ungetcharacter( '{' );
			len = act_compile( Workbuf );

			if ( Rulep && Rulep->action )
			{
				Rulep->nextrule =
					(RULE *)getmem(sizeof(RULE));
				Rulep = Rulep->nextrule;
				fillmem( Rulep, sizeof(RULE), 0 );
			}
			if ( !Rulep )
			{
				/*
				 * This is the first action encountered.
				 * Allocate the first Rules structure and
				 * initialize it
				 */
				Rules = Rulep =
					(RULE *)getmem(sizeof(RULE));
				fillmem( Rulep, sizeof(RULE), 0 );
			}
			Rulep->action = getmem( len );
			movemem( Workbuf, Rulep->action, len );
		}
		else if ( c==',' )
		{
#ifdef DEBUG
			if ( Debug )
				error( "stop pattern", 0 );
#endif
			/*
			 * It's (hopefully) the second part of a two-part
			 * pattern string.  Swallow the comma and start
			 * compiling an action string.
			 */
			if ( !Rulep || !Rulep->pattern.start )
				error( "stop pattern without a start",
					RE_ERROR );
			if ( Rulep->pattern.stop )
				error( "already have a stop pattern",
					RE_ERROR );
			len = pat_compile( Workbuf );
			Rulep->pattern.stop = getmem( len );
			movemem( Workbuf, Rulep->pattern.stop, len );
		}
		else
		{
			/*
			 * Assume it's a regular expression pattern
			 */
#ifdef DEBUG
			if ( Debug )
				error( "start pattern", 0 );
#endif

			ungetcharacter( c );
			len = pat_compile( Workbuf );

			if ( *Workbuf == T_BEGIN )
			{
				/*
				 * Saw a "BEGIN" keyword - compile following
				 * action into special "Beginact" buffer.
				 */
				len = act_compile( Workbuf );
				Beginact = getmem( len );
				movemem( Workbuf, Beginact, len );
				continue;
			}
			if ( *Workbuf == T_END )
			{
				/*
				 * Saw an "END" keyword - compile following
				 * action into special "Endact" buffer.
				 */
				len = act_compile( Workbuf );
				Endact = getmem( len );
				movemem( Workbuf, Endact, len );
				continue;
			}
			if ( Rulep )
			{
				/*
				 * Already saw a pattern/action - link in
				 * another Rules structure.
				 */
				Rulep->nextrule =
					(RULE *)getmem(sizeof(RULE));
				Rulep = Rulep->nextrule;
				fillmem( Rulep, sizeof(RULE), 0 );
			}
			if ( !Rulep )
			{
				/*
				 * This is the first pattern encountered.
				 * Allocate the first Rules structure and
				 * initialize it
				 */
				Rules = Rulep =
					(RULE *)getmem(sizeof(RULE));
				fillmem( Rulep, sizeof(RULE), 0 );
			}
			if ( Rulep->pattern.start )
				error( "already have a start pattern",
					RE_ERROR );

			Rulep->pattern.start = getmem( len );
			movemem( Workbuf, Rulep->pattern.start, len );
		}
	}
	endfile();
}

/*
 * Text file main processing loop.
 */
void process()
{
	/*
	 * Read a line at a time from current input file at "Fileptr",
	 * then apply each rule in the Rules chain to the input line.
	 */
	int i;

#ifdef DEBUG
	if ( Debug )
		error( "processing...", 0 );
#endif

	Recordcount = 0;

	while ( getline() )
	{
		/*
		 * Parse the input line.
		 */
		Fieldcount = parse( Linebuf, Fields, Fldsep );
#ifdef DEBUG
		if ( Debug>1 )
		{
			printf( "parsed %d words:\n", Fieldcount );
			for(i=0; i<Fieldcount; ++i )
				printf( "<%s>\n", Fields[i] );
		}
#endif

		Rulep = Rules;
		do
		{
			if ( ! Rulep->pattern.start )
			{
				/*
				 * No pattern given - perform action on
				 * every input line.
				 */
				doaction( Rulep->action );
			}
			else if ( Rulep->pattern.startseen )
			{
				/*
				 * Start pattern already found - perform
				 * action then check if line matches
				 * stop pattern.
				 */
				doaction( Rulep->action );
				if ( dopattern( Rulep->pattern.stop ) )
					Rulep->pattern.startseen = 0;
			}
			else if ( dopattern( Rulep->pattern.start ) )
			{
				/*
				 * Matched start pattern - perform action.
				 * If a stop pattern was given, set "start
				 * pattern seen" flag and process every input
				 * line until stop pattern found.
				 */
				doaction( Rulep->action );
				if ( Rulep->pattern.stop )
					Rulep->pattern.startseen = 1;
			}
		}
		while ( Rulep = Rulep->nextrule );

		/*
		 * Release memory allocated by parse().
		 */
		while ( Fieldcount )
			free( Fields[ --Fieldcount ] );
	}
}

/*
 * Miscellaneous functions
 */
int parse( str, wrdlst, delim )
char *str;
char *wrdlst[];
char *delim;
{
	/*
	 * Parse the string of words in "str" into the word list at "wrdlst".
	 * A "word" is a sequence of characters delimited by one or more
	 * of the characters found in the string "delim".
	 * Returns the number of words parsed.
	 * CAUTION: the memory for the words in "wrdlst" is allocated
	 * by malloc() and should eventually be returned by free()...
	 */
	int wrdcnt, wrdlen;
	char wrdbuf[ MAXLINELEN ], c;

	wrdcnt = 0;
	while ( *str )
	{
		while ( instr( *str, delim ) )
			++str;
		if ( !*str )
			break;
		wrdlen = 0;
		while ( (c = *str) && !instr( c, delim ) )
		{
			wrdbuf[ wrdlen++ ] = c;
			++str;
		}
		wrdbuf[ wrdlen++ ] = 0;
		/*
		 * NOTE: allocate a MAXLINELEN sized buffer for every
		 * word, just in case user wants to copy a larger string
		 * into a field.
		 */
		wrdlst[ wrdcnt ] = getmem( MAXLINELEN );
		strcpy( wrdlst[ wrdcnt++ ], wrdbuf );
	}

	return wrdcnt;
}

void unparse( wrdlst, wrdcnt, str, delim )
char *wrdlst[];
int wrdcnt;
char *str;
char *delim;
{
	/*
	 * Replace all the words in "str" with the words in "wrdlst",
	 * maintaining the same word seperation distance as found in
	 * the string.
	 * A "word" is a sequence of characters delimited by one or more
	 * of the characters found in the string "delim".
	 */
	int wc;
	char strbuf[ MAXLINELEN ], *sp, *wp, *start;

	wc = 0;		/* next word in "wrdlst" */
	sp = strbuf;	/* points to our local string */
	start = str;	/* save start address of "str" for later... */
	while ( *str )
	{
		/*
		 * Copy the field delimiters from the original string to
		 * our local version.
		 */
		while ( instr( *str, delim ) )
			*sp++ = *str++;
		if ( !*str )
			break;
		/*
		 * Skip over the field in the original string and...
		 */
		while ( *str && !instr( *str, delim ) )
			++str;

		if ( wc < wrdcnt )
		{
			/*
			 * ...copy in the field in the wordlist instead.
			 */
			wp = wrdlst[ wc++ ];
			while ( *wp )
				*sp++ = *wp++;
		}
	}
	/*
	 * Tie off the local string, then copy it back to caller's string.
	 */
	*sp = 0;
	strcpy( start, strbuf );
}

int instr( c, s )
char c, *s;
{
	while ( *s )
		if ( c==*s++ )
			return 1;
	return 0;
}

char *getmem( len )
unsigned len;
{
	char *cp;

	if (cp = malloc(len)) return cp;
	error( "out of memory", MEM_ERROR );
}

void newfile(s)
char *s;
{
	Linecount = 0;
	if ( Filename = s )
	{
		if ( !(Fileptr = fopen( s, "r" )) )
			error( "file not found", FILE_ERROR );
	}
	else
	{
		/*
		 * No file name given - process standard input.
		 */
		Fileptr = stdin;
		Filename = "standard input";
	}
}

int getline()
{
	/*
	 * Read a line of text from current input file.	 Strip off
	 * trailing record seperator (newline).
	 */
	int rtn, len;

	for ( len=0; len<MAXLINELEN; ++len )
	{
		if ( (rtn = getcharacter()) == *Rcrdsep || rtn == -1 )
			break;
		Linebuf[ len ] = rtn;
	}
	Linebuf[ len ] = 0;

	if ( rtn == -1 )
	{
		endfile();
		return 0;
	}
	return 1;
}

int getcharacter()
{
	/*
	 * Read a character from curren input file.
	 * WARNING: your getc() must convert lines that end with CR+LF
	 * to LF and EOF marks (like CP/M's ^Z) to a -1.
	 * Also, getc() must return a -1 when attempting to read from
	 * an unopened file.
	 */
	int c;

	if ((c = getc(Fileptr)) == *Rcrdsep )
		++Recordcount;
	if ( c=='\n' )
		++Linecount;

	return c;
}

int ungetcharacter( c )
{
	/*
	 * Push a character back into the input stream.
	 * If the character is a record seperator, or a newline character,
	 * the record and line counters are adjusted appropriately.
	 */
	if ( c == *Rcrdsep )
		--Recordcount;
	if ( c=='\n' )
		--Linecount;
	return ungetc( c, Fileptr );
}

void endfile()
{
	fclose( Fileptr );
	Filename = NULL;  Linecount = 0;
}

void error( s, severe )
char *s;
int severe;
{
	if ( Filename )
		fprintf( stderr, "%s:", Filename );

	if ( Linecount )
		fprintf( stderr, " line %d:", Linecount );

	fprintf( stderr, " %s\n", s );
	if ( severe )
		exit( severe );
}

void usage()
{
	error( "Usage: bawk <actfile> [<file> ...]\n", USAGE_ERROR );
}

void movemem( from, to, count )
char *from, *to;
int count;
{
	while ( count-- > 0 )
		*to++ = *from++;
}

void fillmem( array, count, value )
char *array, value;
int count;
{
	while ( count-- > 0 )
		*array++ = value;
}

int alpha( c )
char c;
{
	return isalpha(c) || c == '_';
}

int alphanum( c )
char c;
{
	return isalnum(c) || c == '_';
}
