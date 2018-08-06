/*
 *	ls_fcomp -- file and directory comparison functions
 */

#include <string.h>
#include "ls.h"

extern int Modtime;
extern int Reverse;


/*
 *	ls_fcomp -- compare two "file" items
 */

int
ls_fcomp(s1, s2)
struct OUTBUF *s1, *s2;
{
	int result;

	if (Modtime) {
		if ((result = s1->o_date - s2->o_date) == 0)
			result = s1->o_time - s2->o_time;
	}
	else
		result = strcmp(s1->o_name, s2->o_name);

	return (Reverse ? -result : result);
} /* end_fcomp() */


/*
 *	dcomp -- compare two "directory" items
 */

int
ls_dcomp(s1, s2)
char *s1, *s2;
{
	int result;

	result = strcmp(s1, s2);

	return (Reverse ? -result : result);
} /* end ls_dcomp() */
