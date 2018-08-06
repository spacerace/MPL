/*
 *  fatal -- issue a diagnostic message and terminate
 */

#include <stdio.h>
#include <stdlib.h>

void
fatal(pname, mesg, errlevel)
char *pname;	/* program name */
char *mesg;	/* message text */
int errlevel;	/* errorlevel (exit code) */
{
	/* display error message */
	fputs(pname, stderr);
	fputc(':', stderr);
	fputc(' ', stderr);
	fputs(mesg, stderr);

	/* return to DOS with the specified errorlevel */
	exit(errlevel);
}
