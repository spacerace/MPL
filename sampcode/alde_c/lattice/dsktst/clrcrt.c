#include "dos.h"
/******************************************************************************
*									      *
*  clear crt & reset cursor						      *
*									      *
******************************************************************************/
clrcrt(a)			   /* attribute */
int a;
{
  union REGS reg;
  reg.x.cx = 2000;		   /* chars on crt */
  reg.h.bh = 0; 		   /* page number */
  reg.h.bl = a; 		   /* set attribute */
  reg.h.al = ' ';                  /* blank out screen */
  reg.h.ah = 9; 		   /* write char/attr */
  int86(0x10,&reg,&reg);	   /* clr the screen */
}
