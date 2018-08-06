/* FILETWO.C: Visibility in multiple source files. 
*/

#include <stdio.h>

void yonder( void )
{
   extern int chico, harpo;
   printf( "chico = %d, harpo = %d\n", chico, harpo );
}
