/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

/*
 * This file contains the main routine for processing characters in
 * command mode as well as routines for handling the operators.
 */

#include "stevie.h"

static	void	doshift(), dodelete(), doput(), dochange();
static	void	tabinout(), startinsert();
static	bool_t	dojoin(), doyank();

/*
 * Macro evaluates true if char 'c' is a valid identifier character
 */
#define	IDCHAR(c)	(isalpha(c) || isdigit(c) || (c) == '_')

/*
 * 'can_undo' is a relatively temporary hack so I can debug the 'undo'
 * code for various operations independently. If 'can_undo' is set,
 * then the most recent edit can be undone. Otherwise, attempting to
 * undo an edit will result in an apologetic message. Can_undo is
 * cleared in the macro 'CHANGED', so that every change, by default,
 * cannot be undone. If the undo code for an edit works, 'can_undo'
 * should be set, AFTER the CHANGED macro is invoked.
 */
bool_t	can_undo = FALSE;

/*
 * Operators
 */
#define	NOP	0		/* no pending operation */
#define	DELETE	1
#define	YANK	2
#define	CHANGE	3
#define	LSHIFT	4
#define	RSHIFT	5

#define	CLEAROP	(operator = NOP)	/* clear any pending operator */

static	int	operator = NOP;		/* current pending operator */

/*
 * When a cursor motion command is made, it is marked as being a character
 * or line oriented motion. Then, if an operator is in effect, the operation
 * becomes character or line oriented accordingly.
 *
 * Character motions are marked as being inclusive or not. Most char.
 * motions are inclusive, but some (e.g. 'w') are not.
 *
 * Generally speaking, every command in normal() should either clear any
 * pending operator (with CLEAROP), or set the motion type variable.
 */

/*
 * Motion types
 */
#define	MBAD	(-1)		/* 'bad' motion type marks unusable yank buf */
#define	MCHAR	0
#define	MLINE	1

static	int	mtype;			/* type of the current cursor motion */
static	bool_t	mincl;			/* true if char motion is inclusive */

static	LPTR	startop;		/* cursor pos. at start of operator */

/*
 * Operators can have counts either before the operator, or between the
 * operator and the following cursor motion as in:
 *
 *	d3w or 3dw
 *
 * If a count is given before the operator, it is saved in opnum. If
 * normal() is called with a pending operator, the count in opnum (if
 * present) overrides any count that came later.
 */
static	int	opnum = 0;


#define	DEFAULT1(x)	(((x) == 0) ? 1 : (x))

/*
 * normal
 *
 * Execute a command in normal mode.
 */

