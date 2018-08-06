/**********************************************************************
CIRCLE
Description -circle(x coordinate, y coordinate, radius)
**********************************************************************
Function - draws a circle on the screen
Input - integers for x,y center coordinates, integer for radius
Output - to the screen
#includes -
#defines  - none
Routines Called - plot8.c, plotxy.c
Notes - listing 3 page 30 dec83 dobbs
**********************************************************************/
circle(xcenter,ycenter,radius)
int xcenter,ycenter,radius;
{
  int xcrd,ycrd,a,b,f;
  xcrd=radius;
  ycrd=0;
  a=-2*(xcrd+1);
  b=1;
  f=0;
  do{
     plot8(xcenter,ycenter,xcrd,ycrd);
     ycrd=ycrd+1;
     f=f+b;
     if(f>radius){f=f+a;
                  a=a+2;
                  xcrd=xcrd-1;}
     b=b+2;
    }
     while(b<=-a);
  return;
}
/******************** end of function ********************************/
/**********************************************************************
PLOT8
Description - plots 8 points for circle routine
**********************************************************************
Function -  plot8(xcenter,ycenter,xcrd,ycrd);
Input - x,y coordinates of center pt, x,y coord of base octant
Output - 8 characters to screen
#includes - plotxy.c
#defines  - none
Routines Called - plotxy to print character
Notes - no error checking, char is defined in plotxy
**********************************************************************/
plot8(xcenter,ycenter,xcrd,ycrd)

int xcenter,ycenter,xcrd,ycrd;
{
  plotxy(ycrd+xcenter,((2*xcrd)/5)+ycenter);    /* b quart c-clockwise   */
  plotxy(-ycrd+xcenter,((2*xcrd)/5)+ycenter);   /* b quart clockwise */
  plotxy(-ycrd+xcenter,((2*-xcrd)/5)+ycenter);  /* d quart c-clockwise   */
  plotxy(ycrd+xcenter,((2*-xcrd)/5)+ycenter);   /* d quart clockwise */
  plotxy(xcrd+xcenter,((2*-ycrd)/5)+ycenter);   /* a quart c-clockwise   */
  plotxy(xcrd+xcenter,((2*ycrd)/5)+ycenter);    /* a quart clockwise */
  plotxy(-xcrd+xcenter,((2*ycrd)/5)+ycenter);   /* c quart c-clockwise   */
  plotxy(-xcrd+xcenter,((2*-ycrd)/5)+ycenter);  /* c quart clockwise */
  return;
}

/******************** end of function ********************************/
