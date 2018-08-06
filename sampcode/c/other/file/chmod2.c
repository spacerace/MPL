/* CHMOD2.C illustrates reading and changing file attributes and file
 * time using:
 *      _dos_getftime       _dos_setftime
 *      _dos_getfileattr    _dos_setfileattr
 *
 * See CHMOD1.C for a simpler variation of this program using the utime,
 * access, and chmod functions.
 */

#include <dos.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>

char *datestr( unsigned d, char *buf );     /* Prototypes */
char *timestr( unsigned t, char *buf );

main( int argc, char *argv[] )
{
    unsigned fdate, ftime, fattr;
    struct dosdate_t ddate;
    struct dostime_t dtime;
    int hsource;
    char timebuf[10], datebuf[10], *pkind;

    /* Open to get file handle and test for errors (such as nonexistence). */
    if( _dos_open( argv[1], O_RDONLY, &hsource ) )
    {
        printf( "Can't open file\n" );
        exit( 1 );
    }

    /* Get time, date, and attribute of file. */
    _dos_getftime( hsource, &fdate, &ftime );
    _dos_getfileattr( argv[1], &fattr );

    /* Convert information into formatted strings. */
    datestr( fdate, datebuf );
    timestr( ftime, timebuf );

    if( fattr & _A_SUBDIR )
        pkind = "Directory";
    else if( fattr & _A_VOLID )
        pkind = "Label";
    else
        pkind = "File";

    printf( "%-12s   %-8s   %-9s   %-9s    %s %s %s\n",
            "FILE", "TIME", "DATE", "KIND", "RDO", "HID", "SYS" );
    printf( "%-12s   %8s   %8s    %-9s     %c   %c   %c\n",
            argv[1], timebuf, datebuf, pkind,
            (fattr & _A_RDONLY) ? 'Y' : 'N',
            (fattr & _A_HIDDEN) ? 'Y' : 'N',
            (fattr & _A_SYSTEM) ? 'Y' : 'N' );

    /* Update file time or attribute. */
    printf( "Change: (T)ime  (R)ead only  (H)idden  (S)ystem\n" );
    switch( toupper( getch() ) )    /* Use stdlib.h, not ctype.h */
    {
        case 'T':               /* Set to current time */
            _dos_gettime( &dtime );
            _dos_getdate( &ddate );
            ftime = (dtime.hour << 11) | (dtime.minute << 5);
            fdate = ((ddate.year - 1980) << 9) | (ddate.month << 5) |
                      ddate.day;
            _dos_setftime( hsource, fdate, ftime );
            break;
        case 'R':               /* Toggle read only */
            _dos_setfileattr( argv[1], fattr ^ _A_RDONLY );
            break;
        case 'H':               /* Toggle hidden    */
            _dos_setfileattr( argv[1], fattr ^ _A_HIDDEN );
            break;
        case 'S':               /* Toggle system    */
            _dos_setfileattr( argv[1], fattr ^ _A_SYSTEM );
            break;
    }
    _dos_close( hsource );
    exit( 1 );
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
