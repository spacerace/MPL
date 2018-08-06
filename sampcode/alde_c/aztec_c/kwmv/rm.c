#include <stdio.h>
main(argc,argv)
	int argc;
	char *argv[];
{
	static char cmdbuf[128] = "del ";
	while(--argc) 
	{
		++argv;
		if (-1 == unlink(*(argv)))
		{
			fprintf(stderr,"rm : %s not found\n",*argv);
		}
	}
}


