/* 
   This program indexes a text file. 
   author: Greg Haley
   revision date: 04/02/85
   A text file containing a word list is used as input.
   Each word to index on is on a seperate line. Example:
 	  byte
   	  directive
  	  SAMPLE
   An index file will be created using the words byte, directive, and sample
   to index on. The words may be entered in upper or lower case.
   Pages are seperated by the ^L character (ASCII form feed) in the original
   text.

   Usage: index input_pathname output_pathname wordlist_pathname
*/

#include <stdio.h>

#define version "1.00"	/* Current version */
#define w_length 30   	/* Maximum # of chars in a word
                     	   Be sure to change fscanf statement in the 
                           function get_list to match this number */

#define n_words 250	/* Maximum # of words to index on */
#define max_num 20	/* Maximum # of pages per word */
#define page_width 72   /* Number of columns to use for output */	
#define form_feed 12	/* ASCII form feed character */
#define space 32    	/* ASCII space character */
			

/* Global Variables */
char words [n_words] [w_length+1];	/* word list */
int w_page [n_words] [max_num];		/* page numbers */
int w_count [n_words];			/* count of page numbers */
int page;                		/* page counter */
int num_words;                          /* actual number of words to index */
char new_word [w_length+1];		/* buffer for 1 word */

/* Main Program */
main(argc, argv)
int argc;
char *argv[];
{
        FILE *infile, *outfile, *wordlist, *fopen();
	int a, b, i, n;

  /* initialize */
  fprintf(stderr, "INDEX v%s by Greg Haley\n\n", version);

  for (a=0; a <= n_words; a++) {
    for (b=0; b <= w_length; b++) words [a] [b] = '\0';
  }

  for (a=0; a <= n_words; a++) {
    for (b=0; b <= max_num; b++) w_page [a] [b] = 0;
  }

  for (a=0; a <= max_num; a++) w_count [a] = 0;
  for (a=0; a <= max_num; a++) new_word [a] = '\0';

  /* open files */
  if (argc != 4) { /* wrong number of args; exit */
    fprintf(stderr, "Usage: index input_pathname output_pathname");
    fprintf(stderr, " wordlist_pathname\n\n");
    fprintf(stderr, "Maximum word length is %d characters\n", w_length);
    fprintf(stderr, "Maximum number of words is %d\n", n_words);
    fprintf(stderr, "Maximum number of pages per word is %d\n", max_num);
    exit(1);
  } else if ((infile = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Can't open input file.\n");
    exit(1);
  } else if ((outfile = fopen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Can't open output file.\n");
    exit(1);
  } else if ((wordlist = fopen(argv[3], "r")) == NULL) {
    fprintf(stderr, "Can't open word list file.\n");
    exit(1);
  } else {

    /* Echo pathnames */
    fprintf(stderr, "Input pathname is %s\n", argv[1]);
    fprintf(stderr, "Output pathname is %s\n", argv[2]);
    fprintf(stderr, "Word list pathname is %s\n\n", argv[3]);

    /* Read word list */
    num_words = get_list(wordlist); /* Side effects changes global variables */

    /* convert word list to lower case */
    for (a=1; a <= num_words; a++) 
      for (b=0; words [a] [b] != '\0'; b++) 
        if ((words [a] [b] >= 'A') && (words [a] [b] <= 'Z')) 
          words [a] [b] += 32;

    /* sort word list */
    for (a=1; a < num_words; a++) {
      for (b=a+1; b <= num_words; b++) {
        if ((strcmp (words [a], words [b])) > 0) {
          for (n=0; n <= w_length; n++) {  /* Words [0] is temporary var */
            words [0] [n] = words [a] [n];
            words [a] [n] = words [b] [n];
            words [b] [n] = words [0] [n];
          }
        }
      } 
    }

    /* index and write file */
    if (num_words == 0) {
      fprintf (stderr, "No words found in word list file.\n");
      exit(1);  /* Exit program if no words found */
    } else {
      fprintf (stderr, "%d words found in word list file.\n", num_words);
      index_it (infile);  /* Side effects change several global variables */
      print_it (outfile); /* Side effects change several global variables */
    }

    /* close files */
    fclose(infile);
    fclose(outfile);
    fclose(wordlist);
  }

  /* Normal exit */
  exit(0); 
}


/* Function to get word list. Global variable words is accessed */
/* Returns number of words found */
get_list (fp)
FILE *fp;
{
    int a, c;

  a = 1;
  while (((fscanf(fp, "%30s", words[a])) != EOF) && (a < n_words)) a++;
  return (a-1);
}


/* Search input file and build index arrays. */
/* All global variables accessed */
index_it (fp)
FILE *fp;
{
	int a, no_match;
	char c;

  page = 1;
  while ((next_word(fp)) != EOF) {
    a = 0;
    no_match = 0;
    while ((a < num_words) && (no_match == 0)) {
      a++;
      if ((strcmp (new_word, words [a])) == 0) {
        no_match++;
        if (w_page [a] [w_count[a]] != page) {
          if (w_count [a] == max_num) 
          fprintf(stderr, "number of pages exceeded for word %s\n", words [a]);
          else w_count [a]++;
          w_page [a] [w_count[a]] = page;
        } 
      }
    }
  }

  return (0);
}


/* Get next word from input file. */
/* Increment page number if form feed encountered. */
/* Returns the length of the string or EOF if error or EOF encountered. */
/* Global variables new_word and page accessed. */
next_word (fp)
FILE *fp;
{
	int a, c;

  /* skip white spaces */
  do {
    c = fgetc(fp);
    if (c == form_feed) page++;  /* increment page number */
  } while ((c <= space) && (c != EOF));
  if (c != EOF) {
    a = 0;
    if ((c >= 'A') && (c <= 'Z')) c += 32;  /* convert to lower case */
    new_word [a] = c;

    /* put chars in string until white space encountered */
    do {
      c = fgetc(fp);
      if ((c > space) && (c != EOF)) {
        a++;
        if ((c >= 'A') && (c <= 'Z')) c += 32;  /* convert to lower case */
        new_word [a] = c;
      } else if (c == form_feed) page++;  /* increment page number */
    } while ((c > space) && (c != EOF) && (a < w_length));
    a++;
    new_word [a] = '\0';  /* don't forget trailing null */
  } 
  if (c == EOF) return (EOF); else return (a-1);
}


/* Print index from arrays */
/* Global variables accessed but NOT altered. */
print_it (fp)
FILE *fp;
{
	int a, b, i, limit;
  a =0;
  while (a < num_words) {
    a++;
    if (w_count [a] > 0) {
      fprintf (fp, "%s ", words [a]);
      limit = page_width - (strlen (words [a])) - (w_count [a] * 4);
      for (i=1; i <= limit; i++) fprintf (fp, "%c", '.');
      for (b=1; b <= w_count [a]; b++) {
        fprintf (fp, "%3d", w_page [a] [b]);
        if (b < w_count [a]) fprintf (fp, ",");
      } 
      fprintf (fp, "\n");
    }
  }

  return (0);
}
