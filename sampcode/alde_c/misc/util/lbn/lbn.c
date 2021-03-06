/*
 *	L B N . C
 *
 *
 *	0.a.3		current version number
 *	05jan84		date of last revision
 *
 *
 *	This is a Line number and Braces depth Numbering utility for C programs.
 *
 *   Written by:
 *
 *	0.a.0	03sep82		Larry K. Blische
 *
 *
 *   Modified by:
 *
 *	0.a.1	10sep82		Larry K. Blische
 *				fix backslash mishandling
 *
 *      0.a.2   20jan83         Philip N. Hisley
 *                              added user specified profile character
 *
 *	0.a.3	05jan84		Philip N. Hisley
 *				modified to run under MSDOS via Lattice C
 *				compiler
 */

#include	<stdio.h>

#define	MAXCHAR			256
#define	LINES_PER_PAGE		60
#define	BAD			0
#define	GOOD			1
#define	FALSE			0
#define	TRUE			1
#define READ			"r"
#define WRITE			"w"
#define APPEND			"a"
#define PROC			int

static	int	line_number	= 1;
static	int	temp_line_num	= 0;
static	int	page_number	= 1;
static	int	temp_page_num	= 0;

static	int	braces_depth	= 0;
static	int	neg_braces	= FALSE;
static	int	paren_depth	= 0;
static	int	neg_paren	= FALSE;

static	int	backslash	= FALSE;
static	int	quote		= FALSE;
static	int	double_quote	= FALSE;
static	int	comment		= FALSE;

static  char    prfchr			= '_';	/* profile character */
static	char	file_name[25];
static  char	date_buf[25];
static  char	prt_buf[MAXCHAR];
static	char	line_buf[MAXCHAR];
static	char	prefix_buf[17];
static	char	time_buf[25];

PROC
main( argc, argv )
int	argc;
char	*argv[];
{
	register int	n, acnt = 0;
	FILE	*fio;

	/*
	 * see if command line has enough parameters
	 */

	if( argc < 2 ) {
		printf("\nusage: lbn [ -c <char> ] fname.ext [ >fname.ext or device ]\n");
		printf("\n         where: <char> is a user-specified\n");
		printf("                profiling symbol ... the standard\n");
		printf("                symbol is an underscore\n\n");
		exit( BAD );
	}

	/*
	 * get filename and time of day strings, try to open the file and then
	 *	put out the first header
	 */

	while( ++acnt < argc )
		if( strcmp( argv[ acnt ], "-c" ) == 0 )
			prfchr = *argv[ ++acnt ];
		else
			strcpy( &file_name, argv[ acnt ] );
 
	date( &date_buf, 7 );	/* Lattice library function */
	time( &time_buf, 6 );   /* Lattice library function */

	time_buf[ sprintf( &time_buf, "%s     %s  ", &time_buf, &date_buf ) ] = NULL;

	if(( fio = fopen( &file_name, READ )) == NULL ) {
		printf("\nlbn: *FATAL* can't open file %s\n", &file_name );
		exit( BAD );
	}
	putheader();

	/*
	 * while there are lines left in the file, print them
	 */

	while( fgets( &line_buf, MAXCHAR, fio ) ) {
		line_buf[ strlen( &line_buf ) ] = NULL;
		set_prefix( &prefix_buf );
		prt_buf[ 0 ] = NULL;
		strcat( &prt_buf, &prefix_buf );
		set_depth( &line_buf );
		strcat( &prt_buf, &line_buf );
		printf("%s", &prt_buf );
		line_number++;
		temp_line_num++;
		if( temp_line_num == LINES_PER_PAGE ) {
			temp_line_num = 0;
			temp_page_num++;
			putheader();
		}
	}
	
	/*
	 * close the file and print warnings
	 */
	fclose( fio );
	print_warnings();
}

/*
 *	P U T H E A D E R
 *
 * prints the top line of each page with time, filename, pagenumber
 */

