#include <d:\usr\stdlib\stdio.h>

static	char	*_strtok = NULL;	/* local token pointer */

char *strtok(string, delim)
register char *string;
register char *delim;
/*
 *	Return a token from <string>.  If <string> in not NULL, it is
 *	the beginning of a string from which tokens are to be extracted.
 *	Characters found in <delim> are skipped over to find the start
 *	of a token, characters are then accumulated until a character in
 *	<delim> is found, or the terminator of <string> is reached.
 *	A pointer to the '\0' terminated token is then returned.  Note
 *	that this function modifies <string> (by inserting '\0's) in
 *	the process.  Subsequent calls to strtok() may specify NULL as
 *	the <string> argument, in which case subsequent tokens are
 *	returned, or NULL if there are no more tokens.
 */
{
	register char *p;
	char *strchr();

	if(string == NULL)
		string = _strtok;
	while(*string && strchr(delim, *string))
		++string;
	if(*string == '\0')		/* no more tokens */
		return(NULL);
	p = string;
	while(*string && !strchr(delim, *string))
		++string;
	if(*string != '\0')
		*string++ = '\0';
	_strtok = string;
	return(p);
}
