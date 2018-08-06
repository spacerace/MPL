/* INPUT.C: Reads keyboard. */
#include <stdio.h>
#include <conio.h>
#include <ctype.h>

main()
{
   int num, c;
   char name[80];
   float rb;

   puts( "** Type \"Name:\" and your name" );
   scanf( "Name: %40s", name );
   printf( "** You typed this:\n%s", name );
   puts( "\n\n** Try again, with the gets function." );
   fflush( stdin );
   gets( name );
   printf( "** You typed this:\n%s\n", name );

   printf( "\n** Now type an integer.\n" );
   scanf( "%i", &num );
   sprintf( name, "** You typed this number: %i\n", num );
   puts( name );

   fflush( stdin );
   printf( "** Enter a floating-point value.\n" );
   scanf( "%f", &rb );
   printf( "** The answer is %f or %e\n", rb, rb );

   printf( "** Continue? Y or N\n" );

   do
   {
      c = getch();
      c = tolower( c );
   } while( c != 'y' && c != 'n' );
}
