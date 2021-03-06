#include <graphics.h>
#define SIZE 32   /* size of array */
#define MSX 320   /* midscreen */
#define XK 10 /* X scale */

int TILT,ZK;
int map[SIZE+1][SIZE+1];
int ms[641];
double frand();

main(argc,argv)
int argc;
char *argv[];
{
    int i,j,sx,sy,sx1,sy1,min,max;
    double f,w;
    if (argc != 4) {
	printf("syntax is: MFRACT tilt-factor z-scale seed-value\n");
	printf("try: mfract 2 3 1027\n");
	exit(0);
	}
   printf("working ...");
    for (i=0;i<641;i++) ms[i]=199;
    TILT=atoi(argv[1]);
    ZK=atoi(argv[2]);
    srand(atoi(argv[3]));

    sfract(0,0,SIZE);

    min=32766;
    max=-32766;
    for (i=0;i<=SIZE;i++)
     for (j=0;j<=SIZE;j++) {
	 sy=(map[i][j]=(TILT*(i+j)+ZK*map[i][j]));
	 if (sy<min) min=sy;
	 if (sy>max) max=sy;
	 }

    f = 199.0/(double)(max-min);
    for (i=0;i<=SIZE;i++)
     for (j=0;j<=SIZE;j++) map[i][j]=199-(int)(f*(map[i][j]-min));

    scr_setup();
    scr_setmode( 6 );
    scr_clr();
    for (i=0;i<=SIZE;i++) {
       for (j=0;j<=SIZE;j++) {
	   sx=MSX+XK*(i-j);
	   sy=map[i][j];
	   if (j < SIZE) {
	      sy1=map[i][j+1];
	      sx1=sx-XK;
	      hline(sx,sy,sx1,sy1);
	      }
	   if (i < SIZE ) {
	      sx1=sx+XK;
	      sy1=map[i+1][j];
	      hline(sx,sy,sx1,sy1);
	      }
	   }
       }
   while ( 0 == scr_csts()) ;
   scr_setmode( 2 );
   scr_clr();
   exit(0);
}

hline(x,y,x1,y1)
int x,y,x1,y1;
{
   int dx,dy,hx,hy,hx0,hy0,hx1,hy1,hi;
   double hz;

   if (x <= x1) {
      dx = x1-x ;
      dy = y1-y ;
      hx0= x;
      hy0= y;

      }
      else {
	 dx = x-x1 ;
	 dy = y-y1 ;
	 hx0 = x1;
	 hy0 = y1;
      }
   hz = (double)dy/(double)dx;
   for (hi=0; hi < dx; hi++) {
       hx = hx0+hi;
       hy = hy0+hz*hi;
       if (ms[hx]>=hy) { ms[hx]=hy; pset(hx,hy,7) ; }
       }
   }

sfract(x,y,d)
 int x,y,d;
 {
 int d2,xc,yc,xd,yd,lfract();

 xd=x+d;
 yd=y+d;
 d2=d>>1;
 xc=x+d2;
 yc=y+d2;
 map[x][yc] = lfract(x,y,x,yd,d);
 map[xc][y] = lfract(x,y,xd,y,d);
 map[xd][yc] = lfract(xd,y,xd,yd,d);
 map[xc][yd] = lfract(x,yd,xd,yd,d);
 map[xc][yc] = (lfract(x,yc,xd,yc,d) + lfract(xc,y,xc,yd,d))/2 ;

 if (d2 > 1) {
  sfract(x,y,d2);
  sfract(xc,y,d2);
  sfract(x,yc,d2);
  sfract(xc,yc,d2);
 }
}

int lfract(x1,y1,x2,y2,d3)
 int x1,y1,x2,y2,d3;
 {
 return(((map[x1][y1]+map[x2][y2])/2)+d3*(frand()-.5));
 }
                                