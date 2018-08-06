/* ARGV.C: Demonstrate accessing command-line arguments. 
*/

#include <stdio.h>

void show_args( char *argument );

int main( int argc, char *argv[] )
{
   int count;
   for( count=0; count < argc; count++ )
      show_args( argv[count] );
   return 0;
}

void show_args( char *argument )
{
   printf( "%s\n", argument );
}
