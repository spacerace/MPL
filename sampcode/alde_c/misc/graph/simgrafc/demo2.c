#include	<stdio.h>

#define XMAX      319
#define YMAX      199
#define COLORMAX    3
#define WXMAX     220
#define WXMIN     100
#define WYMAX     150
#define WYMIN      50

main()
{
int i,x,y,clr;

   g_init();
   g_pal(0);
   g_bakgnd(0);
   g_window(WXMIN,WYMIN,WXMAX,WYMAX);


   randseed(1,1);

   for (i=0; i<=10000; ++i)
       {
       x = rand(XMAX);
       y = rand(YMAX);
       clr = 1 + rand(COLORMAX);
       g_dot(x,y,clr);
       }
   getch();
   g_text(80);
}

