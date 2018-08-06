
/ * Turbo C 1.5 port by: Denny Muscatelli 061988 */

/* #define	ATARI		*/	/* For the Atari ST */
/* #define	UNIX	        */	/* System V */
#define	OS2		                /* Microsoft OS/2 */
#define TURBO                           /* for Turbo C */

/*
 * If ATARI is defined, one of the following compilers must be selected.
 */
#ifdef	ATARI
#define	MEGAMAX			/* Megamax Compiler */
/* #define	ALCYON		/* Alcyon C compiler */
#endif

/*
 * If HELP is defined, the :help command shows a vi command summary.
 */
#define	HELP			/* enable help command */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ascii.h"
#include "keymap.h"
#include "param.h"
#include "term.h"

extern	char	*strchr();

#define NORMAL 0
#define CMDLINE 1
#define INSERT 2
#define APPEND 3
#define FORWARD 4
#define BACKWARD 5

/*
 * Boolean type definition and constants
 */
typedef	short	bool_t;

#ifndef	TRUE
#define	FALSE	(0)
#define	TRUE	(1)
#endif

/*
 * SLOP is the amount of extra space we get for text on a line during
 * editing operations that need more space. This keeps us from calling
 * malloc every time we get a character during insert mode. No extra
 * space is allocated when the file is initially read.
 */
#define	SLOP	10

/*
 * LINEINC is the gap we leave between the artificial line numbers. This
 * helps to avoid renumbering all the lines every time a new line is
 * inserted.
 */
#define	LINEINC	10

/*
 * See 'normal.c' for a description of can_undo.
 */
extern	bool_t	can_undo;

#define CHANGED Changed = !(can_undo = FALSE)
#define UNCHANGED Changed=0

struct	line {
	struct	line	*prev, *next;	/* previous and next lines */
	char	*s;			/* text for this line */
	int	size;			/* actual size of space at 's' */
	unsigned int	num;		/* line "number" */
};

#define	LINEOF(x)	(x->linep->num)

struct	lptr {
	struct	line	*linep;		/* line we're referencing */
	int	index;			/* position within that line */
};

typedef	struct line	LINE;
typedef	struct lptr	LPTR;

struct charinfo {
	char ch_size;
	char *ch_str;
};

extern struct charinfo chars[];

extern int State;
extern int Rows;
extern int Columns;
extern char *Realscreen;
extern char *Nextscreen;
extern char *Filename;
extern LPTR *Filemem;
extern LPTR *Fileend;
extern LPTR *Topchar;
extern LPTR *Botchar;
extern LPTR *Curschar;
extern LPTR *Insstart;
extern int Cursrow, Curscol, Cursvcol, Curswant;
extern bool_t set_want_col;
extern int Prenum;
extern bool_t Debug;
extern bool_t Changed;
extern bool_t Binary;
extern char Redobuff[], Undobuff[], Insbuff[];
extern LPTR *Uncurschar;
extern char *Insptr;
extern int Ninsert, Undelchars;

extern char *malloc(), *strcpy();

/*
 * alloc.c
 */
char	*alloc(), *strsave();
void	screenalloc(), filealloc(), freeall();
LINE	*newline();
bool_t	bufempty(), buf1line(), lineempty(), endofline(), canincrease();

/*
 * cmdline.c
 */
void	readcmdline(), dotag(), msg(), emsg(), smsg(), gotocmd(), wait_return();

/*
 * edit.c
 */
void	edit(), insertchar(), getout(), scrollup(), scrolldown(), beginline();
bool_t	oneright(), oneleft(), oneup(), onedown();

/*
 * fileio.c
 */
void	filemess(), renum();
bool_t	readfile(), writeit();

/*
 * help.c
 */
bool_t	help();

/*
 * linefunc.c
 */
LPTR	*nextline(), *prevline(), *coladvance();

/*
 * main.c
 */
void	stuffin(), stuffnum(), addtobuff();
int	vgetc(), vpeekc();
bool_t	anyinput();

/*
 * mark.c
 */
void	setpcmark(), clrall(), clrmark();
bool_t	setmark();
LPTR	*getmark();

/*
 * misccmds.c
 */
void	opencmd(), fileinfo(), inschar(), insstr(), delline();
bool_t	delchar();
int	cntllines(), plines();
LPTR	*gotoline();

/*
 * normal.c
 */
void	normal(), resetundo();
char	*mkstr();

/*
 * param.c
 */
void	doset();

/*
 * ptrfunc.c
 */
int	inc(), dec();
int	gchar();
void	pchar(), pswap();
bool_t	lt(), gt(), equal(), ltoreq(), gtoreq();

/*
 * screen.c
 */
void	updatescreen(), updateline();
void	screenclear(), cursupdate();
void	s_ins(), s_del();

/*
 * search.c
 */
void	dosearch(), repsearch();
bool_t	searchc(), crepsearch(), findfunc();
LPTR	*showmatch();
LPTR	*fwd_word(), *bck_word(), *end_word();

/*
 * Machine-dependent routines.
 */
int	inchar();
void	outchar(), outstr(), beep();

/* added 061488 for turbo c port - dlm */

#ifndef	OS2 || TURBO
void	remove(), rename();
#endif
void	windinit(), windexit(), windgoto();

/* added 061488 for turbo c port - dlm */

#ifdef TURBO
void	mdelay();
#else
void    delay();
#endif