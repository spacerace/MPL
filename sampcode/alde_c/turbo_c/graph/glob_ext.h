/*--------------------------------------------------------------------------
 *                                glob_ext.h
 *
 *                       copyright 1987 by Michael Allen
 *
 *       This contains the external declarations for global variables used by the
 *       graphics functions in graph.lib. You must include this in your
 *       sub modules with a: #include "glob_ext.h". ( in your main module,
 *       include: #include "global.h" )
 *
 *-------------------------------------------------------------------------*/

extern int mask[8];

extern union REGS regs;

extern struct SREGS segregs;

extern T_SCREEN    screen;

extern int     ymax,
               xmin,
               ymin,
               xmax;

extern BYTE    draw_option;

/*-------------- end of glob_ext.h ---------------------*/

