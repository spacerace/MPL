/* AGENT2.C - From page 309 of "Microsoft C Programming for the */
/* IBM by Robert Lafore. This is the final program of the data- */
/* base started on page 292. it uses linked lists.              */
/****************************************************************/

#include <stdio.h>           /*defines 'stdin'*/
#define TRUE 1
struct prs {
   char name[30];
   int agnumb;
   float height;
   struct prs *ptrnext;       /*ptr to next structure*/
};
struct prs *ptrfirst, *ptrthis, *ptrnew;

main()
{
char ch;

   ptrfirst = (struct prs *)NULL;         /*no input yet*/
   while (TRUE) {
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

/* newname() */      /*puts a new agent in the database*/
newname()
{
   ptrnew = (struct prs *) malloc(sizeof(struct prs));
   if (ptrfirst == (struct prs *)NULL)  /*if none already, save addr*/
      ptrfirst = ptrthis = ptrnew;
   else {                     /*not first item, go to end of list*/
      ptrthis = ptrfirst;     /* (start at begin ) */
      while (ptrthis->ptrnext != (struct prs *)NULL)
         ptrthis = ptrthis->ptrnext;      /*find next item*/
      ptrthis->ptrnext = ptrnew;          /*pt to new item*/
      ptrthis = ptrnew;                   /*go to new item*/
   }
   printf("\nEnter name: ");
   gets(ptrthis->name);
   printf("Enter number: ");
   scanf("%d", &ptrthis->agnumb);
   printf("Enter height: ");
   scanf("%f", &ptrthis->height);
   fflush(stdin);
   ptrthis->ptrnext = (struct prs *)NULL;    /*this is end*/
}

/* listall() */      /*lists all agents and data */
listall()
{
   if (ptrfirst == (struct prs *)NULL) {  /*if empty list*/
      printf("\nEmpty list.\n");
      return;
   }
   ptrthis = ptrfirst;           /*start at first item*/
   do {
      printf("\nName: %s\n", ptrthis->name);
      printf("Number: %03d\n", ptrthis->agnumb);
      printf("Height: %4.2f\n", ptrthis->height);
      ptrthis = ptrthis->ptrnext;      /*move to next item*/
   } while(ptrthis != (struct prs *)NULL);   /*quit on null ptr*/
}

