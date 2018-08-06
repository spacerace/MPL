/* WRITER.C - From page 448 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. Writes structures to a file us-   */
/* ing the record I/O method of performing standard I/O. Ano-   */
/* ther name for record I/O is block I/O. Record I/O writes to  */
/* files in binary. NOTE!!!! This program will not link cor-    */
/* rectly with Quick C. It gives Fatal error L1101 & manual     */
/* recommends contacting Microsoft.                             */
/****************************************************************/

#include "stdio.h"

main()
{
struct {
   char name[40];
   int agnumb;
   float height;
} agent;
float dummy = 0.0;
FILE *fptr;

   if((fptr = fopen("agents.rec", "wb")) == NULL) {
      printf("\nCan't open file agents.rec");
      exit();
   }
   do {
      printf("\nEnter name: ");
      gets(agent.name);
      printf("Enter number: ");
      scanf("%d", &agent.agnumb);
      printf("Enter height: ");
      scanf("%f", &agent.height);
      fflush(stdin);                   /*flush keyboard buffer*/
      fwrite(&agent, sizeof(agent), 1, fptr);
      printf("Add another agent (y/n) ? ");
   } while(getche() == 'y');
   fclose(fptr);
}

