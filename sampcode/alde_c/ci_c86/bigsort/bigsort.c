/* Reverse Engineered UNIX (tm) inspired sort                       */
/* Copyright (c) Jim Gast, Naperville, IL    April, 1986	    */
/* NOT copyright, I hereby put this here thing in the PUBLIC DOMAIN */

/* SYNTAX:   bigsort [ -tx ] [+dd.dd[n][r]] [-dd.dd] . . .          */
/*			[infile] . . . -o [outfile]		    */
/* "-tx" specifies a new tab (delimeter) character (default is tab) */
/* "+5.6 -6.0" means that a key starts at the 5th delimeter plus    */
/*	six more characters, and ends at the sixth delimeter        */
/* "n" appended to any key specifier makes it a numeric comparison  */
/* "r" appended to any key specifier makes it a reverse comparison  */

/* Assuming a file that contained:
 *	accountname;name;address;balance;phone number
 *
 * Example record:
 *	0023Flint;Flintstone, Fred;43 Quarry Lane;120.00;355-9881
 *
 * SAMPLE sorts:
 *	bigsort people -o namesort
 *		bigsorts by name and puts the output in file "namesort".
 *	bigsort -t; +3n -4 +0 people
 *		sorts by balance (numerically), and accountname if the
 *		balance is the same. outputs to stdout.
 *		The "-t;" changes the tab (field delimeter) to semicolon.
 *		Note that +3n skips to the field after the 3rd delimeter,
 *		you might think of it as the 4th field.
 *	bigsort +0.4 people
 *		sorts by characters starting with the 5th (skips 4).
 */

/*  This program is SHAREWARE, an $8.00 donation puts you on my mailing
 *  list to receive the next version of this and my screen utilities.
 *
 *		Jim Gast
 *		16 N. Columbia
 *		Naperville, IL 60540
 */

/* Compiled under CI-86, and depends on the CI-86 routine qsort()   */
/* To sort any decent size file, compile with BIG model		    */

/* up to MAXKEYS keys can be specified  */
#define MAXKEYS		  20

/* up to MAXRECS records can be sorted  */
#define MAXRECS		5000

/* each record can be up to LINELEN long */
#define LINELEN		 512

/* there can be up to MAXFILES input files */
#define MAXFILES	 50

/* CI-86 is does not understand <stdio.h> */
#include "\lib\stdio.h"

#define STRING		1
#define NUMERIC		2

#define FORWARD		1
#define REVERSE		2

#define INFINITY	LINELEN

#define TAB		'\011'

struct key_struct {
	int k_lo_field;
	int k_lo_char;
	int k_hi_field;
	int k_hi_char;
	int k_comparison;
	int k_direction;
	} key_table[MAXKEYS], *lastkey;

int keypointers;
char tab;
char *rec[MAXRECS];
int numrecs;

char *passdigits();
char *keyptr();
FILE *fopen();
char *fgets();
char *alloc();
long atoi();

main (argc, argv)
 int argc;
 char *argv[];
 {
  int a;
  int hi;
  register char *arg;
  register struct key_struct *k;
  char *pdata;
  int r;
  char *p;
  char *infile_name[MAXFILES];
  char outfile[80];
  int infiles;
  int fi;
  FILE *sort_out;
  int comp();

  infiles = 0;
  numrecs = 0;
  *outfile = 0;
  tab = TAB;
  k = &key_table[-1];
  for (a = 1;a < argc; a++)
	{
	 arg = argv[a];
	 if (*arg != '-' && *arg != '+')
		{
		 /* This must be an input file name */
		 infile_name[infiles] = alloc(strlen(arg) + 1);
		 strcpy(infile_name[infiles], arg);
		 infiles++;
		 continue;
		}
	 if (*arg == '-' && arg[1] == 'o')
		{
		 if (arg[2] == 0) arg = argv[++a];
		 strcpy(outfile,arg);
		 continue;
		}
	 if (*arg == '-' && arg[1] == 't')
		{
		 tab = arg[2];
		 continue;
		}
	 if (*arg == '+' || k < &key_table[0])
		{
		 k++;
		 k->k_lo_field = 0;
		 k->k_lo_char  = 0;
		 k->k_hi_field = INFINITY;
		 k->k_hi_char  = 0;
		 k->k_comparison = STRING;
		 k->k_direction  = FORWARD;
		}

	 hi = (*arg++ == '-');

	 if (hi) k->k_hi_field = atoi(arg);
	    else k->k_lo_field = atoi(arg);
	 arg = passdigits(arg);

	 if (*arg == '.')
		{
		 arg++;
		 if (hi) k->k_hi_char = atoi(arg);
		    else k->k_lo_char = atoi(arg);
		 arg = passdigits(arg);
		}

	 /* handle letters appended after field specifiers */
	 while (*arg)
		{
		 switch (*arg)
			{
		    case 'n':  k->k_comparison = NUMERIC;
			       break;

		    case 'r':  k->k_direction = REVERSE;
			       break;

		    default:   fprintf(stderr,"bad specifier: %s\n",argv[a]);
		 	       fprintf(stderr,"bad letter:    %c\n",*arg);
			       break;
			}
		 arg++;
		}
	}

 /* if no keys were specified */
 if (k == &key_table[-1])
	{
	 k++;
	 k->k_lo_field = 0;
	 k->k_lo_char  = 0;
	 k->k_hi_field = INFINITY;
	 k->k_hi_char  = 0;
	 k->k_comparison = STRING;
	 k->k_direction = FORWARD;
	}

 lastkey = k;

 keypointers = sizeof (char *) * 2 * (lastkey - &key_table[-1]);

 if (*outfile != 0)
	{
	 sort_out = fopen(outfile,"w");
	 if (sort_out == NULL)
		{
		 fprintf(stderr, "bigsort: unable to open out file %s\n",outfile);
		 exit(1);
		}
	}
     else
	{
	 sort_out = stdout; 
	}

 if (infiles == 0)
	{
	 infiles = 1;
	 infile_name[0] = "";
	}

 for (fi = 0; fi < infiles; fi++) get_data(infile_name[fi]);

 qsort(rec, numrecs, sizeof (char *), comp);

 for (r = 0; r < numrecs; r++)
	{
	 pdata = rec[r] + keypointers;
	 fprintf(sort_out,"%s",pdata);
	}
  fclose(sort_out);
 }

