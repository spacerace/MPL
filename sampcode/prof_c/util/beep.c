/*
 *	beep -- sound the terminal beeper
 */

#include <stdio.h>

#define BEL	7

void
beep()
{
	putchar(BEL);
}
