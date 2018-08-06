/* SYSREAD2.C - From page 463 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. This program reads and displays   */
/* a file using system I/O.  Usage: A>sysread2 filename.ext     */
/* This program differs from SYSREAD.C only in that it uses     */
/* the perror() function.                                       */
/****************************************************************/

#include "fcntl.h"
#define BUFFSIZE 512
char buff[BUFFSIZE];

main(argc, argv)
int argc;
char *argv[];
{
int inhandle, bytes, j;

   if(argc != 2) {
      printf("\nFormat: A>sysread2 filename.ext");
      exit();
   }
   if((inhandle = open(argv[1], O_RDONLY | O_BINARY)) < 0)
      perror("\nCan't open input file");
   while((bytes = read(inhandle, buff, BUFFSIZE)) > 0)
      for(j = 0; j < bytes; j++)
         putch(buff[j]);
   close(inhandle);
}

