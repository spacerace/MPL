/* COPYFILE.C: Demonstrate malloc and free functions */

#include <stdio.h>     /* printf function and NULL */
#include <io.h>        /* low-level I/O functions */
#include <conio.h>     /* getch function */
#include <sys\types.h> /* struct members used in stat.h */
#include <sys\stat.h>  /* S_ constants */
#include <fcntl.h>     /* O_ constants */
#include <malloc.h>    /* malloc function */
#include <errno.h>     /* errno global variable */

int copyfile( char *source, char *destin );

main( int argc, char *argv[] )
{
   if( argc == 3 )
      if( copyfile( argv[1], argv[2] ) )
         printf( "Copy failed\n" );
      else
         printf( "Copy successful\n" );
   else
      printf( "  SYNTAX: COPYFILE <source> <target>\n" );

   return 0;
}

int copyfile( char *source, char *target )
{
   char *buf;
   int hsource, htarget, ch;
   unsigned count = 50000;

   if( (hsource = open( source, O_BINARY | O_RDONLY )) == - 1 )
      return errno;
   htarget = open( target, O_BINARY | O_WRONLY | O_CREAT | O_EXCL,
                           S_IREAD | S_IWRITE );
   if( errno == EEXIST )
   {
      cputs( "Target exists. Overwrite? " );
      ch = getch();
      if( (ch == 'y') || (ch == 'Y') )
         htarget = open( target, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IREAD | S_IWRITE );
      printf( "\n" );
   }
   if( htarget == -1 )
      return errno;

   if( filelength( hsource ) < count )
      count = (int)filelength( hsource );

   buf = (char *)malloc( (size_t)count );

   if( buf == NULL )
   {
      count = _memmax();
      buf = (char *)malloc( (size_t)count );
      if( buf == NULL )
         return ENOMEM;
   }

   while( !eof( hsource ) )
   {
      if( (count = read( hsource, buf, count )) == -1 )
         return errno;
      if( (count = write( htarget, buf, count )) == - 1 )
         return errno;
   }

   close( hsource );
   close( htarget );
   free( buf );
   return 0;
}
