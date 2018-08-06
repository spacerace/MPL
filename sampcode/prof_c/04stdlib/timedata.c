/*
 *	timedata -- time zone and time value tests
 */

#include <stdio.h>
#include <time.h>

main()
{
	long now;
	struct tm *tbuf;
	
	/* get TZ data into global variables */
	tzset();

	/* display the global time values */
	printf("daylight savings time flag = %d\n", daylight);
	printf("difference (in seconds) from GMT = %ld\n", timezone);
	printf("standard time zone string is %s\n", tzname[0]);
	printf("daylight time zone string is %s\n", tzname[1]);

	/*
	 *  display the current date and time values for
	 *  local and universal time
	 */
	now = time(NULL);
	printf("\nctime():\t%s\n", ctime(&now));
	tbuf = localtime(&now);
	printf("local time:\t%s\n", asctime(tbuf));
	tbuf = gmtime(&now);
	printf("universal time:\t%s\n", asctime(tbuf));

	exit(0);
}
