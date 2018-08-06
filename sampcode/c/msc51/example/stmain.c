/*
**      This is a sample program using Microsoft C Run-time for Dynalink 
**      Libraries. This program must be compiled Compact or Large model in 
**      order to work correctly with the associated Dynalink Library.
*/

#include <stdio.h>
#include <time.h>

extern void far DynalibTest ( char far * far * envptr );

int main(int argc, char * * argv, char * * envp);

int main(int argc, char * * argv, char * * envp)
{
    int i;
    long clock;

    fprintf(stderr,"Argc = %d, Argv = %p, Envp = %p\n", argc, argv, envp);

    for ( i = 0; argv [ i ]; ++ i )
	printf ( "Argv [ %d ] = %p = ``%s''\n", i, argv [ i ], argv [ i ] );

    DynalibTest ( envp );  /* Call Dynalink Library to print environment */

    time ( & clock ) ;
    printf ( "%s" , ctime ( & clock ) ) ;

    return 0;
}
