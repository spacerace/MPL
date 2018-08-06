/*  initstuf.c -- initializing module for grafchar.c */
/*  assign graphics character codes to an array      */
/*  and initialize screen                            */

#include "scrn.h"   /* Clearscr(), Home(), Setcurs() */
#include "grafchar.h"
extern unsigned char Grchr[];  /* defined in grafchar.c */
void Print_attr(char *, unsigned char, unsigned char);
void Init_stuff()
{
    int i;

    /* initialize array with graphics characters */
    for (i = 0; i < NUMCHARS; i++)
        Grchr[i] = GCSTART + i;
    Clearscr();
    Home();

    /*   show key meanings at bottom of screen */
    Setcurs(BOTLINE + 1, 0, PAGE);
    for (i = 0; i < 40; i++) /* graphics chars */
    {
        putch(Grchr[i]);
        putch(SPACE);
    }
    Setcurs(BOTLINE + 2, 0, PAGE);
    for (i = 0; i < 40; i++)  /* key assignments */
    {
        putch('0' + i);
        putch(SPACE);
    }
    Setcurs(BOTLINE + 3, 0, PAGE);
    for (i = 40; i < NUMCHARS; i++) /* second row */
    {
        putch(Grchr[i]);
        putch(SPACE);
    }
        /* show function key assignments */
    printf(" SPACE : ERASE  PgUp : No Draw ");
    printf(" PgDn : Draw  ESC : Quit");
    Setcurs(BOTLINE + 4, 0, PAGE);
    for (i = 40; i < NUMCHARS; i++) /* second row */
    {
        putch('0' + i);
        putch(SPACE);
    }
       /* more function key assignments */
    Print_attr("F1 : Normal ", NORMAL, PAGE);
    Print_attr("F2 : Reverse Video ", VIDREV, PAGE);
    Setcurs(BOTLINE + 5, 16, PAGE);
    Print_attr("F3 : Blinking ", NORMAL | BLINK, PAGE);
    Print_attr("F4 : Intense ", NORMAL | INTENSE, PAGE);
    Home();
}

void Print_attr(str, attr, page)
char *str;
unsigned char attr, page;
{
    while (*str != '\0')
    {
        Write_ch_atr(*str++, attr, page, 1);
        Cursrt();
    }
}
