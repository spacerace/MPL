/* PUTMENUS.C - Program to print menus on the screen. Typed     */
/* from the book "Microsoft C Programming for the IBM" by Rob-  */
/* ert Lafore, page 264.                                        */
/****************************************************************/

#define SIZE1 5               /* Items on menu1 */
#define SIZE2 4               /* Items on menu2 */
#define CLEAR "\x1B[2J"       /* Clears screen  */

main()
{
static char *menu1[] =        /*first menu*/
            {  "Open",
               "Close",
               "Save",
               "Print",
               "Quit"   };
static char *menu2[] =        /*second menu*/
            {  "Cut",
               "Copy",
               "Paste",
               "Reformat"  };
   printf(CLEAR);              /*Clear screen*/
   display(menu1, SIZE1, 20);    /*Display first menu*/
   display(menu2, SIZE2, 40);    /*Display second menu*/
   getch();
}

/* display() */
/* Displays menu at given column number */
display(arr, size, hpos)
char *arr[];                  /*array to display*/
int size;                     /*size of array*/
int hpos;                     /*column number*/
{
int j;

   for(j = 0; j < size; j++) {         /*for each menu item*/
      printf("\x1B[%d;%df", j + 1, hpos);    /*position cursor*/
      printf("%s\n", *(arr + j));            /*print item*/
   }
}
