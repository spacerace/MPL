/* readkey.c -- contains the Readkey() function     */
#include <dos.h>
#define KEYINTR 0x16  /* keyboard read interrupt */
#define GETCHAR 0     /* read scancode function  */
struct SCANCODE {
                unsigned char ascii;  /* ascii code */
                unsigned char scan;   /* scan code  */
                };

struct SCANCODE Readkey()
{
    union REGS reg;
    struct SCANCODE scancode;

    reg.h.ah = GETCHAR;         /* specify function */
    int86(KEYINTR, &reg, &reg); /* note use of & oper.*/
    scancode.ascii = reg.h.al;
    scancode.scan = reg.h.ah;
    return (scancode);
}
