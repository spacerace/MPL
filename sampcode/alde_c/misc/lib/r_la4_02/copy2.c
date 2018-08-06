/* COPY2.C - From page 467 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. This does the same as DOS,s       */
/* COPY command.                                                */
/****************************************************************/

#include "fcntl.h"            /*needed for oflags*/
#include "types.h"            /*needed for stat.h*/
#include "stat.h"             /*needed for pmode*/
#define BUFSIZ 4096
char buff[BUFSIZ];

main(argc, argv)
int argc;
char *argv[];
{
int inhandle, outhandle, bytes;

   if(argc != 3) {
      printf("\nFormat: A>copy2 source.xxx dest.xxx");
      exit();
   }
   if ((inhandle = open(argv[1], O_RDONLY | O_BINARY)) < 0) {
      printf("\nCan't open file %s.", argv[1]);
      exit();
   }
   if ((outhandle = open(argv[2],
         O_CREAT | O_WRONLY | O_BINARY, S_IWRITE)) < 0) {
      printf("\nCan't open file %s.", argv[2]);
      exit();
   }
   while((bytes = read(inhandle, buff, BUFSIZ)) > 0)
      write(outhandle, buff, bytes);
   close(inhandle);
   close(outhandle);
}


