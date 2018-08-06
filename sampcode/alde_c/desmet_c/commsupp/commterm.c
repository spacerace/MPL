/*
  commterm.c - a stupid communications program demonstrating the use of
               commsupp.c and commasm.a

  Tom Poindexter, March 1984

*/

#include "commsupp.h"
/* commsupp.h    - commsupp declares and defines */

/*
  init_com   - initialize comm port, interrupts, baud, data, parity, stop, buf
  uninit_com - disable interrupts, flushes buffer
  send_brk   - send break signal
  set_xoff   - enable XON, XOFF protocol
  recd_xoff  - return true if XOFF received, reset recd_xoff flag
  sent_xoff  - return true if XOFF sent, reset sent_xoff flag
  inp_cnt    - returns the number of unread bytes in receive buffer
  inp_char   - return one byte from buffer
  inp_strn   - transfer n bytes from buffer to string
  outp_char  - send one byte
  outp_strn  - send n bytes of a string, up to end of string
  com_stat   - return line status (high byte) and modem status (low byte)
  on_com     - specify a routine to execute when a character is received
*/

/* declare entry points to commsupp routines */

int  init_com();   /* init_com(port,baud,parity,data,stop,buff_len); */
int  uninit_com(); /* uninit_com(port,lv_open);  */
int  send_brk();   /* send_brk(port);            */
int  set_xoff();   /* set_xoff(port,xoff_state); */
int  recd_xoff();  /* recd_xoff(port);           */
int  sent_xoff();  /* sent_xoff(port);           */
int  inp_cnt();    /* inp_cnt(port);             */
int  inp_char();   /* inp_char(port);            */
int  inp_strn();   /* inp_strn(port,&str,cnt);   */
int  outp_char();  /* outp_char(port,c);         */
int  outp_strn();  /* outp_strn(port,&str,cnt);  */
int  com_stat();   /* com_stat(port);            */
int  on_com();     /* on_com(port,routine);      */

/* define for values returned by routines */
#define ERROR     -1
#define TRUE      1
#define FALSE     0

/* defines for parity (init_com) */
#define PARITY_NONE  0
#define PARITY_ODD   1
#define PARITY_EVEN  2
#define PARITY_MARK  3
#define PARITY_SPACE 4

/* defines for comm errors - mask using com_stat value */
#define OVER_RUN    0x0200
#define PARITY_ERR  0x0400
#define FRAME_ERR   0x0800
#define BREAK_IND   0x1000

/* defines for modem conditions - mask using com_stat value */
#define CLEAR_SEND  0x0010
#define DATASET_RDY 0x0020
#define RING_IND    0x0040
#define CARRIER     0x0080

/* end of commsupp.h */


#define ALT_H  0x2300  /* help  */
#define ALT_B  0x3000  /* break */
#define ALT_X  0x2d00  /* exit  */
#define ALT_E  0x1200  /* echo  */
#define ALT_S  0x1f00  /* pause */
#define ALT_Q  0x1000  /* resume*/
#define PGUP   0x4900  /* upload */
#define PGDN   0x5100  /* download */
#define NUL    0
#define BS     8
#define LF     10
#define CR     13
#define XON    17
#define XOFF   19
#define DELAY  5000

int max;
unsigned total,min_sp;

#include <stdio.h>

main ()

