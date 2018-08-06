/*
 *	getname -- strip drive identifier and directory node
 *	list, if any, from a pathname;  returns a pointer to
 *	the resulting filename[.ext] string or NULL for error
 */

#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>

#define MAXFSPEC 13

char *
getname(path)
char *path;	/* string to modify */
{
	register char *cp;	/* character pointer */
	struct stat buf;

	/* try to get information about the pathname */
	if (stat(path, &buf) != 0)
		return (NULL);	/* bad pathname */

	/* locate the end of the pathname string */
	cp = path;
	while (*cp != '\0')
		++cp;
	--cp;		/* went one too far */

	/* find the start of the filename part */
	while (cp > path && *cp != '\\' && *cp != ':' && *cp != '/')
		--cp;
	if (cp > path)
		++cp;	/* on a separator (\, :, or /) -- move one past */

	/* return the full filespec (filename[.ext]) */
	return (cp);
}
