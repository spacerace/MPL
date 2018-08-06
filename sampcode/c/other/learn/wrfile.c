/* WRFILE.C: Creates and writes to a disk file. */

#include <stdio.h>

main()
{
   FILE *fp;

   if( (fp = fopen( "c:\\testfile.asc","w" )) != NULL )
   {
      fputs( "Example string", fp );
      fputc( '\n', fp );
      fclose( fp );
   }
   else
      printf( "error message\n" );
}
