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
 *	1.1 -  Added style member to wcb structure, wcls and
 *		use of wn->cx for horizontal scrolling in wrt_txt().
 *
 *	1.2 -	Added oldx, oldy members to wcb structure and use them
 *		 in wopen() and wclose().
 *
 */

#include	<stdio.h>
#include	"window.h"

int	page;		/* Active page number.	*/

/************************************************************************
* vwrtch(), write a character and attribute to a specific XY location	*
*	on the screen. The attribute is the high byte of the character.	*
************************************************************************/

vwrtch(x, y, c)
int		x, y, c;
{
	gotoxy(x, y, page);
	vputca(c, page, 1);
}


/************************************************************************
* vrdch(), return the character and attribute at screen XY		*
************************************************************************/
vrdch(x, y)
int		x, y;
{
	gotoxy(x, y, page);
	return(vgetc(page));
}


/************************************************************************
* draw_row(), output a row of one character/attribute at XY		*
************************************************************************/

draw_row(x, y, count, c)
int		x, y, count,c;
{
	gotoxy(x, y, page);
	vputca(c, page, count);
}

/************************************************************************
* wopen(), 	open a window of given size with upper left corner at	*
*		XY. Allocates buffers for the window control block and	*
*		screen save buffers. Copies overwritten screen to the	*
*		buffer. Draws the blank window. Returns the address of	*
*		the window control block or NULL if no buffer space.	*
*	NOTE:	The border attributes are given in the structure 	*
*		pointer bord and can be different than the window's	*
*		contents attribute.					*
************************************************************************/

WINDOW *wopen(wbd, x, y, width, height, attrib)
BORDER *wbd;
int	x, y, width, height, attrib;
{
	WINDOW *wn;
	register int tx, ty, xend, yend;
	int	*tptr, c;
	char	*calloc();

	if ((wn = (WINDOW *)calloc(1, sizeof(WINDOW))) == NULL)
		return(NULL);
	else if ((wn->scrnsave = (int *)calloc((width+2) * (height+2),
			 sizeof(int))) == NULL)
	{
		free(wn);
		return(NULL);
	}
	else
	{
		/* store parameters in window control block */

		wn->xmargin = wn->ymargin = 0;
		wn->ulx = x;
		wn->uly = y;
		wn->xsize = width;
		wn->ysize = height;
		wn->cx = 1;
		wn->cy = 1;
		wn->atrib = attrib;
		wn->oldx = getxy(page) & 255;
		wn->oldy = getxy(page) >> 8;

		/* Copy existing text where the window will be placed	*/
		/* to the scrnsave buffer. Obviously, a less portable	*/
		/* routine could be much faster.			*/

		tptr = wn->scrnsave;
		xend = x + width + 2;
		yend = y + height + 2;
		for (ty = y; ty < yend; ty++)
		{
			for (tx = x; tx < xend; tx++)
				*tptr++ = vrdch(tx, ty);
		}
		/* draw the window border and clear the text area	*/

		vwrtch(x, y, wbd->ul);	     /* ul corner*/
		draw_row(x + 1, y, width, wbd->h); /* horiz bar*/
		vwrtch(x + width + 1, y, wbd->ur); /* ur corner*/

		yend = y + height;

		for (c=wbd->v,ty = y+1; ty <= yend; ty++)
		{
			vwrtch(x, ty, c);   /* draw the sides*/
			vwrtch(x+width+1, ty, c);
		}

		vwrtch(x, y + height + 1, wbd->ll);  /* ll corner*/
	/* horiz bar */
		draw_row(x + 1, y + height + 1, width, wbd->h);
	/* lr corner */
		vwrtch(x + width + 1, y + height + 1, wbd->lr);

		wcls(wn);
		return(wn);
	}
}


/************************************************************************
* wclose(), erase the window at the window control block.		*
*	Must be the "top" window if overlapping windows are		*
*	used. "Tiled" windows could be removed randomly.		*
************************************************************************/

wclose(wn)
WINDOW *wn;
{
	register
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
			vwrtch(tx, ty, *tptr++);
	}

	/* put cursor back where it was before this rude interruption */

	gotoxy(wn->oldx, wn->oldy, page);

	/* then release the dynamic storage used */

	free(wn->scrnsave);
	free(wn);
}


/************************************************************************
* wputstr(), print a string inside a window using cx, cy in WCB		*
************************************************************************/

wputstr(wn, string)
register WINDOW *wn;
register char	*string;
{
	int		tcx;

	/* Print as much of the string as will fit in the window.	*
	 * cx is used for relative left margin. If cx is negative then	*
	 * the first cx characters will be removed from the string to	*
	 * allow horizontal scrolling in the window.			*/

	while(*string)
		wputchar(wn, *string++);
	return set_wcxy(wn,0,0);
}


