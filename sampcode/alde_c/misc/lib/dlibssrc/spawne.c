#include <osbind.h>
#include <stdio.h>

int spawne(program, cmdline, envp)
char *program;
char *cmdline;
char *envp;
/*
 *	Calls the OS "Pexec" trap with the parameters specified.  Since
 *	the OS uses a Pascal-like string for the command line, the
 *	<cmdline> parameter, which is a normal C-string, is used to
 *	create the actual command line which is passed to the OS trap.
 */
{
	char execbuf[128];		/* buffer for exec command line */
	register int n;

	if(cmdline) {
		if((n = strlen(cmdline)) > 126)
			n = 126;
		execbuf[0] = n;
		strncpy(execbuf+1, cmdline, n);
		execbuf[n+2] = '\0';
	}
	else
		execbuf[0] = execbuf[1] = '\0';
	n = Pexec(0, program, execbuf, envp);
	return(n);
}
