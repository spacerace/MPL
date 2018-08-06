/* drawchar.c -- drawing module for grafdraw.c         */
/* translates keystrokes to graphic characters,        */
/* manages cursor control and function keys            */

#include <conio.h>
#include "keys.h"
#include "scrn.h"
#include "grafchar.h"
extern unsigned char Grchr[];  /* defined in grafchar.c */

void Draw_chars()
{
    int ch, chout;
    unsigned char attrib = NORMAL;
    unsigned char draw = TRUE;

    chout = Grchr[0];  /* default graphics character */
    while ((ch = getch()) != ESC)
        {
        if (ch >= '0' && ch <= '_')
            chout = Grchr[ch - '0'];
            /* this maps the 0 key to the first */
            /* graphics character, etc.         */
        else if (ch == SPACE)
            chout = SPACE;
        else if (ch == 0) /* process cursor keys */
            {             /* and function keys   */
            ch = getch();
            switch (ch)
                {
                case PU : draw = FALSE;
                          break;
                case PD : draw = TRUE;
                          break;
                case UP : if (draw)
                              Write_ch_atr(chout, attrib,
                                           PAGE, 1);
                          if (!Cursup())
                              putch(BEEP);
                          break;
                case DN : if (draw)
                              Write_ch_atr(chout, attrib,
                                           PAGE, 1);
                          if (!Cursdn_lim(BOTLINE))
                              putch(BEEP);
                          break;
                case LT : if (draw)
                              Write_ch_atr(chout, attrib,
                                           PAGE, 1);
                          if (!Curslt())
                              putch(BEEP);
                          break;
                case RT : if (draw)
                              Write_ch_atr(chout, attrib,
                                           PAGE, 1);
                          if (!Cursrt())
                              putch(BEEP);
                          break;
                case F1 : attrib = NORMAL; break;
                case F2 : attrib = VIDREV; break;
                case F3 : attrib ^= BLINK; break;
                case F4 : attrib ^= INTENSE; break;
                default : putch(BEEP);
                }
            }
        }
}
