#include <stdio.h>

char *pfindfile(afn, ext)
register char *afn;
char *ext;
/*
 *	Like findfile() but search all directories listed in the environment
 *	variable "PATH", if no match is found in the current directory. If
 *	<afn> specifies a drive or directory, "PATH" is not used.
 */
{
	static char tmp[PATHSIZE];
	register char *path, *p;
	char *findfile(), *strchr(), *getenv(), *strcpy();

	if((p = findfile(afn, ext)) ||			/* search . first */
	   strchr(afn, '\\') || strchr(afn, ':'))	/* path specified */
		return(p);
	if(path = getenv("PATH")) {			/* get PATH= */
		while(*path) {
			p = tmp;
			while((*path != '\0') &&
			      (*path != ';') &&
			      (*path != ','))	/* copy directory */
				*p++ = *path++;
			if(*path)			/* move past delim */
				++path;
			if(p[-1] != '\\')		/* add \ if needed */
				*p++ = '\\';
			strcpy(p, afn);			/* copy filename */
			if(p = findfile(tmp, ext))	/* do search */
				return(p);
		}
	}
	return(NULL);
}
