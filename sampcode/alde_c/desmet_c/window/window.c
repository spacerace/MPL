/*	module:		window.c
 *	programmer:	Ray L. McVay
 *	started:	1 Aug 84
 *	version:	1.2, 23 Aug 84
 *
 *	A simple window package based on the article and programs
 *	by Edward Mitchell in the Jan 84 Dr. Dobb's Journal.  This
 *	implementation uses dynamically allocated buffers for the
 *	window control blocks and screen save buffers.
 *
 *	An assembly language support library called VLIB is used to
 *	interface the DeSmet C compiler with the IBM ROM BIOS video
 *	routines.  VLIB will undoubtedly have to be rewritten if the
 *	window package is to be used with other compilers.
 *
 *	History -
 *
 *	1.1	-	Added style member to wcb structure, clr_window() and
 *			use of wn->cx for horizontal scrolling in write_text().
 *
 *	1.2 -	Added oldx, oldy members to wcb structure and use them
 *			in draw_window() and remove_window().
 *
 */

#include	<stdio.h>

typedef struct wcb	/* --- window control block	---		*/
{
	int		ulx,	/* upper left corner x coordinate	*/
			uly,	/* ditto y coordinate				*/
			xsize,	/* line width of inside of window	*/
			ysize,	/* number of lines inside of window	*/
			cx,		/* current cursor offset in window	*/
			cy,
			style,	/* attribute to be used in window	*/
			*scrnsave,	/* pointer to screen save buffer */
			oldx,	/* cursor position when window was	*/
			oldy;	/* opened (used for screen restore)	*/
} WINDOW, *WINDOWPTR;


/************************************************************************
* putch(), write a character and attribute to a specific XY location on *
*		   the screen. The attribute is the high byte of the character.	*
************************************************************************/

putch(x, y, c)
int		x, y, c;
{
	gotoxy(x, y, 0);
	vputca(c, 0, 1);
}


/************************************************************************
* getch(), return the character and attribute at screen XY				*
************************************************************************/

getch(x, y)
int		x, y;
{
	gotoxy(x, y, 0);
	return(vgetc(0));
}


/************************************************************************
* draw_row(), output a row of one character/attribute at XY				*
************************************************************************/

draw_row(x, y, count, c)
int		x, y, count,c;
{
	gotoxy(x, y, 0);
	vputca(c, 0, count);
}


/************************************************************************
* draw_window(), open a window of given size with upper left corner at	*
*				 XY. Allocates buffers for the window control block and *
*				 screen save buffers. Copies overwritten screen to the	*
*				 buffer. Draws the blank window. Returns the address of	*
*				 the window control block or NULL if no buffer space.	*
************************************************************************/

WINDOWPTR draw_window(x, y, width, height, attrib)
int		x, y, width, height, attrib;
{
	WINDOWPTR wn;
	int		tx, ty,
			xend, yend;
	int		*tptr;
	char	*calloc();

	if ((wn = (WINDOWPTR)calloc(1, sizeof(WINDOW))) == NULL)
		return(NULL);
	else if ((wn->scrnsave = (int *)calloc((width+2) * (height+2), sizeof(int))) == NULL)
	{
		free(wn);
		return(NULL);
	}
	else
	{
		/* store parameters in window control block */

		wn->ulx = x;
		wn->uly = y;
		wn->xsize = width;
		wn->ysize = height;
		wn->cx = 1;
		wn->cy = 1;
		wn->style = attrib;
		attrib <<= 8;	/* will make things below go quicker */
		wn->oldx = getxy(0) & 255;
		wn->oldy = getxy(0) >> 8;

		/* Copy existing text where the window will be placed */
		/* to the scrnsave buffer. Obviously, a less portable */
		/* routine could be much faster.					  */

		tptr = wn->scrnsave;
		xend = x + width + 2;
		yend = y + height + 2;

		for (ty = y; ty < yend; ty++)
		{
			for (tx = x; tx < xend; tx++)
				*tptr++ = getch(tx, ty);
		}

		/* draw the window border and clear the text area */

		putch(x, y, 0xda + attrib);					/* ul corner */
		draw_row(x + 1, y, width, 0xc4 + attrib);	/* horiz bar */
		putch(x + width + 1, y, 0xbf + attrib);		/* ur corner */

		yend = y + height;

		for (ty = y+1; ty <= yend; ty++)
		{
			putch(x, ty, 0xb3 + attrib);			/* draw the sides */
			putch(x+width+1, ty, 0xb3 + attrib);
		}

		putch(x, y + height + 1, 0xc0 + attrib);				/* ll corner */
		draw_row(x + 1, y + height + 1, width, 0xc4 + attrib);	/* horiz bar */
		putch(x + width + 1, y + height + 1, 0xd9 + attrib);	/* lr corner */

		clr_window(wn);

		return(wn);
	}
}


