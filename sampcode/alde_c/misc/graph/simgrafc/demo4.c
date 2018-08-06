#include	<stdio.h>

#define	CHAINS	4
#define	LINES	12
#define	XMIN	0
#define	XMAX	319
#define	YMIN	0
#define	YMAX	199
#define	DXMAX	10
#define	DYMAX	6
#define	HICOLOR	3

int	ax[CHAINS*LINES],ay[CHAINS*LINES],adx[CHAINS],ady[CHAINS];
int	bx[CHAINS*LINES],by[CHAINS*LINES],bdx[CHAINS],bdy[CHAINS];
int	lax[CHAINS],lay[CHAINS],lbx[CHAINS],lby[CHAINS];
int	color[CHAINS];
int	chain,line,lptr,i;

_main()
{
   g_init();		/*	Put screen in Medium Res Graphics Mode	*/
   g_pal(1);
   g_bakgnd(0);
   randseed(0,0);
   
				 /****  INITIALIZE EACH LINE CHAIN  ****/
      for (chain=0; chain<CHAINS; ++chain)
   {
      lax[chain] = lbx[chain] = (XMIN+XMAX)/2;
      lay[chain] = (YMIN+YMAX)/2 - 20;
      lby[chain] = (YMIN+YMAX)/2 + 20;
      color[chain] = rand(HICOLOR)+1;
      adx[chain] = (rand(1)?1:-1) * (rand(DXMAX)+1);
      ady[chain] = (rand(1)?1:-1) * (rand(DYMAX)+1);
      bdx[chain] = (rand(1)?1:-1) * (rand(DXMAX)+1);
      bdy[chain] = (rand(1)?1:-1) * (rand(DYMAX)+1);
   }

   line = 0;
   while (!keyboard(1))  /** WHILE (no key) **/
      for (line=0; line<LINES; ++line)
	 for (chain=0; chain<CHAINS; ++chain)
	 {
	    lptr = chain*LINES+line;
	    g_line(ax[lptr],ay[lptr],bx[lptr],by[lptr],0);    /* ERASE LINE */
	    newline();
	    g_line(ax[lptr],ay[lptr],bx[lptr],by[lptr],color[chain]); /*DRAW*/
	 }
   g_text(80);
   _exit();
}


newline()
{
   int change=0;

				/** CALC NEW AX **/
   i = lax[chain]+adx[chain];
   if (i>=XMIN && i<=XMAX)
      lax[chain]=ax[lptr]=i;
   else
   {
      lax[chain]+= (adx[chain]= (adx[chain]<0 ?1:-1)*(rand(DXMAX)+1));
      ax[lptr]=lax[chain];
      change=1;
   }

				/** CALC NEW AY **/
   i = lay[chain]+ady[chain];
   if (i>=YMIN && i<=YMAX)
      lay[chain]=ay[lptr]=i;
   else
   {
      lay[chain]+= (ady[chain]= (ady[chain]<0 ?1:-1)*(rand(DYMAX)+1));
      ay[lptr]=lay[chain];
      change=1;
   }

				/** CALC NEW BX **/
   i = lbx[chain]+bdx[chain];
   if (i>=XMIN && i<=XMAX)
      lbx[chain]=bx[lptr]=i;
   else
   {
      lbx[chain]+= (bdx[chain]= (bdx[chain]<0 ?1:-1)*(rand(DXMAX)+1));
      bx[lptr]=lbx[chain];
      change=1;
   }

				/** CALC NEW BY **/
   i = lby[chain]+bdy[chain];
   if (i>=YMIN && i<=YMAX)
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