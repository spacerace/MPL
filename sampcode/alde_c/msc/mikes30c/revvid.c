highlight(c)    /* Put character on screen in reverse video */
int	c;
{
    union REGS REG;

    REG.h.ah = 0x9;
    REG.h.al = c;
    REG.h.bl = 0x78;
    REG.h.bh = 00;
    REG.x.cx = 1;
    int86(0x10, &REG, &REG);

    return(REG.x.ax);

}

