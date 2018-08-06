/*MENU.C - Menu program from "Microsoft C Programming for the   */
/*IBM" by Robert Lafore, page267. Use the up and down arrow     */
/*keys to move in the menu and the INSert ket to print the menu */
/*item.                                                         */
/****************************************************************/

#define TRUE 1
#define NUM 5                 /*Number of menu items*/
#define CLEAR "\x1B[2J"       /*Clear screen*/
#define ERASE "\x1B[K"        /*Erase line*/
#define NORMAL "\x1B[0m"      /*Normal attribute*/
#define REVERSE "\x1B[7m"     /*Reverse video attribute*/
#define HOME "\x1B[1;1f"      /*Cursor to top left*/
#define BOTTOM "\x1B[20;1f"   /*Cursor to lower left*/
#define U_ARRO 72             /*up-arrow key*/
#define D_ARRO 80             /*down-arrow key*/
#define INSERT 82             /*"Ins" key*/

main()
{
static char *items[NUM] = {         /*menu items*/
            "Open",
            "Close",
            "Save",
            "Print",
            "Quit"      };
int curpos;                /*position of selected item*/
int code;
printf(CLEAR);             /*clear screen*/
curpos = 0;                /*select top of menu*/

   while (TRUE) {
      display(items, NUM, curpos);     /*display menu*/
      code = getcode();                /*check keyboard*/
      switch (code) {         /*act on key pressed*/
               case U_ARRO:
                  if (curpos > 0)
                     --curpos;
                     break;
               case D_ARRO:
                  if (curpos < NUM - 1)
                     ++curpos;
                     break;
               case INSERT:
                  action(curpos);
                  break;
      }
   }
}

/* display() */
/* displays menu */
display(arr, size, pos)
char *arr[];
int size, pos;
{
int j;

   printf(HOME);                    /*cursor to top left */
   for(j = 0; j < size; j++) {      /*for each menu item*/
      if(j == pos)                  /*if selected*/
         printf(REVERSE);           /*print in reverse video*/
      printf("%s\n", *(arr + j));   /*print item*/
      printf(NORMAL);               /*restore normal attribute*/
   }
   printf(BOTTOM);                  /*cursor to lower left*/
}

/* getcode() */
/* gets keyboard code */
getcode()
{
int key;

   while(getch() != 0)              /*wait for initial 0*/
      ;                             /*ignores normal keys*/
   return(getch());                 /*return code*/
}

/* action() */
/* performs action based on cursor position */
action(pos)
int pos;
{
   printf(ERASE);                   /*erase lower line*/
   switch(pos) {
            case 0:                 /*calls to routines*/
               printf("Open");
               break;
            case 1:                 /*could be inserted here*/
               printf("Close");
               break;
            case 2:
               printf("Save");
               break;
            case 3:
               printf("Print");
               break;
            case 4:
               exit();
   }
}

