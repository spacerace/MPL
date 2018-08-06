/*
 *	fconfig -- return a FILE pointer to a local or
 *	global configuration file, or NULL if none found
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <local\std.h>

FILE *
fconfig(varname, fname)
char *varname;
char *fname;
{
	FILE *fp;
	char pname[MAXPATH + 1];
	char *p;
	
	/* look for a local configuration file */
	if ((fp = fopen(fname, "r")) != NULL)
		return (fp);

	/* look for a directory variable */
	if ((p = getenv(strupr(varname))) != NULL) {
		strcpy(pname, p);
		strcat(pname, "\\");
		strcat(pname, fname);
		if ((fp = fopen(pname, "r")) != NULL)
			return (fp);
	}
	
	/* didn't find anything to read */
	return (NULL);
}
