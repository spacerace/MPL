#include "dos.h"
/******************************************************************************
*									      *
*  locate the cursor							      *
*									      *
******************************************************************************/
csrpos(row,col) 		   /* cursor  row, col, attribute */
int *row,*col;
{
  union REGS reg;
  reg.h.bh = 0; 		   /* page number */
  reg.h.ah = 3; 		   /* set csr command */
  int86(0x10,&reg,&reg);	   /* clr the screen */
  *row = reg.h.dh;		   /* set row number */
  *col = reg.h.dl;		   /* set col number */
}
