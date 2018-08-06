
/*           FUNKEY.C - a program for redefining fyunction keys  */
/*                                                               */
/*           Copyright  (C)  1983  Kenneth C. Wood               */
/*                                                               */
/*           From PC Magazine - - June 1983  Page 424            */
/*                                                               */
/*     Works with IBM PC-DOS 2.0 - to use it, have on your       */
/*     system disk the files:                                    */
/*     ANSI.SYS - supplied with DOS                              */
/*     CONFIG.SYS - file with the line "device = ansi.sys" in it */
/*     and one or more files with the extension .KEY i.e.        */
/*     WORDSTAR.KEY, MYOWN.KEY, FUNKEY.KEY, or                   */
/*     DOS.KEY - supplied by you and containing                  */
/*     your chosen new key definitions, and of course            */
/*     FUNKEY.EXE - the compiled and linked FUNKEY.C             */
/*                                                               */
/*     Now when the system is booted, the ANSI terminal          */
/*     emulator device driver will be installed, giving          */
/*     you the capability to do marvelous things, including      */
/*     redefine the function keys by issuing the command:        */
/*              FUNKEY MYOWN.KEY                                 */


#define  SEQ "\033[0;"       /* Initial sequence to ANSI driver */

#include "stdio.h"          /* Copy in the standard I/O routines */

main(argc,argv)
int argc;
char *argv[];

{
        FILE *fp, *fopen();

        if  (argc == 1)
                {
                printf("\nEnter new key definitions\n\n");
                fkey(stdin);         /* get input from keyboard */
                }
        else
                {
                fp = fopen(*++argv,"r");
                fkey(fp);       /* get input from the file */
                }
}

int count = 0;  /* counter for tracking size of the table */

fkey(fp)
FILE *fp;
{
        int c,offset;
        /* input character and function key identifier */

        while((c=getch(fp)) != EOF)
        {
                switch (c)
                {
                case 'f':       /* F1 through F10 */
                        offset = 58;
                        func(getch(fp),offset,fp);
                        break;
                case 's':       /* Shift F1 through F10 */
                        offset = 83;
                        func(getch(fp),offset,fp);
                        break;
                case 'c':       /* Control F1 through F10 */
                        offset = 93;
                        func(getch(fp),offset,fp);
                        break;
                case 'a':       /* Alt F1 through F10 */
                        offset = 103;
                        func(getch(fp),offset,fp);
                        break;
                case 'F':       /* F1 through F10 */
                        offset = 58;
                        func(getch(fp),offset,fp);
                        break;
                case 'S':       /* Shift F1 through F10 */
                        offset = 83;
                        func(getch(fp),offset,fp);
                        break;
                case 'C':       /* Control F1 through F10 */
                        offset = 93;
                        func(getch(fp),offset,fp);
                        break;
                case 'A':       /* Alt F1 through F10 */
                        offset = 103;
                        func(getch(fp),offset,fp);
                        break;
                default:
                        printf("Unknown function key\n");
                        break;
                }
        }
}

getch(fp)
/* throw away white space until a character */
FILE *fp;
{
int x;

        while((x=getc(fp)) == ' '|| x == '\n' || x == '\t')
                ;       /* ignore white space */
        return(x);
}


func(c,offset,fp)
/* send out the required definition sequence */
int c,offset;
FILE  *fp;
{
        int x,save;

        save = ' ';
        while ((x = getch(fp)) != '=')
                if (x == '0') c = ':';
                /* read until get = sign */
                /* if character is 0 then F10 was chosen */
                /* rather than F1.  Change c so that offset is */
                /* calculated correctly */
        printf(SEQ);
        printf("%d",offset+(c-'0'));

/*      putchar(';');
        putchar('"');
*/
        while ((x = getc(fp)) != '\n')
                {
                if (x != '!')
                        {
/*                      putchar(x);  */
                        printf(";%2d",x);

                        if (++count == 128)
                        {
                printf("\nTable size exceeded.  Program terminated...\n");
                exit();
                        }
                }
                else
                        save = x;
        }
        /*putchar('"');*/
        if (save == '!')
                {
                printf(";13p");
                count++;
                }
        else
                printf("p");
        save = ' ';
        if (count  == 128)
        {
                printf("\nTable size exceeded.  Program terminated...\n");
                exit();
        }
}

/* end of Program Funkey.C  */

