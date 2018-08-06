/*
 *	tone -- set the frequency of the sound generator
 */

#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
	extern void setfreq(unsigned int);

	if (argc != 2) {
		fprintf(stderr, "Usage: tone hertz\n");
		exit(1);
	}

	/* set the frequency in Hertz */
	setfreq(atoi(*++argv));
	exit(0);
}
