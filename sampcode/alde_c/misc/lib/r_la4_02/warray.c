/* WARRAY.C - From page 450 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. Writes an array to a file using   */
/* binary mode.                                                 */
/****************************************************************/

#include "stdio.h"
int table[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

main()
{
FILE *fptr;

   if((fptr = fopen("c:table.rec", "wb")) == NULL) {
      printf("\nCan't open file agents.rec");
      exit();
   }
   fwrite(table, sizeof(table), 1, fptr);
   fclose(fptr);
}

