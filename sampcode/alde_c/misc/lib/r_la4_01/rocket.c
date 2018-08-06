/****************************************************************/
/* ROCKET.C - A revised program typed from the book "Microsoft C*/
/* Programming for the IBM" by Robert Lafore. The program is on */
/* page 241. ANSI.SYS must be set up to use. This program is a  */
/* slightly changed version of MOVE.C (page 241).               */
/****************************************************************/

#define ROWS 25
#define COLS 5
#define HOMECLR "\x1B[2J"
#define CUR_BOTT "\x1B[%d;%df"
#define CUR_TOP "\x1B[%d;%df"

main()
{
int count, j, k, m;
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
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
      { 0,        0,       0,       0,       0          },
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

   for(count = 0; count < ROWS - 1; count++) {  /*do ROWS-1 # of times*/
      printf(HOMECLR);
      printf(CUR_BOTT, 25,0);
      printf("%s", gnd);                   /*print ground*/
      printf(CUR_TOP, 0,0);
      for(j = 0; j < ROWS-1; j++) {   /*print screen*/
         for(k = 0; k < COLS; k++) {
            if(*(*(ptr + j) + k) != 0) {     /*is rocket on line?*/
               for(m = 0; m < COLS; m++)     /*Yes, print line*/
                  printf("%c", *(*(ptr + j) + m));
               break;
            }
         }
         printf("%c", '\n');
      }
      temp = *ptr;               /*rotate pointers to move rocket*/
      for(j = 0; j < ROWS - 1; j++)
         *(ptr + j) = *(ptr + j + 1);
      *(ptr + ROWS - 1) = temp;
   }
}