/************************************************************************
* remove_window(), erase the window at the window control block.		*
*				   Must be the "top" window if overlapping windows are  *
*				   used. "Tiled" windows could be removed randomly.		*
************************************************************************/

remove_window(wn)
WINDOWPTR wn;
{
	int		tx, ty,
			xend, yend;
	int		*tptr;

	/* just repaint the saved text at the appropriate location */

	tptr = wn->scrnsave;
	xend = wn->ulx + wn->xsize + 2;
	yend = wn->uly + wn->ysize + 2;

	for (ty = wn->uly; ty < yend; ty++)
	{
		for (tx = wn->ulx; tx < xend; tx++)
			putch(tx, ty, *tptr++);
	}

	/* put cursor back where it was before this rude interruption */

	gotoxy(wn->oldx, wn->oldy, 0);

	/* then release the dynamic storage used */

	free(wn->scrnsave);
	free(wn);
}


/************************************************************************
* write_text(), print a string inside a window using cx, cy in WCB		*
************************************************************************/

write_text(wn, string)
WINDOWPTR wn;
char	*string;
{
	int		tx, ty, xend;

	/* first check to see if we're at the bottom of the window		*/
	/* if we are then scroll the contents of the window up			*/

	if (wn->cy > wn->ysize)
	{
		delete_row(wn, 1);
		--wn->cy;
	}

	/* Print as much of the string as will fit in the window.		*
	 * cx is used for relative left margin. If cx is negative then	*
	 * the first cx characters will be removed from the string to	*
	 * allow horizontal scrolling in the window.					*
	 * NOTE: This obviously simple-minded technique is usable for	*
	 * 		 displaying predetermined messages but a more general	*
	 *		 console output intercept should be used for Star-like	*
	 *		 environments.											*/

	if (wn->cx > 0)
		tx = wn->ulx + wn->cx;
	else
	{
		if (-wn->cx < strlen(string))
			string -= wn->cx;
		else
			*string = '\0';
		tx = wn->ulx + 1;
	}
	xend = wn->ulx + wn->xsize + 1;
	ty = wn->uly + wn->cy;
	while ((tx < xend) && *string)
	{
		gotoxy(tx++, ty, 0);
		vputc(*string++, 0, 1);
	}
	++wn->cy;		/* move the internal cursor to the next line */
}


/************************************************************************
* insert_row(), insert a row of blanks by scrolling the lower portion	*
*				of a window down										*
************************************************************************/

insert_row(wn, row)
WINDOWPTR wn;
int		row;
{
	int		scrlwn[4];

	/* calculate corners of the scrolling window */

	scrlwn[0] = wn->ulx + 1;			/* ulx */
	scrlwn[1] = wn->uly + row;			/* uly */
	scrlwn[2] = wn->ulx + wn->xsize;	/* lrx */
	scrlwn[3] = wn->uly + wn->ysize;	/* lry */

	scrldn(scrlwn, 1, wn->style);
}


/************************************************************************
* delete_row(), delete a row by scrolling the lower portion of a window *
*				up and inserting a row of blanks at the bottom row		*
************************************************************************/

delete_row(wn, row)
WINDOWPTR wn;
int		row;
{
	int		scrlwn[4];

	/* calculate corners of the scrolling window */

	scrlwn[0] = wn->ulx + 1;			/* ulx */
	scrlwn[1] = wn->uly + row;			/* uly */
	scrlwn[2] = wn->ulx + wn->xsize;	/* lrx */
	scrlwn[3] = wn->uly + wn->ysize;	/* lry */

	scrlup(scrlwn, 1, wn->style);
}


/************************************************************************
* clr_window(), clear the "active" part of a window	and "home" internal	*
*				text cursor												*
************************************************************************/

clr_window(wn)
WINDOWPTR wn;
{
	int		scrlwn[4];

	/* calculate corners of the scrolling window */

	scrlwn[0] = wn->ulx + 1;			/* ulx */
	scrlwn[1] = wn->uly + 1;			/* uly */
	scrlwn[2] = wn->ulx + wn->xsize;	/* lrx */
	scrlwn[3] = wn->uly + wn->ysize;	/* lry */

	scrlup(scrlwn, 0, wn->style);
	wn->cx = 1;
	wn->cy = 1;
}
