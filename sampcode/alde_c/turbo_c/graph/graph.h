/*--------------------------------------------------------------------------
 *                                  graph.h
 *
 *                       copyright 1987 by Michael Allen
 *
 *      This contains the function prototypes for the functions in graph.lib
 *      You must include this in your each module that uses these functions
 *      with a: #include "graph.h".
 *
 *-------------------------------------------------------------------------*/

int    baseaddress(int y);  /* returns the line offset in the screen buffer */
							 /* for Y */

BYTE   hardwarepresent();	     /* returns true if the CGA is present   */

void   init_graphics(int mode); /* sets graphics mode. these functions	 */
                                /* support high - res mode only..       */ 

void   hirescolor(int color); /* sets foreground color, see glob_def.h */

void   gotoxy(int x, int y);	 /* moves cursor to x y */

void   clearscreen();			 /* clears full screen */

void   plot(int x, int y);      /* draws one pixel at x, y using current */
                                /* drawing mode (normal or xor_mode)     */

void   draw_line(int x, int y, int newx, int newy);  /* draws line from x, y  */
                                                     /* to newx, newy         */ 

void   draw_box(int x1, int y1, int x2, int y2);     /* draws box given corners */

void   draw_circle(int bx, int by, int tox, int toy); /* bx,by is center, tox, toy */
                                                      /* on circumference          */

void   lips(int bx, int by, int tx, int ty);  /* draws ellipse: by,by at center,   */
                                              /* tx,ty = corner of box containing  */

/* ------------------------------------------------------------ */




