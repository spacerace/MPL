/*
    PROTO.C: prototype character-oriented filter

    Copyright 1988 Ray Duncan

    Build:  CL PROTO.C

*/

#include <stdio.h>

main(int argc, char *argv[])
{       
    char ch;

    while((ch=getchar()) != EOF)    /* read a character */  
    {   
        ch = translate(ch);         /* translate it if necessary */

        putchar(ch);                /* write the character */
    }       
    exit(0);                        /* terminate at end of file */
}


/*
    Perform any necessary translation on character 
    from input file.  This example just returns 
    the same character.
*/

int translate(char ch)
{       
    return (ch);
}
