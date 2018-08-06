/* LIFE.C - From page 576 of "Microsoft C Programming for       */
/* the IBM" by Robert Lafore. Conway's Game of Life.            */
/****************************************************************/

#define MAXCOL 80
#define MAXROW 50
#define ERASE "\x1B[2J"          /* code to erase screen */

main()
{
static char array[MAXCOL][MAXROW];  /* array echoes screen */
void putscr(), getinit();
char neigh, getscr();
int col, row;
int count = 0;                      /* generation counter */

   getinit();                       /* get initial cells */
   while (1)  {                     /* once per generation */
      for(col = 1; col < MAXCOL - 1; col++)  /*transfer screen*/
         for(row = 1; row < MAXROW - 1; row++)  /*to array*/
            array[col][row] = getscr(col, row);
      printf(ERASE);
      printf("\x1B[24;1H");         /* position cursor */
      printf("Generation %d", ++count);    /*print generation*/
      for(col = 1; col < MAXCOL - 1; col++)     /*for every cell*/
         for(row = 1; row < MAXROW - 1; row++)  {
            neigh =  array[col-1][row-1] +    /*find total no.*/
                     array[col  ][row-1] +      /* of neighbors*/
                     array[col+1][row-1] +
                     array[col-1][row  ] +
                     array[col+1][row  ] +
                     array[col-1][row+1] +
                     array[col  ][row+1] +
                     array[col+1][row+1];
            if(array[col][row] == 0)  {      /*if no cell*/
               if(neigh == 3)                /* & 3 neighbors*/
                  putscr(col, row);          /*a cell is born*/
            }
            else if(neigh == 2 || neigh == 3) /*if 2 or 3 neighbors*/
               putscr(col, row);             /*cell lives*/
         }                                   /*done one cell*/
      for(col = 0; col < MAXCOL; col++)      /*clear array*/
         for(row = 0; row < MAXROW; row++)
            array[col][row] = 0;
   }        /*end while*/  /*done generation*/
}        /*end main*/

/* getinit() */   /* gets initial elements, puts them on screen */
#define END 79
#define INSERT 82
#define C_UPUP 72
#define C_DOWN 80
#define C_LEFT 75
#define C_RITE 77
void getinit()
{
int col,row;
char keycode;
void putscr();

   printf(ERASE);
   printf("\x1B[22;1H");               /*position cursor*/
   printf("Move cursor with arrow keys.\n");
   printf("Place cell with [Ins] key, [End] when done.");
   col = 40;
   row = 25;
   while(keycode != END)  {            /*return on [End] */
      if(kbhit())                      /*true if key struck*/
         if(getch() == 0)  {           /*extended code*/
            switch((keycode = getch()))  {   /*read code*/
               case C_UPUP:
                  --row;
                  break;
               case C_DOWN:
                  ++row;
                  break;
               case C_RITE:
                  ++col;
                  break;
               case C_LEFT:
                  --col;
                  break;
               case INSERT:
                  putscr(col, row);
                  break;
            }     /*end switch*/
         }     /*end if*/
   }     /*end while*/
}     /*end getinit() */

/* putscr() */    /*puts small square in screen mem at col, row*/
                  /*cols from 0 to 79, rows from 0 to 49*/
#define TOP '\xDF'      /*upper half of char*/
#define BOT '\xDC'      /*lower half of char*/
#define BOTH '\xDB'     /*full char rectangle*/

void putscr(col, row)
int col, row;
{
char newch, ch;
int far *farptr;                    /*ptr to screen mem*/
int actrow;                         /*actual row num*/
int topbot;                         /*top or bott of row*/

   farptr = (int far *) 0xB0000000;
   actrow = row >> 1;               /*row divided by 2*/
   topbot = row & 0x0001;           /*odd or even row*/
   ch = *(farptr + actrow*80 + col);   /*get actual char*/
   if(topbot)                       /*if we're placing bot*/
      if(ch == TOP || ch == BOTH)
         newch = BOTH;
      else
         newch = BOT;
   else                             /*if we're placing top*/
      if(ch == BOT || ch == BOTH)
         newch = BOTH;
      else
         newch = TOP;
   *(farptr + actrow*80 + col) = (newch & 0x00ff) | 0x0700;
}

/* getscr() */    /*returns 1 or 0 from screen location col, row*/
                  /*cols from 0 to 79, rows from 0 to 49 */
#define TOP '\xDF'      /*upper half of char*/
#define BOT '\xDC'      /*lower half of char*/
#define BOTH '\xDB'     /*full char rectangle*/
#define TRUE 1
#define FALSE 0

char getscr(col, row)
int col, row;
{
char ch;
int far *farptr;                    /*ptr to screen mem*/
int actrow;                         /*actual row num*/
int bottom;                         /*top or bott of row*/

   farptr = (int far *) 0xB0000000;
   actrow = row >> 1;               /*row divided by 2*/
   bottom = row & 0x0001;           /*odd or even row*/
   ch = *(farptr + actrow*80 + col);   /*get actual char*/
   if(ch == BOTH)
      return(TRUE);
   else
      if((bottom && ch == BOT) || (!bottom && ch == TOP))
         return(TRUE);
      else
         return(FALSE);
}

