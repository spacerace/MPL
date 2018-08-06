/****************************************************************/
/* TWOAGENT.C - From page 292 of the book "Microsoft C Program- */
/* ming for the IBM by Robert Lafore. This is the beginning of  */
/* a simple database program developed from the above page to   */
/* the end of the chapter (9).                                  */
/****************************************************************/

#include "stdio.h"

main()
{
struct personnel {
   char name [30];
   int agnumb;
};
struct personnel agent1;
struct personnel agent2;

   printf("\nAgent 1:\nEnter name: ");
   gets(agent1.name);
   printf("Enter agent number (3 digits): ");
   scanf("%d", &agent1.agnumb);
   fflush(stdin);                   /*clear buffer*/
   printf("\nAgent 2:\nEnter name: ");
   gets(agent2.name);
   printf("Enter agent number (3 digits); ");
   scanf("%d", &agent2.agnumb);

   printf("\nList of agents:\n\n");
   printf("   Name: %s\n", agent1.name);
   printf("   Agent number: %03d\n", agent1.agnumb);
   printf("   Name: %s\n", agent2.name);
   printf("   Agent number: %03d\n", agent2.agnumb);
}


