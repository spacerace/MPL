/*
	HEADER:		CUG236;
	TITLE:		Compare Text Files;
	DATE:		05/17/1987;
	DESCRIPTION:	"Best version of DIFF (file comparator) from Jan '86
			issue of Software Practice and Experience.";
	VERSION:	1.1;
	KEYWORDS:	File Comparator, File Compare, File Comparison, File
			Comparison Utility;
	FILENAME:	FCOMP.C;
	SEE-ALSO:	DIFF;
	COMPILERS:	vanilla;
	AUTHORS:	Chuck Allison;
*/

/* fcomp.c:	file comparator that beats DIFF! */

#include <stdio.h>

/*
 * Portability Note:  8-bit systems often don't have header file string.h.
 * If your system doesn't have it, uncomment the following #define.
 */

/*
#define NO_STRING_H
*/

/*
 * Portability Note:  Back in K & R days, standard library function malloc()
 * was called alloc().	Some compilers (e.g. Eco-C under CP/M) haven't made
 * the name change.  If yours is one of these compilers, uncomment the
 * following #define:
 */

/*
#define malloc(p)	alloc(p)
*/

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * pair of #defines if you are running AZTEC C:
 */

/*
#define getc(f)		agetc(f)
#define putc(c,f)	aputc(c,f)
*/

#ifdef	NO_STRING_H
int strcmp(), strlen();
#else
#include <string.h>
#endif

#define MAXLINES 1000
#define ORIGIN MAXLINES
#define INSERT 1
#define DELETE 2

struct edit {
   struct edit *link;
   int op;
   int line1;
   int line2;
};

char *A[MAXLINES], *B[MAXLINES];

void exit();

void main(argc,argv)
int argc;
char *argv[];
{
   int	 col, d, k, lower, m, max_d, n, row, upper;
   int last_d[2*MAXLINES+1];
   struct edit *new, *script[2*MAXLINES+1];
   char *malloc();
   int atoi(), in_file();
   void exceed(), fatal(), put_scr();

   if (argc > 1 && argv[1][0] == '-')
   {
      max_d = atoi(&argv[1][1]);
      ++argv;
      --argc;
   }
   else
      max_d = 2*MAXLINES;

   if(argc != 3)
      fatal("fcomp requires two file names.");

   m = in_file(argv[1],A);
   n = in_file(argv[2],B);

   for (row=0 ; row<m && row < n && strcmp(A[row],B[row]) == 0 ; ++row)
      ;
   last_d[ORIGIN] = row;
   script[ORIGIN] = NULL;
   lower = (row == m) ? ORIGIN + 1 : ORIGIN - 1;
   upper = (row == n) ? ORIGIN - 1 : ORIGIN + 1;
   if (lower > upper)
   {
      puts("The files are identical.");
      exit(0);
   }

   for (d = 1 ; d <= max_d ; ++d)
   {
      for ( k = lower ; k <= upper ; k +=2)
      {
	 new = (struct edit *) malloc(sizeof(struct edit));
	 if (new == NULL)
	    exceed(d);

	 if (k == ORIGIN-d || k != ORIGIN+d && last_d[k+1] >= last_d[k-1])
	 {
	    row = last_d[k+1]+1;
	    new->link = script[k+1];
	    new->op = DELETE;
	 }
	 else
	 {
	    row = last_d[k-1];
	    new->link = script[k-1];
	    new->op = INSERT;
	 }
	 new->line1 = row;
	 new->line2 = col = row + k - ORIGIN;
	 script[k] = new;

	 while(row < m && col < n && strcmp(A[row],B[col]) == 0)
	 {
	    ++row;
	    ++col;
	 }
	 last_d[k] = row;

	 if (row == m && col == n)
	 {
	    put_scr(script[k]);
	    exit(!0);
	 }
	 if (row == m)
	    lower = k+2;

	 if (col == n)
	    upper = k-2;
      }
      --lower;
      ++upper;
   }
   exceed(d);
}

int in_file(filename,P)
char *filename, *P[];
{
   char buf[100], *malloc(), *save, *b;
   FILE *fp;
   int lines = 0;
   void fatal();

   if ((fp = fopen(filename,"r")) == NULL)
   {
      fprintf(stderr, "Cannot open file %s.\n",filename);
      exit(!0);
   }

   while(fgets(buf,100,fp) != NULL)
   {
      if (lines >= MAXLINES)
	 fatal("File is too large for diff.");
      if ((save = malloc(strlen(buf)+1)) == NULL)
	 fatal("Not enough room to save the files.");
      P[lines++] = save;
      for (b = buf ; *save++ = *b++ ; )
	 ;
   }
   fclose(fp);
   return(lines);
}

void put_scr(start)
struct edit *start;
{
   struct edit *ep, *behind, *ahead, *a, *b;
   int change;

   ahead = start;
   ep = NULL;
   while (ahead != NULL)
   {
      behind = ep;
      ep = ahead;
      ahead = ahead->link;
      ep->link = behind;
   }

   while( ep != NULL)
   {
      b = ep;
      if (ep->op == INSERT)
	 printf("Inserted after line %d:\n",ep->line1);
      else
      {
	 do
	 {
	    a = b;
	    b = b->link;
	 } while (b!=NULL && b->op == DELETE && b->line1 == a->line1+1);

	 change = (b!=NULL && b->op == INSERT && b->line1 == a->line1);
	 if (change)
	    printf("\nChanged ");
	 else
	    printf("\nDeleted ");
	 if (a == ep)
	    printf("line %d\n",ep->line1);
	 else
	    printf("lines %d-%d:\n",ep->line1,a->line1);

	 do
	 {
	    printf(" %s",A[ep->line1-1]);
	    ep = ep->link;
	 } while (ep != b);

	 if (!change)
	    continue;
	 printf("To:\n");
      }
      do
      {
	 printf(" %s",B[ep->line2-1]);
	 ep = ep->link;
      } while (ep != NULL && ep->op == INSERT && ep->line1 == b->line1);
   }
}

void fatal(msg)
char *msg;
{
   fprintf(stderr,"%s\n",msg);
   exit(!0);
}

void exceed(d)
int d;
{
   fprintf(stderr,"The files differ in at least %d lines. \n",d);
   exit(!0);
}
