/* DFILL2.C - From page 353 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. Uses direct memory access to      */
/* fill the screen. To quit, enter X.                           */
/****************************************************************/

#define ROMAX 25
#define COMAX 80

main()
{
int far *farptr;
int col, row;
char ch;

   printf("\nType a character to start, another to change, X to exit.");
   farptr = (int far *) 0xB0000000;
   while((ch = getche()) != 'X')
      for(row = 0; row < ROMAX; row++)
         for(col = 0; col < COMAX; col++)
            *(farptr + row*COMAX + col) = ch | 0x0700;
}

