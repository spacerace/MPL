/* SVBIN.C: Saves integer variables in binary format. 
*/

#include <stdio.h>
#define ASIZE 10

main()
{
   FILE *ap;
   int zebra[ASIZE], acopy[ASIZE], bcopy[ASIZE];
   int i;

   for( i = 0; i < ASIZE; i++ )
      zebra[i] = 7700 + i;

   if( (ap = fopen( "binfile", "wb" )) != NULL )
   {
      fwrite( zebra, sizeof(zebra), 1, ap );
      fclose( ap );
   }
   else
      perror( "Write error" );

   if( (ap = fopen( "morebin", "wb" )) != NULL )
   {
      fwrite( &zebra[0], sizeof(zebra[0]), ASIZE, ap );
      fclose( ap );
   }
   else
      perror( "Write error" );

   if( (ap = fopen( "binfile", "rb" )) != NULL )
   {
      printf( "Hexadecimal values in binfile:\n" );
      while( (i = fgetc( ap )) != EOF )
         printf( "%02X ", i );
      rewind( ap );
      fread( acopy, sizeof(acopy), 1, ap );



      rewind( ap );
      fread( &bcopy[0], sizeof( bcopy[0] ), ASIZE, ap);
      for( i=0; i<ASIZE; i++ )
         printf( "\nItem %d = %d\t%d", i, acopy[i], bcopy[i] );
      fclose( ap );

   }
   else
      perror( "Read error" );

}
