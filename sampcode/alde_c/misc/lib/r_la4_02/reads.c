/* READS.C - From page 437 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. Reads strings from a file,        */
/* TEXTFILE.TXT, and displays them on screen.                   */
/****************************************************************/

#include <stdio.h>

main()
{
FILE *fptr;
char string[81];

   fptr = fopen("c:textfile.txt", "r");
   while(fgets(string, 80, fptr) != NULL)   /*read string*/
      printf("%s", string);
   fclose(fptr);
}