{
  int port,baud,parity,data,stop,buff_len;
  int c,e,x,i,n,x_state,counter;
  int bytecount();
  FILE f;
  char strn[128];


  puts("\n\n\ncommterm\n");

  puts("\nComm port (1 or 2)? ");
  gets(strn);
  port = atoi(strn);

  puts("\nBaud rate? ");
  gets(strn);
  baud = atoi(strn);

  puts("\nParity (0=none, 1=odd, 2=even, 3=mark, 4=space)? ");
  gets(strn);
  parity = atoi(strn);

  puts("\nData length (5 - 8)? ");
  gets(strn);
  data = atoi(strn);

  puts("\nStop bits (1 or 2)? ");
  gets(strn);
  stop = atoi(strn);

  puts("\nBuffer length? ");
  gets(strn);
  buff_len = atoi(strn);

  puts("\nXON/XOFF enabled (y or n)? ");
  gets(strn);
  x_state = (tolower(strn[0]) == 'y') ? TRUE : FALSE;

  if (init_com(port,baud,parity,data,stop,buff_len) == ERROR)
   {
    putchar(7);
    puts("\n\nSorry, you screwed up somewhere along the line.\n\n");
    exit();
   }
  else
   {
    set_xoff(port,x_state);
    on_com(port,bytecount);
   }

  puts("\nEnter modem initialization string: ");
  gets(strn);
  outp_strn(port,strn,128);
  outp_char(port,CR);

  puts("\n\nPress ALT-H for help on special keys.\n\n");
  total = max = c = e = n = 0;
  min_sp = 0xffff;

  while (c != ALT_X)
   {

    termin(port,1);

    if (com_stat(port) & BREAK_IND)
      puts("\n\n**B R E A K**\n\n");

    c = key_in();
    if (c > 0)
     {
      switch (c)
       {
        case ALT_H:
         puts("\n\ncommterm help\n\n");
         puts("press.........to\n");
         puts("Alt-E.........echo key strokes\n");
         puts("Alt-B.........send break signal\n");
         puts("Alt-H.........help on keys\n");
         puts("Alt-X.........exit program\n");
         puts("Alt-S.........pause (XOFF)\n");
         puts("Alt-Q.........resume (XON)\n");
         puts("PgUp..........upload a file\n");
         puts("PgDn..........download to a file\n");
         puts("\n\ncontinue...\n\n");
         break;

        case ALT_B:
         send_brk(port);
         break;

        case ALT_E:
         e = !e;
         break;

        case PGUP:
         puts("\n\nUpload what file? ");
         gets(strn);
         f=fopen(strn,"r");
         if (f == FALSE)
          {
           puts("\nERROR - file ");
           puts(strn);
           puts(" not found.....\n");
          }
         else
          {
           puts("\nUploading ");
           puts(strn);
           puts(", press PgUp key to abort.....\n\n");
           while (fgets(strn,128,f) != FALSE && key_in() != PGUP)
            {
             termin(port,0);
             for (i = 0; i < 128 && strn[i] != '\0'; i++)
               {
                if (x_state && recd_xoff(port) == TRUE)
                 {
                  puts("\nXOFF received.  Waiting for XON or press any key to continue.\n");
                  while (inp_char(port) != XON && csts() == FALSE)
                    ;
                 }
                outp_char(port,strn[i]);
                termin(port,0);
               }
              outp_char(port,CR);
            }
           fclose(f);
           puts("\nUpload complete.\n\n");
          }
         break;

        case PGDN:
         puts("\n\nDownload to what file? ");
         gets(strn);
         if ((f=fopen(strn,"r")) == FALSE)
          {
           if (strlen(strn) > 0)
            {
             puts("\nCreating ");
             puts(strn);
             puts("\n\n");
             f = creat(strn);
            }
           else
             f = 0;
          }
         else
          {
           fclose(f);
           puts("\nFile ");
           puts(strn);
           puts(" exists, press 'e' to erase file or 'a' to add to it: ");
           i = getchar();
           if (tolower(i) == 'e')
             f = creat(strn);
           else
             f = fopen(strn,"a");
          }
         puts("\n\nDownloading, press PgDn again to close file.\n\n");
         counter = DELAY;
         while (((c=key_in()) != PGDN) && (f != 0))
          {
           if (c != 0)
             conout(c,e,port);
           if (inp_cnt(port) > max) max = inp_cnt(port);
           i = inp_cnt(port);
           i = (i <= 127) ? i : 127;
           inp_strn(port,strn,i);
           strn[i] = '\0';
           fputs(strn,f);
           for (n = 0; strn[n] != '\0'; n++)
             switch (strn[n])
              {
               case BS:
                putchar('\b');
                break;
               case CR:
                putchar(CR);
                putchar(LF);
                break;
               case LF:
                break;
               case NUL:
                break;
               default:
                putchar(strn[n]);
              }
           if (x_state && sent_xoff(port) == TRUE)
            {
             puts("\nXOFF sent. No test delay till buffer empties to 25%.\n");
             outp_char(port,XOFF);
             counter = 0;
            }
           if (x_state && counter == 0)
            {
             if (inp_cnt(port) < (buff_len * 1/4))
               {
                puts("\nXON sent\n");
                outp_char(port,XON);
                counter = DELAY;
               }
            }
           else
             if (x_state)
               /* dummy loop to test the XON/XOFF flow control */
               for(i=0; i<counter; i++)
                 ;
          }
         fclose(f);
         puts("\nDownload complete.\n\n");

         break;

        default:
         conout(c,e,port);
         break;
        }
      }
   }

  puts("\n\nExiting..\n");
  puts("\nLeave port initialized (y or n)? ");
  gets(strn);
  x = (tolower(strn[0]) == 'y') ? TRUE : FALSE;

  uninit_com(port,x);

  printf("\nThe buffer high water mark reached %d of a buffer length of %d",
         max,buff_len);

  printf("\nThe total number of characters received was %u",
         total);

  printf("\nThe smallest stack space available was %u",
         min_sp);

}  /* end of commterm.c */

