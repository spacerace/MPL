/* FUNCPTR1.C: Passing function pointers as arguments. */
#include <stdio.h>

void gimme_func( void (*func_ptr) () );

main()
{
   gimme_func( puts );
   gimme_func( printf );
}

void gimme_func( void (*func_ptr) () )
{
   (*func_ptr) ( "Ausgezeichnet!" );
}