/************************************************************************
* wputs(), print a string inside a window using cx, cy in WCB. 		*
*	Note that the CR, LF, BS, TAB, and DEL keys are processed.	*
************************************************************************/

wputs(wn, string)
register WINDOW *wn;
register char	*string;
{
	while(*string)
		wputch(wn, *string++);
	return wbound(wn);
}

/************************************************************************
* wputch(), print a character inside a window using cx, cy in WCB	*
*	while processing for CR, LF, BS, TAB, and DEL.			*
************************************************************************/

wputch(wn, c)
register WINDOW *wn;
char	c;
{
	int rcode;

	rcode = 0;
	switch (c) {
		case '\r':	/* Carriage return */
			wn->cx = -wn->xmargin+1;
			break;
		case '\n':	/* Linefeed */
			++wn->cy;
			break;
		case '\b':	/* Backspace */
			--wn->cx;
			break;
		case 0x7f:	/* Delete */
			if(wn->cx < 1) {	/* If we are left of the */
				++wn->cx;	/* window account for the */
				return;		/* delete. */
			}
			wputch(wn, 0x08);
			wputchar(wn, ' ');
			wputch(wn, 0x08);
			break;
		case 0x09:	/* Tab */
			do
				wputchar(wn,' ');
			while(wn->cx & 0x07);
			break;
		case 0x0c:	/* Formfeed */
			wcls(wn);
			break;
		default:
			wputchar(wn, c);
			break;
	}
	return set_wcxy(wn,0,0);
}

/************************************************************************
* wputchar(), print a character as is where ever the cursor might be 	*
* 	inside a window using cx, cy in WCB.				*
*	Note that the negitive value of cx is handled and that the	*
*	   window is not scrolled.					*
************************************************************************/

wputchar(wn, c)
register WINDOW *wn;
char	c;
{
	int rcode;

	if((rcode = wbound(wn))) {
		++wn->cx;
		return (rcode);
	}
	gotoxy(wn->ulx + wn->cx++, wn->uly + wn->cy, page);
	vputca(wn->atrib<<8|c, page, 1);
	return(0);
}


/************************************************************************
* wins_row(), insert a row of blanks by scrolling the lower portion	*
*	of a window down.						*
*	The current (cy) row is inserted.				*
************************************************************************/

wins_row(wn)
register WINDOW *wn;
{
	int	rcode, scrlwn[4];

	/* Don't if we are not in the window. */
	if((rcode = wbound(wn)))
		return(rcode);

	/* If it is the last line in the window just delete it. */
	if(wn->cy == wn->ysize)
		draw_row(wn->ulx+1, wn->uly + wn->cy,
			 wn->xsize, (wn->atrib <<8) |' ');
	else {			
	/* calculate corners of the scrolling window */
		scrlwn[0] = wn->ulx +1;			/* ulx */
		scrlwn[1] = wn->uly + wn->cy;		/* uly */
		scrlwn[2] = wn->ulx + wn->xsize;	/* lrx */
		scrlwn[3] = wn->uly + wn->ysize;	/* lry */

		scrldn(scrlwn, 1, wn->atrib);
	}
	return(0);
}


/************************************************************************
* wdel_row(), delete a row by scrolling the lower portion of a window 	*
*	up and inserting a row of blanks at the bottom row		*
*	The current (cy) row is deleted.				*
************************************************************************/

wdel_row(wn)
register WINDOW *wn;
{
	int	rcode, scrlwn[4];

	/* Don't if we are not in the window. */
	if((rcode = wbound(wn)))
		return(rcode);

	if(wn->cy == wn->ysize)
		draw_row(wn->ulx+1, wn->uly + wn->cy,
			 wn->xsize, (wn->atrib <<8) |' ');
	else {			
	/* calculate corners of the scrolling window */
		scrlwn[0] = wn->ulx + 1;		/* ulx */
		scrlwn[1] = wn->uly + wn->cy;		/* uly */
		scrlwn[2] = wn->ulx + wn->xsize;	/* lrx */
		scrlwn[3] = wn->uly + wn->ysize;	/* lry */

		scrlup(scrlwn, 1, wn->atrib);
	}
	return(0);
}


/************************************************************************
* wcls(), clear the "active" part of a window				*
*	and "home" internal text cursor					*
************************************************************************/

wcls(wn)
register WINDOW *wn;
{
	int		scrlwn[4];

	/* calculate corners of the scrolling window */

	scrlwn[0] = wn->ulx + 1;			/* ulx */
	scrlwn[1] = wn->uly + 1;			/* uly */
	scrlwn[2] = wn->ulx + wn->xsize;	/* lrx */
	scrlwn[3] = wn->uly + wn->ysize;	/* lry */

	scrlup(scrlwn, 0, wn->atrib);
	wn->cx = 1;
	wn->cy = 1;
}

