/*   grafchar.c -- draws graphics characters with      */
/*                 attributes on the screen            */
/*  Program list : grafchar.c, initstuf.c, drawchar.c, */
/*                  scrfun.c                           */
/*  User include files: keys.h, scrn.h, grafchar.h     */
/*  Note: activate Screen Swapping On in Debug menu    */
 
#include "grafchar.h"
unsigned char Grchr[NUMCHARS];  /* to store graphics set */
void Init_stuff(void);      /* in initstuf.c */
void Draw_chars(void);      /* in drawchar.c */

main()
{
    Init_stuff();  /* initialize vital elements */
    Draw_chars();  /* map keys to graphics characters */
}
