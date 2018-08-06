/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

static	void	openfwd(), openbwd();

extern	int	did_ai;

/*
 * opencmd
 *
 * Add a blank line above or below the current line.
 */

void
opencmd(dir, can_ai)
int	dir;
int	can_ai;			/* if true, consider auto-indent */
{
	if (dir == FORWARD)
		openfwd(can_ai);
	else
		openbwd(can_ai);
}

static void
openfwd(can_ai)
int	can_ai;
{
	LINE	*l;
	LPTR	*next;
	char	*s;		/* string to be moved to new line, if any */

	/*
	 * If we're in insert mode, we need to move the remainder of the
	 * current line onto the new line. Otherwise the new line is left
	 * blank.
	 */
	if (State == INSERT)
		s = &Curschar->linep->s[Curschar->index];
	else
		s = "";

	if ((next = nextline(Curschar)) == NULL)	/* open on last line */
		next = Fileend;

	/*
	 * By asking for as much space as the prior line had we make sure
	 * that we'll have enough space for any auto-indenting.
	 */
	if ((l = newline(strlen(Curschar->linep->s) + SLOP)) == NULL)
		return;

	if (*s != NUL)
		strcpy(l->s, s);		/* copy string to new line */
#if 0
	else if (can_ai && P(P_AI)) {
		/*
		 * Auto-indent removed due to buggy implementation...
		 */
		 did_ai = TRUE;
	}
#endif

	if (State == INSERT)		/* truncate current line at cursor */
		*s = NUL;
			

	Curschar->linep->next = l;	/* link neighbors to new line */
	next->linep->prev = l;

	l->prev = Curschar->linep;	/* link new line to neighbors */
	l->next = next->linep;

	if (next == Fileend)			/* new line at end */
		l->num = Curschar->linep->num + LINEINC;

	else if ((l->prev->num) + 1 == l->next->num)	/* no gap, renumber */
		renum();

	else {					/* stick it in the middle */
		unsigned long	lnum;
		lnum = ((long)l->prev->num + (long)l->next->num) / 2;
		l->num = lnum;
	}

	*Curschar = *nextline(Curschar);	/* cursor moves down */
	Curschar->index = 0;

	s_ins(Cursrow+1, 1);	/* insert a physical line */

	updatescreen();		/* because Botchar is now invalid... */

	cursupdate();		/* update Cursrow before insert */
}

static void
openbwd(can_ai)
int	can_ai;
{
	LINE	*l;
	LPTR	*prev;

	prev = prevline(Curschar);

	if ((l = newline(strlen(Curschar->linep->s) + SLOP)) == NULL)
		return;

	Curschar->linep->prev = l;	/* link neighbors to new line */
	if (prev != NULL)
		prev->linep->next = l;

	l->next = Curschar->linep;	/* link new line to neighbors */
	if (prev != NULL)
		l->prev = prev->linep;

#if 0
	if (can_ai && P(P_AI)) {
		did_ai = TRUE;
	}
#endif

	*Curschar = *prevline(Curschar);	/* cursor moves up */
	Curschar->index = 0;

	if (prev == NULL)			/* new start of file */
		Filemem->linep = l;

	renum();	/* keep it simple - we don't do this often */

	cursupdate();			/* update Cursrow before insert */
	if (Cursrow != 0)
		s_ins(Cursrow, 1);		/* insert a physical line */

	updatescreen();
}

int
cntllines(pbegin,pend)
LPTR *pbegin, *pend;
{
	LINE *lp;
	int lnum = 1;

	for (lp = pbegin->linep; lp != pend->linep ;lp = lp->next)
		lnum++;

	return(lnum);
}

/*
 * plines(p) - return the number of physical screen lines taken by line 'p'
 */
int
plines(p)
LPTR	*p;
{
	register int	col;
	register char	*s;

	s = p->linep->s;

	if (*s == NUL)		/* empty line */
		return 1;

	/*
	 * If list mode is on, then the '$' at the end of
	 * the line takes up one extra column.
	 */
	col = P(P_LS) ? 1 : 0;

	for (; *s != NUL ;s++) {
		if ( *s == TAB && !P(P_LS))
			col += P(P_TS) - (col % P(P_TS));
		else
			col += chars[(unsigned)(*s & 0xff)].ch_size;
	}
	return ((col + (Columns-1)) / Columns);
}

void
fileinfo()
{
	long	l1, l2;
	char	buf[80];

	if (bufempty()) {
		msg("Buffer Empty");
		return;
	}

	l1 = cntllines(Filemem, Curschar);
	l2 = cntllines(Filemem, Fileend) - 1;
	sprintf(buf, "\"%s\"%s line %ld of %ld -- %ld %% --",
		(Filename != NULL) ? Filename : "No File",
		Changed ? " [Modified]" : "",
		l1, l2, (l1 * 100)/l2);
	msg(buf);
}

/*
 * gotoline(n) - return a pointer to line 'n'
 *
 * Returns a pointer to the last line of the file if n is zero, or
 * beyond the end of the file.
 */
