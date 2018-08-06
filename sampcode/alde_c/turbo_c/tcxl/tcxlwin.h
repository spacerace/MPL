
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLWIN.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  windowing functions.                                                    ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

int      _Cdecl disktowin(char *fname);
void     _Cdecl videoinit(void);
int      _Cdecl wactiv(int whandle);
int      _Cdecl wcclear(int attr);
int      _Cdecl wchgattr(int newattr);
int      _Cdecl wclose(void);
int      _Cdecl wcloseall(void);
int      _Cdecl wclreol(void);
int      _Cdecl wcopy(int nsrow,int nscol);
char    *_Cdecl werrmsg(void);
int      _Cdecl wgetc(void);
int      _Cdecl wgetchf(char *valid);
int      _Cdecl wgets(char *str);
int      _Cdecl wgetns(char *str,int max);
int      _Cdecl wgotoxy(int wrow,int wcol);
int     *_Cdecl whide(int **wbuf);
int      _Cdecl whline(int wsrow,int wscol,int count,int btype);
int      _Cdecl winpdef(int wrow,int wcol,char *str,int ftype,int fattr);
int      _Cdecl winpread(void);
int      _Cdecl winputsf(char *str,char *fmt);
int      _Cdecl winpxcel(void);
int      _Cdecl wintodisk(int srow,int scol,int erow,int ecol,char *fname);
int      _Cdecl wmbardef(int wrow,int wcol,int attr,char *str,int tagchar);
int      _Cdecl wmbarget(int barattr,int taginit,int pulldown);
int      _Cdecl wmbarxcel(void);
int      _Cdecl wmove(int nsrow,int nscol);
int      _Cdecl wopen(int srow,int scol,int erow,int ecol,int btype,int attr);
int      _Cdecl wpgotoxy(int wrow,int wcol);
int      _Cdecl wprintc(int wrow,int wcol,int attr,int ch);
int      _Cdecl wprintf(const char *format,...);
int      _Cdecl wprints(int wrow,int wcol,int attr,char *str);
int      _Cdecl wputc(int ch);
int      _Cdecl wputns(char *str,int width);
int      _Cdecl wputs(char *str);
int      _Cdecl wreadcur(int *wrow,int *wcol);
void     _Cdecl wrestore(int *wbuf);
int     *_Cdecl wsave(int srow,int scol,int erow,int ecol);
int      _Cdecl wscanf(const char *format,...);
int      _Cdecl wscroll(int count,int direction);
int      _Cdecl wsize(int nerow,int necol);
int      _Cdecl wtextattr(int attr);
int      _Cdecl wtitle(char *str,int tpos);
int      _Cdecl wvline(int wsrow,int wscol,int count,int btype);


/*----------------------[ structure of window records ]----------------------*/

struct  _wrecord {
    int *wbuf;                          /*  address of window's buffer       */
    int whandle;                        /*  window's handle                  */
    unsigned char srow;                 /*  start row of window              */
    unsigned char scol;                 /*  start column of window           */
    unsigned char erow;                 /*  end row of window                */
    unsigned char ecol;                 /*  end column of window             */
    unsigned char row;                  /*  window's current cursor row      */
    unsigned char column;               /*  window's current cursor column   */
    unsigned char attr;                 /*  window's current text attribute  */
    unsigned char btype;                /*  window's box type                */
    unsigned char wattr;                /*  window's initial text attribute  */
    char *title;                        /*  address of window's title str    */
    unsigned char tpos;                 /*  position of window's title       */
};


/*-------------------[ variables used by window manager ]--------------------*/

extern struct _wrecord *_wrecord;      /*  array of window records           */
extern int _wcurrent;                  /*  array subscript of active window  */
extern int _werrno;                    /*  last window function error code   */
extern int _whandle;                   /*  last assigned handle number       */
extern int _wesc;                      /*  Escape checking flag              */


/*-------------[ error codes returned from windowing functions ]-------------*/

#define W_NOERROR   0                   /*  no error                        */
#define W_ALLOCERR  1                   /*  memory allocation error         */
#define W_NOTFOUND  2                   /*  window handle not found         */
#define W_NOACTIVE  3                   /*  no active (open) windows        */
#define W_INVCOORD  4                   /*  invalid coordinates             */
#define W_ESCPRESS  5                   /*  Escape key was pressed          */
#define W_INVFORMT  6                   /*  invalid format string           */
#define W_NOINPDEF  7                   /*  no inputs defined               */
#define W_INVFTYPE  8                   /*  invalid field type              */
#define W_STRLONG   9                   /*  string too long for window      */
#define W_NOBARDEF  10                  /*  no bar menu records defined     */
#define W_INVTAGCH  11                  /*  invalid tag character           */
#define W_TITLLONG  12                  /*  title string too long           */
#define W_INVBTYPE  13                  /*  invalid box type                */
#define W_INVTPOS   14                  /*  invalid title position          */


/*---------------------[ direction codes for wscroll() ]---------------------*/

#define DOWN        0                   /*  scroll down                     */
#define UP          1                   /*  scroll up                       */


/*---------------------[ window input record definition ]--------------------*/

struct _winprec {
    unsigned char wrow;                 /*  start of input - window row     */
    unsigned char wcol;                 /*  start of input - window column  */
    char *str;                          /*  address of receiving string     */
    int length;                         /*  length of receiving string      */
    char *buf;                          /*  address of temp receive string  */
    unsigned char ftype;                /*  input field type                */
    unsigned char fattr;                /*  input field attribute           */
};


/*-----------------[ variables used by window input functions ]--------------*/

extern struct _winprec *_winprec;       /*  window input record array       */
extern int _winplast;                   /*  highest inp rec array subscript */
extern int _winpattr;                   /*  attribute of input fields       */


/*------------------[ window bar menu record definition ]--------------------*/

struct _wmbarrec {
    unsigned char wrow;                 /*  start of text opt - window row  */
    unsigned char wcol;                 /*  start of text opt - window col  */
    unsigned char attr;                 /*  attribute of text option        */
    char *str;                          /*  string containing text option   */
    unsigned char tagchar;              /*  menu bar tag character          */
};


/*-------------[ variables used by window menu bar functions ]---------------*/

extern struct _wmbarrec *_wmbarrec;     /*  window menu bar record array    */
extern int _wmbarlast;                  /*  highest menu bar rec subscript  */


/*--------------[ pull-down menu definitions for wmbarget() ]----------------*/

#define PDMAIN      1                   /*  main menu of a pull-down system  */
#define PDPREV      2                   /*  main menu, select prev pull-down */
#define PDNEXT      3                   /*  main menu, select next pull-down */
#define PDMENU      4                   /*  pull-down menu                   */


/*------------[ window title position definitions for wtitle() ]-------------*/

#define TDELETE     0                   /*  delete title                    */
#define TLEFT       1                   /*  left justified                  */
#define TCENTER     2                   /*  centered                        */
#define TRIGHT      3                   /*  right justified                 */


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
#define wclear()            wcclear(_wrecord[_wcurrent].wattr)
#define wisactiv(a)         ((a==_wcurrent)?1:0)
#define wnopen()            (_wcurrent+1)
#define wsetesc(a)          _wesc=((a)?ON:OFF)
#define wunhide(a)          whide(a)

