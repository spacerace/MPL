/* RWFILE.C: Reads and writes a file. */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>

#define BUFF 512

main()
{
   char inbuffer[BUFF];
   char outbuffer[BUFF];
   int infile, outfile, length, num;

   strcpy( outbuffer, "Happy Birthday." );
   length = strlen( outbuffer );
   length++;

   if( (outfile = open( "testfile.bin",
      O_CREAT | O_WRONLY | O_BINARY,  S_IWRITE )) != -1 )
   {
      if( (num = write( outfile, outbuffer, length )) == -1 )
         perror( "Error in writing" );
      printf( "\nBytes written to file: %d\n", num );
      close( outfile );
   }
   else
      perror( "Error opening outfile" );

   if( (infile = open( "testfile.bin", O_RDONLY | O_BINARY )) != -1  )
   {
      while( length = read( infile, inbuffer, BUFF ) )
         printf( "%d bytes received so far.\n", length );
      close( infile );
      printf( "%s\n", inbuffer );
   }
   else
      perror( "Error opening infile" );
}