/************************************************************************
* wceol(), clear to end of the line of the window.			*
************************************************************************/

wceol(wn)
register WINDOW *wn;
{
	register int tx, ty;

	if((tx=wbound(wn)))
		return(tx);
	tx = wn->ulx + wn->cx;
	ty = wn->uly + wn->cy;
	draw_row(tx, ty, wn->xsize-wn->cx+1, (wn->atrib <<8) |' ');
	gotoxy(tx, ty, page);
	return(0);
}

/************************************************************************
* wcursor(), cursor controls for a window.				*
************************************************************************/

wcursor(wn,c)
register WINDOW *wn;
{
	register int tcx, n, tx, ty;

	switch (c) {
		case CUR_UP:	/* Cursor up */
			--wn->cy;
			break;
		case CUR_DWN:	/* Cursor down */
			++wn->cy;
			break;
		case CUR_LEFT:	/* Cursor left */
			--wn->cx;
			break;
		case CUR_RIGHT:	/* Cursor right */
			++wn->cx;
			break;
		case HOME:	/* Home cursor */
			return set_wcxy(wn,1,1);
		case END:	/* End cursor */
			return set_wcxy(wn,1, wn->ysize);
		default:
			return(-1);
	}
	if(wn->cx <1) {
		wn->cx = wn->xsize;
	} else if(wn->cx > wn->xsize)
		wn->cx = 1;
	if(wn->cy <1)
		wn->cy = wn->ysize;
	else if(wn->cy > wn->ysize)
		wn->cy = 1;
	return set_wcxy(wn, wn->cx, wn->cy);
}


/************************************************************************
* wdelch(), Deletes a character at the current window cursor position.	*
************************************************************************/

wdelch(wn)
register WINDOW *wn;
{
	register int tx, ty, xend, rcode;

	/* Don't if we are not in the window. */
	if((rcode = wbound(wn))) {
		--wn->cx;
		return(rcode);
	}

	tx = wn->cx + wn->ulx;
	ty = wn->cy + wn->uly;
	xend = wn->ulx + wn->xsize;

	for(; tx < xend; ++tx)
		vwrtch(tx, ty, vrdch(tx+1,ty));
	vwrtch(tx, ty, wn->atrib<<8 | ' ');

	return set_wcxy(wn, 0, 0);
}


/************************************************************************
* winsch(), Inserts a character at the current window cursor position.	*
************************************************************************/

winsch(wn,c)
register WINDOW *wn;
int c;
{
	int tcx, tcy;
	register int tx, ty, n, rcode, xend;

	/* Don't if we are not in the window. */
	if((rcode = wbound(wn))) {
		++wn->cx;
		return(rcode);
	}
	tcx = wn->cx;
	tcy = wn->cy;
	tx = wn->cx + wn->ulx;
	ty = wn->cy + wn->uly;
	switch (c) {
		case '\n':
			if(wn->cy == wn->ysize) {
				wscroll(wn);
				if(wn->cy >1)
					--ty;
			} else {
				++wn->cy;
				++tcy;
				wins_row(wn);
			}
			for( ; wn->cx <= wn->xsize;) {
				wputchar(wn, (n=vrdch(tx,ty)));
				vwrtch(tx++, ty, (n&0xff00)|' ');
			}
			break;
		case '\b':
			--tcx;
			if(wn->cy == 1 && wn->cx ==1)
				break;
			n=vrdch(tx,ty);
			wputch(wn,c);
			wputchar(wn,n);
			wdelch(wn);
			return set_wcxy(wn,-1,0);
		case '\r':
 			return wputch(wn, c);
		case 0x09:	/* Tab */
			do
				winsch(wn,' ');
			while(wn->cx & 0x07);
			break;
		default:
			c |= (wn->atrib<<8);
			if(wn->cx == wn->xsize) {
				vwrtch(tx,ty,c);
				++tcx;
				break;
			}
			xend = wn->ulx + wn->xsize;
			for( ; tx < xend; c=n) {
				n=vrdch(tx++,ty);
				vputca(c, page, 1);
			}
			++tcx;
			break;
	}
	return set_wcxy(wn, tcx, tcy);
}

/************************************************************************
* wgets(), Gets a string of characters at the current  window cursor	*
*	position.  The number of characters to obtain cnt or reaching	*
*	the end of the window terminates the function.  The number of	*
*	charaters retrived is returned.					* 
************************************************************************/

