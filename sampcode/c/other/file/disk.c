/* DISK.C illustrates low-level disk access using functions:
 *      _bios_disk          _dos_getdiskfree
 */

#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <stdlib.h>

char far diskbuf[512];

main( int argc, char *argv[] )
{
    unsigned status = 0, i;
    struct diskinfo_t di;
    struct diskfree_t df;
    unsigned char far *p, linebuf[17];

    if( argc != 5 )
    {
        printf( "  SYNTAX: DISK <driveletter> <head> <track> <sector>" );
        exit( 1 );
    }

    if( (di.drive = toupper( argv[1][0] ) - 'A' ) > 1 )
    {
        printf( "Must be floppy drive" );
        exit( 1 );
    }
    di.head     = atoi( argv[2] );
    di.track    = atoi( argv[3] );
    di.sector   = atoi( argv[4] );
    di.nsectors = 1;
    di.buffer   = diskbuf;

    /* Get information about disk size. */
    if( _dos_getdiskfree( di.drive + 1, &df ) )
        exit( 1 );

    /* Try reading disk three times before giving up. */
    for( i = 0; i < 3; i++ )
    {
        status = _bios_disk( _DISK_READ, &di ) >> 8;
        if( !status )
            break;
    }

    /* Display one sector. */
    if( status )
        printf( "Error: 0x%.2x\n", status );
    else
    {
        for( p = diskbuf, i = 0; p < (diskbuf + df.bytes_per_sector); p++ )
        {
            linebuf[i++] = (*p > 32) ? *p : '.';
            printf( "%.2x ", *p );
            if( i == 16 )
            {
                linebuf[i] = '\0';
                printf( " %16s\n", linebuf );
                i = 0;
            }
        }
    }
    exit( 1 );
}
