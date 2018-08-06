/* READF.C - From page 441 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. This program reads a program      */
/* created by a file that writes formatted data to a string,    */
/* such as WRITEF.C which is on this disk.                      */
/****************************************************************/

#include <stdio.h>

main()
{
FILE *fptr;
char name[40];
int code;
float height;

   fptr = fopen("c:textfile.txt", "r");
   while(fscanf(fptr, "%s %d %f", name, &code, &height) != EOF)
      printf("%s %03d %.2f\n", name, code, height);
   fclose(fptr);
}

