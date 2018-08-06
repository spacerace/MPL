#include "dos.h"
/******************************************************************************
*									      *
*  locate the cursor							      *
*									      *
******************************************************************************/
locate(row,col) 		   /* cursor  row, col, attribute */
int row,col;
{
  union REGS reg;
  reg.h.bh = 0; 		   /* page number */
  reg.h.dh = row;		   /* set row number */
  reg.h.dl = col;		   /* set col number */
  reg.h.ah = 2; 		   /* set csr command */
  int86(0x10,&reg,&reg);	   /* clr the screen */
}
