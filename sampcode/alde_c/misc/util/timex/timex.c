/*
        	Time a DOS command and display its exit code.
			Portability of this program  approaches zero.
			Note that the program contains functions which
			are peculiar to the Lattice C library, and 
			that it references locations peculiar to the 
			IBM PC's memory map . Portability to workalikes is 
			unknown.  Invoking the program with no arguments 
			on the commandline produces a help screen.
			Noncommercial distribution of source and object
			code is permitted and encouraged provided that
			this notice is left intact.
			

									Ken Rice 
									Rt #2
									Watkinsville, GA 30677
									July 4, 1985
*/

#include <stdio.h>
char *malloc();

main(argc,argv)
    int argc;
    char **argv;
    {
	float ticki, tickf;			/* initial and final clock ticks */
	int code;					/* dos return code */
	unsigned i = 0,buf[2];		/* holds 32-bit clock tick count */
	char *nargv[32];			/* new commandline argument vector */
	extern char *helptxt[];		/* the help screen */

	if (argc == 1)
		help(helptxt);

	while (--argc > 0) {
		if ((nargv[i] = malloc(strlen(argv[i+1]))) == NULL) {
			fprintf(stderr,"timex: out of memory timing %s\n",argv[1]);
			exit(1);
		}
		strcpy(nargv[i],argv[i+1]);
		i++;
	}
	nargv[i] = NULL;

	peek(0x0040,0x006c,buf,4);
	ticki = (0x1000 * (float)buf[1]) + (float)buf[0];

	if (forkvp(argv[1],nargv)) {
		fprintf(stderr,"timex: cannot find %s for timing\n",nargv[0]);
		exit(1);
		}

	code = wait();
	peek(0x0040,0x006c,buf,4);	
	tickf = (0x1000 * (float)buf[1]) + (float)buf[0];
	printf("\n%9.1fs   (%d)\n",(tickf-ticki) * 0.0549254,code);
	}

/*
	help module:  function and text
*/

help(mesg)
char **mesg;
{
	while (*mesg != NULL)
		puts(*mesg++);
		exit(0);
}

char *helptxt[] = {
"\nDisplays the running time and exit code of a program.\n",
"              Usage: timex <commandline>\n",
"Where <commandline>  is any COM or EXE program in the current", 
"directory or command search path and its associated arguments",
"and parameters.     The timex command won't time BAT files or",
"resident DOS commands (COPY, e.g.).  Run time is displayed in", 
"seconds to the nearest tenth,  and the exit code is displayed",
"as an eight-bit integer set off by parentheses.      Internal",
"resolution of the timer is ca. 0.055 seconds.   Noncommercial",
"distribution  of this  program  is  permitted  and encouraged",
"provided that this notice is left intact.\n",
"          Ken Rice  Rt #2  Watkinsville GA 30677",
NULL };
