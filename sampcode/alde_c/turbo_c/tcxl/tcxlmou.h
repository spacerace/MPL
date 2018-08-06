
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLMOU.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  Microsoft compatible mouse functions.                                   ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

void    _Cdecl msbpress(int button,int *bstat,int *bcount,int *x,int *y);
void    _Cdecl msbreles(int button,int *bstat,int *bcount,int *x,int *y);
void    _Cdecl mscursor(int curtype,int smask,int cmask);
void    _Cdecl msgotoxy(int x,int y);
void    _Cdecl mshbounds(int left,int right);
void    _Cdecl mshidecur(void);
int     _Cdecl msinit(void);
void    _Cdecl msmotion(int *xcount,int *ycount);
void    _Cdecl msshowcur(void);
void    _Cdecl msspeed(int xratio,int yratio);
void    _Cdecl msstatus(int *bstat,int *x,int *y);
void    _Cdecl msvbounds(int top,int bottom);
