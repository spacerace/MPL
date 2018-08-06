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
  /* Joe R Wyatt    2311 Lubbock, Tx  79412                          */
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
     if (argc == 1)  {                  /* no file specified for sort */
          printf("usage: srt infile.ext <outfile.exe>\n");
          exit(0);
     }
     else{                                         /* open input file */
          if ((fp = fopen(*(argv+1), "r")) == NULL) {
               printf("cannot read %s\n",*(argv+1));
               exit(1);
          }
          if (argc == 3) {                        /* open output file */
               if ((fp2 = fopen(*(argv+2), "w")) == NULL) {
                    printf("cannot open %s \n",*argv[3]);
                    exit(1);
               }
          }
          else
               fp2 = stdout;                   /* no output specified */
     }
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
  /* Variables :    