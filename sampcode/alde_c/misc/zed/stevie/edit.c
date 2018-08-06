/*
 * STevie - ST editor for VI enthusiasts.     ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

/*
 * This flag is used to make auto-indent work right on lines where only
 * a <RETURN> or <ESC> is typed. It is set when an auto-indent is done,
 * and reset when any other editting is done on the line. If an <ESC>
 * or <RETURN> is received, and did_ai is TRUE, the line is truncated.
 */
bool_t	did_ai = FALSE;

void
edit()
{
	int c;
	char *p, *q;

	Prenum = 0;

	/* position the display and the cursor at the top of the file. */
	*Topchar = *Filemem;
	*Curschar = *Filemem;
	Cursrow = Curscol = 0;

	for ( ;; ) {

	/* Figure out where the cursor is based on Curschar. */
	cursupdate();

	windgoto(Cursrow,Curscol);

	c = vgetc();

	if (State == NORMAL) {

		/* We're in the normal (non-insert) mode. */

		/* Pick up any leading digits and compute 'Prenum' */
		if ( (Prenum>0 && isdigit(c)) || (isdigit(c) && c!='0') ){
			Prenum = Prenum*10 + (c-'0');
			continue;
		}
		/* execute the command */
		normal(c);
		Prenum = 0;

	} else {

		switch (c) {	/* We're in insert mode */

		case ESC:	/* an escape ends input mode */

			set_want_col = TRUE;

			/* Don't end up on a '\n' if you can help it. */
			if (gchar(Curschar) == NUL && Curschar->index != 0)
				dec(Curschar);

			/*
			 * The cursor should end up on the last inserted
			 * character. This is an attempt to match the real
			 * 'vi', but it may not be quite right yet.
			 */
			if (Curschar->index != 0 && !endofline(Curschar))
				dec(Curschar);

			State = NORMAL;
			msg("");
			*Uncurschar = *Insstart;
			Undelchars = Ninsert;
			/* Undobuff[0] = '\0'; */
			/* construct the Redo buffer */
			p=Redobuff;
			q=Insbuff;
			while ( q < Insptr )
				*p++ = *q++;
			*p++ = ESC;
			*p = NUL;
			updatescreen();
			break;

		case CTRL('D'):
			/*
			 * Control-D is treated as a backspace in insert
			 * mode to make auto-indent easier. This isn't
			 * completely compatible with vi, but it's a lot
			 * easier than doing it exactly right, and the
			 * difference isn't very noticeable.
			 */
		case BS:
			/* can't backup past starting point */
			if (Curschar->linep == Insstart->linep &&
			    Curschar->index <= Insstart->index) {
				beep();
				break;
			}

			/* can't backup to a previous line */
			if (Curschar->linep != Insstart->linep &&
			    Curschar->index <= 0) {
				beep();
				break;
			}

			did_ai = FALSE;
			dec(Curschar);
			delchar(TRUE);
			Insptr--;
			Ninsert--;
			cursupdate();
			updateline();
			break;

		case CR:
		case NL:
			*Insptr++ = NL;
			Ninsert++;
			opencmd(FORWARD, TRUE);		/* open a new line */
			cursupdate();
			updatescreen();
			break;

		default:
			did_ai = FALSE;
			insertchar(c);
			break;
		}
	}
	}
}

/*
 * Special characters in this context are those that need processing other
 * than the simple insertion that can be performed here. This includes ESC
 * which terminates the insert, and CR/NL which need special processing to
 * open up a new line. This routine tries to optimize insertions performed
 * by the "redo" command, so it needs to know when it should stop and defer
 * processing to the "normal" mechanism.
 */
#define	ISSPECIAL(c)	((c) == NL || (c) == CR || (c) == ESC)

