/* SER1200.C - From page 495 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. This program modifies the pro-    */
/* gram SER300.C to communicate with the serial port at 1200 b. */
/****************************************************************/

#include "dos.h"           /*for union REGS*/
#define COM 1              /*serial port no. 0 = COM1, 1 = COM2*/
#define CONF 0x83          /*1200bpi, no par, 1 stop, 8 bits*/
#define RS232 0x14         /*RS-232 serial port int number*/
#define DATAR 0x100        /*data ready bit in status word*/
#define TRUE 1
#define FALSE 0
#define VIDEO 0x10         /*ROM BIOS video int number*/
union REGS inregs, outregs;   /*for ROM BIOS calls*/
int far *fptr;
int row, col;

main()
{
char ch, receive();
int j;
init();
clscr();                /*clear screen for putscr()*/

   while(TRUE)  {
      if(kbhit())  {    /*if key pressed*/
         ch = getch();
         send(ch);            /*send char to serial port*/
      }
      if(ready())  {          /*if serial port ready*/
         ch = receive();      /*get char*/
         putscr(ch);
      }
   }
}

/* clscr() */     /*clear the screen*/
clscr()
{
int addr;

   fptr = (int far *) 0xB0000000;
   for(addr = 0; addr < 2000; addr++)
      *(fptr + addr) = 0x0700;
   row = 0; col = 0;          /*initialize for putscr()*/
   inregs.h.ah = 2;           /*'set cursor position' service*/
   inregs.h.dh = row;
   inregs.h.dl = col;
   inregs.h.bh = 0;           /*page number*/
   int86(VIDEO, &inregs, &outregs);
}

/* putscr() */    /* puts char on screen at row, col */
putscr(ch)
char ch;
{
int j;

   switch(ch)  {
      case(0x0A):             /*if return*/
         col = 0;             /*start at beginning of line*/
         break;
      case(0x0D):             /*if linefeed*/
         ++row;               /*go to next row*/
         break;
      case(0x08):             /*if backspace*/
         if(col > 0)
            --col;            /*backspace cursor*/
         *(fptr + col + row*80) = 0x0700;  /*print space*/
         break;
      default:
         *(fptr + col + row*80) = (int)ch | 0x0700;
         ++col;               /*display & go to next col*/
   }
   if(col >= 80)  {           /*if past end of row*/
      col = 0;                /*do carriage return and*/
      ++row;                  /*linefeed*/
   }
   if(row >= 25)              /*if below bottom row*/
      row = 0;                /*start at top*/
   if(col == 0)               /*if at beginning of line*/
      for(j = 0; j < 80; j++)    /*clear line*/
         *(fptr + j + row*80) = 0x0700;
   inregs.h.ah = 2;           /* 'set cursor position' service*/
   inregs.h.dh = row;
   inregs.h.dl = col;
   inregs.h.bh = 0;           /*page number*/
   int86(VIDEO, &inregs, &outregs);
}

/* init() */      /* initialize serial port */
init(conf)
char conf;        /*addr of configuration code*/
{
   inregs.h.ah = 0;        /* 'initialize port' service */
   inregs.x.dx = COM;      /*port number*/
   inregs.h.al = CONF;     /*configuration*/
   int86(RS232, &inregs, &outregs);
}

/* send() */      /* send char to serial port */
send(c)
char c;
{
   inregs.h.ah = 1;        /* 'send char' service */
   inregs.x.dx = COM;      /*port number*/
   inregs.h.al = c;        /*character*/
   int86(RS232, &inregs, &outregs);
}

/* ready */    /* get serial port ready status */
ready()
{
   inregs.h.ah = 3;        /* 'get status' service */
   inregs.x.dx = COM;      /*port number*/
   int86(RS232, &inregs, &outregs);
   return(outregs.x.ax & DATAR);    /*return 'data ready' status*/
}

/* receive */     /* get character from serial port */
char receive()
{
   inregs.h.ah = 2;        /* 'receive char' service */
   inregs.x.dx = COM;      /*port number*/
   int86(RS232, &inregs, &outregs);
   return(outregs.h.al & 0x7F);     /*return character*/
                                    /* (AND off 8th bit*/
}

