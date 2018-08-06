/* PTRPTR.C: Demonstrate a pointer to a pointer. */
#include <stdio.h>

main()
{
   int val = 501;
   int *ptr = &val;
   int **ptr_ptr = &ptr;
   printf( "val = %d\n", **ptr_ptr );
}
