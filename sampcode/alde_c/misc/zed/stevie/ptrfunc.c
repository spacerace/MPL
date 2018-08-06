/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

/*
 * The routines in this file attempt to imitate many of the operations
 * that used to be performed on simple character pointers and are now
 * performed on LPTR's. This makes it easier to modify other sections
 * of the code. Think of an LPTR as representing a position in the file.
 * Positions can be incremented, decremented, compared, etc. through
 * the functions implemented here.
 */

/*
 * inc(p)
 *
 * Increment the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at end of file, 0 otherwise.
 */
int
inc(lp)
register LPTR	*lp;
{
	register char *p = &(lp->linep->s[lp->index]);

	if (*p != NUL) {			/* still within line */
		lp->index++;
		return ((p[1] != NUL) ? 0 : 1);
	}

	if (lp->linep->next != Fileend->linep) {  /* there is a next line */
		lp->index = 0;
		lp->linep = lp->linep->next;
		return 1;
	}

	return -1;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
int
dec(lp)
register LPTR	*lp;
{
	if (lp->index > 0) {			/* still within line */
		lp->index--;
		return 0;
	}

	if (lp->linep->prev != NULL) {		/* there is a prior line */
		lp->linep = lp->linep->prev;
		lp->index = strlen(lp->linep->s);
		return 1;
	}

	return -1;				/* at start of file */
}

/*
 * gchar(lp) - get the character at position "lp"
 */
int
gchar(lp)
register LPTR	*lp;
{
	return (lp->linep->s[lp->index]);
}

/*
 * pchar(lp, c) - put character 'c' at position 'lp'
 */
void
pchar(lp, c)
register LPTR	*lp;
char	c;
{
	lp->linep->s[lp->index] = c;
}

/*
 * pswap(a, b) - swap two position pointers
 */
void
pswap(a, b)
register LPTR	*a, *b;
{
	LPTR	tmp;

	tmp = *a;
	*a  = *b;
	*b  = tmp;
}

/*
 * Position comparisons
 */

bool_t
lt(a, b)
register LPTR	*a, *b;
{
	register int an, bn;

	an = LINEOF(a);
	bn = LINEOF(b);

	if (an != bn)
		return (an < bn);
	else
		return (a->index < b->index);
}

bool_t
gt(a, b)
LPTR	*a, *b;
{
	register int an, bn;

	an = LINEOF(a);
	bn = LINEOF(b);

	if (an != bn)
		return (an > bn);
	else
		return (a->index > b->index);
}

bool_t
equal(a, b)
register LPTR	*a, *b;
{
	return (a->linep == b->linep && a->index == b->index);
}

bool_t
ltoreq(a, b)
register LPTR	*a, *b;
{
	return (lt(a, b) || equal(a, b));
}

bool_t
gtoreq(a, b)
LPTR	*a, *b;
{
	return (gt(a, b) || equal(a, b));
}
