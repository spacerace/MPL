/* cdvutl.c - COMDVR Utility
 *     Jim Kyle - 1987
 *     for use with COMDVR.SYS Device Driver
 */

#include <stdio.h>                 /* i/o definitions      */
#include <conio.h>                 /* special console i/o  */
#include <stdlib.h>                /* misc definitions     */
#include <dos.h>                   /* defines intdos()     */

/*       the following define the driver status bits       */

#define HWINT 0x0800           /* MCR, first word, HW Ints gated */
#define o_DTR 0x0200           /* MCR, first word, output DTR    */
#define o_RTS 0x0100           /* MCR, first word, output RTS    */

#define m_PG  0x0010           /* LCR, first word, parity ON     */
#define m_PE  0x0008           /* LCR, first word, parity EVEN   */
#define m_XS  0x0004           /* LCR, first word, 2 stop bits   */
#define m_WL  0x0003           /* LCR, first word, wordlen mask  */

#define i_CD  0x8000           /* MSR, 2nd word, Carrier Detect  */
#define i_RI  0x4000           /* MSR, 2nd word, Ring Indicator  */
#define i_DSR 0x2000           /* MSR, 2nd word, Data Set Ready  */
#define i_CTS 0x1000           /* MSR, 2nd word, Clear to Send   */

#define l_SRE 0x0040           /* LSR, 2nd word, Xmtr SR Empty   */
#define l_HRE 0x0020           /* LSR, 2nd word, Xmtr HR Empty   */
#define l_BRK 0x0010           /* LSR, 2nd word, Break Received  */
#define l_ER1 0x0008           /* LSR, 2nd word, FrmErr          */
#define l_ER2 0x0004           /* LSR, 2nd word, ParErr          */
#define l_ER3 0x0002           /* LSR, 2nd word, OveRun          */
#define l_RRF 0x0001           /* LSR, 2nd word, Rcvr DR Full    */

/*           now define CLS string for ANSI.SYS            */
#define CLS    "\033[2J"

FILE * dvp;
union REGS rvs;
int iobf [ 5 ];

main ()
{ cputs ( "\nCDVUTL - COMDVR Utility Version 1.0 - 1987\n" );
  disp ();                         /* do dispatch loop           */
}

disp ()                            /* dispatcher; infinite loop  */
{ int c,
    u;
  u = 1;
  while ( 1 )
    { cputs ( "\r\n\tCommand (? for help): " );
      switch ( tolower ( c = getche ()))   /* dispatch           */
        {
        case '1' :                 /* select port 1              */
          fclose ( dvp );
          dvp = fopen ( "ASY1", "rb+" );
             u = 1;
          break;

        case '2' :                 /* select port 2              */
          fclose ( dvp );
          dvp = fopen ( "ASY2", "rb+" );
          u = 2;
          break;

        case 'b' :                 /* set baud rate              */
          if ( iobf [ 4 ] == 300 )
            iobf [ 4 ] = 1200;
          else
            if ( iobf [ 4 ] == 1200 )
              iobf [ 4 ] = 2400;
          else
            if ( iobf [ 4 ] == 2400 )
              iobf [ 4 ] = 9600;
          else
            iobf [ 4 ] = 300;
          iocwr ();
          break;

        case 'e' :                 /* set parity even            */
          iobf [ 0 ] |= ( m_PG + m_PE );
          iocwr ();
          break;

        case 'f' :                 /* toggle flow control        */
          if ( iobf [ 3 ] == 1 )
            iobf [ 3 ] = 2;
          else
            if ( iobf [ 3 ] == 2 )
              iobf [ 3 ] = 4;
          else
            if ( iobf [ 3 ] == 4 )
              iobf [ 3 ] = 0;
          else
            iobf [ 3 ] = 1;
          iocwr ();
          break;

        case 'i' :                 /* initialize MCR/LCR to 8N1 : */
          iobf [ 0 ] = ( HWINT + o_DTR + o_RTS + m_WL );
          iocwr ();
          break;

        case '?' :                 /* this help list              */
          cputs ( CLS )            /* clear the display           */
          center ( "COMMAND LIST \n" );
center ( "1 = select port 1           L = toggle word LENGTH  " );
center ( "2 = select port 2           N = set parity to NONE  " );
center ( "B = set BAUD rate           O = set parity to ODD   " );
center ( "E = set parity to EVEN      R = toggle error REPORTS" );
center ( "F = toggle FLOW control     S = toggle STOP bits    " );
center ( "I = INITIALIZE ints, etc.   Q = QUIT                " );
         continue;

        case 'l' :                  /* toggle word length         */
          iobf [ 0 ] ^= 1;
          iocwr ();
          break;

        case 'n' :                  /* set parity off             */
          iobf [ 0 ] &=~ ( m_PG + m_PE );
          iocwr ();
          break;

        case 'o' :                  /* set parity odd             */
          iobf [ 0 ] |= m_PG;
          iobf [ 0 ] &=~ m_PE;
          iocwr ();
          break;

        case 'r' :                  /* toggle error reports       */
          iobf [ 2 ] ^= 1;
          iocwr ();
          break;

        case 's' :                  /* toggle stop bits           */
          iobf [ 0 ] ^= m_XS;
          iocwr ();
          break;

        case 'q' :
          fclose ( dvp );
          exit ( 0 );               /* break the loop, get out    */
        }
      cputs ( CLS );                /* clear the display          */
      center ( "CURRENT COMDVR STATUS" );
      report ( u, dvp );            /* report current status      */
    }
}

