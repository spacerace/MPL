/* 1.2  10-08-85						(scrnio.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/*----------------------------------------------------------------------*/

GLOBAL int outrow[], outcol[];
GLOBAL tiny HEIGHT, WIDTH;	/* See scrnio.h for definitions.	*/
GLOBAL TEXT LEADIN[], RCSEPARATOR[], RCENDER[], CLEARSCREEN[], EREOPG[],
	HOMER[], EREOLN[], NORM_SCRN[], ALT_SCRN[], SCRNINI[], SCRNUNI[];
GLOBAL TBOOL CB4L;
GLOBAL tiny ROFFSET, COFFSET;
GLOBAL TBOOL XLT2ASCII, SCRNWRAP, USELAST;

/*----------------------------------------------------------------------*/

#define toscrn(a, b)	write(STDOUT, a, b) /* write bchars of string a */
#define TABSTOP		8		/* standard tab stops.		*/

/************************************************************************/
	VOID
iniscrn()		/* Send initialization sequence to terminal	*/

/*----------------------------------------------------------------------*/
{
	if (*SCRNINI)
		scrsend(SCRNINI);
}

/************************************************************************/
	VOID
uniscrn()		/* Send un-initialization sequence to terminal.	*/

/*----------------------------------------------------------------------*/
{
	if (*SCRNUNI)
		scrsend(SCRNUNI);
}

/*\p********************************************************************/
	VOID
clrscrn()	/* Clear the terminal screen and home the cursor.	*/

/*----------------------------------------------------------------------*/
{
int i;
	if (*CLEARSCREEN)
		scrsend(CLEARSCREEN);
	else
		eraeop(0, 0);
	outrow[STDOUT] = outcol[STDOUT] = 0;
}

/************************************************************************/
	VOID
home()			/* Move cursor to row 0, column 0 of terminal.	*/

/*----------------------------------------------------------------------*/
{
	if (*HOMER)
	{	scrsend(HOMER);
		outrow[STDOUT] = outcol[STDOUT] = 0;
	}
	else
		cursor(0, 0);
}

/************************************************************************/
	BOOL
normscrn()		/* Begin normal-intensity mode.  Return TRUE if
			   supported, FALSE if not.			*/
/*----------------------------------------------------------------------*/
{
	scrsend(NORM_SCRN);
	return (*NORM_SCRN ISNT NULL);
}

/************************************************************************/
	BOOL
altscrn()		/* Begin alternate-intensity mode.  Return TRUE 
			   if supported, FALSE if not.			*/
/*----------------------------------------------------------------------*/
{
	scrsend(ALT_SCRN);
	return (*ALT_SCRN ISNT NULL);
}
/*\p*********************************************************************/
	BOOL
eraeop(r, c)		/* Erase from row r, col c to end of page. Return
			   TRUE, or FALSE if improper r, c.		*/
/*----------------------------------------------------------------------*/
{
	int i;

	if (NOT cursor(r, c))
		return (FALSE);

	if (*EREOPG)
		scrsend(EREOPG);
	else
	{	i = r;
		eraeol(r, c);
		while (++i <= HEIGHT - 1)
			eraeol(i, 0);
	}
	return (cursor(r, c));
}

/************************************************************************/
	BOOL
eraeol(r, c)		/* Erase row r from column c to end of line. Return
			   TRUE, or FALSE on improper r, c.		*/
/*----------------------------------------------------------------------*/
{
	int i;

	if (NOT cursor(r, c))
		return (FALSE);

	if (*EREOLN)
		scrsend(EREOLN);
	else
	{	i = WIDTH - 1 - c;
		while (i--)
			toscrn(" ", 1);
		if ((r ISNT HEIGHT - 1) OR USELAST)
			toscrn(" ", 1);
		cursor(r, c);
	}
	return (TRUE);
}
/*\p*********************************************************************/
	BOOL
cursor(r, c)		/* Position the cursor at row r, column c.  Return
			   TRUE if successful, or FALSE if coordinate 
			   addressing error is sensed.			*/
/*----------------------------------------------------------------------*/
{
	if ((r > HEIGHT - 1) OR (c > WIDTH - 1) OR (r < 0 AND c < 0))
		return (FALSE);

	scrsend(LEADIN);
	CB4L ? coord(c + COFFSET) : coord(r + ROFFSET);
	scrsend(RCSEPARATOR);
	CB4L ? coord(r + ROFFSET) : coord(c + COFFSET);
	scrsend(RCENDER);
	outrow[STDOUT] = r;
	outcol[STDOUT] = c;
	return (TRUE);
}

/************************************************************************/
	LOCAL VOID
scrsend(s)		/* Send the string s to the terminal		*/

/*----------------------------------------------------------------------*/
STRING s;
{
	if (*s)
		toscrn(s, strlen(s));
}

/************************************************************************/
	LOCAL VOID
coord(x)		/* Output the coordinate x to the screen.	*/

/*----------------------------------------------------------------------*/
{
	BUFFER buf[10];

	if (XLT2ASCII)
	{	itoa(buf, x, 0);
		if (*buf)
			scrsend(buf);
	}
	else
		toscrn(&x, 1);

}
/*\p*********************************************************************/
	METACHAR
putscrn(c)		/* put character c to the screen and update row
			   and column values appropriately.		*/
/*----------------------------------------------------------------------*/
{
	int i;
	char ch;

#ifdef CROUTADD
	if (c IS '\n')
		toscrn("\r", 1);
#endif
	if (NOT USELAST AND outrow[STDOUT] IS (HEIGHT -1)
	    AND outcol[STDOUT] IS (WIDTH -1))
		return EOF;

	ch = c;
	switch(c)
	{   case '\n':
		outrow[STDOUT]++;
	    case '\r':
		outcol[STDOUT] = 0;
		break;
	    case '\b':
		if (outcol[STDOUT] > 0)
			--outcol[STDOUT];
			toscrn(&ch, 1);
			return c;
	    case '\t':
		i = TABSTOP - (outcol[STDOUT] % TABSTOP);
		while ((SCRNWRAP OR outcol[STDOUT] < WIDTH) AND i--)
		{	toscrn(" ", 1);
			++outcol[STDOUT];
		}
		break;
	    default :
		outcol[STDOUT]++;
	}
	if (SCRNWRAP AND outcol[STDOUT] >= WIDTH)
	{	outrow[STDOUT]++;
		outcol[STDOUT] -= WIDTH;
	}
	if (c ISNT '\t')
		toscrn(&ch, 1);
	return c;
}
