/*PASSTWO.C - From page 298 of "Microsoft C Programming for the */
/* IBM" by Robert Lafore. This program demonstrates that the    */
/* value of a structure variable can be passed to a function as */
/* a parameter. Also a continuation of simple database started  */
/* on page 292.                                                 */
/****************************************************************/

#include <stdio.h>

struct personnel {            /*define data structure*/
   char name [30];
   int agnumb;
};

main()
{
struct personnel agent1;      /*declare structure variable*/
struct personnel agent2;      /*declare structure variable*/
struct personnel newname();   /*declare function*/

   agent1 = newname();        /*get data for 1st agent*/
   agent2 = newname();        /*get data for 2nd agent*/
   list(agent1);              /*print data for 1st agent*/
   list(agent2);              /*print data for 2nd agent*/
}

/* newname() */
/* puts a new agent in the database */
struct personnel newname()
{
struct personnel agent;             /*new structure*/

   printf("\nNew agent\nEnter name: ");
   gets(agent.name);
   printf("Enter agent number (3 digits): ");
   scanf("%d",&agent.agnumb);
   fflush(stdin);
   return(agent);
}

/* list() */
/* prints data on one agent */
list(age)
struct personnel age;               /*struct passed from main*/
{
   printf("\n\nAgent: \n");
   printf("   Name: %s\n", age.name);
   printf("   Agent number: %03d\n", age.agnumb);
}

