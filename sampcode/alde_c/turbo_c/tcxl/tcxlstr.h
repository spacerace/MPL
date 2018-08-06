

/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLSTR.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  string manipulating functions.                                          ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

int      _Cdecl cvaltype(int ch,int ctype);
char    *_Cdecl strbmatch(char *str,char *strarr[]);
char    *_Cdecl strchg(char *str,int oldch,int newch);
char    *_Cdecl strcode(char *str,int key);
char    *_Cdecl strdel(char *substr,char *str);
char    *_Cdecl strichg(char *str,int oldch,int newch);
char    *_Cdecl stridel(char *substr,char *str);
char    *_Cdecl striinc(char *str1,char *str2);
char    *_Cdecl strinc(char *str1,char *str2);
char    *_Cdecl strins(char *instr,char **str,int st_pos);
int      _Cdecl striocc(char *str,int ch);
char    *_Cdecl strltrim(char **str);
int      _Cdecl strmatch(char *str1,char *str2);
char    *_Cdecl strmid(char *str,int st_pos,int num_chars);
int      _Cdecl strocc(char *str,int ch);
char    *_Cdecl strrol(char *str,int count);
char    *_Cdecl strror(char *str,int count);
char    *_Cdecl strsetsz(char **str,int newsize);
char    *_Cdecl strshl(char *str,int count);
char    *_Cdecl strshr(char *str,int count);
char    *_Cdecl strtrim(char *str);
char    *_Cdecl struplow(char *str);
int      _Cdecl touplow(char *str,char *pos,int ch);


/*-----------------------[ Macro-Function Definitions ]----------------------*/

#define strleft(st,nu)      strmid(st,0,nu)
#define strright(st,nu)     strmid(st,strlen(st)-nu,nu)
#define strrtrim(st)        strtrim(st)
