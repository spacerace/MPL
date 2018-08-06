/*
 *	setfreq -- sets PC's tone generator to run
 *	continuously at the specified frequency
 */

#include <conio.h>
#include <local\timer.h>

void
setfreq(f)
unsigned f;	/* frequency in Hertz (approximate) */
{
	unsigned divisor = TIMER_CLK / f;

	outp(TIMER_CTRL, TIMER_PREP);		/* prepare timer */
	outp(TIMER_COUNT, (divisor & 0xFF));	/* low byte of divisor */
	outp(TIMER_COUNT, (divisor >> 8));	/* high byte of divisor */
}
