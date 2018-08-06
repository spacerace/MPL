/* EXTDIR.C illustrates wild card handling using functions:
 *      _dos_findfirst      _dos_findnext       sprintf
 */

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <sys\types.h>
#include <sys\utime.h>
#include <sys\stat.h>

long fileinfo( struct find_t *find );       /* Prototypes */
char *timestr( unsigned d, char *buf );
char *datestr( unsigned d, char *buf );

main( int argc, char *argv[] )
{
    struct find_t find;
    long size;

    /* Find first matching file, then find additional matches. */
    if( !_dos_findfirst( argv[1], 0xffff, &find ) )
    {
        printf( "%-12s   %-8s    %-8s   %-8s   %-9s   %s %s %s %s\n",
                "FILE", "SIZE", "TIME", "DATE", "KIND",
                "RDO", "HID", "SYS", "ARC" );
        size = fileinfo( &find );
    }
    else
    {
        printf( "  SYNTAX: TOUCH <wildfilespec>" );
        exit( 1 );
    }
    while( !_dos_findnext( &find ) )
        size += fileinfo( &find );
    printf( "%-12s   %8ld\n\n", "Total", size );
    exit( 0 );
}

long fileinfo( struct find_t *pfind )
{
    char timebuf[10], datebuf[10], *pkind;

    datestr( pfind->wr_date, datebuf );
    timestr( pfind->wr_time, timebuf );

    if( pfind->attrib & _A_SUBDIR )
        pkind = "Directory";
    else if( pfind->attrib & _A_VOLID )
        pkind = "Label";
    else
        pkind = "File";

    printf( "%-12s   %8ld    %8s   %8s   %-9s    %c   %c   %c   %c\n",
            pfind->name, pfind->size, timebuf, datebuf, pkind,
            (pfind->attrib & _A_RDONLY) ? 'Y' : 'N',
            (pfind->attrib & _A_HIDDEN) ? 'Y' : 'N',
            (pfind->attrib & _A_SYSTEM) ? 'Y' : 'N',
            (pfind->attrib & _A_ARCH)   ? 'Y' : 'N' );
    return pfind->size;
}

/* Take unsigned time in the format:                fedcba9876543210
 * s=2 sec incr, m=0-59, h=23                       hhhhhmmmmmmsssss
 * Change to a 9-byte string (ignore seconds):      hh:mm ?m
 */
char *timestr( unsigned t, char *buf )
{
    int h = (t >> 11) & 0x1f, m = (t >> 5) & 0x3f;

    sprintf( buf, "%2.2d:%02.2d %cm", h % 12, m,  h > 11 ? 'p' : 'a' );
    return buf;
}

/* Take unsigned date in the format:    fedcba9876543210
 * d=1-31, m=1-12, y=0-119 (1980-2099)  yyyyyyymmmmddddd
 * Change to a 9-byte string:           mm/dd/yy
 */
char *datestr( unsigned d, char *buf )
{
    sprintf( buf, "%2.2d/%02.2d/%02.2d",
             (d >> 5) & 0x0f, d & 0x1f, (d >> 9) + 80 );
    return buf;
}
