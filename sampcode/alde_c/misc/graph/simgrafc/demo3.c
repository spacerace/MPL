#include <stdio.h>
#include <math.h>
#include <limits.h>

int clr1, clr2, clr3;
int xp[9], yp[9], zp[9];
float x0, y0, z0, xa, ya, za;
int xc[9], yc[9], zc[9];
double the, phi, rho;
float cc[4][4];
float x,y;

main()
{
         
   float dtor;
   int i;
   int ret;

   x0 = 160;
   y0 = 100;
   z0 = 100;
   the = 0;
   phi = 0;
   rho = 0;
   dtor = 3.14159/180;

g_init();
g_pal(0);
gbox(40,20,280,180,1);
locate(1,24);
printf("Hit <ENTER> to rotate cube");
locate(1,1);
   for (i=0;i<=18;++i){
           getcoord();
	   xmat();
	   rotate();
	   ymat();
 	   rotate();
           the += 10*dtor;
	   phi += 10*dtor;
           clr1 = 1, clr2 = 2, clr3 = 3;
           draw_box();
           scanf("%c",&ret);
           clr1 = 0, clr2 = 0, clr3 = 0;
           draw_box();
         }

scanf("%c",&ret);
g_text(80);
}
getcoord()
{
float da,db,dc;

   da = 100;
   db =  80;
   dc =  80;
   xp[1] = xp[2] = xp[5] = xp[6] = x0 - da/2;
   xp[3] = xp[4] = xp[7] = xp[8] = x0 + da/2;
   yp[1] = yp[4] = yp[5] = yp[8] = y0 + db/2;
   yp[2] = yp[3] = yp[6] = yp[7] = y0 - db/2;
   zp[1] = zp[2] = zp[3] = zp[4] = z0 - dc/2;
   zp[5] = zp[6] = zp[7] = zp[8] = z0 + dc/2;

   /* coordinates for the number on the face */

   xc[1] = xp[1] + 20;
   yc[1] = yp[1] - 15;
   xc[2] = xc[1] + 40;
   yc[2] = yc[1];
   xc[3] = xc[2] + 20;
   yc[3] = yc[2] - 10;
   xc[4] = xc[3];
   yc[4] = yc[3] - 10;
   xc[5] = xc[2];
   yc[5] = yc[4] - 10;
   xc[6] = xc[1];
   yc[6] = yc[5];
   xc[7] = xc[1];
   yc[7] = yc[6] - 20;
   xc[8] = xc[1] + 60;
   yc[8] = yc[7];
   zc[1]=zc[2]=zc[3]=zc[4]=zc[5]=zc[6]=zc[7]=zc[8]=zp[1];
}
xmat()
{
float sinthe = sin(the);
float costhe = cos(the);
 
   cc[1][1] = 1;
   cc[1][2] = cc[1][3] = cc[2][1] = cc[3][1] = 0;
   cc[2][2] = cc[3][3] = costhe;
   cc[2][3] = sinthe;
   cc[3][2] = -sinthe;
}
ymat()
{
float sinphi = sin(phi);
float cosphi = cos(phi);

   cc[1][1] = cc[3][3] = cosphi;
   cc[1][2] = cc[2][1] = cc[2][3] = cc[3][2] = 0;
   cc[1][3] = -sinphi;
   cc[2][2] = 1;
   cc[3][1] = sinphi;
}
zmat()
{
float sinrho = sin(rho);
float cosrho = cos(rho);

   cc[1][1] = cc[2][2] = cosrho;
   cc[1][2] = sinrho;
   cc[2][1] = -cc[1][2];
   cc[1][3] = cc[2][3] = cc[3][1] = cc[3][2] = 0;
   cc[3][3] = 1;  
}
draw_box()
{
int i;

   /* individual lines*/

   for (i=1; i<=4; ++i){
       g_line(xp[i],yp[i],xp[i+4],yp[i+4],clr3);
       }
  
   /* back rectangle  */

   g_line(xp[5],yp[5],xp[6],yp[6],clr2);
   g_line(xp[6],yp[6],xp[7],yp[7],clr2);
   g_line(xp[7],yp[7],xp[8],yp[8],clr2);
   g_line(xp[8],yp[8],xp[5],yp[5],clr2);

   /* front rectangle */

   g_line(xp[1],yp[1],xp[2],yp[2],clr1);
   g_line(xp[2],yp[2],xp[3],yp[3],clr1);
   g_line(xp[3],yp[3],xp[4],yp[4],clr1);
   g_line(xp[4],yp[4],xp[1],yp[1],clr1);

   /* number */
   for (i=2; i<=8; ++i)
       {
       g_line(xc[i-1],yc[i-1],xc[i],yc[i],clr3);
       }
  

}
rotate()
{
   int i;
	for (i=1;i<9;++i){
		xa = xp[i] - x0;
		ya = yp[i] - y0;
		za = zp[i] - z0;
		xp[i] = x0 + xa*cc[1][1] + ya*cc[2][1] + za*cc[3][1];
		yp[i] = y0 + xa*cc[1][2] + ya*cc[2][2] + za*cc[3][2];
		zp[i] = z0 + xa*cc[1][3] + ya*cc[2][3] + za*cc[3][3];
	}

	for (i=1;i<=8;++i){
		xa = xc[i] - x0;
		ya = yc[i] - y0;
		za = zc[i] - z0;
		xc[i] = x0 + xa*cc[1][1] + ya*cc[2][1] + za*cc[3][1];
		yc[i] = y0 + xa*cc[1][2] + ya*cc[2][2] + za*cc[3][2];
		zc[i] = z0 + xa*cc[1][3] + ya*cc[2][3] + za*cc[3][3];
	}
}
write_coords()
{
   int i;
   int row = 1;
   int col = 1;

        locate(row,col);
	for (i=1;i<9;++i){
		printf("%4d  %4d  %4d\n",xp[i],yp[i],zp[i]);
	}
}
gbox(x1,y1,x2,y2,clr)
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