center ( s ) char * s;              /* centers a string on CRT    */
{ int i ;
  for ( i = 80 - strlen ( s ); i > 0; i -= 2 )
    putch ( ' ' );
  cputs ( s );
  cputs ( "\r\n" );
}

iocwr ()                            /* IOCTL Write to COMDVR      */
{ rvs . x . ax = 0x4403;
  rvs . x . bx = fileno ( dvp );
  rvs . x . cx = 10;
  rvs . x . dx = ( int ) iobf;
  intdos ( & rvs, & rvs );
}

char * onoff ( x ) int x ;
{ return ( x ? " ON" : " OFF" );
}

report ( unit ) int unit ;
{ char temp [ 80 ];
  rvs . x . ax = 0x4402;
  rvs . x . bx = fileno ( dvp );
  rvs . x . cx = 10;
  rvs . x . dx = ( int ) iobf;
  intdos ( & rvs, & rvs );         /* use IOCTL Read to get data */
  sprintf ( temp, "\nDevice ASY%d\t%d BPS, %d-c-%c\r\n\n",
           unit, iobf [ 4 ],           /* baud rate              */
           5 + ( iobf [ 0 ] & m_WL ),  /* word length            */
           ( iobf [ 0 ] & m_PG ?
             ( iobf [ 0 ] & m_PE ? 'E' : 'O' ) : 'N' ),
           ( iobf [ 0 ] & m_XS ? '2' : '1' )); /* stop bits      */
  cputs ( temp );

  cputs ( "Hardware Interrupts are" );
  cputs ( onoff ( iobf [ 0 ] & HWINT ));
  cputs ( ", Data Terminal Rdy" );
  cputs ( onoff ( iobf [ 0 ] & o_DTR ));
  cputs ( ", Rqst To Send" );
  cputs ( onoff ( iobf [ 0 ] & o_RTS ));
  cputs ( ".\r\n" );

  cputs ( "Carrier Detect" );
  cputs ( onoff ( iobf [ 1 ] & i_CD ));
  cputs ( ", Data Set Rdy" );
  cputs ( onoff ( iobf [ 1 ] & i_DSR ));
  cputs ( ", Clear to Send" );
  cputs ( onoff ( iobf [ 1 ] & i_CTS ));
  cputs ( ", Ring Indicator" );
  cputs ( onoff ( iobf [ 1 ] & i_RI ));
  cputs ( ".\r\n" );

  cputs ( l_SRE & iobf [ 1 ] ? "Xmtr SR Empty, " : "" );
  cputs ( l_HRE & iobf [ 1 ] ? "Xmtr HR Empty, " : "" );
  cputs ( l_BRK & iobf [ 1 ] ? "Break Received, " : "" );
  cputs ( l_ER1 & iobf [ 1 ] ? "Framing Error, " : "" );
  cputs ( l_ER2 & iobf [ 1 ] ? "Parity Error, " : "" );
  cputs ( l_ER3 & iobf [ 1 ] ? "Overrun Error, " : "" );
  cputs ( l_RRF & iobf [ 1 ] ? "Rcvr DR Full, " : "" );
  cputs ( "\b\b.\r\n" );

  cputs ( "Reception errors " );
  if ( iobf [ 2 ] == 1 )
    cputs ( "are encoded as graphics in buffer" );
  else
    cputs ( "set failure flag" );
  cputs ( ".\r\n" );

  cputs ( "Outgoing Flow Control " );
  if ( iobf [ 3 ] & 4 )
    cputs ( "by XON and XOFF" );
  else
    if ( iobf [ 3 ] & 2 )
      cputs ( "by RTS and CTS" );
  else
    if ( iobf [ 3 ] & 1 )
      cputs ( "by DTR and DSR" );
  else
    cputs ( "disabled" );
  cputs ( ".\r\n" );
}

/*end of cdvutl.c */
