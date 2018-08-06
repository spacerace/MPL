#include <osbind.h>
#include <stdio.h>

char *fullpath(full, part)
register char *full;
register char *part;
/*
 *	<part> is a (possibly) ambiguous file/path specification.  A
 *	non-ambiguous file/path spec is created which includes a drive
 *	letter and all intermediate sub-directories.  If the partial
 *	specification is not valid, NULL is returned, otherwise a
 *	pointer to <full> is returned.  If NULL is specified for <full>,
 *	an internal buffer is used and a pointer to it is returned.
 */
{
	static char buf[PATHSIZE];
	register char *bp = buf;
	register int drv;
	char *strlwr(), *strcpy();

	if (*part && (part[1] == ':')) {
		drv = *bp++ = *part++;
		drv = ((drv - 1) & 0xF);
		*bp++ = *part++;
	}
	else {
		*bp++ = (drv = Dgetdrv()) + 'a';
		*bp++ = ':';
	}
	if (*part == '\\')
		++part;
	else {
		Dgetpath(bp, drv+1);	/* 1 BASED drive number here */
		while(*bp++)
			;
		--bp;
	}
	*bp++ = '\\';
	*bp = '\0';
	while(*part) {			/* handle dots and copy path & file */
		if (*part == '.') {
			if (*++part == '.') {
				if (*++part == '\\') {	/* parent */
					--bp;
					while((*--bp != '\\') && (*bp != ':'))
						;
					if (*bp++ == ':')
						++bp;
					++part;
				}
				else
					return(NULL);	/* illegal pathname */
			}
			else if (*part++ != '\\')
				return(NULL);		/* illegal pathname */
		}
		else
			while(*part && ((*bp++ = *part++) != '\\'))
				;
	}
	*bp = '\0';
	if (full == NULL)	/* special case to use local buffer */
		full = buf;
	return(strlwr(strcpy(full, buf)));	/* lowercase and return */
}
