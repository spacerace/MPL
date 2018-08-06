/* TOUCH.C illustrates wild card handling and changing file time and
 * attribute. Functions used include:
 *      _dos_findfirst      _dos_findnext
 *      utime               chmod
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

void fileinfo( struct find_t *find );
char *timestr( unsigned d, char *buf );
char *datestr( unsigned d, char *buf );

main( int argc, char *argv[] )
{
    struct find_t find;

    /* Find first matching file, then find additional matches. */
    if( !_dos_findfirst( argv[1], 0xffff, &find ) )
        fileinfo( &find );
    else
    {
        printf( "  SYNTAX: TOUCH <wildfilespec>" );
        return 1;
    }
    while( !_dos_findnext( &find ) )
        fileinfo( &find );
}

void fileinfo( struct find_t *pfind )
{
    char timebuf[10], datebuf[10], *pkind;
    int ch;

    datestr( pfind->wr_date, datebuf );
    timestr( pfind->wr_time, timebuf );

    if( pfind->attrib & _A_SUBDIR )
        pkind = "Directory";
    else if( pfind->attrib & _A_VOLID )
        pkind = "Label";
    else
        pkind = "File";

    printf( "%-12s   %6ld   %8s   %9s   %-9s    %c %c %c %c\n",
            pfind->name, pfind->size, timebuf, datebuf, pkind,
            (pfind->attrib & _A_RDONLY) ? 'R' : ' ',
            (pfind->attrib & _A_HIDDEN) ? 'H' : ' ',
            (pfind->attrib & _A_SYSTEM) ? 'S' : ' ',
            (pfind->attrib & _A_ARCH) ? 'A' : ' ' );

    printf( "(T)ime update   (R)ead only   (Q)uit   Any other next\n" );
    ch = getch();
    switch( toupper( ch ) )
    {
        case 'T':               /* Set to current time */
            utime( pfind->name, NULL );
            break;
        case 'R':               /* Toggle read only */
            if( pfind->attrib & _A_RDONLY )
                chmod( pfind->name, S_IWRITE );
            else
                chmod( pfind->name, S_IREAD );
            break;
        case 'Q':               /* Quit */
            exit( 1 );
    }
}

/* Take unsigned date in the format:    fedcba9876543210
 * (year is 1980 - year)                yyyyyyymmmmddddd
 * Change to a 9-byte string:           mm/dd/yy
 */
char *datestr( unsigned d, char *buf )
{
    sprintf( buf, "%2.2d/%02.2d/%02.2d",
             (d >> 5) & 0x0f, d & 0x1f, (d >> 9) + 80 );
    return buf;
}

/* Take unsigned time in the format:                fedcba9876543210
 * (hours and minutes 0-based)                      hhhhhmmmmmmsssss
 * Change to a 9-byte string (ignore seconds):      hh:mm ?m
 */
char *timestr( unsigned t, char *buf )
{
    int h = (t >> 11) & 0x1f, m = (t >> 5) & 0x3f;

    sprintf( buf, "%2.2d:%02.2d %cm", h % 12, m,  h > 11 ? 'p' : 'a' );
    return buf;
}
