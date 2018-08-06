/* savegraf.c -- uses DMA to save screen of graphics  */
/*               characters and attributes            */
/* Program list - savegraf.c, initstuf.c, drawchar.c, */
/*                savescrn.c, scrfun.c                */
/* User include files - scrn.h, keys.h, grafchar.h    */
/* Note: activate Screen Swapping On in Debug menu    */


#include "grafchar.h"
unsigned char Grchr[NUMCHARS];  /* to store graphics set */
void Init_stuff(void);
void Draw_chars(void);
void Save_screen(void);  /* in savescrn.c */

main()
{
    int ch;

    Init_stuff();  /* initialize vital elements */
    Draw_chars();  /* map keys to graphics characters */
    Setcurs(BOTLINE + 1, 0, PAGE);
    printf("%-80s", "Save screen? <y/n> ");
    Setcurs(BOTLINE + 1, 20, PAGE);
    ch = getche();
    if (ch == 'y' || ch == 'Y')
        Save_screen();
    Setcurs(BOTLINE + 2, 0, PAGE);
    printf("%-80s\n", "BYE!");
}
