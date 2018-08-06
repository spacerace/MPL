/* AGENTR.C - From page 453 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. This is a simple database program.*/
/* I added 2 if statements to file in book. One is after else   */
/* statement in rfile() function. This prevents adding exist-   */
/* ing records to array (a duplication making the array twice   */
/* as large) when selecting 'r' while an array already exists.  */
/* It will then only read a file to array at program startup.   */
/* The second if statement added is in the first case statement.*/
/* This will prevent erasing an existing database by automat-   */
/* ically writing it to disk before selecting enter a name,     */
/* otherwise selecting enter before writing will start the new  */
/* database at n == 0 after opening the disk file.              */
/****************************************************************/

#include "stdio.h"
#define TRUE 1

struct personnel {                  /*define data structure*/
   char name [40];
   int agnumb;
   float height;
};
struct personnel agent[50];         /*array of 50 structures*/
int n = 0;

main()
{
char ch;

   while(TRUE) {
      printf("\n'e' Enter new agent\n'l' List all agents");
      printf("\n'w' Write file\n'r' Read file: ");
      ch = getche();
      switch(ch) {
         case 'e':
            if (n == 0)
               rfile();
            newname();
            break;
         case 'l':
            listall();
            break;
         case 'w':
            wfile();
            break;
         case 'r':
            rfile();
            break;
         default:
            puts("\nEnter only selections listed.");
      }
   }
}

/* newname */    /* puts a new agent in the database */
newname()
{

   printf("\nRecord %d.\nEnter name: ", n + 1);
   gets(agent[n].name);
   printf("\nEnter agent number (3 digits): ");
   scanf("%d", &agent[n].agnumb);
   printf("Enter height in inches: ");
   scanf("%f", &agent[n++].height);
   fflush(stdin);
}

/* listall() */   /*lists all agents and data */
listall()
{
int j;

   if(n < 1)
      printf("\nEmpty list.\n");
   for(j = 0; j < n; j++) {
      printf("\nRecord number %d\n", j + 1);
      printf("   Name: %s\n", agent[j].name);
      printf("   Agent number: %03d\n", agent[j].agnumb);
      printf("   Height: %4.2f\n", agent[j].height);
   }
}

/* wfile */    /* writes array of structures to file */
wfile()
{
FILE *fptr;

   if(n < 1) {
      printf("\nCan't write empty list.\n");
      return;
   }
   if((fptr = fopen("c:agents.rec", "wb")) == NULL)
      printf("\nCan't open file agents.rec\n");
   else {
      fwrite(agent, sizeof(agent[0]), n, fptr);
      fclose(fptr);
      printf("\nFile of %d records written.\n", n);
   }
}

/* rfile() */  /* reads records from file into array */
rfile()
{
FILE *fptr;

   if((fptr = fopen("c:agents.rec", "rb")) == NULL)
      printf("\nCan't open file agents.rec\n");
   else {
      if(n == 0)  /*Fill array only at start, otherwise will dup entire file*/
         while(fread(&agent[n], sizeof(agent[n]), 1, fptr) == 1)
            n++;
   fclose(fptr);
   printf("\nFile read. Total agents is %d.\n", n);
   }
}