void
normal(c)
int c;
{
	char *p, *q;
	int n;
	bool_t flag = FALSE;
	int type = 0;		/* used in some operations to modify type */
	int dir = FORWARD;	/* search direction */
	int nchar = NUL;
	bool_t finish_op;

	/*
	 * If there is an operator pending, then the command we take
	 * this time will terminate it. Finish_op tells us to finish
	 * the operation before returning this time (unless the operation
	 * was cancelled.
	 */
	finish_op = (operator != NOP);

	/*
	 * If we're in the middle of an operator AND we had a count before
	 * the operator, then that count overrides the current value of
	 * Prenum. What this means effectively, is that commands like
	 * "3dw" get turned into "d3w" which makes things fall into place
	 * pretty neatly.
	 */
	if (finish_op) {
		if (opnum != 0)
			Prenum = opnum;
	} else
		opnum = 0;

	switch(c & 0xff){

	case K_HELP:
		CLEAROP;
		if (help()) {
			screenclear();
			updatescreen();
		}
		break;

	case CTRL('L'):
		CLEAROP;
		screenclear();
		updatescreen();
		break;

	case CTRL('D'):
		CLEAROP;
		if (Prenum)
			P(P_SS) = (Prenum > Rows-1) ? Rows-1 : Prenum;
		scrollup(P(P_SS));
		onedown(P(P_SS));
		updatescreen();
		break;

	case CTRL('U'):
		CLEAROP;
		if (Prenum)
			P(P_SS) = (Prenum > Rows-1) ? Rows-1 : Prenum;
		scrolldown(P(P_SS));
		oneup(P(P_SS));
		updatescreen();
		break;

	/*
	 * ^F and ^B are neat hacks, but don't take counts. This is very
	 * code-efficient, and does the right thing. I'll fix it later
	 * to take a count. The old code took a count, but didn't do the
	 * right thing in other respects (e.g. leaving some context).
	 */
	case CTRL('F'):
#if 1
		screenclear();
		stuffin("Lz\nM");
#else
		/*
		 * Old code
		 */
		CLEAROP;
		n = DEFAULT1(Prenum);
		if ( ! onedown(Rows * n) )
			beep();
		cursupdate();
#endif
		break;

	case CTRL('B'):
#if 1
		screenclear();
		stuffin("Hz-M");
#else
		/*
		 * Old code
		 */
		CLEAROP;
		n = DEFAULT1(Prenum);
		if ( ! oneup(Rows * n) )
			beep();
		cursupdate();
#endif
		break;

	case CTRL('E'):
		CLEAROP;
		scrollup(DEFAULT1(Prenum));
		updatescreen();
		break;

	case CTRL('Y'):
		CLEAROP;
		scrolldown(DEFAULT1(Prenum));
		updatescreen();
		break;

	case 'z':
		CLEAROP;
		switch (vgetc()) {
		case NL:		/* put Curschar at top of screen */
		case CR:
			*Topchar = *Curschar;
			Topchar->index = 0;
			updatescreen();
			break;

		case '.':		/* put Curschar in middle of screen */
			n = Rows/2;
			goto dozcmd;

		case '-':		/* put Curschar at bottom of screen */
			n = Rows-1;
			/* fall through */

		dozcmd:
			{
				register LPTR	*lp = Curschar;
				register int	l = 0;

				while ((l < n) && (lp != NULL)) {
					l += plines(lp);
					*Topchar = *lp;
					lp = prevline(lp);
				}
			}
			Topchar->index = 0;
			updatescreen();
			break;

		default:
			beep();
		}
		break;

	case CTRL('G'):
		CLEAROP;
		fileinfo();
		break;

	case 'G':
		mtype = MLINE;
		*Curschar = *gotoline(Prenum);
		break;

	case 'H':
		mtype = MLINE;
		*Curschar = *Topchar;
		for (n = Prenum; n && onedown(1) ;n--)
			;
		beginline(TRUE);
		break;

	case 'M':
		mtype = MLINE;
		*Curschar = *Topchar;
		for (n = 0; n < Rows/2 && onedown(1) ;n++)
			;
		beginline(TRUE);
		break;

	case 'L':
		mtype = MLINE;
		*Curschar = *prevline(Botchar);
		for (n = Prenum; n && oneup(1) ;n--)
			;
		beginline(TRUE);
		break;

	case 'l':
	case K_RARROW:
	case ' ':
		mtype = MCHAR;
		mincl = FALSE;
		n = DEFAULT1(Prenum);
		while (n--) {
			if ( ! oneright() )
				beep();
		}
		set_want_col = TRUE;
		break;

	case 'h':
	case K_LARROW:
	case CTRL('H'):
		mtype = MCHAR;
		mincl = FALSE;
		n = DEFAULT1(Prenum);
		while (n--) {
			if ( ! oneleft() )
				beep();
		}
		set_want_col = TRUE;
		break;

	case '-':
		flag = TRUE;
		/* fall through */

	case 'k':
	case K_UARROW:
	case CTRL('P'):
		mtype = MLINE;
		if ( ! oneup(DEFAULT1(Prenum)) )
			beep();
		if (flag)
			beginline(TRUE);
		break;

	case '+':
	case CR:
	case NL:
		flag = TRUE;
		/* fall through */

	case 'j':
	case K_DARROW:
	case CTRL('N'):
		mtype = MLINE;
		if ( ! onedown(DEFAULT1(Prenum)) )
			beep();
		if (flag)
			beginline(TRUE);
		break;

	/*
	 * This is a strange motion command that helps make operators
	 * more logical. It is actually implemented, but not documented
	 * in the real 'vi'. This motion command actually refers to "the
	 * current line". Commands like "dd" and "yy" are really an alternate
	 * form of "d_" and "y_". It does accept a count, so "d3_" works to
	 * delete 3 lines.
	 */
	case '_':
	lineop:
		mtype = MLINE;
		onedown(DEFAULT1(Prenum)-1);
		break;

	case '|':
		mtype = MCHAR;
		mincl = TRUE;
		beginline(FALSE);
		if (Prenum > 0)
			*Curschar = *coladvance(Curschar, Prenum-1);
		Curswant = Prenum - 1;
		break;
		
	case CTRL(']'):			/* :ta to current identifier */
		CLEAROP;
		{
			char	c;
			LPTR	save;

			save = *Curschar;
			/*
			 * First back up to start of identifier. This
			 * doesn't match the real vi but I like it a
			 * little better and it shouldn't bother anyone.
			 */
			c = gchar(Curschar);
			while (IDCHAR(c)) {
				if (!oneleft())
					break;
				c = gchar(Curschar);
			}
			if (!IDCHAR(c))
				oneright();

			stuffin(":ta ");
			/*
			 * Now grab the chars in the identifier
			 */
			c = gchar(Curschar);
			while (IDCHAR(c)) {
				stuffin(mkstr(c));
				if (!oneright())
					break;
				c = gchar(Curschar);
			}
			stuffin("\n");

			*Curschar = save;	/* restore, in case of error */
		}
		break;

	case '%':
		mtype = MCHAR;
		mincl = TRUE;
		{
			LPTR	*pos;

			if ((pos = showmatch()) == NULL)
				beep();
			else {
				setpcmark();
				*Curschar = *pos;
				set_want_col = TRUE;
			}
		}
		break;
		
	/*
	 * Word Motions
	 */

	case 'B':
		type = 1;
		/* fall through */

	case 'b':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			LPTR	*pos;

			if ((pos = bck_word(Curschar, type)) == NULL) {
				beep();
				break;
			} else
				*Curschar = *pos;
		}
		break;

	case 'W':
		type = 1;
		/* fall through */

	case 'w':
		/*
		 * This is a little strange. To match what the real vi
		 * does, we effectively map 'cw' to 'ce', and 'cW' to 'cE'.
		 * This seems impolite at first, but it's really more
		 * what we mean when we say 'cw'.
		 */
		if (operator == CHANGE)
			goto doecmd;

		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			LPTR	*pos;

			if ((pos = fwd_word(Curschar, type)) == NULL) {
				beep();
				break;
			} else
				*Curschar = *pos;
		}
		break;

	case 'E':
		type = 1;
		/* fall through */

	case 'e':
	doecmd:
		mtype = MCHAR;
		mincl = TRUE;
		set_want_col = TRUE;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			LPTR	*pos;

			if ((pos = end_word(Curschar, type)) == NULL) {
				beep();
				break;
			} else
				*Curschar = *pos;
		}
		break;

	case '$':
		mtype = MCHAR;
		mincl = TRUE;
		while ( oneright() )
			;
		Curswant = 999;		/* so we stay at the end */
		break;

	case '^':
		flag = TRUE;
		/* fall through */

	case '0':
		mtype = MCHAR;
		mincl = TRUE;
		beginline(flag);
		break;

	case 'x':
		CLEAROP;
		if (lineempty())	/* can't do it on a blank line */
			beep();
		if (Prenum)
			stuffnum(Prenum);
		stuffin("d.");
		break;

