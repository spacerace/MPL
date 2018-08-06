/****************************************************************/
/* MOVE.C - A rocket program typed from the book "Microsoft C   */
/* Programming for the IBM" by Robert Lafore. The program is on */
/* page 241. This program doesn't work like it should. See      */
/* the file ROCKET.C on this disk for one that works a little   */
/* better.                                                      */
/****************************************************************/

#define ROWS 10
#define COLS 5

main()
{
int count, j, k;
char *ptr[ROWS];           /*pointers to rows*/
char *temp;                /*pointer storage*/
static char pict[ROWS][COLS] = {       /* rocketship */
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       '\x1E',  0,       0          },
      { 0,        '\x1E',  '\x1E',  '\x1E',  0          }  };

static char gnd[] =        /* ground line */
            { '\xCD', '\xCD', '\xCD', '\xCD', '\xCD' };

   for(count = 0; count < ROWS; count++)     /*set up pointers*/
      *(ptr + count) = *(pict + count);

   for(count = 0; count < ROWS - 1; count++) {
      for(j = 0; j < ROWS; j++) {            /*print rocket*/
         for(k = 0; k < COLS; k++)
            if (*(*(ptr + j) + k) != 0) {
               for(k = 0; k < COLS; k++)
                  printf("%c", *(*(ptr + j) + k));
               printf("%c", '\n');
               break;
            }
      }
      printf("%s\n", gnd);                   /*print ground*/
      temp = *ptr;               /*rotate pointers*/
      for(j = 0; j < ROWS - 1; j++)
         *(ptr + j) = *(ptr + j + 1);
      *(ptr + ROWS - 1) = temp;
   }
}

