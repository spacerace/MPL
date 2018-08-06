/* AUTODIAL.C - From page 500 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. This program dials a number that  */
/* can be inserted by editing the program. Dial with program,   */
/* pick up the phone and wait for answer. At this point, pres-  */
/* sing any key will hang up modem and let the call continue    */
/* by voice.                                                    */
/****************************************************************/

#include "dos.h"           /*initialize port*/
#define COM 1              /*serial port no. (0 = COM1, 1 = COM2)*/
#define CONF 0x43          /*300 bpi, no par, 1 stop, 8 bits*/
#define RS232 0x14         /*RS232 serial port int number*/
#define DATAR 0x100        /*data ready bit in status word*/
union REGS inregs, outregs;   /*for ROM BIOS calls*/

main()
{
   init();
   command("AT D T 883-7045;\r");      /*dial phone*/
   /*** (Semicolon after phone # returns modem to command mode) ***/
   printf("\nPress any key to hang up: \n");
   getche();               /*await char*/
   command("AT H0\r");     /*hang up*/
}

/* command() */   /* sends commands to modem */
command(comstr)
char *comstr;
{
int j, k;
char ch, receive();

   for(j = 0; (ch = *(comstr + j)) != '\0'; j++)  { /*send command*/
      send(ch);            /*send char to modem*/
      for(k = 0; k < 500 && !ready(); k++)   /*wait for echo*/
         ;
      ch = receive();            /*get echo*/
      putch(ch);
   }
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
