/* SAVEIMAG.C - From page 582 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. Writes a screen image to a file.  */
/* Usage:   A>saveimag filename.ext   where filename.ext is     */
/* the file of the screen image you want to create. This pro-   */
/* gram doesn't work exactly right. It skips the first line of  */
/* the screen and puts in the command line that is used to      */
/* invoke this program in the file. The screen is moved up one  */
/* line when this program is invoked and that's whats on the    */
/* screen when this program takes over, not the exact original  */
/* screen. I'm unable to figure out how to fix it. Try your     */
/* luck.                                                        */
/****************************************************************/

#include <stdio.h>
#define SPACE '\x20'

main(argc, argv)
int argc;
char *argv[];
{
FILE *fptr;
int col, row, lastchar, whatchar();
char ch, buff[80];               /*buffer to hold one row*/

   if (argc != 2)  {
      printf("\nType \"saveimag filename\".");
      exit();
   }
   if((fptr = fopen(argv[1], "w")) == NULL)  {
      printf("\nCan't open file %s.", argv[1]);
      exit();
   }
   for(row = 1; row < 25; row++)  {
      lastchar = 0;              /*last nonspace char*/
      for(col = 1; col < 80; col++)  {
         ch = whatchar(col, row);
         buff[col - 1] = ch;
         if(ch != SPACE)         /*remember col # of last*/
            lastchar = col;      /* nonspace character*/
      }
      buff[++lastchar - 1] = '\n';     /*insert \n and \0 after*/
      buff[++lastchar - 1] = '\0';     /*last nonspace char    */
      fputs(buff, fptr);               /*write row to file*/
   }
   fclose(fptr);
}

/* wharchar() */  /* returns char at screen location col, row */
                  /* cols from 1 to 80, rows from 1 to 25 */
int whatchar(col, row)
int col, row;
{
int far *farptr;
char ch;

   farptr = (int far *) 0xB0000000;
   ch = *(farptr + (row - 1)*80 + col - 1);
   return(ch);
}

