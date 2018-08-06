#include <bios.h>
#include <dos.h>

#pragma inline

#define EM_MEMSIZE 0x88
#define EM_INT 0x15

unsigned int get_size( void )
{
unsigned int flags;
unsigned int res;

    _AH = EM_MEMSIZE;
	geninterrupt( EM_INT );
	asm pushf;
	asm pop bx;
	flags = _BX;
	res = _AX;
	if ( (flags & 1) != 0 )
	    return( 0 );
	return( res );
}

