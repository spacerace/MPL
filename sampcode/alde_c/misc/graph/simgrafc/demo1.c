#include	<stdio.h>

main()
{
int i,j,k,x1,x2,y1,y2;

   g_init();
   g_pal(1);
   g_bakgnd(1);
for (j=0; j<=3; ++j)
   {
   pause();
   for (i=0; i<= 15; ++i)
       {
       x1 = i*6 + 2*j;
       y1 = i*6 + 2*j;
       x2 = 319-x1;
       y2 = 199-y1;
       box(x1,y1,x2,y2,j);
       }
    }
for (k=0; k<=15; ++k)
   { 
   pause();
   g_bakgnd(k);
   }

   getch();
   g_clear();
   g_text();
}
box(x1,y1,x2,y2,clr)
int x1,y1,x2,y2,clr;
{
    g_line(x1,y1,x2,y1,clr);
    g_line(x2,y1,x2,y2,clr);
    g_line(x2,y2,x1,y2,clr);
    g_line(x1,y2,x1,y1,clr);
}
pause()
{
    int l;
    for (l=0; l<=20000; ++l) {}
}
