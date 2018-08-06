/**********************************************************************
PLOTXY
Description - plotxy(x coordinate, y coordinate)
**********************************************************************
Function - plots a character at a given coordinate
Input - integer plot routine
Output - just to the screen
#includes - none
#defines  - none
Routines Called - needs desmet c functions scr_rowcol and scr_co
Notes - does error check--change as screen size changes; specify char
  to print
**********************************************************************/

plotxy(x,y)
int x,y;
{
  if(x<80 && x>-1 && y<24 && y>-1);  /* catch screen plot error */
    {
     scr_rowcol(y,x); /* desmet pc instruction - position cursor */
     scr_co('.');  /* desmet pc instruction - print on screen */
    }
  return;
}

/******************** end of function ********************************/
