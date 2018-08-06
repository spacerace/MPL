/* DFILL.C - From page 352 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. Uses direct memory access to      */
/* fill the screen. To quit, enter X.                           */
/****************************************************************/

#define LENGTH 2000

main()
{
int far *farptr;
int addr;
char ch;

   printf("\nType character to start, another to change, X to exit.");
   farptr = (int far *) 0xB0000000;
   while((ch = getche()) != 'X')
      for(addr = 0; addr < LENGTH; addr++)
         *(farptr + addr) = ch | 0x0700;
}

