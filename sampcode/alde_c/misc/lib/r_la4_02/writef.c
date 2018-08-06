/* WRITEF.C - From page 441 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. This program writes formatted     */
/* data to a file. To use, enter data at prompt as follows:     */
/*   Bond 007 74.5 <return>.   To exit after entering desired   */
/* number of lines, enter at prompt:   x 0 0 <return>.          */
/* This program will not work correctly if the DOS type comm-   */
/* and is used to read it. To read the file, use READF.C, also  */
/* on this disk. The file WRITEF_A.C on this disk can be read   */
/* with the DOS type command.                                   */
/****************************************************************/

#include <stdio.h>

main()
{
FILE *fptr;
char name[40];
int code;
float height;

   fptr = fopen("c:textfile.txt", "w");
   do {
      printf("Type name, code number, and height: ");
      scanf("%s %d %f", name, &code, &height);
      fprintf(fptr, "%s %d %f", name, code, height);
   } while(strlen(name) > 1);              /* no name given ? */
   fclose(fptr);
}