#if 0
		/* Can't do it if we're on a blank line. */
		if (lineempty())
			beep();
		else {
			addtobuff(Redobuff,'x',NULL);
			/* To undo it, we insert the same character back. */
			resetundo();
			addtobuff(Undobuff, 'i', gchar(Curschar), ESC, NUL);
			*Uncurschar = *Curschar;
			delchar(TRUE);
			updateline();
		}
		break;
#endif

	case 'X':
		CLEAROP;
		if (!oneleft())
			beep();
		else {
			addtobuff(Redobuff, 'X', NUL);
			resetundo();
			addtobuff(Undobuff, 'i', gchar(Curschar), ESC, NUL);
			*Uncurschar = *Curschar;
			delchar(TRUE);
			updateline();
		}
		break;

	case 'A':
		set_want_col = TRUE;
		while (oneright())
			;
		/* fall through */

	case 'a':
		CLEAROP;
		/* Works just like an 'i'nsert on the next character. */
		if (!lineempty())
			inc(Curschar);
		resetundo();
		startinsert(mkstr(c), FALSE);
		break;

	case 'I':
		beginline(TRUE);
		/* fall through */

	case 'i':
	case K_INSERT:
		CLEAROP;
		resetundo();
		startinsert(mkstr(c), FALSE);
		break;

	case 'o':
		CLEAROP;
		opencmd(FORWARD, TRUE);
		resetundo();
		addtobuff(Undobuff, 'J', NULL);
		startinsert("o", TRUE);
		break;

	case 'O':
		CLEAROP;
		opencmd(BACKWARD, TRUE);
		resetundo();
		startinsert("O", TRUE);
		break;

	case 'd':
		if (operator == DELETE)		/* handle 'dd' */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = DELETE;
		break;

	/*
	 * Some convenient abbreviations...
	 */

	case 'D':
		stuffin("d$");
		break;

	case 'Y':
		if (Prenum)
			stuffnum(Prenum);
		stuffin("yy");
		break;

	case 'C':
		stuffin("c$");
		break;

	case 'c':
		if (operator == CHANGE) {	/* handle 'cc' */
			CLEAROP;
			stuffin("0c$");
			break;
		}
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = CHANGE;
		break;

	case 'y':
		if (operator == YANK)		/* handle 'yy' */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = YANK;
		break;

	case 'p':
		doput(FORWARD);
		break;

	case 'P':
		doput(BACKWARD);
		break;

	case '>':
		if (operator == RSHIFT)		/* handle >> */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = RSHIFT;
		break;

	case '<':
		if (operator == LSHIFT)		/* handle << */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;	/* save current position */
		operator = LSHIFT;
		break;

	case 's':				/* substitute characters */
		if (Prenum)
			stuffnum(Prenum);
		stuffin("c.");
		break;

	case '?':
	case '/':
	case ':':
		CLEAROP;
		readcmdline(c, NULL);
		break;

	case 'n':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		repsearch(0);
		break;

	case 'N':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		repsearch(1);
		break;

	/*
	 * Character searches
	 */
	case 'T':
		dir = BACKWARD;
		/* fall through */

	case 't':
		type = 1;
		goto docsearch;

	case 'F':
		dir = BACKWARD;
		/* fall through */

	case 'f':
	docsearch:
		mtype = MCHAR;
		mincl = TRUE;
		set_want_col = TRUE;
		if ((nchar = vgetc()) == ESC)	/* search char */
			break;
		if (!searchc(nchar, dir, type))
			beep();
		break;

	case ',':
		flag = 1;
		/* fall through */

	case ';':
		mtype = MCHAR;
		mincl = TRUE;
		set_want_col = TRUE;
		if (!crepsearch(flag))
			beep();
		break;

	/*
	 * Function searches
	 */

	case '[':
		dir = BACKWARD;
		/* fall through */

	case ']':
		mtype = MLINE;
		set_want_col = TRUE;
		if (vgetc() != c)
			beep();

		if (!findfunc(dir))
			beep();
		break;

	/*
	 * Marks
	 */

	case 'm':
		CLEAROP;
		if (!setmark(vgetc()))
			beep();
		break;

	case '\'':
		flag = TRUE;
		/* fall through */

	case '`':
		{
			LPTR	mtmp, *mark = getmark(vgetc());

			if (mark == NULL)
				beep();
			else {
				mtmp = *mark;
				setpcmark();
				*Curschar = mtmp;
				if (flag)
					beginline(TRUE);
			}
			mtype = flag ? MLINE : MCHAR;
			mincl = TRUE;		/* ignored if not MCHAR */
			set_want_col = TRUE;
		}
		break;

	case 'r':
		CLEAROP;
		if (lineempty()) {	/* Nothing to replace */
			beep();
			break;
		}
		if ((nchar = vgetc()) == ESC)
			break;
		resetundo();

		addtobuff(Undobuff, 'r', gchar(Curschar), NULL);
		*Uncurschar = *Curschar;

		/* Change current character. */
		pchar(Curschar, nchar);

		/* Save stuff necessary to redo it */
		addtobuff(Redobuff, 'r', nchar, NULL);

		CHANGED;
		can_undo = TRUE;
		updateline();
		break;

	case '~':		/* swap case */
		CLEAROP;
		if (lineempty()) {
			beep();
			break;
		}
		c = gchar(Curschar);

		if (isalpha(c)) {
			stuffin("r");		/* replace with other case */
			if (islower(c))
				stuffin(mkstr(toupper(c)));
			else
				stuffin(mkstr(tolower(c)));
		}
		stuffin("l");			/* move right when done */

		break;

	case 'J':
		CLEAROP;

		if (!dojoin())
			beep();

		resetundo();
		*Uncurschar = *Curschar;
		addtobuff(Undobuff, 'i', NL, ESC, NULL);
		addtobuff(Redobuff,'J',NULL);
		updatescreen();
		break;

	case K_CGRAVE:			/* shorthand command */
		CLEAROP;
		stuffin(":e #\n");
		break;

	case 'Z':			/* write, if changed, and exit */
		if (vgetc() != 'Z') {
			beep();
			break;
		}

		if (Changed) {
			if (Filename != NULL) {
				if (!writeit(Filename, NULL, NULL))
					return;
			} else {
				emsg("No output file");
				return;
			}
		}
		getout();
		break;

	case '.':
		/*
		 * If a delete is in effect, we let '.' help out the same
		 * way that '_' helps for some line operations. It's like
		 * an 'l', but subtracts one from the count and is inclusive.
		 */
		if (operator == DELETE || operator == CHANGE) {
			if (Prenum != 0) {
				n = DEFAULT1(Prenum) - 1;
				while (n--)
					if (! oneright())
						break;
			}
			mtype = MCHAR;
			mincl = TRUE;
		} else {			/* a normal 'redo' */
			CLEAROP;
			stuffin(Redobuff);
		}
		break;

	case 'u':
	case K_UNDO:
		CLEAROP;
		if (!can_undo) {
			msg("Sorry, can't undo last edit");
			break;
		}

		if ( *Undobuff != NUL ) {
			*Curschar = *Uncurschar;
			stuffin(Undobuff);
			*Undobuff = NUL;
		}
		if ( Undelchars > 0 ) {
			*Curschar = *Uncurschar;
			/* construct the next Undobuff and Redobuff, which */
			/* will re-insert the characters we're deleting. */
			p = Undobuff;
			q = Redobuff;
			*p++ = *q++ = 'i';
			/*
			 * Fix this loop to effectively turn nulls into
			 * NL's in the Undo and Redo buffs and do the
			 * joins needed.
			 */
			while ( Undelchars-- > 0 ) {
				char	c = gchar(Curschar);

				if (c == NUL) {
					*p++ = *q++ = NL;
					dojoin();
				} else {
					*p++ = *q++ = c;
					delchar(FALSE);
				}
			}
			/* Finish constructing Uncursbuff, and Uncurschar */
			/* is left unchanged. */
			*p++ = *q++ = ESC;
			*p = *q = NUL;
			/* Undelchars has been reset to 0 */
			updatescreen();
		}
		can_undo = FALSE;
		break;

	default:
		CLEAROP;
		beep();
		break;
	}

	/*
	 * If an operation is pending, handle it...
	 */
	if (finish_op) {		/* we just finished an operator */
		if (operator == NOP)	/* ... but it was cancelled */
			return;

		switch (operator) {

		case LSHIFT:
		case RSHIFT:
			doshift(operator, c, nchar, Prenum);
			break;

		case DELETE:
			dodelete(c, nchar, Prenum);
			break;

		case YANK:
			doyank();	/* no redo on yank... */
			break;

		case CHANGE:
			dochange(c, nchar, Prenum);
			break;

		default:
			beep();
		}
		operator = NOP;
	}
}

