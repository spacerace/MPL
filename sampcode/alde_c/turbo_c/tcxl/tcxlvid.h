
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLVID.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  screen/video functions.  Windowing functions are defined in TCXLWIN.H   ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

void     _Cdecl box(int srow,int scol,int erow,int ecol,int btype,int atr);
void     _Cdecl boxd(int srow,int scol,int erow,int ecol,int btype,int atr);
void     _Cdecl clreol_(void);
void     _Cdecl clrscrn(void);
int      _Cdecl disktoscrn(char *fname);
void     _Cdecl fill(int srow,int scol,int erow,int ecol,int ch,int atr);
void     _Cdecl filld(int srow,int scol,int erow,int ecol,int ch,int atr);
void     _Cdecl gotoxy_(int row,int col);
void     _Cdecl mode(int mode_code);
void     _Cdecl printc(int row,int col,int attr,int ch,int count);
void     _Cdecl printcd(int row,int col,int attr,int ch);
void     _Cdecl prints(int row,int col,int attr,char *str);
void     _Cdecl printsd(int row,int col,int attr,char *str);
unsigned _Cdecl readchat(void);
void     _Cdecl readcur(int *row,int *col);
void     _Cdecl revattr(int count);
int      _Cdecl scrntodisk(char *fname);
void     _Cdecl setattr(int attr,int count);
void     _Cdecl setcursz(int sline,int eline);
int      _Cdecl setlines(int numlines);
void     _Cdecl spc(int num);
void     _Cdecl srestore(int *sbuf);
int     *_Cdecl ssave(void);
void     _Cdecl videoinit(void);
int      _Cdecl vidtype(void);


/*-------------[ display adapter types returned from vidtype() ]-------------*/

#define MDA         0
#define HGC         1
#define HGCPLUS     2
#define INCOLOR     3
#define CGA         4
#define EGA         5
#define VGA         6


/*-----------------------[ video RAM segment address ]-----------------------*/

extern unsigned _videoseg;


/*--------------[ attribute codes for functions that use them ]--------------*/

    /*  attribute codes are created by ORing the codes for foreground
        color, background color, and optionally blink, together.  For
        example, to create an attribute code for light blue on green,
        you would use LBLUE|_GREEN and to make the foreground blink,
        you would add |BLINK onto the end of that.                      */

#ifndef BLACK                           /*  foreground colors  */
#define BLACK       0                   
#define BLUE        1
#define GREEN       2
#define CYAN        3
#define RED         4
#define MAGENTA     5
#define BROWN       6
#define YELLOW      14
#define WHITE       15
#endif
#ifndef LGREY
#define LGREY       7                   /*  TCXL abbreviates LIGHT as L    */
#define DGREY       8                   /*  and DARK as D for ease of use  */
#define LBLUE       9
#define LGREEN      10
#define LCYAN       11
#define LRED        12
#define LMAGENTA    13

#define _BLACK      0                   /*  background colors  */
#define _BLUE       16
#define _GREEN      32
#define _CYAN       48
#define _RED        64
#define _MAGENTA    80
#define _BROWN      96
#define _LGREY      112

#define BLINK       128                 /*  blink attribute  */
#endif


/*-----------------------[ Macro-Function Definitions ]----------------------*/

#ifndef attrib
#define attrib(f,b,i,bl)    ((b<<4)|(f)|(i<<3)|(bl<<7))
#endif
#ifndef clrwin
#define clrwin(a,b,c,d)     gotoxy_(a,b);fill(a,b,c,d,' ',(readchat()>>8))
#endif
#define cls()               clrscrn()
#define home()              gotoxy_(0,0)

