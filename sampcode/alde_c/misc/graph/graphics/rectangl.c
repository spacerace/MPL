/**********************************************************************
RECTANGL.C
Description -  draws a rectangle
**********************************************************************
Function - rectangle(x,y,horiz width,vertical height)
Input - xy coord of upper left corner, width, height
Output - to the screen
#includes - none
#defines  - none
Routines Called - line, plotxy
Notes - offsets vertical scale
**********************************************************************/
rectangle(x,y,sidex,sidey)
int x,y,sidex,sidey;
{
  int x1,y1;
  x1=x+sidex; /* horizontal width */
  y1=y+((sidey*2)/5); /* vertical height */
  line(x,y,x1,y); /* top horiz line */
  line(x,y1,x1,y1); /* bott horiz line */
  line(x,y,x,y1); /* left vertical line */
  line(x1,y,x1,y1); /* right vertical line */
  return;
}
/******************** end of function ********************************/
