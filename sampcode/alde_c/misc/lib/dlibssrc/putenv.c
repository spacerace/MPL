#include <stdio.h>

extern char	*_envp;

#define	ENVSIZ	2048

static	int	envset = FALSE;		/* local env created? */

int putenv(entry)
char *entry;
/*
 *	Add <entry> to the environment.  <entry> can be any of the following
 *	forms:
 *		<VARIABLE>
 *		<VARIABLE>=
 *		<VARIABLE>=<value>
 *	The first form removes <VARIABLE> from the environment.  getenv()
 *	will return NULL if looking for this variable.  The second form adds
 *	<VARIABLE> to the environment, with a null value.  getenv() will
 *	return a pointer to a '\0' character if looking for this variable.
 *	Many environment handlers don't support such "tag variables", so
 *	their use is not recommended.  The final form is the most common,
 *	and safest to use.  <VARIABLE> is installed (or replaced) with the
 *	value <value>.  It should be noted that this function itself is not
 *	supported in many systems and should be used will care to prevent
 *	overflowing the space allocated for the environment.
 */
{
	register char *p, *q, *t, c;
	register int len;
	char *malloc(), *getenv();

	if(!envset) {					/* no local env */
		if((p = malloc(ENVSIZ)) == NULL)
			return(FALSE);
		q = _envp;
		_envp = p;
		envset = TRUE;
		if(q) {
			while(*q)
				while(*p++ = *q++)
					;
		}
		else
			*p++ = '\0';
		*p++ = '\0';
		*p = 0xFF;
	}
	for(t=entry; (c = *t) && (c != '='); ++t)
		;
	*t = '\0';
	if(p = getenv(entry)) {				/* remove old var */
		q = p;
		while(*q++)			/* find end of old val */
			;
		p -= (len = strlen(entry));
		while(strncmp(--p, entry, len))	/* find start of old var */
			;
		while(*q)			/* copy environment tail */
			while(*p++ = *q++)
				;
		*p++ = '\0';			/* tie off environment */
		*p = 0xFF;
	}
	if(c == '=') {					/* install new var */
		p = _envp;
		while(*p)			/* find end of env */
			while(*p++)
				;
		*t = c;
		q = entry;
		while(*p++ = *q++)		/* copy new entry */
			;
		*p++ = '\0';			/* tie off environment */
		*p = 0xFF;
	}
	return(TRUE);
}
