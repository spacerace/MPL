/****************************************************************************/
/*                                                                          */
/* ASCIDUMP.C                                                               */
/*                                                                          */
/* This program prints the ASCII decimal codes and actual text for a file   */
/* specified as the first command line argument. It reads the file a block  */
/* at a time and prints the decimal value of each character, then prints    */
/* the translated characters at the end of the line. It translates certain  */
/* special characters like BELL, LF, FF to spaces for printing              */
/*                                                                          */
/* Written by: Jim Niederriter   12/26/83                                   */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This program was written for the Lattice `C' Compiler, and works under   */
/* version 1.04 or 2.00.                                                    */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#define  BELL   7
#define  MAXCHR 30
#define  NULL   0
     
main(argc, argv)
int argc;
char *argv[];
{
     int loopx, linecnt;
     int x, f1, stats, flag, fmode;
     long countr;
     char buffer[MAXCHR + 1];
     FILE *f2, *fopen();

/****** Looks for filename as first command line argument *******************/

     if (argc < 2)
     {
          puts("Requires file name in command line\n");
          putch(BELL);
          exit(1);
     }

/****** Opens printer as LPT1 ***********************************************/

     if ((f2 = fopen("LPT1:", "w")) == NULL)
     {
          puts("Can't open printer file\n");
          putch(BELL);
          exit(1);
     }
/****** Open file as read-only, and in `untranslated' mode...(does not   */
/****** convert CR/LF). *************************************************/

     fmode = 0;          /* mode for `read-only' */
     fmode = 0x8000;     /* set to read in `untranslated' or binary mode */
     
     if ((f1 = open(argv[1], fmode)) == NULL)
     {
          printf("Can't open file %s.\n", argv[1]);
          putch(BELL);
          exit(1);
     }
/***** Printer codes are set up for EPSON FX (change to match your printer */

     fprintf(f2, "\x1BP\x0F");     /* set printer to PICA, CONDENSED  */

/**** This one sets the `skip over perforation' to 3 lines *****************/

     fprintf(f2, "\x1BN\x03\nCount                              File ");
     fprintf(f2, "dump for file name: %s \n", argv[1]);
     
     countr = loopx = linecnt = 0;
     flag = 1;
     while (flag)
     {
          flag = 0;
          setmem(buffer, MAXCHR, NULL);               /* clear buffer  */
          if ((stats = read(f1, buffer, MAXCHR)) > 0) /* read blk from file */
          {
               fprintf(f2, "%05ld   ", countr);        /* print char count */ 
               countr += MAXCHR;
               flag = 1;
               
               for (x=0; x <= (MAXCHR-1); x++)
               {
                    fprintf(f2, "%03d", buffer[x]);  /* print decimal value */

/****** change any special characters to SPACE for printing *****************/

                    if (buffer[x] < 32 || buffer[x] > 128)
                         buffer[x] = ' ';    /* get rid of unprintables */
                    
                    if ((loopx +=1) == 5)
                    {
                         fprintf(f2, " ");            /* 5 at a time */
                         loopx = 0;
                    }
               }
               fprintf(f2, "*");             /* print translated characters */
               for (x=0; x <= (MAXCHR-1); x++)
                    fprintf(f2, "%c", buffer[x]);
               fprintf(f2, "*\n");
               if ((linecnt += 1) == 5)
               {
                    fprintf(f2, "\n");       /* skip 1 after every 5 lines */
                    linecnt = 0;
               }
          }
     }
     fprintf(f2, "\x1BP\x0C");     /* set back to normal & do forms feed */
     fflush(f2);
     fclose(f2);
}
