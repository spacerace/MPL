/* Terminal Emulator    (cterm.c)
 *      Jim Kyle, 1987
 *
 *      Uses files CH1, CH1A, and CH2 for MASM support...
 */

#include <stdio.h>
#include <conio.h>                      /* special console i/o      */
#include <stdlib.h>                     /* misc definitions         */
#include <dos.h>                        /* defines intdos()         */
#include <string.h>
#define BRK  'C'-'@'                    /* control characters       */
#define ESC  '['-'@'
#define XON  'Q'-'@'
#define XOFF 'S'-'@'

#define True  1
#define False  0

#define Is_Function_Key(C) ( (C) == ESC )

static char capbfr [ 4096 ];            /* capture buffer           */
static int wh,
     ws;

static int I,
    waitchr = 0,
    vflag = False,
    capbp,
    capbc,
    Ch,
    Want_7_Bit = True,
    ESC_Seq_State = 0;        /* escape sequence state variable     */

int _cx ,
     _cy,
     _atr = 0x07,                       /* white on black           */
     _pag = 0,
     oldtop = 0,
     oldbot = 0x184f;

FILE * in_file = NULL;         /* start with keyboard input         */
FILE * cap_file = NULL;

#include "cterm.h"             /* external declarations, etc.       */

int Wants_To_Abort ()          /* checks for interrupt of script    */
{ return broke ();
}
void

main ( argc, argv ) int argc ; /* main routine                      */
 char * argv [];
{ char * cp,
     * addext ();
  if ( argc > 1 )                /* check for script filename       */
    in_file = fopen ( addext ( argv [ 1 ], ".SCR" ), "r" );
  if ( argc > 2 )                /* check for capture filename      */
    cap_file = fopen ( addext ( argv [ 2 ], ".CAP" ), "w" );
  set_int ();                    /* install CH1 module              */
  Set_Vid ();                    /* get video setup                 */
  cls ();                        /* clear the screen                */
  cputs ( "Terminal Emulator" ); /* tell who's working              */
  cputs ( "\r\n< ESC for local commands >\r\n\n" );
  Want_7_Bit = True;
  ESC_Seq_State = 0;
  Init_Comm ();                  /* set up drivers, etc.            */
  while ( 1 )                           /* main loop                */
    { if (( Ch = kb_file ()) > 0 )      /* check local              */
        { if ( Is_Function_Key ( Ch ))
            { if ( docmd () < 0 )       /* command                  */
                break;
            }
          else
            Send_Byte ( Ch & 0x7F );    /* else send it             */
        }
      if (( Ch = Read_Modem ()) >= 0 )  /* check remote             */
        { if ( Want_7_Bit )
            Ch &= 0x7F;                 /* trim off high bit        */
          switch ( ESC_Seq_State )      /* state machine            */
            {
            case 0 :                    /* no Esc sequence          */
              switch ( Ch )
                {
                case ESC  :             /* Esc char received        */
                  ESC_Seq_State = 1;
                  break;

                default :
                  if ( Ch == waitchr )  /* wait if required         */
                    waitchr = 0;
                  if ( Ch == 12 )       /* clear screen on FF       */
                    cls ();
                  else
                    if ( Ch != 127 )    /* ignore rubouts           */
                      { putchx ( (char) Ch );  /* handle all others */
                        put_cap ( (char) Ch );
                      }
                }
              break;

            case 1 : /* ESC -- process any escape sequences here    */
              switch ( Ch )
                {
                case 'A' :              /* VT52 up                  */
                  ;                     /* nothing but stubs here   */
                  ESC_Seq_State = 0;
                  break;

                case 'B' :              /* VT52 down                */
                  ;
                  ESC_Seq_State = 0;
                  break;

                case 'C' :              /* VT52 left                */
                  ;
                  ESC_Seq_State = 0;
                  break;

                case 'D' :              /* VT52 right               */
                  ;
                  ESC_Seq_State = 0;
                  break;

                case 'E' :              /* VT52 Erase CRT           */
                  cls ();               /* actually do this one     */
                  ESC_Seq_State = 0;
                  break;

                case 'H' :              /* VT52 home cursor         */
                  locate ( 0, 0 );
                  ESC_Seq_State = 0;
                  break;

                case 'j' :              /* VT52 Erase to EOS        */
                  deos ();
                  ESC_Seq_State = 0;
                  break;

                case '[' :      /* ANSI.SYS - VT100 sequence        */
                  ESC_Seq_State = 2;
                  break;

                default :
                  putchx ( ESC );       /* pass thru all others     */
                  putchx ( (char) Ch );
                  ESC_Seq_State = 0;
                }
              break;

            case 2 :                    /* ANSI 3.64 decoder        */
              ESC_Seq_State = 0;        /* not implemented          */
            }
        }
      if ( broke ())                    /* check CH1A handlers      */
        { cputs ( "\r\n***BREAK***\r\n" );
          break;
        }
    }                                   /* end of main loop         */
  if ( cap_file )                       /* save any capture         */
    cap_flush ();
  Term_Comm ();                         /* restore when done        */
  rst_int ();                           /* restore break handlers   */
  exit ( 0 );                           /* be nice to MS-DOS        */
}

