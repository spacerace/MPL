/*
 * System-dependent routines for UNIX System V Release 3.
 */

#include "stevie.h"
#include <termio.h>

/*
 * inchar() - get a character from the keyboard
 */
int
inchar()
{
	char	c;

	flushbuf();		/* flush any pending output */

	while (read(0, &c, 1) != 1)
		;

	return c;
}

#define	BSIZE	2048
static	char	outbuf[BSIZE];
static	int	bpos = 0;

flushbuf()
{
	if (bpos != 0)
		write(1, outbuf, bpos);
	bpos = 0;
}

/*
 * Macro to output a character. Used within this file for speed.
 */
#define	outone(c)	outbuf[bpos++] = c; if (bpos >= BSIZE) flushbuf()

/*
 * Function version for use outside this file.
 */
void
outchar(c)
register char	c;
{
	outbuf[bpos++] = c;
	if (bpos >= BSIZE)
		flushbuf();
}

void
outstr(s)
register char	*s;
{
	while (*s) {
		outone(*s++);
	}
}

void
beep()
{
	outone('\007');
}

/*
 * remove(file) - remove a file
 */
void
remove(file)
char *file;
{
	unlink(file);
}

/*
 * rename(of, nf) - rename existing file 'of' to 'nf'
 */
void
rename(of, nf)
char	*of, *nf;
{
	unlink(nf);
	link(of, nf);
	unlink(of);
}

void
delay()
{
	/* not implemented */
}

static	struct	termio	ostate;

void
windinit()
{
	char	*getenv();
	char	*term;
	struct	termio	nstate;

	if ((term = getenv("TERM")) == NULL || strcmp(term, "vt100") != 0) {
		fprintf(stderr, "Invalid terminal type '%s'\n", term);
		exit(1);
	}
	Columns = 80;
	P(P_LI) = Rows = 24;

	/*
	 * Go into cbreak mode
	 */
	 ioctl(0, TCGETA, &ostate);
	 nstate = ostate;
	 nstate.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL);
	 nstate.c_cc[VMIN] = 1;
	 nstate.c_cc[VTIME] = 0;
	 ioctl(0, TCSETAW, &nstate);
}

void
windexit(r)
int r;
{
	/*
	 * Restore terminal modes
	 */
	ioctl(0, TCSETAW, &ostate);

	exit(r);
}

#define	outone(c)	outbuf[bpos++] = c; if (bpos >= BSIZE) flushbuf()

void
windgoto(r, c)
register int	r, c;
{
	r += 1;
	c += 1;

	/*
	 * Check for overflow once, to save time.
	 */
	if (bpos + 8 >= BSIZE)
		flushbuf();

	outbuf[bpos++] = '\033';
	outbuf[bpos++] = '[';
	if (r >= 10)
		outbuf[bpos++] = r/10 + '0';
	outbuf[bpos++] = r%10 + '0';
	outbuf[bpos++] = ';';
	if (c >= 10)
		outbuf[bpos++] = c/10 + '0';
	outbuf[bpos++] = c%10 + '0';
	outbuf[bpos++] = 'H';
}

FILE *
fopenb(fname, mode)
char	*fname;
char	*mode;
{
	return fopen(fname, mode);
}