LPTR *
gotoline(n)
int n;
{
	static	LPTR	l;

	l.index = 0;

	if ( n == 0 )
		l = *prevline(Fileend);
	else {
		LPTR	*p;

		for (l = *Filemem; --n > 0 ;l = *p)
			if ((p = nextline(&l)) == NULL)
				break;
	}
	return &l;
}

void
inschar(c)
int	c;
{
	register char	*p, *pend;

	/* make room for the new char. */
	if ( ! canincrease(1) )
		return;

	p = &Curschar->linep->s[strlen(Curschar->linep->s) + 1];
	pend = &Curschar->linep->s[Curschar->index];

	for (; p > pend ;p--)
		*p = *(p-1);

	*p = c;

	/*
	 * If we're in insert mode and showmatch mode is set, then
	 * check for right parens and braces. If there isn't a match,
	 * then beep. If there is a match AND it's on the screen, then
	 * flash to it briefly. If it isn't on the screen, don't do anything.
	 */
	if (P(P_SM) && State == INSERT && (c == ')' || c == '}' || c == ']')) {
		LPTR	*lpos, csave;

		if ((lpos = showmatch()) == NULL)	/* no match, so beep */
			beep();
		else if (LINEOF(lpos) >= LINEOF(Topchar)) {
			updatescreen();		/* show the new char first */
			csave = *Curschar;
			*Curschar = *lpos;	/* move to matching char */
			cursupdate();
			windgoto(Cursrow, Curscol);

/* added for turbo c port 6-14-88 dlm */

#ifdef TURBO
			mdelay();		/* brief pause */
#else
                        delay();
#endif
			*Curschar = csave;	/* restore cursor position */
			cursupdate();
		}
	}

	inc(Curschar);
	CHANGED;
}

void
insstr(s)
register char *s;
{
	register char *p, *endp;
	register int k, n = strlen(s);

	/* Move everything in the file over to make */
	/* room for the new string. */
	if (!canincrease(n))
		return;

	endp = &Curschar->linep->s[Curschar->index];
	p = Curschar->linep->s + strlen(Curschar->linep->s) + 1 + n;

	for (; p>endp ;p--)
		*p = *(p-n);

	p = &Curschar->linep->s[Curschar->index];
	for ( k=0; k<n; k++ ) {
		*p++ = *s++;
		inc(Curschar);
	}
	CHANGED;
}

bool_t
delchar(fixpos)
bool_t	fixpos;		/* if TRUE, fix the cursor position when done */
{
	register int i;

	/* Check for degenerate case; there's nothing in the file. */
	if (bufempty())
		return FALSE;

	if (lineempty())	/* can't do anything */
		return FALSE;

	/* Delete the char. at Curschar by shifting everything */
	/* in the line down. */
	for ( i=Curschar->index+1; i < Curschar->linep->size ;i++)
		Curschar->linep->s[i-1] = Curschar->linep->s[i];

	/* If we just took off the last character of a non-blank line, */
	/* we don't want to end up positioned at the newline. */
	if (fixpos) {
		if (gchar(Curschar)==NUL && Curschar->index>0 && State!=INSERT)
			Curschar->index--;
	}
	CHANGED;

	return TRUE;
}


void
delline(nlines)
{
	register LINE *p, *q;
	int	doscreen = TRUE;	/* if true, update the screen */

	/*
	 * There's no point in keeping the screen updated if we're
	 * deleting more than a screen's worth of lines.
	 */
	if (nlines > (Rows - 1)) {
		doscreen = FALSE;
		s_del(Cursrow, Rows-1);	/* flaky way to clear rest of screen */
	}

	while ( nlines-- > 0 ) {

		if (bufempty())			/* nothing to delete */
			break;

		if (buf1line()) {		/* just clear the line */
			Curschar->linep->s[0] = NUL;
			Curschar->index = 0;
			break;
		}

		p = Curschar->linep->prev;
		q = Curschar->linep->next;

		if (p == NULL) {		/* first line of file so... */
			Filemem->linep = q;	/* adjust start of file */
			Topchar->linep = q;	/* and screen */
		} else
			p->next = q;
		q->prev = p;

		clrmark(Curschar->linep);	/* clear marks for the line */

		/*
		 * Delete the correct number of physical lines on the screen
		 */
		if (doscreen)
			s_del(Cursrow, plines(Curschar));

		/*
		 * If deleting the top line on the screen, adjust Topchar
		 */
		if (Topchar->linep == Curschar->linep)
			Topchar->linep = q;

		free(Curschar->linep->s);
		free(Curschar->linep);

		Curschar->linep = q;
		Curschar->index = 0;		/* is this right? */
		CHANGED;

		/* If we delete the last line in the file, back up */
		if ( Curschar->linep == Fileend->linep) {
			Curschar->linep = Curschar->linep->prev;
			/* and don't try to delete any more lines */
			break;
		}
	}
}
