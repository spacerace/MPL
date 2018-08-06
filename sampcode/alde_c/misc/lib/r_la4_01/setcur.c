/* SETCUR.C - From page 326 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. This program will set the cursor  */
/* size from smaller than normal to a size filling the entire   */
/* character block using ROM BIOS call 0x10 function 01. To     */
/* use:  A>setcur xx yy where xx and yy are each numbers from   */
/* 0 - 13. To get a normal cursor use: A>setcur 12 13           */
/* By making the first number larger than the second you will   */
/* get a two part cursor.                                       */
/****************************************************************/

#include "dos.h"        /*declares REGS*/
#define CURSIZE 1       /* "set cursor size service" of int 0x10*/
#define VIDEO 0x10      /*video BIOS interrupt number*/

main(argc, argv)
int argc;
char *argv[];
{
union REGS regs;
int start, end;

   if (argc != 3) {
      printf("\nExample usage: A>setcur 12 13   (gives normal cursor)");
      exit();
   }
   start = atoi(argv[1]);
   end = atoi(argv[2]);
   regs.h.ch = (char)start;      /*starting scan line number*/
   regs.h.cl = (char)end;        /*ending scan line number*/
   regs.h.ah = CURSIZE;          /*service number of int 0x10*/
   int86(VIDEO, &regs, &regs);   /*call video interrupt*/
}


