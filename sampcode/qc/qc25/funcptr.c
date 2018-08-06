/* FUNCPTR.C: Demonstrate function pointers. */
#include <stdio.h>

main()
{
   int (*func_ptr) ();
   func_ptr = printf;
   (*func_ptr) ( "Curiouser and curiouser...\n" );
}
