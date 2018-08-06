/* MAZE.C - From page 588 of "Microsoft C Programming for       */
/* the IBM" by Robert Lafore. This program used direct mem-     */
/* ory access to draw a maze on the screen.                     */
/****************************************************************/

#include <stdio.h>
#include <time.h>
#define MAXROWS 23
#define MAXCOLS 78
#define FALSE 0
#define TRUE 1
#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3
#define BLANK '\x20'    /* space */
#define HORZ '\xCD'     /* vertical line */
#define VERT '\xBA'     /* vertical pipe */
#define NONE '\x07'     /* impossible combination */
#define L_UP '\xBC'     /* L bend left to up */
#define L_DN '\xBB'     /* L bend left to down */ 
#define R_UP '\xC8'     /* L bend right to up */
#define R_DN '\xC9'     /* L bend right to down */
#define T_DN '\xCB'     /* T bend going down */
#define T_UP '\xCA'     /* T bend going up */
#define T_RT '\xCC'     /* T bend going right*/
#define T_LF '\xB9'     /* T bend going left*/
#define BORDER '\xB2'   /* grey square */
int lastd;
      /* going to: rite  left  up    down      coming from */
char table[4][4]={{HORZ, NONE, L_UP, L_DN},     /* left */
                  {NONE, HORZ, R_UP, R_DN},     /* right */
                  {R_DN, L_DN, VERT, NONE},     /* up */
                  {R_UP, L_UP, NONE, VERT} };   /* down */
main()
{
int col, row, j;
char getscr(), spot;

   cprintf("\x1B[2J");              /*clear screen*/
   border();
   col = 0; row = 12; lastd = RIGHT;   /*start at middle left*/
   randline(col, row);                 /*draw first line*/
   for(j = 1; !kbhit(); j++)  {        /*draw more lines*/
      do  {                            /*until key struck*/
         col = random(MAXCOLS - 1);    /*choose random spot*/
         row = random(MAXROWS - 1);
         spot = getscr(col, row);      /*find spot with line*/
      } while(spot != HORZ && spot != VERT); /*on it already*/
      tee(spot, col, row);       /*insert T in line, draw new line*/
      if(j == 200)               /*after 200 lines*/
         drawexit();
   }  /* end of for */
   goexit();                  /* all over */
}  /*end of main*/

/* randline() */  /* draws random line starting at x, y */
randline(x, y)
int x, y;
{
char trapped = FALSE;

   while (trapped == FALSE)  {
      switch (random(7))  {
         case 1:  /*draw line to right*/
         case 2:
         case 3:
         case 4:
            if(getscr(x + 1, y) == BLANK)
               turn(RIGHT, x++, y);
            break;
         case 5:     /*draw line to the left*/
            if(getscr(x - 1, y) == BLANK)
               turn(LEFT, x--, y);
            break;
         case 6:     /*draw line up (decrease y)*/
            if(getscr(x, y - 1) == BLANK)
               turn(UP, x, y--);
            break;
         case 7:     /*draw line down (increase y)*/
            if(getscr(x, y + 1) == BLANK)
               turn(DOWN, x, y++);
            break;
      }  /* end switch */
      /* see if this line trapped in cul-de-sac */
      if((getscr(x+1, y)!=BLANK) && (getscr(x-1, y) !=BLANK) &&
            (getscr(x, y+1)!=BLANK) && (getscr(x, y-1) !=BLANK))
         trapped = TRUE;         /*set flag*/
      if(kbhit() != 0)           /*if key struck*/
         goexit();
   }  /* end while */
}  /* end randline */

/* random() */    /* generates random number between 1 and rmax */
int random(rmax)
int rmax;
{
long seconds;
static int rand;

   time(&seconds);
   rand = 0x7fff & ((int)seconds + rand*273) >> 4;
   return((rand % rmax) + 1);
}

/* turn() */   /* function to print correct character to make turn */
               /* direction is where we're going */
               /* lastd is where we're coming from */
               /* turn characters are in array table */
turn(direction, x, y)
int direction, x, y;
{

   putscr( table[lastd][direction], x, y);
   lastd = direction;
}

/* border() */    /* draws border around screen */
border()
{
int j;

   for( j = 0; j < MAXCOLS + 2; j++)  {
      putscr(BORDER, j, 0);               /*draw  top*/
      putscr(BORDER, j, MAXROWS + 1);     /*draw bottom*/
   }
   for( j = 0; j < MAXROWS + 2; j++)  {
      putscr(BORDER, 0, j);               /*draw  right*/
      putscr(BORDER, MAXCOLS + 1, j);     /*draw left*/
   }
}

/* tee() */    /* inserts T in existing horiz or vert line at col, row */
tee(spot, col, row)
int col, row;
char spot;
{

   if (spot == HORZ)  {                   /* HORIZ LINE */
      if (getscr(col, row + 1) == BLANK)  {        /*go down*/
         putscr(T_DN, col, row);
         lastd = DOWN;
         randline(col, ++row);
      }
      else
         if (getscr(col, row - 1) == BLANK)  {        /*go up*/
            putscr(T_UP, col, row);
            lastd = UP;
            randline(col, --row);
         }
   }
   else  {                                      /* VERT LINE */
      if (getscr(col + 1, row) == BLANK)  {        /*go right*/
         putscr(T_RT, col, row);
         lastd = RIGHT;
         randline(++col, row);
      }
      else
         if (getscr(col - 1, row) == BLANK)  {        /*go left*/
            putscr(T_LF, col, row);
            lastd = LEFT;
            randline(--col, row);
         }
   }  /* end else */
}  /* end tee */

/* drawexit() */  /* draws exit line on right edge of maze */
                  /* looks for vertical line to start on */
drawexit()
{
int j;

               /* look for vert line */
   for(j = 6; getscr(MAXCOLS, j) != VERT; j++)
      ;
   putscr(T_RT, MAXCOLS, j);           /* insert right tee */
   putscr(HORZ, MAXCOLS + 1, j);       /* add horiz line */
}

/* goexit() */    /* puts cursor in lower right corner & exits */
goexit()
{

   printf("\x1B[%d;%dH", 24, 1);       /* move cursor */
   exit();
}

/* putscr() */    /* puts char on screen at location col, row */
                  /* (cols 0 to 79, rows 0 to 24) */
putscr( ch, col, row)
char ch;
int col, row;
{
int far *farptr;
int attr = 0x0700;         /* attribute for normal text */

   farptr = (int far *) 0xB0000000;    /* start of screen mem */
   *(farptr + row*80 + col) = ((int)ch & 0x00ff) | attr;
}

/* getscr */   /* returns char stored in screen mem */
               /* at location col, row */
char getscr(col, row)
int col, row;
{
int far *farptr;
char ch;

   farptr = (int far *) 0xB0000000;
   ch = *(farptr + row*80 + col);
   return(ch);
}

