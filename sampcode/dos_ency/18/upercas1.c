/************************************************************************
 *                                                                      *
 * UPPERCAS.C                                                           *
 *    This routine converts a fixed string to uppercase and prints it.  *
 *                                                                      *
 ************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>

main(argc,argv)

int argc;
char *argv[];

{
char    *cp,c; 

        cp = "a string\n";
     
        /*  Convert *cp to uppercase and write to standard output  */

        while (*cp != '\0')
                {
                c = toupper(*cp++);
                putchar(c);
                }

}
