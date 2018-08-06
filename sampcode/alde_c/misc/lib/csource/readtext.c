                                         /* Chapter 10 - Program 4 */
#include "stdio.h"

main()
{
FILE *fp1;
char oneword[100];
char c;

   fp1 = fopen("TENLINES.TXT","r");

   do {
      c = fscanf(fp1,"%s",oneword); /* got one word from the file */
      printf("%s\n",oneword);       /* display it on the monitor  */
   } while (c != EOF);              /* repeat until EOF           */

   fclose(fp1);
}
