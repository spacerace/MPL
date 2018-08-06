#include <stdio.h>

void abort()
/*
 *	Prints the message "Abnormal program termination" to stderr and
 *	calls _exit() with a status code of 3.
 */
{
	fputs("Abnormal program termination\n", stderr);
	fflush(stderr);
	_exit(3);
}
