/* POWER2.C */
#include <stdio.h>

int power2( int num, int power );

void main( void )
{
   printf( "3 times 2 to the power of 5 is %d\n", \
           power2( 3, 5) );
}

int power2( int num, int power )
{
   _asm
   {
      mov ax, num    ; Get first argument
      mov cx, power  ; Get second argument
      shl ax, cl     ; AX = AX * ( 2 to the power of CL )
   }
   /* Return with result in AX */
}
