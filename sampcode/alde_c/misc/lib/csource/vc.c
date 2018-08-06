/*         VC.C    VC.C    VC.C    VC.C    VC.C    VC.C    VC.C

VISUAL CALCULATOR                           X   X   XXX
  MAIN PROGRAM                              X   X  X   X
                                            X   X  X
July 1, 1987                                 X X   X
                                             X X   X
                                              X    X   X
                                              X     XXX

     This program will evaluate single value expressions in a
     manner similar to those evaluated by a hand-held calculator,
     hence its name, the Visual Calculator. It was never intended
     to be programmable, so no loop constructs are included in
     its design. It is possible to write a series of statements,
     store them in a file, and recall them while using them to
     calculate with new values of input variables in the six
     variable storage registers. The input variables can be
     changed, and the entire series recalculated.

     Although this is a potentially useful program in its own
     right, it was originally written as an illustration of a
     rather large C program. It is especially useful because
     the student of C can run the program to determine its
     operating characteristics, then study the code needed to
     perform the various operations. For that reason, the entire
     program is heavily commented. An actual production program
     would probably not have as many comments as this example
     but it would not be bad practice to comment all of your
     programs to this extent.
*/
#include "ctype.h"
#include "stdio.h"
#include "string.h"
#include "conio.h"
#include "process.h"
#include "struct.def"
#include "defin.h"

struct vars allvars[12]; /* this is the main variable storage */
int varinuse = 0;        /* which variable is being used now  */
char inline[200];        /* input line area                   */
int col;                 /* used for searching across the input */
int errcode;             /* error code number                 */
int colerr;              /* column where error occurred       */
int printit = 0;         /* 1 = print a transcript            */
int ignore;              /* 1 = ignore calculations for line  */
extern char strngout[];  /* output message area               */
extern int valattr;      /* value and variable attribute      */
extern int helpattr;     /* help box attribute                */
FILE *prtfile;           /* file pointers                     */

struct lines *top, *bot, *q, *p, *arrow, *trnsend;

