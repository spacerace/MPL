
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLKEY.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  keyboard functions.  Keyboard functions for windowing functions are     ³
   ³  defined in TCXLWIN.H                                                    ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

int      _Cdecl getchf(char *valid);
int      _Cdecl getns(char *str,int max);
void     _Cdecl getxch(int *ch,int *xch);
int      _Cdecl inputsf(char *str,char *fmt);
int      _Cdecl waitkey(void);
int      _Cdecl waitkeyt(int duration);


/*-------------[ keyboard status codes returned from kbstat() ]--------------*/

#define RSHIFT      1                   /*  right shift pressed          */
#define LSHIFT      2                   /*  left shift pressed           */
#define CTRL        4                   /*  <Ctrl> pressed               */
#define ALT         8                   /*  <Alt> pressed                */
#define SCRLOCK     16                  /*  <Scroll Lock> toggled        */
#define NUMLOCK     32                  /*  <Num Lock> toggled           */
#define CAPSLOCK    64                  /*  <Caps Lock> toggled          */
#define INS         128                 /*  <Ins> toggled                */


/*---------------[ seg,ofs address of keyboard status flag ]-----------------*/

#define KBFLAG      0,0x417


/*-----------------------[ macro-function definitions ]-----------------------*/

#define capsoff()           poke(KBFLAG,peek(KBFLAG)&191)
#define capson()            poke(KBFLAG,peek(KBFLAG)|64)
#define clearkeys()         while(kbhit()) getch()
#define kbstat()            peek(KBFLAG)
#define numoff()            poke(KBFLAG,peek(KBFLAG)&223)
#define numon()             poke(KBFLAG,peek(KBFLAG)|32)
#define prompts(pr,st)      printf("%s",pr);gets(st)

