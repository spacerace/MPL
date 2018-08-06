#include <osbind.h>
#include <stdio.h>
#include <io.h>

extern char	*_base;

shell()
/*
 *	Invoke a command line interface shell.  If the "SHELL" environment
 *	variable is not defined, a prompt showing the current working
 *	directory will be given.  Each line entered will then be passed
 *	to the system() function for execution until the command "exit"
 *	is entered to terminate the shell.  If "SHELL" is defined, and
 *	the "_shell_p" variable is valid, the value of "SHELL" will be
 *	passed to the program pointed to by "_shell_p" in order to allow
 *	the shell to invoke a command line interaction of its own.  If
 *	the "_shell_p" variable is not valid, the program defined by
 *	"SHELL" will be searched for along the "PATH", and executed with
 *	no arguments.  If the "SHELL" can't be found, the internal command
 *	line described above will be used.
 */
{
	register char *p, *q;
	register int rv = 0;
	register int (*shell)();
	char command[128];
	char *getenv(), *strtok(), *pfindfile(), *fullpath();
	int getch(), putch();

	/* SHELL= variable? */
	if((p = getenv("SHELL")) && (*p)) {	/* SHELL is valid? */
		q = Super(0L);
		shell = *((long *) 0x4F6L);	/* get _shell_p value */
		Super(q);
		if((shell) &&				/* shell available */
		   (((long) shell) < ((long) _base)) &&	/* reasonable value */
		   (strncmp(shell, "PATH", 4))) {	/* not corrupted */
			rv = (*shell)(p);		/* call shell */
			return;
		}
		if(p = pfindfile(p, "prg\0ttp\0tos\0")) {
			spawn(p, "");			/* execute shell */
			return;
		}
	}
	/* internal command line interface */
	for(;;) {
		cprintf("%s> ", fullpath(NULL, ""));	/* cwd as prompt */
		getln(NULL, getch, putch, command, 128);/* editable field */
		cputs("\r\n");
		if(command[0] == '\0')			/* ignore blank lines */
			continue;
		if(!stricmp(command, "exit"))		/* leave the shell */
			return;
		system(command);			/* execute a command */
	}
}
