/*   roamscrn.c  -- puts text on screen, positions    */
/*                  cursor with arrow keys, uses F1   */
/*                  and F2 to control video inverse.  */
/*   program list -- roamscreen.c, scrfun.lib         */
/*   user include files -- keys.h, scrn.h             */
/*  Note: Activate Screen Swapping On in Debug menu   */
#include <conio.h>
#include "keys.h"
#include "scrn.h"
#define BELL '\a'
#define ESC '\033'
#define PAGE 0

char *Heading =
"Use standard keys to enter text. Use arrow keys to "
"reposition cursor.\nUse F2 to turn on video inverse "
"and F1 to turn it off.\nHit the ESC key to quit.\n";

main()
{
    int ch;
    unsigned char atr = NORMAL;

    Clearscr();
    Home();
    printf("%s", Heading);
    while ((ch = getch()) != ESC)
        {
        if (ch == '\r')
            {
            putch('\n');
            putch('\r');
            }
        else if (ch != 0)
            {
               Write_ch_atr(ch, atr, PAGE, 1);
            if (!Cursrt())
                putch(BELL);
            }
        else
            {
            ch = getch();
            switch (ch)
                {
                case F1 : atr = NORMAL; break;
                case F2 : atr = VIDREV; break;
                    case UP : Rewrite(atr, PAGE);
                          if (!Cursup())
                            putch(BELL);
                          break;
                    case DN : Rewrite(atr, PAGE);
                          if (!Cursdn())
                            putch(BELL);
                          break;
                    case LT : Rewrite(atr, PAGE);
                          if (!Curslt())
                            putch(BELL);
                          break;
                    case RT : Rewrite(atr, PAGE);
                          if (!Cursrt())
                            putch(BELL);
                          break;
                default : break;
                }
            }
        }
}
