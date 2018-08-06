/*
 * STEVIE - ST Editor for VI Enthusiasts   ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

int Rows;		/* Number of Rows and Columns */
int Columns;		/* in the current window. */

char *Realscreen = NULL;	/* What's currently on the screen, a single */
				/* array of size Rows*Columns. */
char *Nextscreen = NULL;	/* What's to be put on the screen. */

char *Filename = NULL;	/* Current file name */

LPTR *Filemem;		/* The contents of the file, as a single array. */

LPTR *Fileend;		/* Pointer to the end of the file in Filemem. */
			/* (It points to the byte AFTER the last byte.) */

LPTR *Topchar;		/* Pointer to the byte in Filemem which is */
			/* in the upper left corner of the screen. */

LPTR *Botchar;		/* Pointer to the byte in Filemem which is */
			/* just off the bottom of the screen. */

LPTR *Curschar;		/* Pointer to byte in Filemem at which the */
			/* cursor is currently placed. */

int Cursrow, Curscol;	/* Current position of cursor */

int Cursvcol;		/* Current virtual column, the column number of */
			/* the file's actual line, as opposed to the */
			/* column number we're at on the screen.  This */
			/* makes a difference on lines that span more */
			/* than one screen line. */

int Curswant = 0;	/* The column we'd like to be at. This is used */
			/* try to stay in the same column through up/down */
			/* cursor motions. */

bool_t set_want_col;	/* If set, then update Curswant the next time */
			/* through cursupdate() to the current virtual */
			/* column. */

int State = NORMAL;	/* This is the current state of the command */
			/* interpreter. */

int Prenum = 0;		/* The (optional) number before a command. */

LPTR *Insstart;		/* This is where the latest insert/append */
			/* mode started. */

bool_t Changed = 0;	/* Set to 1 if something in the file has been */
			/* changed and not written out. */

bool_t Debug = 0;

char Redobuff[1024];	/* Each command should stuff characters into this */
			/* buffer that will re-execute itself. */

char Undobuff[1024];	/* Each command should stuff characters into this */
			/* buffer that will undo its effects. */

char Insbuff[1024];	/* Each insertion gets stuffed into this buffer. */

LPTR *Uncurschar;	/* Curschar is restored to this before undoing. */

int Ninsert = 0;	/* Number of characters in the current insertion. */
int Undelchars = 0;	/* Number of characters to delete, when undoing. */
char *Insptr = NULL;

char **files;		/* list of input files */
int  numfiles;		/* number of input files */
int  curfile;		/* number of the current file */

static void
usage()
{
	fprintf(stderr, "usage: stevie [file ...]\n");
	fprintf(stderr, "       stevie -t tag\n");
	fprintf(stderr, "       stevie +[num] file\n");
	fprintf(stderr, "       stevie +/pat  file\n");
	exit(1);
}

main(argc,argv)
int	argc;
char	*argv[];
{
	char	*initstr, *getenv();	/* init string from the environment */
	char	*tag = NULL;		/* tag from command line */
	char	*pat = NULL;		/* pattern from command line */
	int	line = -1;		/* line number from command line */

	/*
	 * Process the command line arguments.
	 */
	if (argc > 1) {
		switch (argv[1][0]) {
		
		case '-':			/* -t tag */
			if (argv[1][1] != 't')
				usage();

			if (argv[2] == NULL)
				usage();

			Filename = NULL;
			tag = argv[2];
			numfiles = 1;
			break;

		case '+':			/* +n or +/pat */
			if (argv[1][1] == '/') {
				if (argv[2] == NULL)
					usage();
				Filename = strsave(argv[2]);
				pat = &(argv[1][1]);
				numfiles = 1;

			} else if (isdigit(argv[1][1]) || argv[1][1] == NUL) {
				if (argv[2] == NULL)
					usage();
				Filename = strsave(argv[2]);
				numfiles = 1;

				line = (isdigit(argv[1][1])) ?
					atoi(&(argv[1][1])) : 0;
			} else
				usage();

			break;

		default:			/* must be a file name */
			Filename = strsave(argv[1]);
			files = &(argv[1]);
			numfiles = argc - 1;
			break;
		}
	} else {
		Filename = NULL;
		numfiles = 1;
	}
	curfile = 0;

	windinit();

	/*
	 * Allocate LPTR structures for all the various position pointers
	 */
	if ((Filemem = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}
	if ((Fileend = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}
	if ((Topchar = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}
	if ((Botchar = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}
	if ((Curschar = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}
	if ((Insstart = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}
	if ((Uncurschar = (LPTR *) malloc(sizeof(LPTR))) == NULL) {
		fprintf(stderr, "Can't allocate data structures\n");
		windexit(0);
	}

	screenalloc();
	filealloc();		/* Initialize Filemem & Fileend */

	screenclear();

	if ((initstr = getenv("EXINIT")) != NULL) {
		char *lp, buf[128];

		if ((lp = getenv("LINES")) != NULL) {
			sprintf(buf, "%s lines=%s", initstr, lp);
			readcmdline(':', buf);
		} else
			readcmdline(':', initstr);
	}

	if (Filename != NULL) {
		if (readfile(Filename, Filemem, FALSE))
			filemess("[New File]");
	} else
		msg("Empty Buffer");

	setpcmark();

	updatescreen();
	
	if (tag) {
		stuffin(":ta ");
		stuffin(tag);
		stuffin("\n");

	} else if (pat) {
		stuffin(pat);
		stuffin("\n");

	} else if (line >= 0) {
		if (line > 0)
			stuffnum(line);
		stuffin("G");
	}

	edit();

	windexit(0);
        return(0);
}

#define	RBSIZE	1280		/* should be a little bigger than YBSIZE */
static char getcbuff[RBSIZE];
static char *getcnext = NULL;

void
stuffin(s)
char *s;
{
	if ( getcnext == NULL ) {
		strcpy(getcbuff,s);
		getcnext = getcbuff;
	} else
		strcat(getcbuff,s);
}

void
stuffnum(n)
int	n;
{
	char	buf[32];

	sprintf(buf, "%d", n);
	stuffin(buf);
}

void
addtobuff(s,c1,c2,c3,c4,c5,c6)
char *s;
char c1, c2, c3, c4, c5, c6;
{
	char *p = s;
	if ( (*p++ = c1) == NUL )
		return;
	if ( (*p++ = c2) == NUL )
		return;
	if ( (*p++ = c3) == NUL )
		return;
	if ( (*p++ = c4) == NUL )
		return;
	if ( (*p++ = c5) == NUL )
		return;
	if ( (*p++ = c6) == NUL )
		return;
}

int
vgetc()
{
	if ( getcnext != NULL ) {
		int nextc = *getcnext++;
		if ( *getcnext == NUL ) {
			*getcbuff = NUL;
			getcnext = NULL;
		}
		return(nextc);
	}
	return(inchar());
}

int
vpeekc()
{
	if ( getcnext != NULL )
		return(*getcnext);
	return(-1);
}

/*
 * anyinput
 *
 * Return non-zero if input is pending.
 */

bool_t
anyinput()
{
	return (getcnext != NULL);
}
