/*--------------------------------------------------------------------
 *                                grdemo.c
 *
 *         illustrates the use of the graphic functions in graph.lib
 *
 *                     copyright 1987 by Michael Allen
 *
 *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "glob_defs.h"
#include "global.h"
#include "graph.h"

void
p(int x, int y, char *item)
{
  gotoxy(x, y);
  puts(item);
}

main()
{
   register int x, y, i;

   hires();
   hirescolor(WHITE);

   p(20,8,"testing plot...");
   for (i=0;i < 1500;i++)
      plot(rand() % xmax, rand() % ymax);
   getchar();

   p(20,10,"testing draw_line...");
   for (i=0,x = 20;x < 220;x+=6,i++)
      draw_line(x, 1, x+150+i, 190);
   getchar();
   clearscreen();

   hirescolor(BLUE);
   p(20,12,"testing draw_box...");
   for (i=0,y = 10,x = 10;x < 350;x+=4,y+=2,i+=2)
      draw_box(x, y, x+50+i, y+25);
   getchar();
   clearscreen();

   hirescolor(GREEN);
   p(20,13,"testing draw_circle...");
   for (y = 10,x = 320;y < 100;y+=2)
      draw_circle(320,100, x, y);
   getchar();
   clearscreen();

   hirescolor(YELLOW);
   p(20,14,"testing draw_ellipse...");
   for (i = 0,y = 20,x = 150;x < 300;x+=4,y+=2,i+=2)
      lips(x,y,x+150,y+20+i);
   getchar();

   hirescolor(CYAN);
   p(20,16,"draw boxes with xor_mode...");
   draw_option = XOR_MODE;
   for (y = 0,x = 150;x < 500;x+=2,y++)
   {
      draw_box(x, y, x + 48, y + 24);
	   for (i = 0;i < 1000;i++);         /* pause */
      draw_box(x, y, x + 48, y + 24);
   }
   draw_option = NORMAL;
   draw_box(x, y, x + 48, y + 24);

   getchar();

   textmode();

}