wgets(wn,s,cnt)
register WINDOW *wn;
char *s;		/* Where to place the null terminated string.	*/
int cnt;		/* Number of character to retrive. 		*/
{
	int xend; register int tx, ty, n;

	if(wbound(wn))
		return(0);
	tx = wn->ulx + wn->cx;
	ty = wn->uly + wn->cy;
	xend = wn->ulx + wn->xsize +1;
	for(n=0; cnt-- && tx < xend; ++n)
		*s++ = vrdch(tx++, ty);
	*s = 0;
	return(n);
}

/************************************************************************
* wscroll(), Scrolls the window up one line.				*
************************************************************************/

wscroll(wn)
register WINDOW *wn;
{
	int tcy;
	
	tcy = wn->cy;
	wn->cy =1;
	wdel_row(wn);
	wn->cy = tcy;
}

/************************************************************************
* wbound(), Check to see if cursor is within the window.		*
*	Returns a code indicating whether the cursor is in the window.  *
************************************************************************/

wbound(wn)
register WINDOW *wn;
{
	register int rcode;

	rcode =0;
	if(wn->cx <1)
		rcode = LEFT_OF;
	else if(wn->cx > wn->xsize)
		rcode = RIGHT_OF;
	if(wn->cy <1)
		rcode |= TOP_OF;
	else if(wn->cy > wn->ysize)
		rcode |= BOTTOM_OF;
	return(rcode);
}


/************************************************************************
* wgetch(), Returns the character at window cursor location or the	*
*	boundry code if the location is not within the window.		*
************************************************************************/

wgetch(wn)
register WINDOW *wn;
{
	int rcode;

	if((rcode=wbound(wn)))
		return (rcode);
	return (vrdch(wn->ulx + wn->cx, wn->uly + wn->cy) &0xff);
}

/************************************************************************
* set_wcxy(), Sets the relative window cursor to the give x,y location. *
*	If either x or y is zero the cursor is moved in the given axis  *
* 	the number of spaces given, where positive x is to the right 	*
*	and positive y is down.						*
*	Note that the home position for a window is 1,1.		*
************************************************************************/

set_wcxy(wn,x,y)
register WINDOW *wn;
int x,y;
{
	if(x && y) {
		wn->cx = x;
		wn->cy = y;
	} else {
		if(!x && y)
			wn->cy += y;
		if(!y && x)
			wn->cx += x;
	}
	gotoxy(wn->ulx + wn->cx, wn->uly + wn->cy, page);
	return wbound(wn);
}


/************************************************************************
* get_wcxy(), Gets the relative window cursor.				*
*	x = get_wcur() & 0xff, and y = get_wcur() >> 8			*
*	Note that the home position for a window is 1,1.		*
************************************************************************/

get_wcxy(wn)
register WINDOW *wn;
{
	return((wn->cy << 8) | wn->cx);
}

/************************************************************************
* set_cxy(), Sets the screen cursor to the give x,y location. 		*
*	Note that the home position is 0,0.				*
************************************************************************/

set_cxy(x,y)
register int x,y;
{
	if(!x || !y) {
		if(!x && y)
			y += (getxy(page)>>8);
		if(!y && x)
			x += (getxy(page) & 0xff);
	}
	gotoxy(x,y,page);
}


/************************************************************************
* get_cxy(), Gets the screen cursor location of page 0.			*
*	x = get_wcur() & 0xff, and y = get_wcur() >> 8			*
*	Note that the home position is 0,0.				*
************************************************************************/

get_cxy()
{
	return(getxy(page));
}

/************************************************************************
* get_page(), Gets the current video page being used.
************************************************************************/

get_page()
{
	return (page);
}

/************************************************************************
* set_page(), Sets the active video page and udates page.		*
************************************************************************/

set_page(n)
char n;
{
	switch (get_mode()) {
		case 0:
		case 1:
			if(n > 7)
				return(-1);
			break;
		case 2:
		case 3:
			if(n > 3)
				return(-1);
			break;
		case 7:
			if(n > 1)
				return(-1);
			break;
		default:
			return(-1);
	}
	vsetpage(n);
	page = n;
	return(n);
}

/************************************************************************
* wprintf(), Works like printf for a window.				*
************************************************************************/

wprintf(wn,cs,a,b,c,d,e,f,g,h,i,j,k,l,m,n)
WINDOW *wn;
char *cs;
{
	static buf[256];

	sprintf(buf,cs,a,b,c,d,e,f,g,h,i,j,k,l,m,n);
	return wputs(wn,buf);
}


/************************************************************************
* wprinta(), Works like wprintf with the addtion of the x,y		*
*	position.							*
************************************************************************/

wprinta(x,y,wn,cs,a,b,c,d,e,f,g,h,i,j,k,l,m,n)
int x,y;
WINDOW *wn;
char *cs;
{
	set_wcxy(wn,x,y);
	return wprintf(wn,cs,a,b,c,d,e,f,g,h,i,j,k,l,m,n);
}
