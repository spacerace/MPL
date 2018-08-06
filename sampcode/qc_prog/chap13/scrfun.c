/*  scrfun.c -- contains several video BIOS calls     */
/*  Setcurs() sets the cursor position                */
/*  Getcurs() gets the cursor postion                 */
/*  Setpage() sets the current video page             */
/*  Setvmode() sets the video mode                    */
/*  Clearscr() clears the screen                      */
/*  Read_ch_atr() reads the character and             */
/*                attribute at the cursor             */
/*  Write_ch_atr() writes a character and             */
/*                 attribute at the cursor            */
/*  Rewrite() rewrites a screen character             */
/*            with a new attribute                    */
/*  Getvmode() gets the current video mode            */
/*  Getpage() gets the current video page             */
/*                                                    */
/*  The following functions use Setcurs() to move the */
/*  one position at a time up to a limit.             */
/*  Curslt_lim() moves cursor one column left         */
/*  Cursrt_lim() moves cursor one column right        */
/*  Cursup_lim() moves cursor one line up             */
/*  Cursdn_lim() moves cursor one line down           */
/*                                                    */
/*  Programs using these functions should include the */
/*  scrn.h file                                       */

#include <dos.h>
#include "scrn.h"

/* sets cursor to row, column, and page */
void Setcurs(row, col, page)
unsigned char row, col, page;
{
    union REGS reg;

    reg.h.ah = SETCURSOR;
    reg.h.dh = row;
    reg.h.dl = col;
    reg.h.bh = page;
    int86(VIDEO, &reg, &reg);
}

/* gets current cursor row, column for given page */
void Getcurs(pr, pc, page)
unsigned char *pr, *pc, page;
{
    union REGS reg;

    reg.h.ah = GETCURSOR;
    reg.h.bh = page;
    int86(VIDEO, &reg, &reg);
    *pr = reg.h.dh;  /* row number */
    *pc = reg.h.dl;   /* column number */
}


/* sets page to given value */
void Setpage(page)
unsigned char page;
{
    union REGS reg;

    reg.h.ah = SETPAGE;
    reg.h.al = page;
    int86(VIDEO, &reg, &reg);
}

/* sets video mode to given mode */
void Setvmode(mode)
unsigned char mode;
{
    union REGS reg;

    reg.h.ah = SETMODE;
    reg.h.al = mode;
    int86(VIDEO, &reg, &reg);
}

/* clear the screen */
void Clearscr()
{
    union REGS reg;

    reg.h.ah = SCROLL;
    reg.h.al = 0;
    reg.h.ch = 0;
    reg.h.cl = 0;
    reg.h.dh = ROWS - 1;
    reg.h.dl = COLS - 1;
    reg.h.bh = NORMAL;
    int86(VIDEO, &reg, &reg);
}

/* reads the character and attribute at the cursor */
/* position on a given page                        */
void Read_ch_atr(pc, pa, page)
unsigned char *pc, *pa;
unsigned char page;
{
    union REGS reg;

    reg.h.ah = READCHATR;
    reg.h.bh = page;
    int86(VIDEO, &reg, &reg);
    *pc = reg.h.al;  /* character at cursor */
    *pa = reg.h.ah;  /* attribute at cursor */
}

/* writes a given character and attribute at the */
/* cursor on a given page for num times          */
void Write_ch_atr(ch, atr, page, num)
unsigned char ch, atr, page;
unsigned int num;
{
    union REGS reg;

    reg.h.ah = WRITECHATR;
    reg.h.al = ch;
    reg.h.bl = atr;
    reg.h.bh = page;
    reg.x.cx = num;
    int86(VIDEO, &reg, &reg);
}

/* rewrites the character at the cursor using    */
/* attribute at                                  */
void Rewrite(at, page)
unsigned char at, page;
{
     unsigned char ch, atr;

     Read_ch_atr(&ch, &atr, page);
     Write_ch_atr(ch, at, page, 1);
}

/* obtains the current video mode */
unsigned char Getvmode()
{
    union REGS reg;

    reg.h.ah = GETMODE;
    int86(VIDEO, &reg, &reg);
    return reg.h.al;
}

/* obtains the current video page */
unsigned char Getpage()
{
    union REGS reg;

    reg.h.ah = GETMODE;
    int86(VIDEO, &reg, &reg);
    return reg.h.bh;
}

/* moves cursor one column left, but not past */
/* the given limit                            */
unsigned char Curslt_lim(limit)
unsigned char limit;
{
    unsigned char row, col, page;
    unsigned char status = 1;

    Getcurs(&row, &col, page = Getpage());
    if (col > limit)
        Setcurs(row, col - 1, page);
    else
        status = 0;
    return status;
}

/* moves cursor one column right, but not past */
/* the given limit                             */
unsigned char Cursrt_lim(limit)
unsigned char limit;
{
    unsigned char row, col, page;
    unsigned char status = 1;

    Getcurs(&row, &col, page = Getpage());
    if (col < limit)
        Setcurs(row, col + 1, page);
    else
        status = 0;
    return status;
}

/* move cursor one row down, but not past */
/* the given limit                        */
unsigned char Cursup_lim(limit)
unsigned char limit;
{
    unsigned char row, col, page;
    unsigned char status = 1;

    Getcurs(&row, &col, page = Getpage());
    if (row > limit)
        Setcurs(row - 1, col, page);
    else
        status = 0;
    return status;
}

/* move cursor one row down, but not past */
/* the given limit                        */
unsigned char Cursdn_lim(limit)
unsigned char limit;
{
    unsigned char row, col, page;
    unsigned char status = 1;

    Getcurs(&row, &col, page = Getpage());
    if (row < limit)
        Setcurs(row + 1, col, page);
    else
        status = 0;
    return status;
}
