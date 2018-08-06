/* blank.c -- blanks MDA screen                        */
/* program list -- blank.c (outp() not in core lib)    */

#include <conio.h>
#define CONTROLREG 0x3B8 /* control register MDA */
#define DEFAULTSET 0x29
#define VIDEOOFF 0x21
main()
{
    outp(CONTROLREG, VIDEOOFF);
    getch();
    outp(CONTROLREG, DEFAULTSET);
}
