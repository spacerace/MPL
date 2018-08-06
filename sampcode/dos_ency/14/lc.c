/*
        LC:     a simple character-oriented filter to translate
                all uppercase {A-Z} to lowercase {a-z} characters.

        Usage:  LC [< source] [> destination]

        Ray Duncan, June 1987

*/

#include <stdio.h>

main(argc,argv)
int argc;
char *argv[];
{       char ch;
                                        /* read a character */
        while ( (ch=getchar() ) != EOF )
        {       ch=translate(ch);       /* perform any necessary
                                           character translation */
                putchar(ch);            /* then write character */
        }       
        exit(0);                        /* terminate at end of file */
}

/*
        Translate characters A-Z to lowercase equivalents
*/

int translate(ch)
char ch;
{       if (ch >= 'A' && ch <= 'Z') ch += 'a'-'A';
        return (ch);
}
