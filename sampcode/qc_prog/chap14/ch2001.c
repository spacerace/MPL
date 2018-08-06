/* ch2001.c -- fill screen with 2000 characters        */
/*    This program demonstrates direct memory access   */
/*    of video memory.  It uses the current video mode */
/*    value to select the proper video RAM address.    */
/*    Press a key to fill; press Esc to quit.          */
/* Program list: ch2001.c, scrfun.lib                  */
/* Note: activate Screen Swapping On in Debug menu     */

#include <conio.h>
#include "scrn.h"
typedef unsigned short (far * VIDMEM);
#define MONMEM ((VIDMEM) (0xB000L << 16)) /* monochrome */
#define CGAMEM ((VIDMEM) (0xB800L << 16)) /* cga, ega */
#define ESC '\033'
#define CHARS 2000
#define AMASK 0xFF
main()
{
     unsigned ch, mode;
     unsigned attrib = 7;
     VIDMEM screen;         /* pointer to video RAM */
     int offset;

     if ((mode = Getvmode()) == TEXTMONO)
          screen = MONMEM;
     else if (mode == TEXTC80 || mode == TEXTBW80)
          screen = CGAMEM;
     else
          exit(1);
     while ((ch = getch()) != ESC)
          {
          for (offset = 0; offset < CHARS; offset++)
              *(screen + offset) = ((attrib++ & AMASK) << 8) | ch;
          }
}
