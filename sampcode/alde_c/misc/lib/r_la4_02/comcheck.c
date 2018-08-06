/* COMCHECK.C - From page 581 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. Checks for matching comment       */
/* symbols, reports first occurrence of mispairing.             */
/****************************************************************/

#include "stdio.h"
#define TRUE 1
#define FALSE 0

main(argc, argv)
int argc;
char *argv[];
{
FILE *fptr;
char ch;
int slash = FALSE;
int star = FALSE;
int comments = 0;
int line = 1;
void chcount(), finalcnt();

   if(argc != 2)  {
      printf("\nType \"comcheck filename\".");
      exit();
   }
   if((fptr = fopen(argv[1], "r")) == NULL)  {
      printf("\nCan't open file %s", argv[1]);
      exit();
   }
   while((ch = getc(fptr)) != EOF)  {
      switch(ch)  {
         case '*':
            star = TRUE;
            if(slash == TRUE)  {          /*found start comment?*/
               comments++;
               if(comments > 1)  {        /*unmatched start comm*/
                  printf("\nUnexpected start comment, line %d", line);
                  exit();
               }
               slash = star = FALSE;      /*reset flags*/
            }
            break;
         case '/':
            slash = TRUE;
            if(star == TRUE)  {           /*found end comment ?*/
               comments--;
               if(comments < 0)  {        /*unmatched end comment*/
                  printf("\nUnexpected end comment, line %d", line);
                  exit();
               }
               slash = star = FALSE;
            }
            break;
         case '\n':
            line++;
         default:
            slash = star = FALSE;
      }  /*end switch*/
   }  /*end while*/
   fclose(fptr);
   if(comments > 0)           /*open comment at EOF*/
      printf("\nUnmatched open comment.");
}

