/* lines43.c -- leaves EGA in 43-line mode */

#include <dos.h>
#include <conio.h>
#define VIDEO 0x10
#define SETVMODE 0
#define CHAR_GEN 0x11   /* an EGA BIOS function number */
#define ROM8X8   0x12
#define BLOCK 0
#define TEXTC80 3

main()
{
     union REGS reg;

     reg.h.ah = SETVMODE;    /* set text mode */
     reg.h.al = TEXTC80;
     int86(VIDEO, &reg, &reg);

     reg.h.ah = CHAR_GEN;   /* char generator routine */
     reg.h.al = ROM8X8;  /* use 8x8 ROM character box */
     reg.h.bl = BLOCK;   /* copy to block 0 */
     int86(VIDEO, &reg, &reg);
}
