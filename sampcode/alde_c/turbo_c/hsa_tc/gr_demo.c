/************************************************************************/
/*																		*/
/*				   G R _ D E M O . C - Demo of HSA_GRAF					*/
/*																		*/
/************************************************************************/

/************************************************************************/
/*																		*/
/*			  Copyright (c) Hardwood Software Associates 1987			*/
/*																		*/
/*					   Hardwood Software Associates						*/
/*							  364 Benson Road							*/
/*						   Northbridge, Ma 01534						*/
/*																		*/
/************************************************************************/

/************************************************************************
 *			G R _ D E M O . C    E D I T    L O G
 *
 *	Edit	Date	Person		Modification
 *	----	----	------		------------
 *
 ************************************************************************/

/************************************************************************
 *													+-------------------+
 *	Author: R. Evans								| F U N C T I O N S |
 *	Date:	July, 1987								+-------------------+
 *
 ************** GR_DEMO - Demo of HSA_GRAF
 * SYNOPSIS:
 *				gr_demo
 * DESCRIPTION:
 *				Demo some of the important aspects of the HSA_GRAF library.
 *
 ************************************************************************/
#include "stdio.h"
#include "hsa_gr.h"
#include "ctype.h"

#define FALSE	0
#define TRUE	1
#define BUFFER_SIZE	16384
#define XC	3000
#define YC	2500
#define XC1 6500
#define ARCS	8
#define Y_TEXT	210

int crt_type;
int buffer [BUFFER_SIZE];
unsigned _STACK=0x6800;

main ()
{
crt_type = initialize ();
g_origin (1);									/* Origin lower left corner */
do_circle ();
if (crt_type != EGA)							/* Save doesn't save full EGA */
	g_save (buffer,BUFFER_SIZE);
do_square ();
if (crt_type != EGA)							/* Save doesn't save full EGA */
	{
	g_restore (buffer,BUFFER_SIZE);
	getch ();
	}
bye ();											/* All done */
} /* Of gr_demo */

/************************************************************************/
/*																		*/
/*		   C L E A R _ S C R E E N - Clear Screen and Draw Logo			*/
/*																		*/
/************************************************************************/

void clear_screen ()
{
g_clr ();
g_fore_color (SCR_CYAN);
hsa_logo (10,4000,1300,SCR_CYAN);				/* Draw HSA Logo */
g_fore_color (SCR_WHITE);
} /* Of clear_screen */

/************************************************************************/
/*																		*/
/*					  D O _ S Q U A R E - Mazy Square					*/
/*																		*/
/************************************************************************/
#define WIDTH	250
#define SIZE	3500
do_square ()
{
int x=2000;
int y=WIDTH;
register int yy;
register int size=SIZE;
int i;

clear_screen ();
yy=5000;
gs_str_h (6000,yy-=Y_TEXT,"This demo is only a");
gs_str_h (6000,yy-=Y_TEXT,"very small example");
gs_str_h (6000,yy-=Y_TEXT,"of what can be done");
gs_str_h (6000,yy-=Y_TEXT,"using HSA_GRAF.");

gs_point (x,y);									/* Start it off */
while (size > 4*WIDTH)
	{
	gs_line (-1,-1,-1,y+=size);
	size -= WIDTH;
	gs_line (-1,-1,x+=size,-1);
	size -= WIDTH;
	gs_line (-1,-1,-1,y-=size);
	size -= WIDTH;
	gs_line (-1,-1,x-=size,-1);
	size -= WIDTH;
	}
size += (2*WIDTH);
gs_line (-1,-1,-1,y-=WIDTH);
i=2;
while (size < SIZE)
	{
	gs_line (-1,-1,x+=size,-1);
	size += (i*WIDTH);
	i=1;
	gs_line (-1,-1,-1,y+=size);
	size += WIDTH;
	gs_line (-1,-1,x-=size,-1);
	size += WIDTH;
	if (size > SIZE+WIDTH)
		size = SIZE + WIDTH;
	gs_line (-1,-1,-1,y-=size);
	size += WIDTH;
	}
gs_line (-1,-1,x+WIDTH,-1);
g_loc (&x,&y);
g_fill (x-1,y+1,SCR_RED);
pause ("");
} /* Of do_square */

