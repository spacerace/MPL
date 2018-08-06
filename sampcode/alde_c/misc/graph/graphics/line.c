/**********************************************************************
LINE
Description - draws a line from two points
**********************************************************************
Function - line(x0,y0,x1,y1) coord for point 0 and point 1
Input - see above
Output - to the screen
#includes - none
#defines  - none
Routines Called - plotxy
Notes - BYTE august 1981, pp414-16; digital differential analyzer DDA
**********************************************************************/
line(x0,y0,x1,y1)
int x0,y0,x1,y1;
{
  plotxy(x0,y0); /* print the first point */
  plotxy(x1,y1); /* print the end point */
  /* initialize the remaining variables and set the whole thing up */
  int d1,d2,d3,s1,s2,a1,a2,n1;
  d1=x1-x0;
  d2=y1-y0;
  s1=0;
  s2=1;
  a1=1;
  a2=0;
  if (d1<0){a1=-1;d1=-d1;}
  if (d2<0){d2=-d2;s2=-1;}
  if (d1<d2){ n1=d1;
              d1=d2;
              d2=n1;
              s1=a1;
              a1=0;
              a2=s2;
              s2=0;  }
  d3=d1/2;
  n1=1;

  /* heres where the work gets done */
  do{
      x0=x0+a1;
      y0=y0+a2;
      d3=d3+d2;
      n1=n1+1;
      if(d3>d1){d3=d3-d1; x0=x0+s1; y0=y0+s2;}
      plotxy(x0,y0);
    }while(n1<=d1);
  return;
}
/******************** end of function ********************************/
