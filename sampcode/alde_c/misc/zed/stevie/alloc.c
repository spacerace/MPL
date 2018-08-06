/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

/*
 * This file contains various routines dealing with allocation and
 * deallocation of data structures.
 */

char *
alloc(size)
unsigned size;
{
	char *p;		/* pointer to new storage space */

	p = malloc(size);
	if ( p == (char *)NULL ) {	/* if there is no more room... */
		emsg("alloc() is unable to find memory!");
	}
	return(p);
}

char *
strsave(string)
char *string;
{
	return(strcpy(alloc((unsigned)(strlen(string)+1)),string));
}

void
screenalloc()
{
	/*
	 * If we're changing the size of the screen, free the old arrays
	 */
	if (Realscreen != NULL)
		free(Realscreen);
	if (Nextscreen != NULL)
		free(Nextscreen);

	Realscreen = malloc((unsigned)(Rows*Columns));
	Nextscreen = malloc((unsigned)(Rows*Columns));
}

/*
 * Allocate and initialize a new line structure with room for
 * 'nchars' characters.
 */
LINE *
newline(nchars)
int	nchars;
{
	register LINE	*l;

	if ((l = (LINE *) alloc(sizeof(LINE))) == NULL)
		return (LINE *) NULL;

	l->s = alloc(nchars);		/* the line is empty */
	l->s[0] = NUL;
	l->size = nchars;

	l->prev = (LINE *) NULL;	/* should be initialized by caller */
	l->next = (LINE *) NULL;

	return l;
}

/*
 * filealloc() - construct an initial empty file buffer
 */
void
filealloc()
{
	if ((Filemem->linep = newline(1)) == NULL) {
		fprintf(stderr,"Unable to allocate file memory!\n");
		exit(1);
	}
	if ((Fileend->linep = newline(1)) == NULL) {
		fprintf(stderr,"Unable to allocate file memory!\n");
		exit(1);
	}
	Filemem->index = 0;
	Fileend->index = 0;

	Filemem->linep->next = Fileend->linep;
	Fileend->linep->prev = Filemem->linep;

	*Curschar = *Filemem;
	*Topchar  = *Filemem;

	Filemem->linep->num = 0;
	Fileend->linep->num = 0xffff;

	clrall();		/* clear all marks */
}

/*
 * freeall() - free the current buffer
 *
 * Free all lines in the current buffer.
 */
void
freeall()
{
	LINE	*lp, *xlp;

	for (lp = Filemem->linep; lp != NULL ;lp = xlp) {
		if (lp->s != NULL)
			free(lp->s);
		xlp = lp->next;
		free(lp);
	}

	Curschar->linep = NULL;		/* clear pointers */
	Filemem->linep = NULL;
	Fileend->linep = NULL;
}

/*
 * bufempty() - return TRUE if the buffer is empty
 */
bool_t
bufempty()
{
	return (buf1line() && Filemem->linep->s[0] == NUL);
}

/*
 * buf1line() - return TRUE if there is only one line
 */
bool_t
buf1line()
{
	return (Filemem->linep->next == Fileend->linep);
}

/*
 * lineempty() - return TRUE if the current line is empty
 */
bool_t
lineempty()
{
	return (Curschar->linep->s[0] == NUL);
}

/*
 * endofline() - return TRUE if the given position is at end of line
 *
 * This routine will probably never be called with a position resting
 * on the NUL byte, but handle it correctly in case it happens.
 */
bool_t
endofline(p)
register LPTR	*p;
{
	return (p->linep->s[p->index] == NUL || p->linep->s[p->index+1] == NUL);
}
/*
 * canincrease(n) - returns TRUE if the current line can be increased 'n' bytes
 *
 * This routine returns immediately if the requested space is available.
 * If not, it attempts to allocate the space and adjust the data structures
 * accordingly. If everything fails it returns FALSE.
 */
bool_t
canincrease(n)
register int	n;
{
	register int	nsize;
	register char	*s;		/* pointer to new space */

	nsize = strlen(Curschar->linep->s) + 1 + n;	/* size required */

	if (nsize <= Curschar->linep->size)
		return TRUE;

	/*
	 * Need to allocate more space for the string. Allow some extra
	 * space on the assumption that we may need it soon. This avoids
	 * excessive numbers of calls to malloc while entering new text.
	 */
	if ((s = alloc(nsize + SLOP)) == NULL) {
		emsg("Can't add anything, file is too big!");
		State = NORMAL;
		return FALSE;
	}

	Curschar->linep->size = nsize + SLOP;
	strcpy(s, Curschar->linep->s);
	free(Curschar->linep->s);
	Curschar->linep->s = s;
	
	return TRUE;
}
