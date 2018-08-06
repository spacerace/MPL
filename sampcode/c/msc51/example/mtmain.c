/*
**  This is a sample program using Microsoft C's Dynalink Library C Run-time
*/

#include <stdio.h>
#include <time.h>

extern void far DynalibTest ( char far * far * envptr ) ;

int far main(int argc, char far * far * argv, char far * far * envp) ;

int far main(int argc, char far * far * argv, char far * far * envp)
{
    int i ;
    long clock ;

    fprintf(stderr,"Argc = %d, Argv = %p, Envp = %p\n", argc, argv, envp);

    for ( i = 0 ; argv [ i ] ; ++ i )
    printf ( "Argv [ %d ] = %p = ``%s''\n" , i , argv [ i ] , argv [ i ] ) ;

    DynalibTest ( envp ) ;  /* Call Dynalink Library to print environment */

    time ( & clock ) ;
    printf ( "%s" , ctime ( & clock ) ) ;

    return 0 ;
}
