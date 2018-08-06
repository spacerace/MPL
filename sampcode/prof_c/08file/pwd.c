/*
 *	pwd -- print (display actually) the current directory pathname
 */

#include <stdio.h>
#include <direct.h>
#include <local\std.h>

main()
{
	char *path;

	if ((path = getcwd(NULL, MAXPATH)) == NULL) {
		perror("Error getting current directory");
		exit(1);
	}
	printf("%s\n", path);
	exit(0);
}

_setargv()
{
}

_setenvp()
{
}

_nullcheck()
{
}
