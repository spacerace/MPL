/* MEMSIZE0.C - From page 323 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. It uses a ROM BIOS return to print*/
/* the size of memory in a machine.                             */
/****************************************************************/

#define MEM 0x12           /*BIOS interrupt number*/

main()
{
struct WORDREGS {          /*registers as 16 bit words*/
   unsigned int ax;
   unsigned int bx;
   unsigned int cx;
   unsigned int dx;
   unsigned int si;
   unsigned int di;
   unsigned int flags;
};

struct BYTEREGS {          /*registers as 8 bit bytes*/
   unsigned char al, ah;
   unsigned char bl, bh;
   unsigned char cl, ch;
   unsigned char dl, dh;
};

union REGS {               /*either bytes or words*/
   struct WORDREGS x;
   struct BYTEREGS h;
};

union REGS regs;           /*regs to be type union REGS*/
unsigned int size;

   int86(MEM, &regs, &regs);  /*call memory interrupt*/
   size = regs.x.ax;          /*get value from AX register*/
   printf("\nMemory size is %dK bytes\n", size);
}

