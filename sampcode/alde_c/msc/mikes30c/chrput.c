chr_put(c)    /* Put character with attribute in first 8 bits of int */
int	c;       /* and character in last 8 bits */
{
    union REGS REG;

    REG.h.ah = 0x9;
    REG.h.al = c;
    REG.h.bl = c >> 8;
    REG.h.bh = 00;
    REG.x.cx = 1;
    int86(0x10, &REG, &REG);

    return(REG.x.ax);

}

chr_get()   /* Get character with attribute returns 2byte int used above */
{
    union REGS REG;

    REG.h.ah = 0x8;
    REG.h.bh = 00;
    int86(0x10, &REG, &REG);

    return(REG.x.ax);

}

