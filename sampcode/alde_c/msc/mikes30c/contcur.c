/**************************************************************************
   These routines are very useful in doing the job of cursor control for
   Microsoft C 3.0.  You must #include <dos.h>  for these to work.

          Mike's "C" Board   619-722-8724
**************************************************************************/


gotoxy(row, col)  /* Position cursor at x,y on screen */
int	row, col;
{
    union REGS REG;

    REG.h.ah = 02;
    REG.h.bh = 00;
    REG.h.dh = row;
    REG.h.dl = col;
    int86(0x10, &REG, &REG);
}


getxy(hold) /* Get cursor coordinate and attribute */
int	*hold;
{
    union REGS REG;

    REG.h.ah = 03;
    REG.h.bh = 00;
    int86(0x10, &REG, &REG);
    hold[0] = REG.h.dh;
    hold[1] = REG.h.dl;
    hold[2] = REG.h.ch;
    hold[3] = REG.h.cl;
}


restorxy(hold)  /* Restore cursor gotten above */
int	*hold;
{
    union REGS REG;

    gotoxy(hold[0], hold[1]);
    REG.h.ah = 01;
    REG.h.bh = 00;
    REG.h.ch = hold[2];
    REG.h.cl = hold[3];
    int86(0x10, &REG, &REG);

}


get_mode()   /* Check for Monocrome or graphics  */
{
    union REGS REG;

    REG.h.ah = 15;
    REG.h.bh = 00;
    REG.h.ch = 0;
    REG.h.cl = 0;
    int86(0x10, &REG, &REG);

    return(REG.h.al);
}


del_cur()  /* Make cursor disappear */
{
    union REGS REG;

    REG.h.ah = 01;
    REG.h.bh = 00;
    REG.h.ch = 0x26;
    REG.h.cl = 7;
    int86(0x10, &REG, &REG);
}


restor_cur()  /* Put cursor back on screen checking for adapter */
{
    union REGS REG;
    unsigned	st, en;

    if (get_mode() == 7)       /* Monocrone adapter */
        st = 0x0d, en = 14;
    else
        st = 0x06, en = 7;     /* Color graphics adapter */

    REG.h.ah = 01;
    REG.h.bh = 00;
    REG.h.ch = st;
    REG.h.cl = en;
    int86(0x10, &REG, &REG);


}