/* *********************************************************** main */
/* This is the main control loop for the program. It initializes    */
/* everything and reads statements until no errors are found. It    */
/* continues reading until an F10 is detected in a subordinate      */
/* function where control is returned to DOS                        */
main()
{
   top = bot = q = p = arrow = trnsend = NULL;
   monitor();                 /* initialize video attributes */
   initdata(&allvars[0]);   /* initialize all data           */
   bkgndvid();   /* display video background - double lines  */
   valusvid();   /* display starting values of all variables */
   strtrans("Welcome to the Visual Calculator - Version 1.10",0);
   transout();
   do{
      poscurs(23,7);
      printf("  input >                                 ");
      printf("                              ");
      do {                     /* repeat input until no errors      */
         readline();           /* get an input line                 */
         errdis("                         ");  /* clear error msg   */
         parse();              /* parse the line                    */
         if (errcode) errout();/* output error message              */
      } while (errcode);
      if (ignore == 1)
         strtrans(inline,0);   /* store comment in transcript       */
      else
         strtrans(inline,1);   /* store "inline" in transcript      */
      transout();
   } while (1);                /* continuous loop                   */
}
/* ******************************************************* readline */
/* This function reads a line by inputting one character at a time  */
/* and deciding what to do with it if it is a special character, or */
/* adding it to the input line if it is a special character. The    */
/* routine takes care of such things as backspace, cursor movement  */
/* and delete keys. The final result is a single line of text stored*/
/* in the buffer "inline" and the line displayed on the monitor.    */
void readline(void)
{
int index;
int c,temp;
int row = 23,col = 17;
int attr;

   if (errcode) {                   /* error recovery allow reenter */
      index = colerr;
      errcode = 0;
   }
   else {                                   /* normal input routine */
      index = 0;
      for (temp = 0;temp < 80;temp++)
         inline[temp] = 0;                    /* clear input buffer */
   }

   poscurs(row,col+index);           /* starting location of cursor */

   do {                /* repeat this do loop until a return is hit */
      while ((c = getch()) == EOF);              /* get a keystroke */

      if (c == 0) {       /* a zero here says a special key was hit */
                             /* get the key and act on it as needed */
         int spec;
         spec = getch();          /* this is the special code found */
         switch (spec) {
            case 59 : helpm();                 /* F1 - Help math    */
                      transout();
                      break;
            case 60 : helps();                 /* F2 - Help system  */
                      transout();
                      break;
            case 61 : if (printit) {           /* F3 - Print on/off */
                         printit = 0;          /* print off */
                         fprintf(prtfile,"%s\n\n","Print off");
                         fclose(prtfile);
                         strcpy(strngout,"-----");
                         attr = helpattr;
                      } else {
                         prtfile = fopen("PRN","w");   /* print on */
                         if (prtprblm()) {
                            errcode = 12;   /* printer is not ready */
                            errout();
                            break;
                         }
                         printit = 1;
                         fprintf(prtfile,"%s\n","Print On");
                         strcpy(strngout,"Print");
                         attr = valattr;
                      }
                      strngdis(1,73,attr);
                      break;
            case 62 :                       /* F4 - Mark transcript */
                      arrow->marked = (arrow->marked?0:1);
                      transout();
                      break;
            case 63 : fileout();           /* F5 - Store transcript */
                      break;
            case 64 : filein();             /* F6 - Retrieve trans  */
                      errcode = 0;
                      break;
            case 65 :                       /* F7 -                 */
                      break;
            case 66 :                       /* F8 -                 */
                      break;
            case 67 :                       /* F9 - Edit a line     */
                      strcpy(inline,arrow->lineloc);
                      poscurs(23,17);
                      printf("%s",inline);
                      break;
            case 68 : poscurs(23,17);        /* F10 - Quit to DOS   */
                      printf("Quit? (Y/N) ");
                      c = getch();
                      if ((c == 'Y') || (c == 'y')){
                         clrscrn();
                         exit(0);
                      }
                      poscurs(23,17);
                      printf("              ");
                      break;
            case 75 : if (index) {                    /* left arrow */
                         index = index -1;        /* back up cursor */
                      }
                      break;

            case 77 : if (index < 65) {              /* right arrow */
                         if (inline[index] == 0)      /* zero found */
                            inline[index] = ' ';    /* blank over 0 */
                         index = index + 1;       /* cursor forward */
                      }
                      break;

            case 72 : movarrow(-1);                     /* up arrow */
                      break;

            case 80 : movarrow(1);                    /* down arrow */
                      break;

            case 73 : movarrow(-8);                      /* page up */
                      break;

            case 81 : movarrow(8);                     /* page down */
                      break;

            case 71 : movarrow(-1000);                      /* home */
                      break;

            case 79 : movarrow(1000);                        /* end */
                      break;

            case 83 : temp = index;                   /* delete key */
                              /* move all characters left one space */
                      while (inline[temp]) {
                         inline[temp] = inline[temp+1];
                         putchar(inline[temp++]);
                      }
                      putchar(0);             /* zero in last place */
                      break;

            default : poscurs(15,5);
                      printf(" S%3d",spec);
         }
         poscurs(row,col+index); /* actually put cursor in position */
      }

      else {                           /* normal letter or char hit */
         int curr,next;
         if (islower(c)) c = toupper(c);   /* convert to upper case */
         if ((c >= '\40') && (c <= '\176')) {     /* printable char */
            poscurs(row,col+index);
            putchar(c);
            next = inline[index];
            inline[index++] = c;
            curr = index;
            while((next != 0) && (curr <= 65)) {  /* move remainder */
               temp = next;                           /* line right */
               next = inline[curr];
               inline[curr++] = temp;
               putchar(temp);
            }
         }
         else {
            if ((c == 8) && index){                    /* backspace */
               index--;
               poscurs(row,col+index);            /* back up cursor */
               temp = index;
               while (inline[temp]) {
                  inline[temp] = inline[temp+1];
                  putchar(inline[temp++]);
               }
               putchar(0);
            }
         }
         poscurs(row,col+index);
      }
      if (c == 3) exit(0);                /* ctrl-break, out to DOS */
   } while (c != 13);      /* newline found, line input is complete */
}