/*
 * doshift - handle a shift operation
 */
static void
doshift(op, c1, c2, num)
int	op;
char	c1, c2;
int	num;
{
	LPTR	top, bot;
	int	nlines;
	char	opchar;

	top = startop;
	bot = *Curschar;

	if (lt(&bot, &top))
		pswap(&top, &bot);

	nlines = cntllines(&top, &bot);
	*Curschar = top;
	tabinout((op == LSHIFT), nlines);

	/* construct Redo buff */
	opchar = (op == LSHIFT) ? '<' : '>';
	if (num != 0)
		sprintf(Redobuff, "%c%d%c%c", opchar, num, c1, c2);
	else
		sprintf(Redobuff, "%c%c%c", opchar, c1, c2);

	/*
	 * The cursor position afterward is the prior of the two positions.
	 */
	*Curschar = top;

	/*
	 * If we were on the last char of a line that got shifted left,
	 * then move left one so we aren't beyond the end of the line
	 */
	if (gchar(Curschar) == NUL && Curschar->index > 0)
		Curschar->index--;

	updatescreen();

	if (nlines > P(P_RP))
		smsg("%d lines %ced", nlines, opchar);
}

/*
 * dodelete - handle a delete operation
 */
static void
dodelete(c1, c2, num)
char	c1, c2;
int	num;
{
	LPTR	top, bot;
	int	nlines;
	int	n;

	/*
	 * Do a yank of whatever we're about to delete. If there's too much
	 * stuff to fit in the yank buffer, then get a confirmation before
	 * doing the delete. This is crude, but simple. And it avoids doing
	 * a delete of something we can't put back if we want.
	 */
	if (!doyank()) {
		msg("yank buffer exceeded: press <y> to confirm");
		if (vgetc() != 'y') {
			msg("delete aborted");
			*Curschar = startop;
			return;
		}
	}

	top = startop;
	bot = *Curschar;

	if (lt(&bot, &top))
		pswap(&top, &bot);

	nlines = cntllines(&top, &bot);
	*Curschar = top;
	cursupdate();

	if (mtype == MLINE) {
		delline(nlines);
	} else {
		if (!mincl && bot.index != 0)
			dec(&bot);

		if (top.linep == bot.linep) {		/* del. within line */
			n = bot.index - top.index + 1;
			while (n--)
				if (!delchar(TRUE))
					break;
		} else {				/* del. between lines */
			n = Curschar->index;
			while (Curschar->index >= n)
				if (!delchar(TRUE))
					break;

			top = *Curschar;
			*Curschar = *nextline(Curschar);
			delline(nlines-2);
			Curschar->index = 0;
			n = bot.index + 1;
			while (n--)
				if (!delchar(TRUE))
					break;
			*Curschar = top;
			dojoin();
		}
	}

	/* construct Redo buff */
	if (num != 0)
		sprintf(Redobuff, "d%d%c%c", num, c1, c2);
	else
		sprintf(Redobuff, "d%c%c", c1, c2);

	if (mtype == MCHAR && nlines == 1)
		updateline();
	else
		updatescreen();

	if (nlines > P(P_RP))
		smsg("%d fewer lines", nlines);
}

