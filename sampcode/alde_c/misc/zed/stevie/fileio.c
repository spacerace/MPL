/*
 * STEVIE - ST Editor for VI Enthusiasts   ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

void
filemess(s)
char *s;
{
	smsg("\"%s\" %s", (Filename == NULL) ? "" : Filename, s);
}

void
renum()
{
	LPTR	*p;
	unsigned int l = 0;

	for (p = Filemem; p != NULL ;p = nextline(p), l += LINEINC)
		p->linep->num = l;

	Fileend->linep->num = 0xffff;
}

#ifdef	MEGAMAX
overlay "fileio"
#endif

bool_t
readfile(fname,fromp,nochangename)
char	*fname;
LPTR	*fromp;
bool_t	nochangename;	/* if TRUE, don't change the Filename */
{
	FILE	*f, *fopen();
	LINE	*curr;
	char	buff[1024];
	char	*p;
	int	i, c;
	long	nchars;
	int	unprint = 0;
	int	linecnt = 0;
	bool_t	wasempty = bufempty();

	curr = fromp->linep;

	if ( ! nochangename )
		Filename = strsave(fname);

	if ( (f=fopen(fname,"r")) == NULL )
		return TRUE;

	filemess("");

	for (i=nchars=0; (c=getc(f)) != EOF ;nchars++) {
		if (c >= 0x80) {
			c -= 0x80;
			unprint++;
		}

		/*
		 * Nulls are special, so they can't show up in the file.
		 * We should count nulls seperate from other nasties, but
		 * this is okay for now.
		 */
		if (c == NUL) {
			unprint++;
			continue;
		}

		if (c == '\n') {	/* process the completed line */
			int	len;
			LINE	*lp;

			buff[i] = '\0';
			len = strlen(buff) + 1;
			if ((lp = newline(len)) == NULL)
				exit(1);

			strcpy(lp->s, buff);

			curr->next->prev = lp;	/* new line to next one */
			lp->next = curr->next;

			curr->next = lp;	/* new line to prior one */
			lp->prev = curr;

			curr = lp;		/* new line becomes current */
			i = 0;
			linecnt++;
		}
		else
			buff[i++] = c;
	}
	fclose(f);

	/*
	 * If the buffer was empty when we started, we have to go back
	 * and remove the "dummy" line at Filemem and patch up the ptrs.
	 */
	if (wasempty) {
		LINE	*dummy = Filemem->linep;	/* dummy line ptr */

		free(dummy->s);				/* free string space */
		Filemem->linep = Filemem->linep->next;
		free(dummy);				/* free LINE struct */
		Filemem->linep->prev = NULL;

		Curschar->linep = Filemem->linep;
		Topchar->linep  = Filemem->linep;
	}

	if ( unprint > 0 )
		p="\"%s\" %d lines, %ld characters (%d un-printable))";
	else
		p="\"%s\" %d lines, %ld characters";

	sprintf(buff, p, fname, linecnt, nchars, unprint);
	msg(buff);
	renum();
	return FALSE;
}


/*
 * writeit - write to file 'fname' lines 'start' through 'end'
 *
 * If either 'start' or 'end' contain null line pointers, the default
 * is to use the start or end of the file respectively.
 */
bool_t
writeit(fname, start, end)
char	*fname;
LPTR	*start, *end;
{
	FILE	*f, *fopen();
	FILE	*fopenb();		/* open in binary mode, where needed */
	char	buff[80];
	char	backup[16], *s;
	long	nchars;
	int	lines;
	LPTR	*p;

	sprintf(buff, "\"%s\"", fname);
	msg(buff);

	/*
	 * Form the backup file name - change foo.* to foo.bak
	 */
	strcpy(backup, fname);
	for (s = backup; *s && *s != '.' ;s++)
		;
	*s = NUL;
	strcat(backup, ".bak");

	/*
	 * Delete any existing backup and move the current version
	 * to the backup. For safety, we don't remove the backup
	 * until the write has finished successfully. And if the
	 * 'backup' option is set, leave it around.
	 */
	rename(fname, backup);


	f = P(P_CR) ? fopen(fname, "w") : fopenb(fname, "w");

	if ( f == NULL ) {
		emsg("Can't open file for writing!");
		return FALSE;
	}

	/*
	 * If we were given a bound, start there. Otherwise just
	 * start at the beginning of the file.
	 */
	if (start == NULL || start->linep == NULL)
		p = Filemem;
	else
		p = start;

	lines = nchars = 0;
	do {
		fprintf(f, "%s\n", p->linep->s);
		nchars += strlen(p->linep->s) + 1;
		lines++;

		/*
		 * If we were given an upper bound, and we just did that
		 * line, then bag it now.
		 */
		if (end != NULL && end->linep != NULL) {
			if (end->linep == p->linep)
				break;
		}

	} while ((p = nextline(p)) != NULL);

	fclose(f);
	sprintf(buff,"\"%s\" %d lines, %ld characters", fname, lines, nchars);
	msg(buff);
	UNCHANGED;

	/*
	 * Remove the backup unless they want it left around
	 */
	if (!P(P_BK))
		remove(backup);

	return TRUE;
}
