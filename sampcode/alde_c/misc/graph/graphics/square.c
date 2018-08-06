/**********************************************************************
SQUARE
Description - draws a square
**********************************************************************
Function - square(x,y,side)
Input - xy coord of upper left corner, length of one side
Output - to the screen
#includes - none
#defines  - none
Routines Called - rectangle, line, plotxy
Notes -
**********************************************************************/
square(x,y,side)
int x,y,side;
{
  rectangle(x,y,side,side);
  return;
}
/******************** end of function ********************************/