/*
 * dochange - handle a change operation
 */
static void
dochange(c1, c2, num)
char	c1, c2;
int	num;
{
	char	sbuf[16];
	bool_t	doappend;	/* true if we should do append, not insert */

	doappend = endofline( (lt(Curschar, &startop)) ? &startop: Curschar);

	if (mtype == MLINE) {
		msg("multi-line changes not yet supported");
		return;
	}

	dodelete(c1, c2, num);

	if (num)
		sprintf(sbuf, "c%d%c%c", num, c1, c2);
	else
		sprintf(sbuf, "c%c%c", c1, c2);

	if (doappend && !lineempty())
		inc(Curschar);

	startinsert(sbuf);
}

#define	YBSIZE	1024

static	char	ybuf[YBSIZE];
static	int	ybtype = MBAD;

static bool_t
doyank()
{
	LPTR	top, bot;
	char	*yptr = ybuf;
	char	*ybend = &ybuf[YBSIZE-1];
	int	nlines;

	top = startop;
	bot = *Curschar;

	if (lt(&bot, &top))
		pswap(&top, &bot);

	nlines = cntllines(&top, &bot);

	ybtype = mtype;			/* set the yank buffer type */

	if (mtype == MLINE) {
		top.index = 0;
		bot.index = strlen(bot.linep->s);
		/*
		 * The following statement checks for the special case of
		 * yanking a blank line at the beginning of the file. If
		 * not handled right, we yank an extra char (a newline).
		 */
		if (dec(&bot) == -1) {
			ybuf[0] = NUL;
			if (operator == YANK)
				*Curschar = startop;
			return TRUE;
		}
	} else {
		if (!mincl) {
			if (bot.index)
				bot.index--;
		}
	}

	for (; ltoreq(&top, &bot) ;inc(&top)) {
		*yptr = (gchar(&top) != NUL) ? gchar(&top) : NL;
		if (++yptr >= ybend) {
			msg("yank too big for buffer");
			ybtype = MBAD;
			return FALSE;
		}
	}

	*yptr = NUL;

	if (operator == YANK) {	/* restore Curschar if really doing yank */
		*Curschar = startop;

		if (nlines > P(P_RP))
			smsg("%d lines yanked", nlines);
	}

	return TRUE;
}

