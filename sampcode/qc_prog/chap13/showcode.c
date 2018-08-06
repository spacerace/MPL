/*  showcode.c -- shows ASCII and scan codes for    */
/*                keystrokes                        */
/* Note: Set Screen Swapping On in the Debug menu.  */
#include <stdio.h>
#include <dos.h>
#define KEYINTR 0x16  /* keyboard read interrupt */
#define GETCHAR 0     /* read scancode function  */
#define ESC '\033'    /* escape key              */
struct SCANCODE {
                unsigned char ascii;  /* ascii code */
                unsigned char scan;   /* scan code  */
                };
struct SCANCODE Readkey();

main()
{
    struct SCANCODE keys;

    printf("Press keys to see their scancodes. ");
    printf("Press the Esc key to quit.\n");
    do  {
        keys = Readkey();
        if (keys.ascii > 0 && keys.ascii < 040)
            printf("^%c: ascii = %3d, scan = %3d\n",
                    keys.ascii + 0100, keys.ascii,
                    keys.scan);
        else if (keys.ascii >= 40)
            printf(" %c: ascii = %3d, scan = %3d\n",
                    keys.ascii, keys.ascii, keys.scan);
        else
            printf("  : ascii = %3d, scan = %3d\n",
                    keys.ascii, keys.scan);
        } while (keys.ascii != ESC);
}

struct SCANCODE Readkey()
{
    union REGS reg;
    struct SCANCODE scancode;

    reg.h.ah = GETCHAR;
    int86(KEYINTR, &reg, &reg);
    scancode.ascii = reg.h.al;
    scancode.scan = reg.h.ah;
    return (scancode);
}
