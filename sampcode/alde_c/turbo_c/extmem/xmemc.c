#include <dos.h>

static char ident[] = "VDISK  V3.2";

unsigned long get_loc( void )
{
unsigned far *int19_seg = MK_FP( 0x0000, 4*0x19+2 );
char far *txt_ptr;
char *lcl_ptr;
unsigned far *lo;
unsigned char far *hi;

	/* VDISK's identification string is located in
		the same segment as the int 0x19 handler, at
		offset 0x12. */

    txt_ptr = MK_FP( *int19_seg, 0x12 );
    lcl_ptr = ident;
    while ( *lcl_ptr != 0 )
        if ( *lcl_ptr++ != *txt_ptr++ )
            return( 0x100000U );

	/* if VDISK is installed, the address
		of the first byte of available extended
		memory space is stored in the same segment
		as the int 0x19 handler, at offset 0x2C,
		0x2D, and 0x2E. */
    lo = MK_FP( *int19_seg, 0x2C );
    hi = MK_FP( *int19_seg, 0x2E );

	/* convert the address into a long */
    return( (unsigned long)*hi * 65536L + (unsigned long)*lo );
}

#if defined( TEST )

#include <stdio.h>

unsigned int get_size( void );

void main()
{
unsigned long mem_size;
unsigned long first_loc;

	first_loc = get_loc();
	mem_size = (unsigned long)get_size()*1024 - ( first_loc - 0x100000L );
	printf( "First loc = 0x%lX\nSize = %luK\n", first_loc, mem_size/1024 );
}

#endif /* TEST */
