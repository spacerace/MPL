/*
 *	interval -- report the interval given in seconds as
 *	a human-readable null-terminated string
 */

#include <stdio.h>

char *
interval(seconds, buf)
long seconds;
char *buf;
{
	int hh, mm, ss;
	long remainder;

	/* calculate the values */
	hh = seconds / 3600;
	remainder = seconds % 3600;
	mm = remainder / 60;
	ss = remainder - (mm * 60);
	sprintf(buf, "%02d:%02d:%02d\0", hh, mm, ss);

	return (buf);
}
