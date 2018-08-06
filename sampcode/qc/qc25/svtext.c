/* SVTEXT.C: Save integer variables as text. */
#include <stdio.h>

int list[] = { 53, -23456, 50, 500, 5000, -99 };
extern int errno;
char fname[] = "numtext";
char temp[81];

main()
{
   FILE *fptr;
   int i;

   if( (fptr = fopen( "numtext","wt" )) != NULL )
   {
      for( i=0; i<6; i++ )
         fprintf( fptr, "Item %d: %6d \n", i, list[i] );
      fclose( fptr );
   }
   else
      printf( "Error: Couldn't create file.\n" );

   if( (fptr = fopen( "badname", "rt" )) != NULL )
   {
      /* do nothing */
   }
   else
   {
      printf( "Error number: %d\n\t", errno );
      perror( "Couldn't open file BADNAME\n\t" );
   }

   if( (fptr = fopen( fname, "rt" )) != NULL )
   {
      list[0] = 0;
      fscanf( fptr, "Item %d: %d \n", &i, &list[0] );
      printf( "Values read from file:\t %d %d\n", i, list[0] );
      fgets( temp, 80, fptr );
      printf( "String from file: \t%s\n", temp );
      while( (i = fgetc( fptr )) != '\n' )
         printf( "char: %c \t ASCII: %d \n", i, i );
      rewind( fptr );
      printf( "Rewind to start -->\t%s", fgets( temp, 80, fptr ) );
      fclose( fptr );
   }
   else
      printf( "Trouble opening %s \n", fname );
}
