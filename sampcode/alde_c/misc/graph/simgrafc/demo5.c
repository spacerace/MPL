#include	<stdio.h>

#define	CHAINS	4
#define	LINES	12
#define	XMIN	0
#define	XMAX	319
#define	YMIN	0
#define	YMAX	199
#define	DXMAX	8
#define	DYMAX	4
#define	HICOLOR	3
#define COLORMAX    3
#define W1XMIN      1
#define W1YMIN      1
#define W1XMAX     79
#define W1YMAX     59
#define W2XMIN    161
#define W2YMIN      1
#define W2XMAX    318
#define W2YMAX    198
#define W3XMIN     40
#define W3YMIN    100
#define W3XMAX    120
#define W3YMAX    179

int	ax[CHAINS*LINES],ay[CHAINS*LINES],adx[CHAINS],ady[CHAINS];
int	bx[CHAINS*LINES],by[CHAINS*LINES],bdx[CHAINS],bdy[CHAINS];
int	lax[CHAINS],lay[CHAINS],lbx[CHAINS],lby[CHAINS];
int	color[CHAINS];
int	chain,line,lptr,i;
int     x3,y3,x4,y4,clrb;
main()
{
int i,j,x,y,clr;

   g_init();
   g_pal(0);
   g_bakgnd(1);
   clrb = 1;
   resetbox();

   randseed(0,0);
   
   initlines();
    
   g_window(W1XMIN,W1YMIN,W1XMAX,W1YMAX);
   box(W1XMIN-1,W1YMIN-1,W1XMAX+1,W1YMAX+1,1);
   box(W2XMIN-1,W2YMIN-1,W2XMAX+1,W2YMAX+1,1);

while (!keyboard(1))
   {
       display();
   }
   getch();
   g_text(80);
}

plotpoint()
{  
int x,y,clr;
       x = rand(XMAX);
       y = rand(YMAX);
       clr = 1 + rand(COLORMAX);
       g_dot(x,y,clr);
}  

initlines()
{			 /****  INITIALIZE EACH LINE CHAIN  ****/
      for (chain=0; chain<CHAINS; ++chain)
   {
      lax[chain] = lbx[chain] = (W2XMIN+W2XMAX)/2;
      lay[chain] = (W2YMIN+W2YMAX)/2 - 20;
      lby[chain] = (W2YMIN+W2YMAX)/2 + 20;
      color[chain] = rand(HICOLOR)+1;
      adx[chain] = (rand(1)?1:-1) * (rand(DXMAX)+1);
      ady[chain] = (rand(1)?1:-1) * (rand(DYMAX)+1);
      bdx[chain] = (rand(1)?1:-1) * (rand(DXMAX)+1);
      bdy[chain] = (rand(1)?1:-1) * (rand(DYMAX)+1);
   }
}

display()
{
int k;
      for (line=0; line<LINES; ++line)
	 for (chain=0; chain<CHAINS; ++chain)
	 {
	    lptr = chain*LINES+line;
	    g_line(ax[lptr],ay[lptr],bx[lptr],by[lptr],0);    /* ERASE LINE */
	    newline();
	    g_line(ax[lptr],ay[lptr],bx[lptr],by[lptr],color[chain]); /*DRAW*/
            for (k=0; k<=6; ++k)
                {
                plotpoint();
                }
            if (x4>130)
               resetbox();
            else
               growbox();
	 }
}

newline()
{
   int change=0;

				/** CALC NEW AX **/
   i = lax[chain]+adx[chain];
   if (i>=W2XMIN && i<=W2XMAX)
      lax[chain]=ax[lptr]=i;
   else
   {
      lax[chain]+= (adx[chain]= (adx[chain]<0 ?1:-1)*(rand(DXMAX)+1));
      ax[lptr]=lax[chain];
      change=1;
   }

				/** CALC NEW AY **/
   i = lay[chain]+ady[chain];
   if (i>=W2YMIN && i<=W2YMAX)
      lay[chain]=ay[lptr]=i;
   else
   {
      lay[chain]+= (ady[chain]= (ady[chain]<0 ?1:-1)*(rand(DYMAX)+1));
      ay[lptr]=lay[chain];
      change=1;
   }

				/** CALC NEW BX **/
   i = lbx[chain]+bdx[chain];
   if (i>=W2XMIN && i<=W2XMAX)
      lbx[chain]=bx[lptr]=i;
   else
   {
      lbx[chain]+= (bdx[chain]= (bdx[chain]<0 ?1:-1)*(rand(DXMAX)+1));
      bx[lptr]=lbx[chain];
      change=1;
   }

				/** CALC NEW BY **/
   i = lby[chain]+bdy[chain];
   if (i>=W2YMIN && i<=W2YMAX)
      lby[chain]=by[lptr]=i;
   else
   {
      lby[chain]+= (bdy[chain]= (bdy[chain]<0 ?1:-1)*(rand(DYMAX)+1));
      by[lptr]=lby[chain];
      change=1;
   }

		/** CHANGE COLOR IF DIRECTION CHANGE **/
   if (change)
      color[chain] = rand(HICOLOR)+1;

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
growbox()
{
   box(x3,y3,x4,y4,clrb);
   x3 -= 1;
   y3 -= 1;
   x4 += 1;
   y4 += 1;
}
resetbox()
{
   x3 = x4 =  80;
   y3 = y4 = 130;
   clrb = 1 + (clrb+1)%3;
}
