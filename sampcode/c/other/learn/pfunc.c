/* PFUNC.C: Passing pointers to a function. */

#include <stdio.h>

void swap( int *ptr1, int *ptr2 );

main()
{
   int first = 1, second = 3;
   int *ptr = &second;
   printf( "first: %d  second: %d\n", first, *ptr );
   swap( &first, ptr );
   printf( "first: %d  second: %d\n", first, *ptr );
}

void swap( int *ptr1, int *ptr2 )
{
   int temp;
   temp = *ptr1;
   *ptr1 = *ptr2;
   *ptr2 = temp;
}
