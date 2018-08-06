#include <dos.h>
#include <ctype.h>

char getkey(str)
   char *str;
{
   char c, *s;
   int done = 0;

   while (!done) {
      if (islower(c = getch()))
         c = toupper(c);
      if (*str) {
         s = str;
         while (*s && !done)
            done = (c == *s++);
         }
      else done = 1;
      }
   return(c);
}



int mousebuttons()                    /* see if mouse buttons are pressed */
{
   union REGS inregs;
   union REGS outregs;
   inregs.x.ax = 6;
   int86(0x33,&inregs,&outregs);
   return(outregs.x.ax & 3);
}



int mousecheck()                      /* see if mouse exists, & # of buttons */
{
   union REGS inregs;
   union REGS outregs;
   inregs.x.ax = 0;
   int86(0x33,&inregs,&outregs);
   return(outregs.x.ax ? outregs.x.bx : 0);
}



int mouseclick()                      /* see if mouse buttons were clicked */
{
   int click = 0;
   union REGS inregs;
   union REGS outregs;
   inregs.x.ax = 5;
   inregs.x.bx = 1;
   int86(0x33,&inregs,&outregs);
   click = outregs.x.bx << 1;
   inregs.x.bx--;
   int86(0x33,&inregs,&outregs);
   return(click | outregs.x.bx);
}



int mousecol()                        /* get column where mouse cursor is */
{
   union REGS inregs;
   union REGS outregs;
   inregs.x.ax = 3;
   int86(0x33,&inregs,&outregs);
   return(outregs.x.cx);
}



void mousecursor(toggle)              /* turn mouse cursor on or off */
   int toggle;
{
   union REGS inregs;
   union REGS outregs;
   if (toggle) inregs.x.ax = 1;
   else inregs.x.ax = 2;
   int86(0x33,&inregs,&outregs);
}



void mouseloc(column,row)             /* set location of mouse cursor */
   int column, row;
{
   union REGS inregs;
   union REGS outregs;
   inregs.x.ax = 4;
   inregs.x.cx = column;
   inregs.x.dx = row;
   int86(0x33,&inregs,&outregs);
}



int mouserow()                        /* get row where mouse cursor is */
{
   union REGS inregs;
   union REGS outregs;
   inregs.x.ax = 3;
   int86(0x33,&inregs,&outregs);
   return(outregs.x.dx);
}
