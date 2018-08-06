/* CFILL.C - From page 354 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. Uses C putch() function to        */
/* fill the screen. To quit, enter X. The DOS file ANSI.SYS     */
/* must be on disk and accessable in your path and must be in   */
/* your CONFIG.SYS file as:  device = ANSI.SYS for this pro-    */
/* gram to work.                                                */
/****************************************************************/

#define ROMAX 25
#define COMAX 80
#define CLEAR "\x1B[2J"

main()
{
int col, row;
char ch;

   printf("\nType character to start, another to change, X to exit.");
   while((ch = getche()) != 'X') {
      printf(CLEAR);
      for(row = 0; row < ROMAX; row++)
         for(col = 0; col < COMAX; col++)
            putch(ch);
   }
}

