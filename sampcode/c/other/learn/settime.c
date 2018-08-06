/* SETTIME.C illustates getting and setting the DOS time and date using:
 *      _dos_gettime    _dos_settime    _dos_getdate    _dos_setdate
 *
 * Formatted input to a string is illustrated using:
 *      sscanf
 */

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

struct dosdate_t ddate;
struct dostime_t dtime;

main()
{
    unsigned tmpday, tmpmonth, tmpyear;
    unsigned tmphour, tmpminute, tmpsecond;
    static char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    char tmpbuf[20];

    /* Get current date. */
    _dos_getdate( &ddate );
    printf( "Date: %u/%02u/%02u %s\n",
            ddate.month, ddate.day, ddate.year - 1900,
            days[ddate.dayofweek] );
    printf( "Enter new date: " );

    /* Get a date string and if it's not 0 (for CR), assign it to date. */
    gets( tmpbuf );
    if( strlen( tmpbuf ) )
    {
        /* NOTE: You must read the month and day into a temporary unsigned
         * variable. If you try to read directly into the unsigned char
         * values ddate.day or ddate.month, sscanf (or scanf) will read a
         * word, thus overriding adjacent bytes.
         */
        sscanf( tmpbuf, "%u/%u/%u", &tmpmonth, &tmpday, &tmpyear );
        if( (tmpyear - 80) <= 20 )
            ddate.year = tmpyear + 1900;
        else if( (tmpyear >= 1980) && (tmpyear <= 2099 ) )
            ddate.year = tmpyear;
        if( (tmpmonth + 1) <= 13 )
            ddate.month = (unsigned char)tmpmonth;
        if( (tmpday + 1) <= 32 )
            ddate.day = (unsigned char)tmpday;
        _dos_setdate( &ddate );
        _dos_getdate( &ddate );
        printf( "New date: %u/%02u/%02u %s\n",
                ddate.month, ddate.day, ddate.year - 1900,
                days[ddate.dayofweek] );
    }

    /* Get current time. */
    _dos_gettime( &dtime );
    printf( "Time: %u:%02u:%02u\n", dtime.hour, dtime.minute, dtime.second );
    printf( "Enter new time: " );

    /* Get a time string and if it's not 0 (for CR), assign it to time. */
    gets( tmpbuf );
    if( strlen( tmpbuf ) )
    {
        sscanf( tmpbuf, "%u:%u:%u", &tmphour, &tmpminute, &tmpsecond );
        if( tmphour < 24 )
            dtime.hour = (unsigned char)tmphour;
        if( tmpminute < 60 )
            dtime.minute = (unsigned char)tmpminute;
        if( tmpsecond < 60 )
            dtime.second = (unsigned char)tmpsecond;
        _dos_settime( &dtime );
        _dos_gettime( &dtime );
        printf( "New time: %u:%02u:%02u\n",
                dtime.hour, dtime.minute, dtime.second );
    }
}
