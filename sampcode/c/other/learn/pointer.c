/* POINTER.C: Demonstrate pointer basics. */

#include <stdio.h>

main()
{
   int val = 25;
   int *ptr;
   ptr = &val;
   printf( " val = %d\n", val );
   printf( "*ptr = %d\n\n", *ptr );
   printf( "&val = %u\n", &val );
   printf( " ptr = %d\n", ptr );
}
