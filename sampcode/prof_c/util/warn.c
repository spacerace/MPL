/*
 *	warn -- display a warning message in a highly
 *	visible place; return number of characters written
 */

#include <stdio.h>
 
int
warn(pname, mesg)
char *pname, *mesg;
{
	return (fprintf(stderr, "%s: %s\n", pname, mesg));
}
