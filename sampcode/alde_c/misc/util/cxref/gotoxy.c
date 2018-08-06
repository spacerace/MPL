#include <stdio.h>

void GotoXY(x, y)
int x, y;
{
   struct
   {
      int AX, BX, CX, DX, SI, DI;
   } Regs;

   Regs.DX = (y << 8) | (x & 0xFF);
   Regs.BX = 0;
   Regs.AX = 0x200;
   int86(0x10, &Regs, $Regs);
}
