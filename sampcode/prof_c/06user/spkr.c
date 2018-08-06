/*
 *	spkr -- turn speaker ON/OFF
 *
 *		no args => OFF
 *		any arg(s) => ON
 */

#include <local\sound.h>

main(argc, argv)
int argc;
char **argv;
{
	/* turn speaker on or off */
	if (argc == 1)
		SPKR_OFF;
	else
		SPKR_ON;
	exit(0);
}
