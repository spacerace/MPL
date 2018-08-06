/* QCSORT1.C: Demonstrate sort with pointer notation. */

#include <stdio.h>
#define SIZE 4

void sort( int size, double **p );
void show( int size, double **p, double dd[] );

main()
{
   int x;
   double d[] = { 3.333, 1.111, 2.222, 4.444 };
   double *d_ptr[SIZE];
   for( x = 0; x < SIZE; x++ )
      d_ptr[x] = &d[x];
   show( SIZE, d_ptr, d );
   sort( SIZE, d_ptr );
   show( SIZE, d_ptr, d );
}

void sort( int size, double **p )
{
   int x, x1;
   double *temp;
   for( x = 0; x < size - 1; x++ )
      for( x1 = x + 1; x1 < size; x1++ )
      {
         if( **(p+x) > **(p+x1) )
         {
            temp = *(p+x1);
            *(p+x1) = *(p+x);
            *(p+x) = temp;
         }
      }
}

void show( int size, double **p, double dd[] )
{
   int x;
   printf( "------------------------" );
   printf( "------------------------\n" );
   for( x = 0; x < size; x++ )
   {
      printf( "*d_ptr[%d] = %1.3f   ", x, **(p+x) );
      printf( "d_ptr[%d] = %u ", x, *(p+x) );
      printf( "  d[%d] = %1.3f\n", x, dd[x] );
   }
}
