/* ARGV1.C: Demonstrate null pointers. */
#include <stdio.h>

void show_args( char *argument );

int main( int argc, char **argv )
{
   while( *argv )
      show_args( *(argv++) );
   return 0;
}

void show_args( char *argument )
{
   printf( "%s\n", argument );
}
