#include <osbind.h>
#include <stdio.h>

extern char	*_base;

int system(command)
char *command;
/*
 *	Attempts to pass <command> to the shell program pointed to by
 *	the system variable "_shell_p".  If a valid shell can't be found
 *	there, the "SHELL" environment variable is searched for.  If it
 *	exists and is not empty, it will be the name of the shell program
 *	to execute the given command.  If "SHELL" is not valid, the
 *	"PATH" variable is searched for.  This would specify the paths
 *	to search for the program name which is the first token of the
 *	command.  If "PATH" is not valid, the current directory is
 *	searched as for the given command.  The extensions tried (if none
 *	is specified) are ".TTP", ".TOS", ".PRG" and ".APP".
 */
{
	register char *p, *q;
	register int rv = 0;
	register int (*shell)();
	char *getenv(), *strtok();

	if(!command)
		return(ERROR);

	/* get _shell_p value */
	p = Super(0L);
	shell = *((long *) 0x4F6L);
	Super(p);

	/* validate _shell_p */
	if((shell) &&				/* Shell available. */
	   (((long) shell) < ((long) _base)) &&	/* Reasonable shell pointer. */
	   (strncmp(shell, "PATH", 4))) {	/* Not corrupted */

		/* execute the command */
		rv = (*shell)(command);
		return(rv);
	}

	/* SHELL= variable? */
	if((p = getenv("SHELL")) && (*p))
		return(spawnp(p, command));

	/* parse for spawnp() attempt */
	p = strtok(command, " \t\r\n");
	q = strtok(NULL, "\r\n");
	return(spawnp(p, (q ? q : "")));
}
