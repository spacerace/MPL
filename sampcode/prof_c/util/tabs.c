/*
 *	tabs -- a group of cooperating functions that set
 *	and report the settings of "tabstops"
 */

#include <local\std.h>

static char Tabstops[MAXLINE];

/*
 *	fixtabs -- set up fixed-interval tabstops
 */
void
fixtabs(interval)
register int interval;
{
	register int i;

	for (i = 0; i < MAXLINE; i++)
		Tabstops[i] = (i % interval == 0) ? 1 : 0;
} /* end fixtabs() */

/*
 *	vartabs -- set up variable tabstops from an array
 *	integers terminated by a -1 entry
 */
void
vartabs(list)
int *list;
{
	register int i;

	/* initialize the tabstop array */
	for (i = 0; i < MAXLINE; ++i)
		Tabstops[i] = 0;

	/* set user-sprcified tabstops */
	while (*list != -1)
		Tabstops[*++list] = 1;
} /* end vartabs() */

/*
 *	tabstop -- return non-zero if col is a tabstop
 */
int
tabstop(col)
register int col;
{
	return (col >= MAXLINE ? 1 : Tabstops[col]);
} /* end tabstop() */
