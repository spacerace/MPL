/*
 *	sound -- produce a constant tone for a specified duration
 */

#include <local\sound.h>

void
sound(f, dur)
unsigned int f;	/* frequency of pitch in hertz */
float dur;	/* in seconds and tenths of seconds */
{
	extern void setfreq(unsigned int);
	extern void delay(float);
	
	/* set frequency in hertz */
	setfreq(f);
	
	/* turn the speaker on for specified duration */
	SPKR_ON;
	delay(dur);
	SPKR_OFF;
}