void
insertchar(c)
int c;
{
	char *p;

	if ( ! anyinput() ) {
		inschar(c);
		*Insptr++ = c;
		Ninsert++;
		/*
		 * The following kludge avoids overflowing the statically
		 * allocated insert buffer. Just dump the user back into
		 * command mode, and print a message.
		 */
		if (Insptr+10 >= &Insbuff[1024]) {
			stuffin(mkstr(ESC));
			emsg("No buffer space - returning to command mode");

/* added for turbo c port 6-14-88 dlm */

#ifdef TURBO
			msleep(2);
#else
                        sleep(2);
#endif
		}
	}
	else {
		/* If there's any pending input, grab it all at once. */
		p = Insptr;
		*Insptr++ = c;
		Ninsert++;
		for (c = vpeekc(); !ISSPECIAL(c) ;c = vpeekc()) {
			c = vgetc();
			*Insptr++ = c;
			Ninsert++;
		}
		*Insptr = '\0';
		insstr(p);
	}
	updateline();
}

void
getout()
{
	windgoto(Rows-1,0);
	putchar('\r');
	putchar('\n');
	windexit(0);
}

void
scrolldown(nlines)
int nlines;
{
	register LPTR	*p;
	register int	done = 0;	/* total # of physical lines done */

	/* Scroll up 'nlines' lines. */
	while (nlines--) {
		if ((p = prevline(Topchar)) == NULL)
			break;
		done += plines(p);
		*Topchar = *p;
		if (Curschar->linep == Botchar->linep->prev)
			*Curschar = *prevline(Curschar);
	}
	s_ins(0, done);
}

void
scrollup(nlines)
int nlines;
{
	register LPTR	*p;
	register int	done = 0;	/* total # of physical lines done */
	register int	pl;		/* # of plines for the current line */

	/* Scroll down 'nlines' lines. */
	while (nlines--) {
		pl = plines(Topchar);
		if ((p = nextline(Topchar)) == NULL)
			break;
		done += pl;
		if (Curschar->linep == Topchar->linep)
			*Curschar = *p;
		*Topchar = *p;

	}
	s_del(0, done);
}

/*
 * oneright
 * oneleft
 * onedown
 * oneup
 *
 * Move one char {right,left,down,up}.  Return TRUE when
 * sucessful, FALSE when we hit a boundary (of a line, or the file).
 */

bool_t
oneright()
{
	set_want_col = TRUE;

	switch (inc(Curschar)) {

	case 0:
		return TRUE;

	case 1:
		dec(Curschar);		/* crossed a line, so back up */
		/* fall through */
	case -1:
		return FALSE;
	}
}

bool_t
oneleft()
{
	set_want_col = TRUE;

	switch (dec(Curschar)) {

	case 0:
		return TRUE;

	case 1:
		inc(Curschar);		/* crossed a line, so back up */
		/* fall through */
	case -1:
		return FALSE;
	}
}

void
beginline(flag)
bool_t	flag;
{
	while ( oneleft() )
		;
	if (flag) {
		while (isspace(gchar(Curschar)) && oneright())
			;
	}
	set_want_col = TRUE;
}

bool_t
oneup(n)
{
	LPTR p, *np;
	int k;

	p = *Curschar;
	for ( k=0; k<n; k++ ) {
		/* Look for the previous line */
		if ( (np=prevline(&p)) == NULL ) {
			/* If we've at least backed up a little .. */
			if ( k > 0 )
				break;	/* to update the cursor, etc. */
			else
				return FALSE;
		}
		p = *np;
	}
	*Curschar = p;
	/* This makes sure Topchar gets updated so the complete line */
	/* is one the screen. */
	cursupdate();
	/* try to advance to the column we want to be at */
	*Curschar = *coladvance(&p, Curswant);
	return TRUE;
}

bool_t
onedown(n)
{
	LPTR p, *np;
	int k;

	p = *Curschar;
	for ( k=0; k<n; k++ ) {
		/* Look for the next line */
		if ( (np=nextline(&p)) == NULL ) {
			if ( k > 0 )
				break;
			else
				return FALSE;
		}
		p = *np;
	}
	/* try to advance to the column we want to be at */
	*Curschar = *coladvance(&p, Curswant);
	return TRUE;
}
