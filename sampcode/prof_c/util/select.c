/*
 *	select -- functions to create a selection table and
 *	to determine whether an item is an entry in the table
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>

#define NRANGES	10
#define NDIGITS	5

struct slist_st {
	long int s_min;
	long int s_max;
} Slist[NRANGES + 1];

long Highest = 0;

/*
 *	mkslist -- create the selection lookup table
 */

int
mkslist(list)
char *list;
{
	int i;
	char *listp, *s;
	long tmp;

	static long save_range();

	/* fill in table of selected items */
	if (*list == '\0') {
		/* if no list, select all */
		Slist[0].s_min = 0;
		Slist[0].s_max = Highest = BIGGEST;
		Slist[1].s_min = -1;
	}
	else {
		listp = list;
		for (i = 0; i < NRANGES; ++i) {
			if ((s = strtok(listp, ", \t")) == NULL)
				break;
			if ((tmp = save_range(i, s)) > Highest)
				Highest = tmp;
			listp = NULL;
		}
		Slist[i].s_min = -1;
	}
	return (0);
} /* end mkslist() */

/*
 *	selected -- return non-zero value if the number
 *	argument is a member of the selection list
 */

int
selected(n)
unsigned int n;
{
	int i;

	/* look for converted number in selection list */ 
	for (i = 0; Slist[i].s_min != -1; ++i)
		if (n >= Slist[i].s_min && n <= Slist[i].s_max)
			return (1);
	return (0);
} /* end selected() */

/*
 *	save_range -- convert a string number spec to a
 *	numeric range in the selection table and return
 *	the highest number in the range
 */

static long
save_range(n, s)
int n;
char *s;
{
	int radix = 10;
	char *cp, num[NDIGITS + 1];

	/* get the first (and possibly only) number */
	cp = num;
	while (*s != '\0' && *s != '-')
		*cp++ = *s++;
	*cp = '\0';
	Slist[n].s_min = atol(num);
	if (*s == '\0')
		/* pretty narrow range, huh? */
		return (Slist[n].s_max = Slist[n].s_min);

	/* get the second number */
	if (*++s == '\0')
		/* unspecified top end of range */
		Slist[n].s_max = BIGGEST;
	else {
		cp = num;
		while (*s != '\0' && *s != '-')
			*cp++ = *s++;
		*cp = '\0';
		Slist[n].s_max = atol(num);
	}
	return (Slist[n].s_max);
} /* end save_range() */
