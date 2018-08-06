#include <stdio.h>

int WhereY()
{
   struct
   {
      int AX, BX, CX, DX, SI, DI;
   } Regs;

   Regs.AX = 0x0300;
   Regs.BX = 0;
   int86(0x10, &Regs, &Regs);
   return (Regs.DX >> 8);
}
