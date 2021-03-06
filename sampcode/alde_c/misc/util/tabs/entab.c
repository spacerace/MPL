/* ------------------------------------------- */
/*     ENTAB - Convert blanks to tabs          */
/*             Adapted from Software Tools     */
/*             By Kernighan and Plauger        */
/*                                             */
/*             written by Michael Burton       */
/*             Last Update: 14 Jan 1984       */
/* ------------------------------------------- */
/*     USAGE:                                  */
/*             ENTAB FROMFILE TOFILE N         */
/*               N is the number of columns    */
/*                 between tab stops           */
/* ------------------------------------------- */
#include "stdio.h"

main(argc,argv)
       int argc;
       char *argv[];
{
       static int col = 1, n, *fd, *td, newcol;
       static char c, *sp;

       if (argc != 4)
       {
               fputs("Usage: ENTAB FROMFILE TOFILE N\007\n",stdout);
               return;
       }
       sp = argv[1];
       while ((*sp = toupper(*sp)) != EOS) sp++;
       sp = argv[2];
       while ((*sp = toupper(*sp)) != EOS) sp++;
       if ((fd = fopen(argv[1],"r")) == 0)
       {
               fputs(argv[1],stdout);
               fputs(" not found\007\n",stdout);
               return;
       }
       if ((td = fopen(argv[2],"w")) == 0)
       {
               fputs("Unable to open ",stdout);
               fputs(argv[2],stdout);
               fputs("\007\n",stdout);
               return;
       }
       n = atoi(argv[3]);
       if (n < 1 || n > 32) fputs("Tabs < 1 or > 32\007\n",stdout);

       while(1)
       {
               newcol = col;
               while ((c = fgetc(fd)) == ' ')
               {
                       newcol++;
                       if ((newcol % n) == 0)
                       {
                               fputc('\t',td);
                               col = newcol;
                       }
               }
               for (; col < newcol; col++) fputc(' ',td);
               if (c == EOF) return;
               fputc(c,td);
               switch(c)
               {
                       case '\n':
                               col = 1;
                               break;
                       case '\t':
                               col += n-(col % n);
                               break;
                       default:
                               col++;
               }
       }
       fflush(fd);
       fflush(td);
       fclose(fd);
       fclose(td);
}
    3 f � � � 2d���0c�������57NX����P|����-7h�����9;LVt��� P~���(BTw���	E	f	�	�	�	�	�	�	�	�	O                                 N                                                            