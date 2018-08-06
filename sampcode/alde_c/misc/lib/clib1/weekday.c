/*                     *** weekday.c ***                             */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* function to determine the day of the week a given gregorian date  */
/* falls on.  Returns a 0 if successful or a -1 if not.              */
/*                                                                   */
/* WARNING - day must be declared to be at least 10 characters or a  */
/*           memory overwrite will occure.                           */
/*                                                                   */
/* Written by L. Cuthbertson, March 1983                             */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

int weekday(indate,day)
char indate[],day[];
{
	static char days [7][10] = {'S','u','n','d','a','y','\0',' ',' ',' ',
	                            'M','o','n','d','a','y','\0',' ',' ',' ',
	                            'T','u','e','s','d','a','y','\0',' ',' ',
	                            'W','e','d','n','e','s','d','a','y','\0',
	                            'T','h','u','r','s','d','a','y','\0',' ',
	                            'F','r','i','d','a','y','\0',' ',' ',' ',
	                            'S','a','t','u','r','d','a','y','\0',' '};
	int iday;
	long julian,gtoj();

	/* get julian date */
	if ((julian = gtoj(indate)) == (-1)) return(-1);

	/* calculate day of week */
	iday = (julian-1) % 7;

	/* move weekday into character string */
	strcpy(day,days[iday]);

	/* done */
	return(0);
}
