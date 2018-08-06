#include <dos.h>

void bkscroll(lcol,trow,rcol,brow,lines)  /* scroll a screen area down */
   int lcol, trow, rcol, brow, lines;
{
   union REGS inregs;
   union REGS outregs;

   inregs.h.ah = 15;                    /* get active display page */
   int86(0x10,&inregs,&outregs);
   inregs.h.bh = outregs.h.bh;
   inregs.h.dl = --lcol;
   inregs.h.dh = --trow;
   inregs.h.ah = 2;                     /* set cursor position */
   int86(0x10,&inregs,&outregs);
   inregs.h.ah = 8;                     /* get color/attribute */
   int86(0x10,&inregs,&outregs);
   inregs.h.bh = outregs.h.ah;
   inregs.h.cl = lcol;
   inregs.h.ch = trow;
   inregs.h.dl = --rcol;
   inregs.h.dh = --brow;
   inregs.h.al = lines;
   inregs.h.ah = 7;                     /* do the backscroll */
   int86(0x10,&inregs,&outregs);
}





void clreol()                         /* clear from cursor to end of line */
{
   union REGS inregs;
   union REGS outregs;
   int attr, columns;

   inregs.h.ah = 15;                       /* get display page, columns */
   int86(0x10,&inregs,&outregs);
   inregs.h.bh = outregs.h.bh;
   columns = outregs.h.ah;
   inregs.h.ah = 8;                        /* get color/attribute */
   int86(0x10,&inregs,&outregs);
   attr = outregs.h.ah;
   inregs.h.ah = 3;                        /* get cursor position */
   int86(0x10,&inregs,&outregs);
   inregs.h.bl = attr;
   inregs.x.cx = columns - outregs.h.dl;
   inregs.x.ax = 0x0920;                   /* write spaces until EOLN */
   int86(0x10,&inregs,&outregs);
}





void scroll(lcol,trow,rcol,brow,lines)   /* scroll a screen area up */
   int lcol, trow, rcol, brow, lines;
{
   union REGS inregs;
   union REGS outregs;

   inregs.h.ah = 15;                    /* get active display page */
   int86(0x10,&inregs,&outregs);
   inregs.h.bh = outregs.h.bh;
   inregs.h.dl = --lcol;
   inregs.h.dh = --trow;
   inregs.h.ah = 2;                     /* set cursor position */
   int86(0x10,&inregs,&outregs);
   inregs.h.ah = 8;                     /* get color/attribute */
   int86(0x10,&inregs,&outregs);
   inregs.h.bh = outregs.h.ah;
   inregs.h.cl = lcol;
   inregs.h.ch = trow;
   inregs.h.dl = --rcol;
   inregs.h.dh = --brow;
   inregs.h.al = lines;
   inregs.h.ah = 6;                     /* do the scroll */
   int86(0x10,&inregs,&outregs);
}
