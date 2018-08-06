#include <dos.h>

int commports()                       /* get number of serial ports */
{
   union REGS inregs;
   union REGS outregs;
   int86(0x11,&inregs,&outregs);
   return((outregs.h.ah & 15) >> 1);
}



int displaytype()                     /* get display type */
{
   union REGS inregs;
   union REGS outregs;

   inregs.h.ah = 15;
   int86(0x10,&inregs,&outregs);
   return((outregs.h.al != 7));
}



long drivespace(drv)                  /* get free space on a given drive */
   char drv;
{
   union REGS inregs;
   union REGS outregs;

   inregs.h.ah = 0x36;
   if (drv == '@') inregs.h.dl = 0;
   else inregs.h.dl = toupper(drv) - 'A' + 1;
   intdos(&inregs,&outregs);
   return((long) outregs.x.ax * outregs.x.bx * outregs.x.cx);
}



char getdrive()                       /* get default drive */
{
   return((bdos(0x19,0,0) & 255)+'A');
}



int joystick()                        /* see if there's a joystick */
{
   union REGS inregs;
   union REGS outregs;
   int86(0x11,&inregs,&outregs);
   return((outregs.h.ah >> 4) & 1);
}



int limmfree()                        /* get free pages of expanded memory */
{
   int freemem = 0;
   union REGS inregs;
   union REGS outregs;

   inregs.h.ah = 0x42;
   int86(0x67,&inregs,&outregs);
   if (!outregs.h.ah) freemem = outregs.x.bx;
   return(freemem);
}



int limmtotal()                       /* get total pages of expanded memory */
{
   int totalmem = 0;
   union REGS inregs;
   union REGS outregs;

   inregs.h.ah = 0x42;
   int86(0x67,&inregs,&outregs);
   if (!outregs.h.ah) totalmem = outregs.x.dx;
   return(totalmem);
}



int printports()                      /* get number of parallel ports */
{
   union REGS inregs;
   union REGS outregs;
   int86(0x11,&inregs,&outregs);
   return(outregs.h.ah >> 6);
}



int totalmem()                        /* get Kbytes of installed RAM */
{
   union REGS inregs;
   union REGS outregs;
   int86(0x12,&inregs,&outregs);
   return(outregs.x.ax);
}