bytecount () /* on comm routine */
{
 int m;

 m = _showsp();
 if (m < min_sp)
   min_sp = m;
 total++;
}

conout(c,e,port) /* send a character over the serial port, optionally echo */
int c,e,port;
{
  switch (c)
   {
    case ALT_S:
     /* DOS sometimes eats ^S as a pause (ala Ctrl-NumLock) */
     outp_char(port,XOFF);
     break;

    case ALT_Q:
     outp_char(port,XON);
     break;

    case CR:
     if (e)
      {
       putchar(CR);
       putchar(LF);
      }
     outp_char(port,CR);
     break;

    case BS:
     if (e)
       putchar ('\b');
     outp_char(port,BS);
     break;

    default:
     if (c < 128)
      {
       if (e)
        putchar(c);
       outp_char(port,c);
      }
     break;
    }
}



termin(port,x)  /* get characters from the serial port and print it */
int port,x;
{
  int c;

    if (x)
      if (recd_xoff(port))
        puts("<<XOFF>>");

    if (inp_cnt(port) > max) max = inp_cnt(port);
    while (inp_cnt(port))
     {
      c = inp_char(port);
      switch (c)
       {
        case NUL:
         break;

        case CR:
         putchar(CR);
         putchar(LF);
         break;

        case LF:
         break;

        case BS:
         putchar('\b');
         break;

        default:
         putchar(c);
         break;
       }
     }

}  /* end of termin */

/* ********************************************************************** */

/* key_in.c

      direct read of keyboard, including special keys.

      written to replace scr_in() in pcio.a because scr_in()
      filters most special keys, and returns char.
      key_in, returns an int value.  see tech reference manual
      or basic manual for values of different key combinations.

      key_in() returns: 0 = no key pressed
                        other = key and/or special key code

                        when value < 128 : normal ASCII
                        when value >=128 : special key


    calls: sysint

*/

#define KEYBOARD 0x16
#define GET_KEY  0x0000
#define KEY_STAT 0x0100
#define ZERO     0x0040
/* ZERO is a mask for the 8088 flag register zero bit */

key_in ()

{
 struct {int ax,bx,cx,dx,si,di,ds,es;} r;

 r.ax = r.bx = r.cx = r.dx = r.si = r.di = r.ds = r.es = 0;

 r.ax = KEY_STAT;

 if ( !(sysint (KEYBOARD, &r, &r) & ZERO))
  {
   r.ax = GET_KEY;
   sysint (KEYBOARD, &r, &r);
   return ( (r.ax & 0x00ff) ? (r.ax & 0x00ff) : (r.ax) );
  }
 else
  return (0);

} /* end of key_in.c */

