/**********************************************************
 ***                                                    ***
 ***    Copyright (c) 1981 by David M. Fogg             ***
 ***                                                   ***
 ***            2632 N.E. Fremont                       ***
 ***            Portland, OR 97212                      ***
 ***                                                    ***
 ***        (503) 288-3502{HM} || 223-8033{WK}          ***
 ***                                                    ***
 ***    Permission is herewith granted for non-         ***
 ***    commercial distribution through the BDS C       ***
 ***    User's Group; any and all forms of commercial   ***
 ***    redistribution are strenuously unwished-for.    ***
 ***                                                    ***
 **********************************************************/

/*
          ---> TEXT FILE LISTING PROGRAM <---

   Coded by David M. Fogg @ New Day Computing Co., Portland, OR

         *** (C) COPYRIGHT 1980, New Day Computing Co. ***

   24 OCT 80: creation day
   5 Nov - add optional line numbering
   6 NOV - Chg to xpand tabs
   21 Nov: Filter Form Feeds
   8 Jan 81: add line folding & opt line length spec
   6 Mar: chg DEFLL->128; chg puts->printf 2 show actual DEFLL
   27 mar 1984: extensive revisions to convert this program to 
	Lattice c. and ms.dos. (Pete Mack - Simi Valley RCPM)

    USAGE: list filnam "Title String"  {list a single file}
           list [no arguments]   {list multiple files: title/filnam prompts}
                [no arguments] allows changing:
                                 line numbering   (default: NO)
                                 line length      (default: 70)
                                 initial formfeed (default: NO)

*/

#include "stdio.h"
#include "ctype.h"

#define MAXF   64       /* MAX # OF FILES */
#define MAXFNL 15       /* MAX FILNAM LENGTH (INCL NUL TERMINATOR) */
#define MAXLNS 60       /* MAX LINES/PAGE */
#define NSTARS 85       /* # OF ASTERISKS IN STAR LINE */
#define TABX    8       /* TAB EXPANSION MODULUS */
#define DEFLL  128      /* DEFAULT LINE LENGTH */
FILE *LST, *f1;

main (argc, argv)
int argc;
char *argv[];
{
   char fnbuf[MAXFNL*MAXF];      /* file name buffer */
   char *pfnbuf;                 /* pointo curr pos in fnbuf */
   char *filnam[MAXF];           /* file names (pointers) */
   char titbuf[100];             /* page title (usu. incl. date) buffer */
   char *titl;                   /* pointer to title */
   char stars[NSTARS+1];         /* asterisk line */
   int fno;                      /* file # being listed */
   int nfils;                    /* no. of files to list */
   int pagno;                    /* curr page # */
   int lino;                     /* curr line # on page */
   char iobuf[_BUFSIZ];          /* file I/O buffer */
   int filopn;                   /* holds fopen result */
   char lbuf[MAXLINE];           /* input line buffer */
   BOOL dilin;                   /* whether to disp line #s */
   int lines;                    /* tot lines in curr file */
   int linlen;                   /* maximum line length */
   char c;


   if((LST = fopen("PRN", "w")) == NULL){
	cprintf("I can't create PRN");
	exit(1);
   }

   pfnbuf = fnbuf;
   titl = titbuf + 2;
   *pfnbuf = (MAXFNL * MAXF) - 2;
   titbuf[0] = 98;

   setmem(stars, NSTARS, '*'); stars[NSTARS] = '\0';
   dilin = NO;
   linlen = DEFLL;

   if (argc > 1) {
      nfils = 1;
      filnam[0] = *++argv;
      if (argc > 2)
         titl = *++argv;
      else
         titbuf[0] = '\0';
   }
   else {
      dilin = getyn(0, 0, "\nLine Numbers");
      cprintf("\rLine Length (%d): \0", DEFLL);

	 cgets(lbuf);
	 linlen= atoi(lbuf);
	 if (linlen < 1)
		 linlen = DEFLL;

      cputs("\n\rTitle: ");
      cgets(titbuf);
      nfils = 0;
      do {
         cprintf("\nFile # %2d : ", nfils + 1);
         filnam[nfils] = (char *) cgets(pfnbuf);    
         pfnbuf += strlen(pfnbuf) + 1;
      }
	while (strlen(filnam[nfils++]) > 0 && nfils < MAXF);
      --nfils;
      if (nfils == 0)
	 {
         fputs("Nothing to do...quitting",stderr);
	 exit(1);
	 }
      cputs("\nWant a Formfeed? ");

      c = getch();
      putch(c);
      if (toupper(c) == 'Y')
	 putc(FFEED, LST);
	 putchar('\n');
   }


/*
         >>------> MAIN PRINT LOOP <------<<
*/

   for (fno = 0; fno < nfils; ++fno) {
	rewind(LST);
	if ((f1 = fopen(filnam[fno], "r")) == NULL) { 
         cprintf("*** File I/O Error: %-15s\n", filnam[fno]);
         continue;
      }
      pagno = 0; lines = 0;
      lino = MAXLNS + 3;
      cprintf("\n\r---> Now listing %-15s\n\r", filnam[fno]);
	while (fgets(lbuf,MAXLINE,f1) != 0) {
         if (lino > MAXLNS) {
            if (pagno == 0) {
               if (fno > 0) putc(FFEED, LST);
               fputs(stars, LST); fputs("\n", LST);
               lino = 1;
            }
            else {
               putc(FFEED, LST);
               lino = 0;
            }
            fprintf(LST, "\n---> Listing of: %-15s    ", filnam[fno]);
            fprintf(LST, "%-35s  ", titl);
            fprintf(LST, "Page %2d <---\n", ++pagno);
            lino += 2;
            if (pagno == 1) {
               fputs("\n", LST); fputs(stars, LST); fputs("\n", LST);
               lino += 2;
            }
            fputs("\n\n\n", LST); lino += 3;
         }
         if (dilin)
            fprintf(LST, "%4d: ", ++lines);
         oplin(lbuf, linlen, &lino);
         ++lino;
      } /* while (fgets... */

/*      fclose(iobuf); */
	fclose(f1);

   } /* for (fno... */

	fputs("\n\r",LST);
      fclose(LST);

/*   if (filopn != NULL) putc(FFEED, LST); */

} /* main() */


