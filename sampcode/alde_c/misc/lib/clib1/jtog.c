/*                    *** jtog.c ***                                 */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* function to convert a julian date (1 = 1st day AD) into a         */
/* gregorian date in the format mm/dd/yy.  Returns a 0 if successful */
/* or a -1 if not.                                                   */
/*                                                                   */
/* WARNING - if outdate is not dimensioned at least 9 characters a   */
/*           memory overwrite will occure.                           */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984                             */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define CENTRY 19
#define TRUE 1
#define FALSE 0
 
int jtog(julian,outdate)
char outdate[];
register long julian;
{
	static int monthd[] = {0,31,59,90,120,151,181,212,243,273,304,334};
	int centry,leap,i,iyr,imo,iday;
	static long cdays = 36524, ydays = 365;

	/* reduce julian from 1st day AD to 1st day of CENTRY */
	julian -= CENTRY*cdays + CENTRY/4;

	/* determine year */
	iyr = julian/ydays;
	if ((julian - (iyr*ydays + iyr/4)) < 0)
		iyr -= 1;
	if (iyr < 0)
		return(-1);

	/* determine if this is a leap year or not */
	if (iyr%4 == 0 && iyr != 0 || CENTRY%4 == 0)
		leap = TRUE;
	else
		leap = FALSE;

	/* determine month */
	julian = julian - (iyr*ydays + iyr/4);
	if (leap) julian += 1;
	for (imo=12;imo>0;imo--) {
		i=0;
		if ((leap) && (imo > 2)) i = -1;
		if ((julian - monthd[imo-1] + i) > 0) break;
	}
	if (imo == 0) imo = 1;

	/* determine day */
	iday = julian - monthd[imo-1] + i;
	if (iday == 0) {
		iyr -= 1;
		if (iyr < 0) return(-1);
		imo = 12;
		iday = 31;
	}

	/* transfer into output string */
	if (imo > 9)
		outdate[0] = '1';
	else
		outdate[0] = '0';
	outdate[1] = (char)(imo%10 + '0');
	outdate[2] = '/';

	if (iday > 9)
		outdate[3] = (char)(iday/10 + '0');
	else
		outdate[3] = '0';
	outdate[4] = (char)(iday%10 + '0');
	outdate[5] = '/';

	if (iyr > 9)
		outdate[6] = (char)(iyr/10 + '0');
	else
		outdate[6] = '0';
	outdate[7] = (char)(iyr%10 + '0');
	outdate[8] = '\0';

	/* done */
	return(0);
}
