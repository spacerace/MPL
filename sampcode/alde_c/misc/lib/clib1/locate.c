#include <dos.h>
locate(row,col)
int row,col;
{
	struct HREG REGS;

	REGS.ah=02;
	REGS.bh=00;
	REGS.dh=row;
	REGS.dl=col;
	int86(0x10,&REGS,&REGS);


}
                                                                                         