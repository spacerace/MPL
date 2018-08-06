/*
** WINDOWS - Simple but Elegant Window Functions 
**           (Lattice, Computer Innovations, Microsoft, Mark Williams)
**
** Copyright (c) 1984, 1985, 1986 - Philip A. Mongelluzzo
** All rights reserved.
**
*/

/*
** Truth, Lies, and Compiler Options 
**
** Various equates for the various compilers.
**
** Basic compiler invocation is:
**
** Microsoft 3.0 
**
**      MSC -dMSCV3 sourcefile_name;
**
** Microsoft 4.0 
**
**      MSC -dMSCV4 sourcefile_name;
**
** Lattice 2.XX 
**
**      LCS -dLC2 sourcefile_name
**
** Lattice 3.XX
**
**      LCS -dLC3 sourcefile_name
**
** Datalight
**
**      DLC1 -dDLC -mS sourcefile_name
**      DLC2 sourcefile_name -S
**
** Mark Williams "Lets C"
**
**      CC -DMWC sourcefile_name
**
** Computer Innovations CI86
**
**      cc1 -cm -dC86 sourcefile_name
**      cc2 ...
**
*/

/*
** ONE of the following can be commented out if you prefer to
** use this method instead of the command line arg at compile time.
**
** Registered Users Only: 
**
**   BE SURE TO MAKE CHANGES IN BOTH WINDOWS.C AND WINDOWS.H
*/

/* #define MSCV3 */
/* #define MSCV4 */
/* #define LC2   */
/* #define LC3   */
/* #define DLC   */
/* #define MWC   */
/* #define C86   */

/*
** Microsoft 3.00 
*/

#ifdef MSCV3
#define MSC     1
#define MSC4    0
#define DLC     0
#define MWC     0
#define CI86    0
#define LC2     0
#define LC3     0
#if M_I86SM                             /* small code, small data */
#define SPTR    1
#define LPTR    0
#endif
#if M_I86LM                             /* large code, large data */
#define SPTR    0
#define LPTR    1
#endif
#if M_I86CM                             /* small code, large data */
#define SPTR    0
#define LPTR    1
#endif
#if M_I86MM                             /* large code, small data */
#define SPTR    1
#define LPTR    0
#endif
#define LATTICE 1
#endif

/*
** Microsoft 4.0
*/

#ifdef MSCV4
#define MSC     1
#define MSC4    1
#define DLC     0
#define MWC     0
#define CI86    0
#define LC2     0
#define LC3     0
#if M_I86SM                             /* small code, small data */
#define SPTR    1
#define LPTR    0
#endif
#if M_I86LM                             /* large code, large data */
#define SPTR    0
#define LPTR    1
#endif
#if M_I86CM                             /* small code, large data */
#define SPTR    0
#define LPTR    1
#endif
#if M_I86MM                             /* large code, small data */
#define SPTR    1
#define LPTR    0
#endif
#define LATTICE 1
#endif

/*
** Computer Innovations
*/

#ifdef C86
#define MSC     0
#define MSC4    0
#define DLC     0
#define MWC     0
#define CI86    1
#define LC2     0
#define LC3     0
#define M_I86   0
#ifdef _C86_BIG
#define LPTR    1 
#define SPTR    0
#else
#define LPTR    0
#define SPTR    1
#endif
#define LATTICE 0 
#endif 
#define TRUE    1                       /* truth */
#define FALSE   0                       /* lies */
#define The_BOSS TRUE                   /* convienent equate */

#if M_I86                               /* Microsoft C */
#define LINT_ARGS                       /* enforce type checking */
#include "malloc.h"                     /* for malloc... */
#else                                   /* if not Microsoft C */
char *malloc(), *calloc();              /* keep everybody happy */
#endif

#include "stdio.h"                      /* standard header */
#include "dos.h"                        /* Lattice stuff */
#include "ctype.h"                      /* character conversion stuff */
#if MSC4
#include "stdarg.h"                     /* variable arg list marcos */
#endif

#if CI86     
struct SREGS                            /* for segread */
        {
        unsigned es,cs,ss,ds;
        };
#endif

#define SAVE    TRUE                    /* similar truth */
#define RESTORE FALSE                   /* fibs */
#define REPLACE 1                       /* for flicker free */
#define ERASE   0                       /* scroll w_sapd & w_sapu */
#define FAST    0x01                    /* fast retrace */
#define SLOW    0x08                    /* slow retrace */

