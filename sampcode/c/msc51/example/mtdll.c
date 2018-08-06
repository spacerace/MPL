/*
**  This is a sample Dynalink Library Routine written in Microsoft C
*/

#include <stdio.h>

void far _loadds DynalibTest ( char far * far * envptr ) ;

void far _loadds DynalibTest ( char far * far * envptr )
{
    register int i;

    for ( i = 0 ; envptr [ i ] ; ++ i )
    fprintf ( stdout , "Envp [ %d ] = %p = ``%s''\n" ,
        i , envptr [ i ] , envptr [ i ] ) ;
}
