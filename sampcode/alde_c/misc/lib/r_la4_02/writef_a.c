/* WRITEF_A.C - From page 441 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. This program writes formatted     */
/* data to a file. To use, enter data at prompt as follows:     */
/*   Bond 007 74.5 <return>.   To exit after entering desired   */
/* number of lines, enter at prompt:   x 0 0 <return>.          */
/* This program has been changed from what's in the book to     */
/* make it work correctly. The book version, WRITEF.C is also   */
/* on this disk to use in conjunction with READF.C              */
/****************************************************************/

#include <stdio.h>

main()
{
FILE *fptr;
char name[40];
int code;
float height;
int count = 0;

   fptr = fopen("c:textfile.txt", "w");
   while((strlen(name) > 1) || count == 0) { /* no name given ? */
      count++;
      printf("Type name, code number, and height: ");
      scanf("%s %d %f", name, &code, &height);
      if(strlen(name) > 1) {
         fprintf(fptr, "%-10s %03d %f", name, code, height);
         fprintf(fptr, "\n");
      }
   }
   fclose(fptr);
}