#define NULPTR  (char *) 0              /* null pointer */
#define BEL     0x07                    /* beep */
#define BS      0x08                    /* backspace */
#define NUL     '\0'                    /* NUL char */
#define ESC     0x1b                    /* Escape */
#define CR      0x0d                    /* carriage return */
#define LF      0x0a                    /* linefeed */
#define DEL     0x7f                    /* delete */
#define NAK     0x15                    /* ^U */
#define ETX     0x03                    /* ^C */
#define CAN     0x18                    /* ^X */
#define Del     0x53                    /* Del key scan code */
#define ECHO    0x8000                  /* echo disable bit */

#define BIOS    0x01                    /* BIOS Scrolling */
#define DMAS    0x02                    /* The BOSS's DMA Scrolling */

/*
** Externals
*/

extern int wn_dmaflg;                   /* dma flag */
extern char wn_sbit;                    /* retrace test bit 8 slow, 1 fast */
extern int wn_blank;                    /* vidon & vidoff control flag */
extern int wns_bchars[];                /* box chars */
extern unsigned int wns_mtflg;          /* monitor type flag */
extern int wns_cflag;                   /* close in progress flag */

#if MSC | LATTICE | DLC | CI86          /* NOT for Mark Williams */
extern struct SREGS wns_srp;            /* for segread */
#endif

#if MWC                                 /* Mark Williams */
#define strchr index
#endif

#define BCUL  wns_bchars[0]             /* some shorthand for later */
#define BCUR  wns_bchars[1]
#define BCTB  wns_bchars[2]
#define BCSD  wns_bchars[3]
#define BCLL  wns_bchars[4]
#define BCLR  wns_bchars[5]

/*
** Misc Stuff
*/

extern unsigned wns_mtype();            /* make everyone happy */

#define WMR   wn->bsize                 /* shorthand */

typedef struct wcb                      /* Window control block */
{
int ulx,                                /* upper left corner x coordinate */
    uly,                                /* upper left corner y coordinate */
    xsize,                              /* width of window - INSIDE dimension */
    ysize,                              /* height of window -INSIDE dimension */
    ccx,                                /* virtual cursor offset in window */
    ccy,                                
    style,                              /* attribute to be used in window */
    bstyle,                             /* border attribute */
    bsize;                              /* total border size 0 or 2 only */
char *scrnsave;                         /* pointer to screen save buffer */
int page,                               /* current video page being used */
    oldx,                               /* cursor position when window was */
    oldy,                               /* opened (used for screen restore) */
    wrpflg,                             /* wrap flag */
    synflg;                             /* cursor sync flag */
char *handle;                           /* my own id */
    char *prevptr;                      /* linked list - previous */
    char *nextptr;                      /* linked list - next */
    char *tmpscr;                       /* for activate */
    int  smeth;                         /* scroll method to use */
} WINDOW, *WINDOWPTR;

extern WINDOWPTR wns_last;              /* last window opened */

#if M_I86                               /* allow for LINT_ARGS */
#ifndef GENFNS
#include "windows.fns"                  /* enforce type checking */
#endif
#else                                   /* and almost lint args */
unsigned int wns_mtype();
struct wcb *wn_open();
struct wcb *wn_move();
struct wcb *wn_save();
char *wn_gets();
unsigned int wns_mtype();
#endif

#define BLACK   0x00                    /* foreground */
#define RED     0x04                    /* background */
#define GREEN   0x02                    /* colors */
#define YELLOW  0x06                    /* bg << 4 | fg */
#define BLUE    0x01
#define MAGENTA 0x05
#define CYAN    0x03
#define WHITE   0x07
#define BLINK   0x80
#define BOLD    0x08
#define NDISPB  0x00                    /* non display black */
#define NDISPW  0x77                    /* non display white */
#define RVIDEO  0x70                    /* reverse video */
#define UNLINE  0x01                    /* under line (BLUE) */

#define NVIDEO  0x07                    /* normal video */
#define NORMAL  0x03                    /* cyan is normal for me */

/*
** Display Mode Atributes
*/

#define B4025  0                        /* black & white 40 x 25 */
#define C4025  1                        /* color 40 x 25 */
#define B8025  2                        /* black & white 80 x 25 */
#define C8025  3                        /* color 80 x 25 */
#define C320   4                        /* color graphics 320 x 200 */
#define B320   5                        /* black & white graphics */
#define HIRES  6                        /* B&W hi res 640 * 200 */
#define MONO   7                        /* monocrome 80 x 25 */

/*
** Macro to set attribute byte
*/

#define v_setatr(bg,fg,blink,bold) ((blink|(bg<<4))|(fg|bold))

/* End */
