/* DOSMEM.C illustrates functions:
 *      _dos_allocmem       _dos_setblock       _dos_freemem
 *
 * See COPY2.C for another example of _dos_allocmem and _dos_freemem.
 */

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

main()
{
    char far *buf = NULL, far *p;
    unsigned segbuf, maxbuf, size = 512;

    /* Allocate 512-byte buffer. Convert the size to paragraphs).
     * Assign the segment to the buffer. Fill with A's.
     */
    if( _dos_allocmem( size >> 4, &segbuf ) )
        exit( 1 );
    FP_SEG( buf ) = segbuf;
    for( p = buf; p < (buf + size); p++ )
        *p = 'A';

    /* Double the allocation. Fill the second half with B's. */
    size *= 2;
    if( _dos_setblock( size >> 4, segbuf, &maxbuf ) )
        exit( 1 );
    FP_SEG( buf ) = segbuf;
    for( p = buf + (size / 2); p < (buf + size); p++ )
        *p = 'B';
    *(--p) = '\0';

    printf( "Memory available: %u paragraphs\n", maxbuf );
    printf( "Buffer at %Fp contains:\n%Fs", (int far *)buf, buf );

    /* Free memory */
    exit( !_dos_freemem( segbuf ) );
}
