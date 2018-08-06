/* PSTRING1.C: Look for null at string's end.  */

#include <stdio.h>

main()
{
   static char name[] = "john";
   char *ptr = name;
   while( *ptr )
      printf( "*ptr = %c\n", *ptr++ );
}
