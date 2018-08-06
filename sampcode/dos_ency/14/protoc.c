/*
        PROTOC.C: a template for a character-oriented filter.

        Ray Duncan, June 1987
*/

#include <stdio.h>

main(argc,argv)
int argc;
char *argv[];
{       char ch;

        while ( (ch=getchar())!=EOF )   /* read a character */  
        {       ch=translate(ch);       /* translate it if necessary */
                putchar(ch);            /* write the character */
        }       
        exit(0);                        /* terminate at end of file */
}

/*
        Perform any necessary translation on character from
        input file.  Template action just returns same character.
*/

int translate(ch)
char ch;
{       return (ch);
}
