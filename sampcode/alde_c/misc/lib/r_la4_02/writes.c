/* WRITES.C - From page 437 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. Writes the file C:TEXTFILE.TXT,   */
/* typed at keyboard, to disk.                                  */
/****************************************************************/

#include <stdio.h>

main()
{
FILE *fptr;
char string[81];

   fptr = fopen("c:textfile.txt", "w");
   while(strlen(gets(string)) > 0) {
      fputs(string, fptr);             /*write string to file*/
      fputs("\n", fptr);
   }
   fclose(fptr);
}

