/*
 *	vf_srch -- search functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>
#include "vf.h"

/*
 *	search -- search for a literal string in the buffer
 */

DNODE *
search(buf, dir, str)
DNODE *buf;
DIRECTION dir;
char *str;
{
	int n;
	register DNODE *lp;
	register char *cp;

	extern void showmsg(char *);

	/* try to find a match -- wraps around buffer boundaries */
	n = strlen(str);
	lp = (dir == FORWARD) ? buf->d_next : buf->d_prev;
	while (lp != buf) {
		if ((cp = lp->d_line) != NULL)	/* skip over header node */
			while (*cp != '\n' && *cp != '\0') {
				if (strncmp(cp, str, n) == 0)
					return (lp);
				++cp;
			}
		lp = (dir == FORWARD) ? lp->d_next : lp->d_prev;
	}
	showmsg("Not found");
	return ((DNODE *)NULL);
}


/*
 *	getsstr -- prompt the user for a search string
 */

extern int Startscan, Endscan;

char *
getsstr(str)
char *str;
{
	char line[MAXSTR];
	char *resp;

	extern int putstr(char *, int);
	extern char *getstr(char *, int);
	extern int put_ch(char, int);
	extern void showmsg(char *);
	extern void clrmsg();

	static char prompt[] = { "Search for: " };

	/* get search string */
	showmsg(prompt);
	setctype(Startscan, Endscan);		/* cursor on */
	resp = getstr(line, MAXSTR - strlen(prompt));
	setctype(MAXSCAN, MAXSCAN);		/* cursor off */
	if (resp == NULL)
		return (char *)NULL;
	if (strlen(resp) == 0)
		return (str);
	showmsg(resp);
	return (resp);
}
