/* FSEARCH.C - From page 464 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. Searches a file for a phrase.     */
/* Usage:  A>fsearch filename.ext phrase                        */
/****************************************************************/

#include "fcntl.h"            /*needed for oflags*/
#include "stdio.h"            /*needed for NULL etc.*/
#include "memory.h"           /*needed for memchr();*/
#define BUFFSIZE 1024
char buff[BUFFSIZE];

main(argc, argv)
int argc;
char *argv[];
{
int inhandle, bytes;

   if(argc != 3) {
      printf("\nFormat:  A>fsearch filename.ext phrase");
      exit();
   }
   if((inhandle = open(argv[1], O_RDONLY)) < 0) {
      printf("\nCan't open file %s.", argv[1]);
      exit();
   }
   while((bytes = read(inhandle, buff, BUFFSIZE)) > 0)
      search(argv[2], bytes);
   close(inhandle);
   printf("\nPhrase not found");
}

/* search() */    /* searches buffer for phrase */
search(phrase, buflen)
char *phrase;
int buflen;
{
char *ptr, *p;

   ptr = buff;
   while((ptr = memchr(ptr, phrase[0], buflen)) != NULL)
      if(memcmp(ptr, phrase, strlen(phrase)) == 0) {
         printf("\nFirst occurrence of phrase:\n\n");
         for(p = ptr - 100; p < ptr + 100; p++)
            putchar(*p);
         exit();
      }
      else ptr++;
}



