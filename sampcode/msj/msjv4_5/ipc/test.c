/* test.c RHS 5/1/89
 *
 * prints "This is a test" in a loop
 */

#define	INCL_DOS

#include<os2.h>
#include<stdio.h>
#include<stdlib.h>

void main(int argc, char **argv)
	{
	unsigned long junk = 0, sleeptime = 100L, maxcount = 0xffffffff;

	if(argc > 1)
		junk = sleeptime = (atoi(argv[1])*1L);
	if(argc > 2)
		maxcount = (atoi(argv[2])*1L);

	while(TRUE)
		{
		printf("This is a test %ld\n", junk++);
		DosSleep(sleeptime);
		if(junk >= maxcount)
			break;
		}
	}