/************************************************************************/
/*																		*/
/*				   D O _ C I R C L E - Draw Some Circles				*/
/*																		*/
/************************************************************************/
do_circle ()
{
int x1;
int x,y;
int color = 1;
int i;

clear_screen ();
y=5000;
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"HSA_GRAF has a single");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"simple function that");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"will draw full or");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"partial circles quickly");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"and accurately.");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"Another function will");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"fill an enclosed");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"boundary with a");
gs_str_h (XC+8*(XC/10),y-=Y_TEXT,"specified color");

for (x1=XC-WIDTH;x1>2*WIDTH;x1-=WIDTH)
	{
	g_fore_color (SCR_WHITE);
	gs_arc (x1,YC,x1,YC,XC,YC,1);
	g_loc (&x,&y);
	g_fore_color (color);
	g_fill (x+1,y+1,color);
	if (crt_type != EGA)
		color = !color;
	else
		color++;
	}
gs_line (XC1-ARCS*WIDTH/2,YC,XC1+ARCS*WIDTH/2,YC);
for (x1=XC1-WIDTH/2,i=0;i<ARCS;x1-=(WIDTH/2),i++)
	{
	g_fore_color (SCR_WHITE);
	gs_arc (x1,YC,x1+2*(i+1)*(WIDTH/2),YC,XC1,YC,0);
	g_loc (&x,&y);
	g_fore_color (color);
	g_fill (x-2,y-1,color);
	if (crt_type != EGA)
		color = !color;
	else
		color++;
	}
g_fore_color (SCR_WHITE);
pause ("");
} /* Of do_circle */

/************************************************************************/
/*																		*/
/*			I N I T I A L I Z E - Initialize for Graphics Demo			*/
/*																		*/
/************************************************************************/

initialize ()
{
int crt_type;
int x,y;
int s,seg;
short mode;
short xdots,ydots;
char buffer[80];

for (;;)
	{
	printf ("\n\nHSA_GRAF -  Graphics Library Demonstration\n\n");
	printf ("Select graphics adapter for demonstration:\n\n");
	printf ("\t1. Hercules Monochrome\n");
	printf ("\t2. Color Graphics Adapter (CGA)\n");
	printf ("\t3. Extended Graphics Adapter (EGA)\n");
	printf ("\t4. Other brand of graphics adapter\n");
	printf ("\t9. Quit\n\n");
	printf ("Enter selection: ");
	gets (buffer);
	sscanf (buffer,"%d",&crt_type);
	if (crt_type == 9)
		exit (0);
	if (crt_type < 1 || crt_type > 4)
		continue;
	switch (crt_type)
		{
	case 1:										/* Hercules Monochrome */
		crt_type = MONOCHROME;
		g_page (1);
		break;
	case 2:										/* Color Graphics Adapter */
		crt_type = CGA;
		g_cga_hr ();							/* Initialize */
		break;
	case 3:										/* Extended Graphics Adapter */
		crt_type = EGA;
		g_ega_hr_co ();
		break;
	case 4:										/* Other brand */
		crt_type = CGA;
		printf ("Enter X,Y pixels: ");
		scanf ("%d,%d",&x,&y);
		printf ("Enter scans, segment: ");
		scanf ("%d,%x",&s,&seg);
		printf ("Enter X,Y dots per inch <640x200 = 80,37>: ");
		gets (buffer);
		if (strlen (buffer) > 0)
			sscanf (buffer,"%d,%d",&xdots,&ydots);
		else
			{
			xdots = 80;
			ydots = 37;
			}
		printf ("Enter graphics mode function call <IBM = 6>: ");
		gets (buffer);
		if (strlen (buffer) > 0)
			sscanf (buffer,"%d",&mode);
		else
			mode = 6;
		g_other (x,y,s,seg,mode);
		g_scale (x,y);
		break;
		}
	break;
	}
printf ("Press any key to switch to graphics mode."); getch ();
g_page(1);
g_clr();
disp_graph();
return (crt_type);

} /* Of initialize */
/************************************************************************/
/*																		*/
/*					   B Y E - All Done with GR_DEMO					*/
/*																		*/
/************************************************************************/

bye ()
{
disp_text(3);
printf("All done\n");
exit(0);
} /* Of bye */

/************************************************************************/
/*																		*/
/*					P A U S E - Wait for User Reaction					*/
/*																		*/
/************************************************************************/

pause (type)
char *type;
{
int answer;

g_str_h (0,10,type);
g_str_h (-1,-1,"Press any key to continue, \"Q\" to quit.");
answer = getch ();
if (toupper (answer) == 'Q')
	bye ();
} /* Of pause */


