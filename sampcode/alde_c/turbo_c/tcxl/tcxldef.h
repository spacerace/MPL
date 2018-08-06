
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLDEF.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains miscellaneous function prototypes and         ³
   ³  definitions.                                                            ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

void            _Cdecl beep(void);
char           *_Cdecl biosver(void);
int             _Cdecl clockcal(void);
void            _Cdecl delay_(unsigned duration);
unsigned        _Cdecl expmem(void);
unsigned        _Cdecl extmem(void);
void            _Cdecl sound_(unsigned pitch,unsigned duration);
int             _Cdecl tabstop(int col,int tabwidth);
unsigned long   _Cdecl timer(void);


/*-----------------[ definitions for common control codes ]------------------*/

#define NUL         0
#define BEL         7
#define BS          8
#define HT          9
#define LF          10
#define FF          12
#define CR          13
#define ESC         27


/*------------------------[ definition of NULL ]-----------------------------*/

#ifndef NULL
#   if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#   define      NULL    0
#   else
#   define      NULL    0L
#   endif
#endif


/*-----------------------[ Boolean logic conditions ]------------------------*/

#define NO          0
#define YES         1
#define FALSE       0
#define TRUE        1
#define OFF         0
#define ON          1


/*----------------[ machine ID codes returned from machid() ]----------------*/

#define IBMPC       255                 /*  IBM PC                 */
#define IBMPCXT     254                 /*  IBM PC/XT              */
#define IBMPCJR     253                 /*  IBM PCjr               */
#define IBMPCAT     252                 /*  IBM PC/AT              */
#define IBMPCXT2    251                 /*  IBM PC/XT              */
#define IBMCONV     249                 /*  IBM PC Convertible     */
#define SPERRYPC    48                  /*  Sperry PC              */


/*-----------------------[ macro-function definitions ]-----------------------*/

#define gameport(a)         ((a&4096)>>12)
#define machid()            (unsigned char)peekb(0xf000,0xfffe);
#define mathchip(a)         ((a&2)>>1)
#define numflop(a)          (((a&192)>>6)+1)
#define numpar(a)           (((unsigned)(a&-16384))>>14)
#define numser(a)           ((a&3584)>>9)
