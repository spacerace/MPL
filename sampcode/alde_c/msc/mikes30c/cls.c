cls()         /* Clear the screen  */
{

    union REGS REG;

    REG.x.ax = 0x0600;
    REG.h.bh = 0x07;
    REG.x.cx = 0X0000;
    REG.x.dx = 0x184f;
    int86(0x10, &REG, &REG);

}


