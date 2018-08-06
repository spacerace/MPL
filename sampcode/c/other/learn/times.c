/* TIMES.C illustrates various time and date functions including:
 *      time            ftime           ctime
 *      localtime       asctime         gmtime          mktime
 *      _strtime        _strdate        tzset
 *
 * Also the global variable:
 *      tzname
 */

#include <time.h>
#include <stdio.h>
#include <sys\types.h>
#include <sys\timeb.h>
#include <string.h>

main()
{
    char timebuf[9], datebuf[9];
    static char ampm[] = "AM";
    time_t ltime;
    struct timeb tstruct;
    struct tm *today, *tomorrow, *gmt;

    /* Set time zone from TZ environment variable. If TZ is not set,
     * PST8PDT is used (Pacific standard time, daylight savings).
     */
    tzset();

    /* Get Xenix-style time and display as number and string. */
    time( &ltime );
    printf( "Time in seconds since GMT 1/1/70:\t%ld\n", ltime );
    printf( "Xenix time and date:\t\t\t%s", ctime( &ltime ) );

    /* Display GMT. */
    gmt = gmtime( &ltime );
    printf( "Greenwich Mean Time:\t\t\t%s", asctime( gmt ) );

    /* Convert to local time structure and adjust for PM if necessary. */
    today = localtime( &ltime );
    if( today->tm_hour > 12 )
    {
        strcpy( ampm, "PM" );
        today->tm_hour -= 12;
    }
    /* Note how pointer addition is used to skip the first 11 characters
     * and printf is used to trim off terminating characters.
     */
    printf( "12-hour time:\t\t\t\t%.8s %s\n",
            asctime( today ) + 11, ampm );

    /* Make tomorrow's time. */
    *tomorrow = *today;
    tomorrow->tm_mday++;
    if( mktime( tomorrow ) != (time_t)-1 )
        printf( "Tomorrow's date:\t\t\t%d/%d/%.2d\n",
                tomorrow->tm_mon + 1, tomorrow->tm_mday, tomorrow->tm_year );

    /* Display DOS date and time. */
    _strtime( timebuf );
    printf( "DOS time:\t\t\t\t%s\n", timebuf );
    _strdate( datebuf );
    printf( "DOS date:\t\t\t\t%s\n", datebuf );

    /* Print additional time information. */
    ftime( &tstruct );
    printf( "Plus miliseconds:\t\t\t%u\n", tstruct.millitm );
    printf( "Zone difference in seconds from GMT:\t%u\n", tstruct.timezone );
    printf( "Time zone name:\t\t\t\t%s\n", tzname[0] );
    printf( "Daylight savings:\t\t\t%s\n", tstruct.dstflag ? "YES" : "NO" );
}
