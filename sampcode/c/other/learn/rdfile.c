/* RDFILE.C: Reads a file and prints characters to 
 * the screen.  If you get "Error in opening file" try
 * running wrfile.c before you run this again.
 */

#include <stdio.h>

main()
{
   int c;
   FILE *fp;

   if( fp = fopen( "c:\\testfile.asc", "rb" ) )
   {
      while( (c = fgetc( fp )) != EOF )
         printf( " %c\t%d\n", c, c );
      printf( "\nEnd of file marker: %d", c );
      fclose( fp );
   }
   else
      printf( "Error in opening file\n" );
}