docmd ()                                /* local command shell      */
{ FILE * getfil ();
  int wp;
  wp = True;
  if ( ! in_file || vflag )
    cputs ( "\r\n\tCommand: " );        /* ask for command          */
  else
    wp = False;
  Ch = toupper ( kbd_wait ());          /* get response             */
  if ( wp )
    putchx ( (char) Ch );
  switch  ( Ch )                         /* and act on it           */
    {
    case 'S' :
      if ( wp )
        cputs ( "low speed\r\n" );
      Set_Baud ( 300 );
      break;

    case 'D' :
      if ( wp )
        cputs ( "elay (1-9 sec): " );
      Ch = kbd_wait ();
      if ( wp )
        putchx ( (char) Ch );
      Delay ( 1000 * ( Ch - '0' ));
      if ( wp )
        putchx ( '\n' );
      break;

    case 'E' :
      if ( wp )
        cputs ( "ven Parity\r\n" );
      Set_Parity ( 2 );
      break;

    case 'F' :
      if ( wp )
        cputs ( "ast speed\r\n" );
      Set_Baud ( 1200 );
      break;

    case 'H' :
      if ( wp )
        { cputs ( "\r\n\tVALID COMMANDS:\r\n" );
          cputs ( "\tD = delay 0-9 seconds.\r\n" );
          cputs ( "\tE = even parity.\r\n" );
          cputs ( "\tF = (fast) 1200-baud.\r\n" );
          cputs ( "\tN = no parity.\r\n" );
          cputs ( "\tO = odd parity.\r\n" );
          cputs ( "\tQ = quit, return to DOS.\r\n" );
          cputs ( "\tR = reset modem.\r\n" );
          cputs ( "\tS = (slow) 300-baud.\r\n" );
          cputs ( "\tU = use script file.\r\n" );
          cputs ( "\tV = verify file input.\r\n" );
          cputs ( "\tW = wait for char." );
        }
      break;

    case 'N' :
      if ( wp )
        cputs ( "o Parity\r\n" );
      Set_Parity ( 1 );
      break;

    case 'O' :
      if ( wp )
        cputs ( "dd Parity\r\n" );
      Set_Parity ( 3 );
      break;

    case 'R' :
      if ( wp )
        cputs ( "ESET Comm Port\r\n" );
      Init_Comm ();
      break;

    case 'Q' :
      if ( wp )
        cputs ( " = QUIT Command\r\n" );
      Ch = ( - 1 );
      break;

    case 'U' :
      if ( in_file && ! vflag )
        putchx ( 'U' );
      cputs ( "se file: " );
      getfil ();
      cputs ( "File " );
      cputs ( in_file ? "Open\r\n" : "Bad\r\n" );
      waitchr = 0;
      break;

    case 'V' :
      if ( wp )
        { cputs ( "erify flag toggled " );
          cputs ( vflag ? "OFF\r\n" : "ON\r\n" );
        }
      vflag = vflag ? False : True;
      break;

    case 'W' :
      if ( wp )
        cputs ( "ait for: <" );
      waitchr = kbd_wait ();
      if ( waitchr == ' ' )
        waitchr = 0;
      if ( wp )
        { if ( waitchr )
            putchx ( (char) waitchr );
          else
            cputs ( "no wait" );
          cputs ( ">\r\n" );
        }
      break;

    default :
      if ( wp )
        { cputs ( "Don't know " );
          putchx ( (char) Ch );
          cputs ( "\r\nUse 'H' command for Help.\r\n" );
        }
      Ch = '?';
    }
  if ( wp )                             /* if window open...        */
    { cputs ( "\r\n[any key]\r" );
      while ( Read_Keyboard () == EOF ) /* wait for response        */
        ;
    }
  return Ch ;
}

