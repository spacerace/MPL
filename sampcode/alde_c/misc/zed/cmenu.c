/************************************************
*						*
*	CMENU -- MENU DRIVEN PROGRAM LOADER	*
*	BY WELCOM H. WATSON, JR.		*
*						*
*	DOCUMENTED IN CMENU.DOC			*
*						*
************************************************/
#include "stdio.h"
#define MAXPROGS 51	/* number of selectable programs = 51 (0 - 50) */
#define B_SIZE 3	/* buffer size for selection = 2 */
#define C_SIZE 81	/* command line size = 80 */

int _stack = 5120;

main(argc, argv)
int argc;
char *argv[];
{
	FILE *fp, *fopen();
	int c, fgetc();
	char progs[MAXPROGS][C_SIZE];
	char *p, *stpbrk(), *stpblk();

	extern int _oserr;
	int error;
	static char breaks[] = {' ', '\t', '\n', '\0'};
	int i, n;

	char menu[31];

	static char *dos_err[] = {
		" ",
		"INVALID FUNCTION NUMBER",
		"FILE NOT FOUND",
		"PATH NOT FOUND",
		"TOO MANY OPEN FILES (NO HANDLES LEFT)",
		"ACCESS DENIED",
		"INVALID HANDLE",
		"MEMORY CONTROL BLOCKS DESTROYED",
		"INSUFFICIENT MEMORY",
		"INVALID MEMORY BLOCK ADDRESS",
		"INVALID ENVIRONMENT",
		"INVALID FORMAT",
		"INVALID ACCESS CODE",
		"INVALID DATA",
		" ",
		"INVALID DRIVE WAS SPECIFIED",
		"ATTEMPTED TO REMOVE THE CURRENT DIRECTORY",
		"NOT SAME DEVICE",
		"NO MORE FILES"
	};

/*
If no arguments exit.
*/

	if (argc == 1) {
		exit();
	}

/*
Open and read menu control file 
all characters before '~' are output to
console.  Character string beginning after
first occurrence of '~' and ending with
'\n' is the program if '0' or CR only is entered
from the console.  If nothing is between
'~` and '\n' '0' or CR will cause the program
to terminate with exit.

Strings following are the program options for
the fork, with each program name terminated
with '\n'.  The first program name will be loaded
and executed if '1' is entered from the console.
*/
	strcpy(menu, argv[1]);



loop:
	fp = fopen(menu, "ra");
	if (fp == NULL) {
		cprintf("\nCANNOT OPEN [%s]", menu);
		putch(7);
		waitsec(2);
		exit();
	}

	while ( (c = fgetc(fp)) != '~' ) {
		putch(c);
	}


	/* get the program options */
	i = n = 0;
	while ((c = fgetc(fp)) != EOF && i < MAXPROGS) {
		switch (c) {
			case '\n':
				progs[i][n] = '\0';
				++i;
				n = 0;
				break;
			default:
				if (n < C_SIZE) {
					progs[i][n] = c;
					++n;
				}
				break;
		}
	}

	fclose(fp);

	--i;

	n = enterir(i);


	if (*progs[n] == '\0') {
		exit();
	}


	p = progs[n];

	for (i=0; *p != '\0'; ++i) {

		argv[i] = p;
		p = stpbrk(p, breaks);
		if (*p != '\0') {
			*p = '\0';
			++p;
		}

		p = stpblk(p);			

	}

	argv[i] = NULL;


	error = forkvp(argv[0], argv);
	if (error != 0) {
		if (_oserr > 0) {
			cprintf("\nMS-DOS ERROR [%d] IN LOADING '%s'\n%s", _oserr, argv[0], dos_err[_oserr]);
		}
		else {
			error = wait();
			cprintf("\nMS-DOS ERROR [%d] IN EXECUTING '%s'\n%s", error, argv[0], dos_err[error]);
		}
		putch(7);
		waitsec(2);
	}
	goto loop;
}

/*function for entry of non negative integer characters*/

enteri(charcnt)
int *charcnt;	/*pointer to integer which keeps track of # of chars entered*/
{
	char c, buffer[B_SIZE];
	int i, x;
	for (i = 0; (c = getch()) != '\r'; ) {
		if ((c >= '0' && c <= '9') && i < B_SIZE - 1) {
			buffer[i] = c;
			putch(c);
			++i;
		}
		else if (c == '\b' && i > 0) {
			putch('\b');
			putch(' ');
			putch('\b');
			--i;
		}
		else {
			putch(7);
		}
	}
	buffer[i] = '\0';
	*charcnt = i;
	x = atoi(buffer);
	return(x);
}

/*entry of int >= 0 but not > maximum selection*/
enterir(maxsel)
int maxsel;
{
	int x, charcnt;
	while ((x = enteri(&charcnt)) > maxsel) {
		putch(7);
		while (charcnt > 0) {
			putch('\b');
			putch(' ');
			putch('\b');
			--charcnt;
		}
	}
	return(x);
}
