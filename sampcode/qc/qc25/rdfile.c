/* RDFILE.C: Reads a file and prints characters to the screen. */
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