static void
doput(dir)
int	dir;
{
	if (ybtype == MBAD) {
		beep();
		return;
	}
	
	if (dir == FORWARD)
		stuffin( (ybtype == MCHAR) ? "a" : "o" );
	else
		stuffin( (ybtype == MCHAR) ? "i" : "O" );

	stuffin(ybuf);
	stuffin(mkstr(ESC));
}

/*
 * tabinout(inout,num)
 *
 * If inout==0, add a tab to the begining of the next num lines.
 * If inout==1, delete a tab from the beginning of the next num lines.
 */
static void
tabinout(inout, num)
int	inout;
int	num;
{
	int	ntodo = num;
	LPTR	*p;

	/* construct undo stuff */
	resetundo();
	*Uncurschar = *Curschar;
	sprintf(Undobuff, "%d%s", num, (inout == 0) ? "<<" : ">>");

	beginline(FALSE);
	while ( ntodo-- > 0 ) {
		beginline(FALSE);
		if ( inout == 0 )
			inschar(TAB);
		else {
			if ( gchar(Curschar) == TAB )
				delchar(TRUE);
		}
		if ( ntodo > 0 ) {
			if ( (p=nextline(Curschar)) != NULL )
				*Curschar = *p;
			else
				break;
		}
	}
	can_undo = TRUE;
}

