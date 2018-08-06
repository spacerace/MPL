/* ch2000.c -- fill screen with 2000 characters        */
/*    This program demonstrates direct memory access   */
/*    of video memory.  It is set up for the MDA.      */
/*    Assign CGAMEM instead of MONMEM to screen for    */
/*    CGA and CGA-compatible modes.                    */
/*    Press a key to fill; press Esc to quit.          */
/* Note: activate Screen Swapping On in Debug menu     */

#include <conio.h>
#include "scrn.h"
typedef unsigned short (far * VIDMEM);
#define MONMEM ((VIDMEM) (0xB000L << 16)) /* monochrome */
#define CGAMEM ((VIDMEM) (0xB800L << 16)) /* cga, ega */
#define ESC '\033'
#define CHARS 2000
#define AMASK 0xFF   /* keep attribute in range */
main()
{
     unsigned ch;          /* character to be displayed */
     unsigned attrib = 7;  /* initial attribute         */
     VIDMEM screen;        /* pointer to video RAM      */
     int offset;           /* location on screen        */

     screen = MONMEM;      /* monochrome initialization */
     while ((ch = getch()) != ESC)
     {
        for (offset = 0; offset < CHARS; offset++)
          *(screen + offset) = ((attrib++ & AMASK) << 8)                              | ch;
     }
}
