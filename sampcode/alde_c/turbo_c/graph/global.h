/*--------------------------------------------------------------------------
 *                                 global.h
 *
 *                      Copyright 1987 by Michael Allen
 *
 *	   This contains the declarations for global variables used by the 
 *	   graphics functions in graph.lib. You must include this in your
 *	   main module with a: #include "global.h". Then in sub-modules, include:
 *	   #include "glob_ext.h"
 *-------------------------------------------------------------------------*/


union REGS regs;

struct SREGS segregs;

T_SCREEN screen = (unsigned char far *)SCREENADDR;  /* color card screen buffer */

int            xmax    = 639,
               xmin    = 0,
               ymax    = 199,	/* the size of the screen in pixels */
               ymin    = 0;

BYTE           draw_option;    /* NORMAL, XOR_MODE */


/* -----------------------------------------------------------------  */