/* ********************************************************** parse */
/* This function does a lot of checking of the input line for       */
/* logical errors in construction, then turns control over to the   */
/* function "calcdata" for the actual calculations.                 */
void parse(void)
{
int index,parcol;
double newval;
char name[7];

   varinuse = -1;
   errcode = 0;
   col = 0;
   ignore = 1;                             /* ignore this line      */

   if (inline[0] == '#') {            /* get list of variable names */
      getnames();
      return;
   }

   while (inline[col] == ' ') col++;       /* ignore leading blanks */
   if (inline[col] == '$') return;         /* ignore a comment line */
   if (inline[col] == 0) return;             /* ignore a blank line */
   ignore = 0;                            /* don't ignore this line */

   name[0] = inline[col++];                   /* find variable name */
   index = 1;
   while ((((inline[col] >= 'A') && (inline[col] <= 'Z')) ||
          ((inline[col] >= '0') && (inline[col] <= '9'))) &&
          (index <= 5)) {          /* continue var or function name */
      name[index++] = inline[col++];
   }
   name[index] = 0;                                   /* name found */

   for (index = 0;index < 12;index++) {
      if ((strcmp(name,allvars[index].varname)) == 0)
         varinuse = index;                   /* variable name found */
   }
   if (varinuse < 0) errchk(3);            /* unknown variable name */

   while (inline[col] == ' ') col++;       /* ignore leading blanks */
   if (inline[col] == '=') col++;
      else errchk(8);                         /* missing equal sign */

   parcol = 0;         /* now check for correct parenthesis matchup */
   index = col;
   do {
      if (inline[col] == '(') parcol++;
      if (inline[col++] == ')') parcol--;
      if (parcol < 0) errchk(1);       /* paren count went negative */
   } while (inline[col]);
   if (parcol) errchk(2);                  /* left over parentheses */
   col = index;

   calcdata(&newval);        /* now go evaluate the full expression */
   if (errcode == 0) {         /* don't update value if error found */
      allvars[varinuse].value = newval;
      disnew(varinuse);                /* display the changed value */
   }
}

/* ********************************************************* errout */
/* This is the function that displays the blinking error message on */
/* the monitor. Note the extra errors for expansion of the table.   */
void errout(void)
{
   switch (errcode) {
      case  1 : errdis("extra right parentheses  ");
                break;
      case  2 : errdis("missing right parentheses");
                break;
      case  3 : errdis("unknown variable name    ");
                break;
      case  4 : errdis("invalid math operator    ");
                break;
      case  5 : errdis("negative value for SQRT  ");
                break;
      case  6 : errdis("function not found      ");
                break;
      case  7 : errdis("negative value for LOG   ");
                break;
      case  8 : errdis("equal sign missing       ");
                break;
      case  9 : errdis("invalid data field       ");
                break;
      case 10 : errdis("division by zero         ");
                break;
      case 11 : errdis("File doesn't exist       ");
                break;
      case 12 : errdis("Printer not ready        ");
                break;
      case 13 : errdis("Out of memory            ");
                break;
      case 14 : errdis("Dash expected            ");
                break;
      case 15 : errdis("Invalid format code      ");
                break;
      case 16 : errdis("Neg value for FACTORIAL  ");
                break;
      case 17 : errdis("Err 17                   ");
                break;

      default : errdis("unknown error            ");
                poscurs(21,70);
                printf("%d",errcode);
   }
   poscurs(23,12+colerr);
}
