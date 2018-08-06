/****************************************************************/
/* TWINS.C - From page 296 of the book "Microsoft C Program-    */
/* ming for the IBM by Robert Lafore. This is a continuation of */
/* a simple database program developed from page 292 to         */
/* the end of the chapter (9).                                  */
/****************************************************************/

main()
{
struct personnel {
   char name [30];
   int agnumb;
};

struct personnel agent1;
struct personnel agent2;

   printf("\nAgent 1.\nEnter name: ");
   gets(agent1.name);
   printf("Enter agent number (3 digits): ");
   scanf("%d", &agent1.agnumb);

   agent2 = agent1;           /*assigns one structure to another*/

   printf("\nList of agents:\n\n");
   printf("   Name: %s\n", agent1.name);
   printf("   Agent number: %03d\n", agent1.agnumb);
   printf("   Name: %s\n", agent2.name);
   printf("   Agent number: %03d\n", agent2.agnumb);
}


