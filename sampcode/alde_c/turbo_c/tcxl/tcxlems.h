
/*
   ��������������������������������������������������������������������������Ŀ
   �                                                                          �
   �  TCXLEMS.H - TCXL (c) 1987, 1988 by Mike Smedley.                        �
   �                                                                          �
   �  This header file contains prototypes of TCXL's Expanded Memory          �
   �  Specification (EMS) functions.                                          �
   �                                                                          �
   ����������������������������������������������������������������������������
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


unsigned _Cdecl emsalloc(int numpages);
int      _Cdecl emsdealloc(int emmhandle);
int      _Cdecl emsexist(void);
unsigned _Cdecl emsframe(void);
unsigned _Cdecl emsfree(void);
int      _Cdecl emsmap(int emmhandle,int logpage,int phypage);
int      _Cdecl emsread(void *dest,unsigned emsofs,unsigned numbytes);
unsigned _Cdecl emstotal(void);
char    *_Cdecl emsver(void);
int      _Cdecl emswrite(void *src,unsigned emsofs,unsigned numbytes);
unsigned _Cdecl expmem(void);
