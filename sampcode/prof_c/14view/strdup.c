/*
 *	strdup -- duplicate a string
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

char *
strdup(str)
char *str;
{
	char *buf;

	buf = malloc(strlen(str) + 1);
	return (buf == NULL ? buf : strcpy(buf, str));
}
