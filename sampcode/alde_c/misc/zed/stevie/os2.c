/*
 * OS/2 System-dependent routines.
 */

#include "stevie.h"

/* added for turbo c port 6-15-88 dlm */

#ifndef TURBO
extern	far pascal DOSSLEEP();
#endif

/*
 * inchar() - get a character from the keyboard
 */
int
inchar()
{
	int	c;

	flushbuf();		/* flush any pending output */

	c = getch();

	if (c == 0x1e)		/* control-^ */
		return K_CGRAVE;
	else
          {
          switch (c) {
                case 0x68: return K_HELP;
		case 0x61: return K_UNDO;
		case 0x52: return K_INSERT;
		case 0x47: return K_HOME;
		case 0x48: return K_UARROW;
		case 0x50: return K_DARROW;
		case 0x4b: return K_LARROW;
		case 0x4d: return K_RARROW;
		case 0x29: return K_CGRAVE;
                default:   return c;
          }
          }
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

/* added for turbo c port 6-14-88 dlm */

#ifdef TURBO

msleep(n)
unsigned int	n;
{
       sleep(60 * n);
}

#else  /* not turbo compile */

sleep(n)
int n;
{

	DOSSLEEP(1000L * n);
}

#endif

#ifdef TURBO
void
mdelay()
{
	delay(500U);
}

#else   /* not turbo compile */

void
delay()
{
        DOSSLEEP(500L);

}

#endif

void
windinit()
{
	Columns = 80;
	P(P_LI) = Rows = 25;
}

void
windexit(r)
int r;
{
	flushbuf();
	exit(r);
}

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
	FILE	*fopen();
	char	modestr[16];

	sprintf(modestr, "%sb", mode);
	return fopen(fname, modestr);
}