char *passdigits(p)
 char *p;
  {
   while (*p >= '0' && *p <= '9') p++;
   return(p);
  }

int show_args()
  {
   register struct key_struct *k;

   printf("Tab is octal %4o\n",tab);

   for (k = &key_table[0]; k <= lastkey; k++)
	{
	 printf("\n");
	 printf("lo = %4d . %4d\n",k->k_lo_field, k->k_lo_char);
	 printf("hi = %4d . %4d\n",k->k_hi_field, k->k_hi_char);
	 printf("compare   = %s\n",
		(k->k_comparison == NUMERIC? "numeric" : "string"));
	 printf("direction = %s\n",
		(k->k_direction == FORWARD? "forward" : "reverse"));
	}
  }

char *keyptr(data, fld, chr)
 char *data;
 int fld;
 int chr;
  {
   int f, c;

   for (f = 0; f < fld; )
	{
	 if (*data == tab) f++;
	 if (*data == 0) break;
	 data++;
	}

   for (c = 0; c < chr; c++)
	{
	 if (*data == tab) break;
	 if (*data == 0) break;
	 data++;
	}
   return(data);
  }

int comp(a,b)
 char **a, **b;
  {
   register char *p, *q;
   register struct key_struct *k;
   char *p_end, *q_end;
   char **pp, **qq;
   int high, low;
   long pval, qval;

   pp = (char **) *a;
   qq = (char **) *b;

   for (k = &key_table[0]; k <= lastkey; k++)
	{
	 p = *pp++;
	 p_end = *pp++;
	 q = *qq++;
	 q_end = *qq++;

	 if (k->k_direction == FORWARD)
		{
		 high = 1;
		 low = -1;
		}
	 else
		{
		 high = -1;
		 low = 1;
		}

	 if (k->k_comparison == NUMERIC)
		{
		 pval = atoi(p);
		 qval = atoi(q);
		 if (pval > qval) return(high);
		 if (pval < qval) return(low);
		}

	 /* all other comparisons are string comparisons */
	 else while (1)
		{
		 if (p >= p_end)
			{
			 if (q >= q_end) break;
			    else return(low);
			}
		 if (q >= q_end) return(high);
		 if (*p < *q) return(low);
		 if (*p > *q) return(high);
		 p++; q++;
		}
	}
   return(0);
  }

/* get_data reads records in from a data file. If the file name is */
/* NULL (""), it reads from standard in */

int get_data(filename)
 char *filename;
 {
  register char *p;
  register int r;
  char buffer[LINELEN];
  char **pp;
  char *pdata;
  FILE *sort_in;
  struct key_struct *k;

  if (filename[0] != 0)
	{
	 sort_in = fopen(filename, "r");
	 if (sort_in == NULL)
		{
		 fprintf(stderr, "bigsort: unable to open %s\n",filename);
		 exit(1);
		}
	}
    else sort_in = stdin;

  for (r = numrecs; r < MAXRECS; r++)
	{
	 p = fgets(buffer,LINELEN,sort_in);
	 if (p == NULL) break;
	 rec[r] = alloc(strlen(buffer) + keypointers + 1);
	 pdata = rec[r] + keypointers;
	 strcpy(pdata, buffer);
	 pp = (char **) rec[r];
	 for (k = &key_table[0]; k <= lastkey; k++)
		{
		 *pp++ = keyptr(pdata,k->k_lo_field,k->k_lo_char);
		 *pp++ = keyptr(pdata,k->k_hi_field,k->k_hi_char);
		}
	}
  numrecs = r;

  if (filename[0] != 0) fclose(sort_in);
 }
