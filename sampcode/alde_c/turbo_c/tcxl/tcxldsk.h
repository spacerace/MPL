
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLDSK.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  disk functions.                                                         ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

int      _Cdecl disktoscrn(char *fname);
int      _Cdecl disktowin(char *fname);
int      _Cdecl fcmpdatm(char *fname1,char *fname2);
int      _Cdecl fexist(char *fname);
unsigned _Cdecl getktot(int drive);
char    *_Cdecl getvol(char drive);
int      _Cdecl scrntodisk(char *fname);
int      _Cdecl wintodisk(int srow,int scol,int erow,int ecol,char *fname);


/*-----------------------[ video RAM segment address ]-----------------------*/

extern unsigned _videoseg;


/*-----------------------[ Macro-Function Definitions ]----------------------*/

#define fhide(a)            _chmod(a,1,2)

