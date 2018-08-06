/* RANDR.C - From page 456 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. Reads one agent's record, sel-    */
/* ected by user, from database created by AGENTR.C             */
/****************************************************************/

#include "stdio.h"

main()
{
struct {
   char name[40];
   int agnumb;
   float height;
} agent;
FILE *fptr;
int recno;
long int offset;

   if ((fptr = fopen("c:agents.rec", "r")) == NULL) {
      printf("\nCan't open file agents.rec");
      exit();
   }
   printf("\nEnter record number: ");
   scanf("%d", &recno);
   offset = --recno * sizeof(agent);        /*find offset*/
   if(fseek(fptr, offset, 0) != 0) {      /*go there*/
      printf("\nCan't move pointer there.");
      exit();
   }
   fread(&agent, sizeof(agent), 1, fptr);
   printf("\nName: %s", agent.name);
   printf("\nNumber: %03d", agent.agnumb);
   printf("\nHeight: %.2f", agent.height);
   fclose(fptr);
}

