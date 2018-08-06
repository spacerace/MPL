/* 
 *	delay -- provide a delay of ** approximately ** the
 *	specified duration (resolution is about 0.055 second)
 */

#include <local\timer.h>

void
delay(d)
float d;	/* duration in seconds and fractional seconds */
{
	long ticks, then;
	extern long getticks();

	/* convert duration to number of PC clock ticks */
	ticks = d * TICKRATE;

	/* delay for the specified interval */
	then = getticks() + ticks;
	while (1)
		if (getticks() >= then)
			break;
}