PROC
putheader()
{
	char	page_buf[10];

	printf("\f\nLINE  DEPTH     SOURCE STATEMENTS");
	printf("     %s  %s     PAGE  %d.%-d", &time_buf, &file_name,
		page_number, temp_page_num );
	printf("\n\n");
}





/*
 *	P R I N T _ W A R N I N G S
 *
 * prints braces, parens, quotes, and double quote mismatch warnings
 */

PROC
print_warnings()
{
	if( braces_depth != 0 ) {
		printf("\nlbn: *WARNING* Braces mismatch." );
		printf("%3d unmatched ", abs( braces_depth ) );
		printf( (braces_depth < 0) ? "'}'" : "'{'" );
		printf( (abs( braces_depth ) == 1) ? ".\n" : "s.\n" );
	}
	if( neg_braces )
		printf("\nlbn: *WARNING* Braces depth went negative.\n");

	if( paren_depth != 0 ) {
		printf("\nlbn: *WARNING* Parenthesis mismatch." );
		printf("%3d unmatched ", abs( paren_depth ) );
		printf( (paren_depth < 0) ? "')'" : "'('" );
		printf( (abs( paren_depth ) == 1) ? ".\n" : "s.\n" );
	}
	if( neg_paren )
		printf("\nlbn: *WARNING* Parenthesis depth went negative.\n");
	if( quote )
		printf("\nlbn: *WARNING* Single quote mismatch.\n" );

	if( double_quote )
		printf("\nlbn: *WARNING* Double quote mismatch.\n" );
}


/*
 *	S E T _ P R E F I X
 *
 * formats prefix string with correct braces depth
 */

PROC
set_prefix( pbuf )
char	*pbuf;
{
	register int	i, j;
	char		depth_buf[25];

	if( braces_depth < 0 ) {
		depth_buf[0] = '?';
		j = 1;
	} else {
		for( i = 1 ; i <= braces_depth ; i++ )
			depth_buf[ i-1 ] = prfchr;
		j = braces_depth;
	}
	for( i = j+1 ; i <= 10 ; i++ )
		depth_buf[ i-1 ] = ' ';
	depth_buf[10] = NULL;
	sprintf( pbuf, "%4d  %s", line_number, &depth_buf );
}

/*
 *	S E T _ D E P T H
 *
 * determines braces depth, paren, quote, and double quote mismatch
 */

PROC
set_depth( pstr )
char	*pstr;
{
	FOREVER {
		switch( *pstr++ ) {
		case '{':
			if( backslash )
				backslash = FALSE;
			else if( !quote && !double_quote && !comment )
				braces_depth++;
			break;

		case '}':
			if( backslash )
				backslash = FALSE;
			else if( !quote && !double_quote && !comment )
				braces_depth--;
			if( braces_depth < 0 )
				neg_braces = TRUE;
			break;

		case '(':
			if( backslash )
				backslash = FALSE;
			else if( !quote && !double_quote && !comment )
				paren_depth++;
			break;

		case ')':
			if( backslash )
				backslash = FALSE;
			else if( !quote && !double_quote && !comment )
				paren_depth--;
			if( paren_depth < 0 )
				neg_paren = TRUE;
			break;

		case '\n':
			return;

		case '\\':
			backslash = !backslash;
			break;

		case '\'':
			if( backslash )
				backslash = FALSE;
			else if( !comment && !double_quote )
				quote = !quote;
			break;

		case '\"':
			if( backslash )
				backslash = FALSE;
			else if( !comment && !quote )
				double_quote = !double_quote;
			break;

		case '/':
			if( backslash )
				backslash = FALSE;
			else if( *pstr++ == '*' )
				comment = TRUE;
			else
				pstr--;
			break;

		case '*':
			if( backslash )
				backslash = FALSE;
			else if( *pstr++ == '/' )
				comment = FALSE;
			else
				pstr--;
			break;

		case '\f':
			page_number++;
			temp_page_num = 0;
			temp_line_num = 0;
			putheader();
			*(pstr-1) = NULL;	/* putheader has the \f */
			break;

		default:
			backslash = FALSE;
			break;
		}
	}
}
                                                                                                         