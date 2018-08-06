#include <dos.h>
/******************************************************************************
*									      *
*  absolute disk read (calls disk bios) 0x80 = 1st hard disk		      *
*									      *
******************************************************************************/
absread(drive,cyl,head,record,count,buffer)
int drive, cyl, head, record, count;
char *buffer;
{
  union  REGS  reg;
  struct SREGS seg;
  int err = 0;
  segread(&seg);
  reg.h.ch = (cyl & 0x00ff);	     /* lo order cylinder */
  reg.h.cl = (((cyl & 0xff00) << 6) | record);
  reg.h.dh = head;
  reg.h.dl = drive;
  reg.h.al = count;
  reg.h.ah = 2;
  reg.x.bx = (unsigned int) buffer;		    /* point to buffer */
  err = int86x(0x13,&reg,&reg,&seg); /* read the disk */
  err = (err >> 8);
  return(err);
}
