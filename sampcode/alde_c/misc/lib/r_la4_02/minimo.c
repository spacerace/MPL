/* MINIMO.C - From page 503 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. This is a full modem program,     */
/* although rather limited. I slightly changed this program.    */
/* The #define BUFSIZE is changed from 8192 to 60416 and the    */
/* check() function is called after all commands to the modem   */
/* instead of only once as it is in the book.                   */
/****************************************************************/

#include <dos.h>        /*for union REGS*/
#include <fcntl.h>      /*for system lovel I/O*/
#include <types.h>      /* " */  /*extracted from subdir \include\sys*/
#include <stat.h>       /* " */  /* " */
#define COM 1           /*serial port no. (0 = COM1, 1 = COM2)*/
#define CONF 0x83       /*1200 bps, no par, 1 stop, 8 bits*/
#define RS232 0x14      /*RS232 serial port int number*/
#define VIDEO 0x10      /*video int number*/
#define DATAR 0x100     /*data ready bit in status word*/
#define DELAY 500       /*make larger for faster computers*/
#define TRUE 1
#define FALSE 0
#define OK 0            /*modem result code*/
#define BUFSIZ 60416    /*size of disk file buffer*/
union REGS inregs, outregs;   /* for ROM BIOS calls*/
int far *fptr;          /*pointer to screen memory*/
int row, col;           /*row and col of cursor position*/
char buff[BUFSIZ];      /*disk file buffer*/
int hnd;                /*disk file handle*/
int bytes;              /*number of bytes stored in buffer*/
int saveflag = FALSE;   /*TRUE when data being saved to disk*/

main()
{
char ch, receive();

   init(CONF);          /*initialize port*/
   reset();             /*initialize modem*/
   while(TRUE)  {
      printf("\nPress  [F1] to dial a call");
      printf("\n       [F2] to await call");
      printf("\n       [F3] to resume call");
      printf("\n       [F4] save to disk");
      printf("\n       [F5] close disk file");
      printf("\n       [F10] return to function menu");
      printf("\nCommand: ");
      if(getche() == 0)  {       /*if extended code*/
         switch(getch())  {
            case 59:             /* [F1] dial & call */
               dial();
               sr();
               break;
            case 60:             /* [F2] await call */
               answ();
               sr();
               break;
            case 61:             /* [F3] resume call */
               sr();
               break;
            case 62:             /* [F4] save to disk */
               save();
               break;
            case 63:             /* [F5] close file */
               clo();
               break;
         }     /*end switch*/
      }     /*end if*/
   }     /*end while*/
}     /*end main*/

/* reset() */  /* reset modem parameters */
reset()
{

   command("AT Z\r");            /*reset modem*/
   check();                      /*check result code*/
   /***  echo on, full-duplex, send result codes, numer codes ***/
   command("AT E1 F1 Q0 V0\r");  /*initialize modem*/
   check();                      /*check result code*/
   command("AT\r");              /*reset bps rate*/
   check();                      /*check result code*/
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

/* dial() */   /* dials number */
dial()
{
char phonum[30];              /*space for phone number*/

   reset();                   /*resets modem*/
   printf("\n\nEnter phone number (example: 555-1212):  ");
   gets(phonum);
   command("AT DT ");         /*set up modem*/
   check();                      /*check result code*/
   command(phonum);           /*send number to modem*/
   check();                      /*check result code*/
   command("\r");             /*terminate command*/
   check();                      /*check result code*/
}

/* answ() */   /* sets up answer mode */
answ()
{

   reset();                   /*reset modem*/
   command("AT S0 = 1\r");    /*answer on first ring*/
   check();                      /*check result code*/
}

/* sr() */     /*send-receive, full-duplex communication*/
sr()
{
char ch, receive();

   clscr();
   while(TRUE)  {
      if(kbhit())  {                /*keyboard char ?*/
         if((ch = getch()) != 0)    /*get char from keybd*/
            send(ch);               /*send normal char*/
         else if(getch() == 68)     /*if F[10] function key*/
            return;                 /*return to menu*/
      }
      if(ready())  {             /*port has char ? */
         ch = receive();
         putscr(ch);
         if(saveflag)  {         /*if saving to disk*/
            buff[bytes++] = ch;  /*put in disk buffer*/
            if(bytes > BUFSIZ)  {      /*if buffer full*/
               printf("\n\nBuffer full.\n");
               return;           /*return to menu*/
            }
         }
      }  /*end if(ready())*/
   }  /*end while*/
}  /*end sr*/

/* save() */   /* opens disk file, sets up saveflag to write disk */
save()
{
char fname[20];

   printf("\n\nName of file to save to: ");
   gets(fname);
   if((hnd = open(fname,
               O_CREAT | O_WRONLY | O_TEXT, S_IWRITE)) < 0)
      printf("\n\nCan't open file.\n");
   else  {
      printf("\n\nFile opened. Press [F5] to close.\n");
      bytes = 0;              /*start of buffer*/
      saveflag = TRUE;        /* 'write to disk' flag */
   }
}

/* clo() */    /* writes data from buffer to file & closes file */
clo()
{

   write(hnd, buff, bytes);      /*write buffer to file*/
   close(hnd);
   saveflag = FALSE;             /*turn off flag*/
}

/* command */  /* send command string to modem */
command(comstr)
char *comstr;                    /*pointer to command*/
{
int j, k;
long int jj;
char ch;

   putch('*');                /*reassure user*/
   for(jj = 0; jj < 50*DELAY; jj++)    /*ensure last command done*/
      ;
   for(j = 0; (ch = *(comstr + j)) != '\0'; j++)  {
      send(ch);               /*send each char in turn*/
      for(k = 0; k < DELAY && !ready(); k++)    /*wait for echo*/
         ;                    /*before doing next one*/
      ch = receive();
   }
}

/* check() */  /* check that modem returns '0' for OK */
check()
{
char ch;
int j, k;

   for(j = 0; j < 10; j++)  {       /*read up to 10 chars ? */
      for(k = 0; k < DELAY && !ready(); k++)    /*wait for char*/
         ;
      ch = receive();               /*if we get an OK*/
      if(ch == OK)
         return;
   }                 /*otherwise error*/
   printf("\n\nCan't initialize modem.\n");
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