oplin (lbuf, len, lno)    /* --<O/P A LINE W/ TAB XPANSION>-- */
char lbuf[];
int len;
int *lno;
{
   int cpos;      /* curr printhead pos */
   int i;

   cpos = 0;
   for (i = 0; i < strlen(lbuf); ++i) {
      if (lbuf[i] == '\n')
         fputs("\n", LST);
      else if (lbuf[i] == '\t') {
         putc(' ', LST);
         while (++cpos % TABX) putc(' ', LST);
      }
      else if (lbuf[i] != FFEED) {
         putc(lbuf[i], LST);
         if (++cpos > len) {
            fputs("\\\n", LST);
            cpos = 0;
            ++*lno;
         }
      }
   }
}
/*------------------------------------------------------------*/
/* This function was originally designed to get an answer
   'yes' or 'no' at a specific xy coordinate..
    there is a current problem in that toupper doesnt appear
    to operate properly if we use the statement...
       ans = toupper(getch());
    consequently we have temporarily set up this scheme...(pm) */

BOOL getyn (x, y, prom)         /* --<PROMPT 4 Y/N ANSWER>-- */
int x,y;
char *prom;
{
   char ans;
   char anst;

/*   if (x != 0) toxy(x, y);    */ /* set x=0 2 skip cursor pos */

   cprintf("%s (Y/N)? ", prom);

   do {
      anst = getch() & 0x7f;
      ans = toupper(anst);
   } while (ans != 'Y' && ans != 'N');

   putch(ans); putch('\n');

   return (ans == 'Y' ? YES : NO);
}
/************************************************
 atoi function from K&R pagew 58. 
 atoi -- convert s to integer --
 rev 3-12-84 initial entry
*************************************************/
int atoi(s)
char s[];
{
	int i, n, sign;

	for( i=0; s[i] == ' ' || s[i] == '\n' || s[i] == '\t'; i++)
		; /* skip white space */
	sign = 1;
	if (s[i] == '+' || s[i] == '-')
		sign = (s[i++] == '+') ? 1  : -1;

	for (n = 0; s[i] >= '0' && s[i] <= '9' ; i++)
		n = 10 * n + s[i] - '0';
	return(sign * n);
}
