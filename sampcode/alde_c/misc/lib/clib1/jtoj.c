/*                   *** jtoj.c ***                                  */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* integer function that returns the julian date (1 = 1st day AD)    */
/* associated with the julian date in the form (yddd).  Returns a -1 */
/* if an error occured.                                              */
/*                                                                   */
/* Written by L. Cuthbertson, March 1983.                            */
/*                                                                   */
/*********************************************************************/
                 
#define CENTRY 19	/* current century */
#define DECADE 80	/* current decade */

long jtoj(jin)
char jin[];
{
	static int monthd[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i;
	int leapd,iyr,idays;
	static long cdays = 36524, ydays = 365;

	/* parse input julian date into its pieces */
	sscanf(jin,"%1d%3d",iyr,idays);

	/* calculate year */
	iyr += DECADE;
	if (iyr < 0 || iyr > 99) return(-1);

	/* check for invalid number of days */
	if (idays < 1)
		return(-1);

	if ((iyr%4 == 0) && ((iyr != 0) || (CENTRY%4 == 0))) {
		if (idays > (ydays+1))
			return(-1);
	} else {
		if (idays > ydays)
			return(-1);
	}

	/* determine the number of "extra" leap years caused by the  */
	/* %400 criteria and add to it the number of leap years that */
	/* has occured up to the prior year of current century.      */
	leapd = CENTRY/4;
	if (iyr != 0) leapd += (iyr-1)/4;

	/* calculate julian date */
	return (CENTRY*cdays + iyr*ydays + leapd + idays);
}