kbd_wait ()                             /* wait for input           */
{ int c ;
  while (( c = kb_file ()) == ( - 1 ))
    ;
  return c & 255;
}

kb_file ()                              /* input from kb or file    */
{ int c ;
  if ( in_file )                        /* USING SCRIPT             */
    { c = Wants_To_Abort ();            /* use first as flag        */
      if ( waitchr && ! c )
        c = ( - 1 );                    /* then for char            */
      else
        if ( c || ( c = getc ( in_file )) == EOF || c == 26 )
          { fclose ( in_file );
            cputs ( "\r\nScript File Closed\r\n" );
            in_file = NULL;
            waitchr = 0;
            c = ( - 1 );
          }
      else
        if ( c == '\n' )                /* ignore LF's in file      */
          c = ( - 1 );
      if ( c == '\\' )                  /* process Esc sequence     */
        c = esc ();
      if ( vflag && c != ( - 1 ))       /* verify file char         */
        { putchx ( '{' );
          putchx ( (char) c );
          putchx ( '}' );
        }
    }
  else                                  /* USING CONSOLE            */
    c = Read_Keyboard ();               /* if not using file        */
  return ( c );
}

esc ()                                  /* script translator        */
{ int c ;
  c = getc ( in_file );                 /* control chars in file    */
  switch ( toupper ( c ))
    {
    case 'E' :
      c = ESC;
      break;

    case 'N' :
      c = '\n';
      break;

    case 'R' :
      c = '\r';
      break;

    case 'T' :
      c = '\t';
      break;

    case '^' :
      c = getc ( in_file ) & 31;
      break;
    }
  return ( c );
}

FILE * getfil ()
{ char fnm [ 20 ];
  getnam ( fnm, 15 );                   /* get the name             */
  if ( ! ( strchr ( fnm, '.' )))
    strcat ( fnm, ".SCR" );
  return ( in_file = fopen ( fnm, "r" ));
}

void getnam ( b, s ) char * b;          /* take input to buffer     */
 int s ;
{ while ( s -- > 0 )
    { if (( * b = kbd_wait ()) != '\r' )
        putchx ( * b ++ );
      else
        break ;
    }
  putchx ( '\n' );
  * b = 0;
}

char * addext ( b,                      /* add default EXTension    */
     e ) char * b,
     * e;
{ static char bfr [ 20 ];
  if ( strchr ( b, '.' ))
    return ( b );
  strcpy ( bfr, b );
  strcat ( bfr, e );
  return ( bfr );
}

void put_cap ( c ) char c ;
{ if ( cap_file && c != 13 )            /* strip out CR's           */
    fputc ( c, cap_file );              /* use MS-DOS buffering     */
}

void cap_flush ()                       /* end Capture mode         */
{ if ( cap_file )
    { fclose ( cap_file );
      cap_file = NULL;
      cputs ( "\r\nCapture file closed\r\n" );
    }
}

/*      TIMER SUPPORT STUFF (IBMPC/MSDOS)                           */
static long timr;                       /* timeout register         */

static union REGS rgv ;

long getmr ()
{ long now ;                            /* msec since midnite       */
  rgv.x.ax = 0x2c00;
  intdos ( & rgv, & rgv );
  now = rgv.h.ch;                       /* hours                    */
  now *= 60L;                           /* to minutes               */
  now += rgv.h.cl;                      /* plus min                 */
  now *= 60L;                           /* to seconds               */
  now += rgv.h.dh;                      /* plus sec                 */
  now *= 100L;                          /* to 1/100                 */
  now += rgv.h.dl;                      /* plus 1/100               */
  return ( 10L * now );                 /* msec value               */
}

void Delay ( n ) int n ;                /* sleep for n msec         */
{ long wakeup ;
  wakeup = getmr () + ( long ) n;       /* wakeup time              */
  while ( getmr () < wakeup )
    ;                                   /* now sleep                */
}

