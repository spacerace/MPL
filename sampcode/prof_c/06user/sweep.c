/*
 *	sweep -- produce a sound that sweeps from
 *	a low to a high frequency repeatedly until a
 *	key is pressed
 */

#include <conio.h>
#include <local\sound.h>

main()
{
	unsigned int f;
	int d, n;
	extern void setfreq(unsigned int);

	SPKR_ON;
	while (1) {
		/* give user a way out */
		if (kbhit())
			break;
		n = 10;
		for (f = 100; f <= 5000; f += n) {
			setfreq(f);
			d = 1000;
			/* fake a short delay (machine dependednt) */
			while (d-- > 0)
				;
			n += 10;
		}
	}
	SPKR_OFF;
	exit(0);
}
