#include <time.h>

extern	long	gemdos();

long cnvtime(rawtime, time)
long *rawtime;
register struct tm *time;
/*
 *	Convert <time> structure value to raw time format.  In addition to
 *	returning the raw time value, if the <rawtime> pointer in not NULL,
 *	the value is stored in the long <rawtime> points to. (cf: time)
 */
{
	register long t;

	t = (time->tm_year - 80) & 0x7F;
	t <<= 4;
	t |= (time->tm_mon - 1) & 0x0F;
	t <<= 5;
	t |= (time->tm_mday) & 0x1F;
	t <<= 5;
	t |= (time->tm_hour) & 0x1F;
	t <<= 6;
	t |= (time->tm_min) & 0x3F;
	t <<= 5;
	t |= (time->tm_sec >> 1) & 0x1F;
	if(rawtime)
		*rawtime = t;
	return(t);
}

void stime(rawtime)
long *rawtime;
/*
 *	Set the system clock to <rawtime>.
 */
{
	register int *ip = rawtime;

	gemdos(0x2B, ip[0]);		/* set date from high word */
	gemdos(0x2D, ip[1]);		/* set time from low word */
}

int utime(pathname, rawtime)
char *pathname;
long *rawtime;
/*
 *	Set the modification date of <pathname> to <rawtime>.  Returns zero
 *	for success, or a negative error code.
 */
{
	register int h, *ip = rawtime;
	int d_and_t[2];
	long raw;

	if(!ip) {
		ip = &raw;					/* current */
		time(ip);					/*  time   */
	}
	if((h = ((int) gemdos(0x3D, pathname, 2))) < 0)		/* open */
		return(h);					/* FAILURE */
	d_and_t[0] = ip[1];					/* time */
	d_and_t[1] = ip[0];					/* date */
	gemdos(0x57, d_and_t, h, 1);				/* timestamp */
	gemdos(0x3E, h);					/* close */
	return(0);						/* SUCCESS */
}
