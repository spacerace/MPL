/* BINDUMP.C - From page 446 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. Performs a binary dump of a file. */
/* In the book there is a semicolon that doesn't belong at the  */
/* end of the second if statement. Also there is a space after  */
/* the ">" character in the first printf statement that doesn't */
/* belong.                                                      */
/****************************************************************/

#include <stdio.h>
#define LENGTH 10 
#define TRUE 0
#define FALSE -1

main(argc, argv)
int argc;
char *argv[];
{
FILE *fileptr;
unsigned int ch;
int j, not_eof;
unsigned char string[LENGTH + 1];

   if(argc != 2) {
      printf("\nFormat: C>bindump filename");
      exit();
   }
   if((fileptr = fopen(argv[1], "rb")) == NULL) {
      printf("\nCan't open file %s.", argv[1]);
      exit();
   }
   not_eof = TRUE;                  /*not EOF flag*/
   do {
      for(j = 0; j < LENGTH; j++) {
         if((ch = getc(fileptr)) == EOF)     /*read character*/
            not_eof = FALSE;              /*clear flag on EOF*/
         printf("%3x ", ch);              /*print ASCII code*/
         if(ch > 31)
            *(string + j) = ch;           /*save printable char*/
         else                             /*use period for*/
            *(string + j) = '.';          /*nonprintable char*/
      }
      *(string + j) = '\0';               /*end string*/
      printf("   %s\n", string);          /*print string*/
   } while(not_eof == TRUE);              /*quit on EOF*/
   fclose(fileptr);
}

