#include <stdio.h>

int spawnpe(program, cmdline, envp)
char *program;
char *cmdline;
char *envp;
/*
 *	Operates like spawne() except that the "PATH" environment variable,
 *	if it is valid, is used to locate the program to be executed.
 */
{
	register char *p;
	char *pfindfile();

	if(p = pfindfile(program, "ttp\0tos\0prg\0app\0"))
		return(spawne(p, cmdline, envp));
	return(ERROR);
}
