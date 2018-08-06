  /* TSORT.C - A text file sort program */
  /* Slightly rewritten to run under DRC and UNIX VER 7 C */
  /* Usage:  TSORT <infile >outfile */

#include          <stdio.h>
#include          <ctype.h>

#define  MAXWORD  80                           /* maximum word length */
#define  NEWLINE  putc('\n',fp)     /* put carraige return in file fp */

struct text {               /* global declaration of tree node 'text' */
             char word[MAXWORD];
             struct text *lptr;
             struct text *rptr;
};

struct text *root;         /* global declaration of root of text tree */

main(argc, argv)
  /*-----------------------------------------------------------------*/
  /* driver for sort procedure.  Srt will sort lines of a specified  */
  /* input file and place the result in either the stdout or a       */
  /* specified output file.  Program is written for Lattice C but    */
  /* functions provided in its library are documented so that srt    */
  /* might be modified to suit your compiler.  If you have any       */
  /* trouble or questions give me a call (if you can find me)        */
  /*-----------------------------------------------------------------*/
  /* Joe R Wyatt    2311 49th Lubbock, Tx 79412                      */
  /* Home #: 806/793-5689    Office #: 806/763-8011 ext: 239         */
  /*-----------------------------------------------------------------*/
  /* Variables :        fp --     pointer to input file              */
  /*                   fp2 --     ptr to output file                 */
  /*                     c --     integer returned from getw.        */
  /*                     i --     processed line count               */
  /*                     j --     empty line count                   */
  /*                  word --     temporary input line storage       */
  /* Procedures:      getw --     included                           */
  /*                 putwt --     included                           */
  /*                 ptree --     included                           */
  /* Functions :    printf --     string compare                     */
  /*                 fopen --     open file and return file pointer  */
  /*                  exit --     premature exit from program        */
  /*                fclose --     close file                         */
  /*-----------------------------------------------------------------*/
int argc;
char *argv[];
{
     FILE *fp, *fp2, *fopen();
     int c, i, j;
     char word[MAXWORD];

     root = NULL;                                /* root of text tree */
     fp  = stdin;
     fp2 = stdout;
     i = 0;
     j = 0;
     while((c = getw(fp,word)) != EOF) {     /* get next line of file */
          if (strlen(word) != 0) {
               putwt(word);           /* if not null then put in tree */
               i++;            /* increment number of lines processed */
          }
          else
               j++;                 /* else inc number of empty lines */
     }
     fclose(fp);
     ptree(fp2, root);                           /* print sorted tree */
     printf("\nprocedure concluded.\n%d lines processed\n",i);
     printf("%d empty lines ignored.\n",j);
     fclose(fp2);
}

putwt(w)
  /*-----------------------------------------------------------------*/
  /* procedure places 'w' in tree named 'text'.                      */
  /* Parameters:         w --     null ended string                  */
  /* Variables :      temp --     temporary pointer                  */
  /*                   ptr --     ptr to node to place 'w' in.       */
  /*                 value --     result of strcmp used for placement*/
  /*                              in text tree.                      */
  /* Procedures:    search --     included                           */
  /* Functions :    strcmp --     string compare                     */
  /*                getmem --     allocate memory for tree           */
  /*                sizeof --     return size of structure           */
  /*                printf --     print string to stdout             */
  /*                strcpy --     string copy                        */
  /*                  exit --     leave prgram prematurly            */
  /*-----------------------------------------------------------------*/
char *w;
{
     struct text *search(), *temp, *ptr;
     int value = 1;

     temp = search(w);  /* locate position to place new node in tree */

     if (temp != NULL) /* if NULL then tree is empty. else compare keys */
          value = strcmp(w, temp -> word);
     if (value == 0)                    /* duplicate entry.  ignore. */
          return;

     ptr = (struct text *)malloc(sizeof(struct text));
     if (ptr == NULL) {                   /* memory allocation error */
          printf("out of memory\n");
          exit(1);
     }
       strcpy(ptr -> word, w);                     /* store new node */
       ptr -> lptr = NULL;
       ptr -> rptr = NULL;

     if (temp != NULL) {                   /* place new node in tree */
          if (value > 0)
               temp -> rptr = ptr;
          else
               temp -> lptr = ptr;
          }
     else
          root = ptr;
     return;
}

struct text *search(w)
  /*-----------------------------------------------------------------*/
  /* procedure locates where 'w' is to be placed in text tree and    */
  /* returns pointer to prior node.                                  */
  /* procedure places 'w' in tree named 'text'.                      */
  /* Parameters:         w --     null ended string                  */
  /* Variables :      ptr1 --     temporary pointer                  */
  /*                  ptr2 --     lags ptr1 by one node              */
  /*                 value --     result of strcmp used for placement*/
  /*                              in text tree.                      */
  /* Functions :    strcmp --     string compare                     */
  /*-----------------------------------------------------------------*/
char *w;
{
     struct text *ptr1;
     struct text *ptr2;
     int value;

     ptr1 = root;                             /* initialize pointers */
     ptr2 = root;
     while(ptr1 != NULL) {
          ptr2 = ptr1;
          value = strcmp(w, ptr1 -> word);
          if (value > 0)
               ptr1 = ptr1 -> rptr;
          else if (value < 0)
               ptr1 = ptr1 -> lptr;
          else
               ptr1 = NULL;
     }
     return(ptr2);
}

ptree(fp, ptr)
  /*-----------------------------------------------------------------*/
  /* recursive procedure to print contents of text tree.             */
  /* Parameters:        fp --     pointer to output file.            */
  /*                   ptr --     pointer to root of current subtree */
  /* Procedures:      putw --     included                           */
  /*-----------------------------------------------------------------*/
FILE *fp;
struct text *ptr;
{
     if (ptr != NULL) {
          ptree(fp, ptr -> lptr);   /* find leftmost node of subtree */
          putw(fp, ptr -> word);                       /* print data */
          ptree(fp, ptr -> rptr);             /* print right subtree */
     }
}

#define EOW -2    /* end of word */
getw(fp,w)
  /*-----------------------------------------------------------------*/
  /* gets next line in input file.                                   */
  /* Parameters:        fp --     pointer to input file              */
  /*                     w --     pointer to string for input        */
  /* Variables :         c --     input character                    */
  /*                     f --     end of word flag                   */
  /*                 count --     number of characters               */
  /* Functions :      getc --     get byte from input file           */
  /*-----------------------------------------------------------------*/
FILE *fp;
char *w;
{
     int c,f,count;

     f = 0;                                 /* initialize local vars */
     count = 0;
     while (f != EOW && count < MAXWORD) {
          c = getc(fp);
          if (c != EOF && c != '\n') {       /* input to end of line */
               count++;                    /* increment byte counter */
               *w++ = c;
          }
          else
               f = EOW; /* end of line reached or max chars exceded. */
     }
     *w = '\0';                  /* place null byte on end of string */
     return(c);                         /* return last byte of input */
}

putw(fp,w)
  /*-----------------------------------------------------------------*/
  /* prints string to output file.                                   */
  /* Parameters:        fp --     pointer to input file              */
  /*                     w --     pointer to string for input        */
  /* Variables :         c --     output character                   */
  /* Functions :      putc --     put byte to output file            */
  /*-----------------------------------------------------------------*/
FILE *fp;
char *w;
{
     int c;

     while (*w != '\0')
          putc(*w++, fp);
     NEWLINE;
}
