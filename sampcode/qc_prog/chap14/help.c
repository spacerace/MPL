/*  help.c -- uses paging and direct memory access    */
/*            to display a help screen                */
/*  Program list: help.c, writestr.c, writechr.c,     */
/*                scrfun.c                            */
/*  User include files: scrn.h, color.h               */
/*  Note: activate Screen Swapping On in Debug menu   */

#include <stdio.h>
#include <conio.h>
#include "color.h"
#include "scrn.h"
typedef unsigned int (far * VIDMEM);
#define CGAMEM ((VIDMEM) (0xB800L << 16))
#define PAGESIZE 2000
#define PAGEOFFSET  0x800L
#define ESC '\033'
#define ATTR1 (BG_BLUE | YELLOW)
#define ATTR2 (BG_YELLOW | BLUE)
#define ATTR3 (BG_RED | YELLOW | BLINK | INTENSE)
#define CH1  (unsigned short) '\xB1'
char *str1 = "Press ? key for help.";
char *str2 = "Press Enter key to return.";
char *str3 = "Press Esc key to quit.";
char *str4 = "\xB1HELP!\xB1";
void Write_chars(VIDMEM, unsigned short, unsigned
                    short, unsigned short);
void Write_str(VIDMEM, unsigned short, char *);

main()
{
    int ch;
    unsigned char page = 0;
    unsigned char mode;

    mode = Getvmode();
    if (mode != TEXTC80 && mode != TEXTBW80)
    {
        printf("Only modes 2 and 3 supported. Bye.\n");
        exit(1);
    }
    Setpage (page);
    Write_chars(CGAMEM, '\0', ATTR2, PAGESIZE);
    Write_str(CGAMEM + 2 * COLS, ATTR1, str1);
    Write_str(CGAMEM + 2 * COLS, ATTR1, str1);
    Write_str(CGAMEM + 22 * COLS, ATTR1, str3);
    Write_chars(CGAMEM + PAGEOFFSET, '\0', ATTR1, PAGESIZE);
    Write_str(CGAMEM + PAGEOFFSET + 20 * COLS, ATTR2, str2);
    Write_str(CGAMEM + PAGEOFFSET + 22 * COLS, ATTR1, str3);
    Write_chars(CGAMEM + PAGEOFFSET + 10 * COLS + 36,
                CH1, ATTR3, 7);
    Write_str(CGAMEM + PAGEOFFSET + 11 * COLS + 36,
              ATTR3, str4);
    Write_chars(CGAMEM + PAGEOFFSET + 12 * COLS + 36,
                CH1, ATTR3, 7);

    while ((ch = getch()) != ESC)
        {
        if (ch == '?' && page == 0)
            Setpage (page = 1);
        else if (ch == '\r' && page == 1)
            Setpage(page = 0);
        }
    Write_chars(CGAMEM, '\0', NORMAL, PAGESIZE);
    Write_chars(CGAMEM + PAGEOFFSET, '\0', NORMAL, PAGESIZE);
}
