/*                     *** cvtdate.c ***                             */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to check and convert a "packed" gregorian date (1/1/83)  */
/* into its "expanded" form (01/01/83).  Will return a 0 if success- */
/* ful or a -1 if unable to convert gregorian date received.         */
/*                                                                   */
/* WARNING - if indate has not been declared at least 9 characters a */
/*           memory overwrite will occure.                           */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#include <ctype.h>

int cvtdate(indate)
char indate[];
{
	char hold[9];
	int i;

	i = 0;		/* position within indate */

	/* get month */
	if (isdigit(indate[i]))
		if (isdigit(indate[i+1])) {
			hold[0] = indate[i++];
			hold[1] = indate[i++];
		} else {
			hold[0] = '0';
			hold[1] = indate[i++];
		}
	else
		return(-1);

	/* insert slash */
	if (isdigit(indate[i++]))
		return(-1);
	else
		hold[2] = '/';

	/* get day */
	if (isdigit(indate[i]))
		if (isdigit(indate[i+1])) {
			hold[3] = indate[i++];
			hold[4] = indate[i++];
		} else {
			hold[3] = '0';
			hold[4] = indate[i++];
		}
	else
		return(-1);

	/* insert slash */
	if (isdigit(indate[i++]))
		return(-1);
	else
		hold[5] = '/';

	/* get year */
	if (isdigit(indate[i]))
		if (isdigit(indate[i+1])) {
			hold[6] = indate[i++];
			hold[7] = indate[i++];
		} else {
			hold[6] = '0';
			hold[7] = indate[i++];
		}
	else
		return(-1);

	/* insert null terminator */
	if (indate[i] != '\0')
		return(-1);
	else
		hold[8] = '\0';

	/* copy hold into indate */
	strcpy(indate,hold);

	/* done */
	return(0);
}
