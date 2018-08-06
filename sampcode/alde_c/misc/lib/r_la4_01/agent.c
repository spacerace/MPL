/* AGENTS.C - From "Microsoft C Programming for the IBM, page   */
/* 300, by Robert Lafore. The program uses an array of struct-  */
/* ures to maintain a list of agents in memory. It is also a    */
/* continuation of the development of a simple database started */
/* on page 292.                                                 */
/****************************************************************/

#include <stdio.h>
#define TRUE 1

struct personnel {
   char name [30];
   int agnumb;
   float height;
};
struct personnel agent[50];      /*array of 50 structures*/
int n = 0;

main()
{
char ch;

   while(TRUE) {
      printf("\nType 'e' to enter new agent");
      printf("\n  'L' to list all agents: ");
      ch = getche();
      switch (ch) {
         case 'e':
            newname();
            break;
         case 'L':
            listall();
            break;
         default:
            puts("\nEnter only selections listed");
      }
   }
}

/* newname */
/* puts a new agent in the database */
newname()
{
   printf("\nRecord %d: \nEnter name: ", n + 1);
   gets(agent[n].name);
   printf("Enter agent number (3 digits): ");
   scanf("%d", &agent[n].agnumb);
   printf("Enter height in inches: ");
   scanf("%f", &agent[n++].height);
   fflush(stdin);
}

/* listall() */
/* lists all agents and data */
listall()
{
int j;

   if (n < 1)
      printf("\nEmpty list.\n");
   for(j = 0; j < n; j++) {
      printf("\nRecord number %d\n", j + 1);
      printf("   Name: %s\n", agent[j].name);
      printf("   Agent number: %03d\n", agent[j].agnumb);
      printf("   Height: %4.2f\n", agent[j].height);
   }
}

