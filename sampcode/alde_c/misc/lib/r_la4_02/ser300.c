/* SER300.C - From page 485 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. This program is a simple prog-    */
/* ram that communicates with a serial port and needs a Hayes   */
/* compatible modem as the program is written.                  */
/****************************************************************/

#include "dos.h"              /*For ROM BIOS calls*/
#define COM 1                 /*Serial port (0 = COM1, 1 = COM2)*/
#define CONF 0x43             /*300 baud, no par, 1 stop, 8 bits*/
#define DATAR 0x100           /*data-ready bit in status word*/
#define TRUE 1

main()
{
union REGS inregs, outregs;
char ch;

   inregs.h.ah = 0;           /*initialize port service*/
   inregs.x.dx = COM;         /*port number*/
   inregs.h.al = CONF;        /*configuration*/
   int86(0x14, &inregs, &outregs);  /*serial port interrupt*/
   printf("\n\nPort status after initialization: 0x%x.\n\n", outregs.x.ax);
   while(TRUE)  {
      if(kbhit())  {          /*if key pressed*/
         ch = getch();        /*get char from keyboard*/
         inregs.h.ah = 1;     /*'sent char' service*/
         inregs.x.dx = COM;   /*port number*/
         inregs.h.al = ch;    /*character*/
         int86(0x14, &inregs, &outregs);
      }
      inregs.h.ah = 3;        /*'get port status' service*/
      inregs.x.dx = COM;      /* port number*/
      int86(0x14, &inregs, &outregs);
      if(outregs.x.ax & DATAR)  {      /*data ready ? */
         inregs.h.ah = 2;     /*'receive char' service*/
         inregs.x.dx = COM;   /*port number*/
         int86(0x14, &inregs, &outregs);
         ch = outregs.h.al & 0x7F;     /*get char*/
         putch(ch);                    /*display char*/
      }                    /*end if*/
   }                       /*end while*/
}