static void
startinsert(initstr, startln)
char *initstr;
int	startln;	/* if set, insert point really at start of line */
{
	char *p, c;

	*Insstart = *Curschar;
	if (startln)
		Insstart->index = 0;
	Ninsert = 0;
	Insptr = Insbuff;
	for (p=initstr; (c=(*p++))!='\0'; )
		*Insptr++ = c;
	State = INSERT;
	if (P(P_MO))
		msg("Insert Mode");
}

void
resetundo()
{
	Undelchars = 0;
	*Undobuff = '\0';
	Uncurschar->linep = NULL;
}

static bool_t
dojoin()
{
	int	scol;		/* save cursor column */
	int	size;		/* size of the joined line */

	if (nextline(Curschar) == NULL)		/* on last line */
		return FALSE;

	if (!canincrease(size = strlen(Curschar->linep->next->s)))
		return FALSE;

	while (oneright())			/* to end of line */
		;

	strcat(Curschar->linep->s, Curschar->linep->next->s);

	/*
	 * Delete the following line. To do this we move the cursor
	 * there briefly, and then move it back. Don't back up if the
	 * delete made us the last line.
	 */
	Curschar->linep = Curschar->linep->next;
	scol = Curschar->index;

	if (nextline(Curschar) != NULL) {
		delline(1);
		Curschar->linep = Curschar->linep->prev;
	} else
		delline(1);

	Curschar->index = scol;

	oneright();		/* go to first char. of joined line */

	if (size != 0) {
		/*
		 * Delete leading white space on the joined line
		 * and insert a single space.
		 */
		while (gchar(Curschar) == ' ' || gchar(Curschar) == TAB)
			delchar(TRUE);
		inschar(' ');
	}

	return TRUE;
}

char *
mkstr(c)
char	c;
{
	static	char	s[2];

	s[0] = c;
	s[1] = NUL;

	return s;
}
