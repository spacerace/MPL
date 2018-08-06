#include <stdio.h>
#include <io.h>

char *findfile(afn, ext)
register char *afn, *ext;
/*
 *	Return full file spec for <afn> if found. If <afn> has no extension,
 *	extensions from <ext> are tried until a match is found, or the list
 *	ends.  <ext> is a list of extensions separated by '\0' characters
 *	and ending with an additional '\0', ie. "ttp\0tos\0ttp\0" (note that
 *	the final null is by the compiler to the string constant.  If <afn>
 *	alerady has an extension, <ext> is not used.  If no matching files
 *	are found, NULL is returned.  The pointer returned when a match is
 *	found points to a buffer which is internal to fullpath().  If you
 *	want to save the value returned, you must make a copy before the
 *	buffer is overwritten by subsequent calls.
 */
{
	char *fullpath(), *strrchr(), *strchr(), *strcpy();
	register char *p;

	afn = fullpath(NULL, afn);
	if((p = strrchr(afn, '\\')) &&
	   (p = strchr(p, '.')))		/* .EXT specified */
		return(access(afn, _ACCr) ? afn : NULL);
	p = strrchr(afn, '\0');
	*p++ = '.';
	do {						/* try list .EXT's */
		strcpy(p, ext);
		if(access(afn, _ACCr))
			return(afn);
		while(*ext++)
			;
	} while(*ext);
	return(NULL);
}