void Start_Timer ( n ) int n ;          /* set timeout for n sec    */
{ timr = getmr () + ( long ) n * 1000L;
}

Timer_Expired ()        /* if timeout return 1 else return 0        */
{ return ( getmr () > timr );
}

Set_Vid ()
{ _i_v ();                              /* initialize video         */
  return 0;
}

void locate ( row, col ) int row ,
     col;
{ _cy = row % 25;
  _cx = col % 80;
  _wrpos ( row, col );                  /* use ML from CH2.ASM      */
}

void deol ()
{ _deol ();                             /* use ML from CH2.ASM      */
}

void deos ()
{ deol ();
  if ( _cy < 24 )                       /* if not last, clear       */
    { rgv.x.ax = 0x0600;
      rgv.x.bx = ( _atr << 8 );
      rgv.x.cx = ( _cy + 1 ) << 8;
      rgv.x.dx = 0x184F;
      int86 ( 0x10, & rgv, & rgv );
    }
  locate ( _cy, _cx );
}

void cls ()
{ _cls ();                              /* use ML                   */
}

void cursor ( yn ) int yn ;
{ rgv.x.cx = yn ? 0x0607 : 0x2607;      /* ON/OFF                   */
  rgv.x.ax = 0x0100;
  int86 ( 0x10, & rgv, & rgv );
}

void revvid ( yn ) int yn ;
{ if ( yn )
    _atr = _color ( 8, 7 );             /* black on white           */
  else
    _atr = _color ( 15, 0 );            /* white on black           */
}

putchx ( c ) char c ;                   /* put char to CRT          */
{ if ( c == '\n' )
    putch ( '\r' );
  putch ( c );
  return c ;
}

Read_Keyboard ()                    /* get keyboard character
                                       returns -1 if none present   */
{ int c ;
  if ( kbhit ())                        /* no char at all           */
    return ( getch ());
  return ( EOF );
}

/*      MODEM SUPPORT                   */
static char mparm,
     wrk [ 80 ];

void Init_Comm ()               /* initialize comm port stuff       */
{ static int ft = 0;                    /* firstime flag            */
  if ( ft ++ == 0 )
    i_m ();
  Set_Parity ( 1 );                     /* 8,N,1                    */
  Set_Baud ( 1200 );                    /* 1200 baud                */
}

#define B1200 0x80                      /* baudrate codes           */
#define B300 0x40

Set_Baud ( n ) int n ;                  /* n is baud rate           */
{ if ( n == 300 )
    mparm = ( mparm & 0x1F ) + B300;
  else
    if ( n == 1200 )
      mparm = ( mparm & 0x1F ) + B1200;
  else
    return 0;                           /* invalid speed            */
  sprintf ( wrk, "Baud rate = %d\r\n", n );
  cputs ( wrk );
  set_mdm ( mparm );
  return n ;
}

#define PAREVN 0x18                     /* MCR bits for commands    */
#define PARODD 0x10
#define PAROFF 0x00
#define STOP2 0x40
#define WORD8 0x03
#define WORD7 0x02
#define WORD6 0x01

Set_Parity ( n ) int n ;                /* n is parity code         */
{ static int mmode;
  if ( n == 1 )
    mmode = ( WORD8 | PAROFF );         /* off                      */
  else
    if ( n == 2 )
      mmode = ( WORD7 | PAREVN );       /* on and even              */
  else
    if ( n == 3 )
      mmode = ( WORD7 | PARODD );       /* on and odd               */
  else
    return 0;                           /* invalid code             */
  mparm = ( mparm & 0xE0 ) + mmode;
  sprintf ( wrk, "Parity is %s\r\n", ( n == 1 ? "OFF" :
                                     ( n == 2 ? "EVEN" : "ODD" )));
  cputs ( wrk );
  set_mdm ( mparm );
  return n ;
}

Write_Modem ( c ) char c ;              /* return 1 if ok, else 0   */
{ wrtmdm ( c );
  return ( 1 );                         /* never any error          */
}

Read_Modem ()
{ return ( rdmdm ());                   /* from int bfr             */
}

void Term_Comm ()               /* uninstall comm port drivers      */
{ u_m ();
}

/* end of cterm.c */
