/* COMTEST.C - From page 499 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. Program tests automatic command   */
/* transmission to modem. Program works at 300 bpi. To work     */
/* at 1200 with PC or XT, add the clscr() and putscr() func-    */
/* tions from the SER1200.C program and change CONF to 0x83.    */
/****************************************************************/

#include "dos.h"              /*for union REGS */
#define COM 1                 /*serial port no. (0 = COM1, 1 = COM2)*/
#define CONF 0x43             /*300 bpi, no par, 1 stop, 8 bits*/
#define RS232 0x14            /*RS232 serial port interrupt number*/
#define DATAR 0x100           /*data ready bit in status word*/
char *comstr = "AT Z\r";      /*modem reset command*/
union REGS inregs, outregs;   /*for ROM BIOS calls*/

main()
{
char ch, receive();
int j, k;
init();

   for(j = 0; (ch = *(comstr + j)) != '\0'; j++)  {     /*send command*/
      send(ch);               /*send char to modem*/
      while(!ready())         /*wait for echo*/
         ;
      ch = receive();         /*get echo*/
      putch(ch);
   }
   do  {
      while(!ready())         /*wait for response char*/
         ;
      ch = receive();         /*get response*/
      putch(ch);
   } while(ch != 'K');        /*quit on 'K' in "OK"*/
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
