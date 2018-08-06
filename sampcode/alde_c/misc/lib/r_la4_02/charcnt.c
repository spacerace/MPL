/* CHARCNT.C - From page 434 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. This file counts characters in    */
/* a file that is opened as a text file as opposed to a file    */
/* opened as a binary mode file. These 2 modes differ in the    */
/* way they handle the EOF and the CR/LF-newline. See CHARCNT2.C*/
/****************************************************************/

#include <stdio.h>

main(argc, argv)
int argc;
char *argv[];
{
FILE *fptr;
char string[81];
int count = 0;

   if(argc != 2) {
      printf("\nFormat: C>charcnt2 filename");
      exit();
   }
   if((fptr = fopen(argv[1], "r")) == NULL) {
      printf("\nCan't open file %s.", argv[1]);
      exit();
   }
   while(getc(fptr) != EOF)
      count++;
   fclose(fptr);
   printf("\nFile %s contains %d characters.", argv[1], count);
}



