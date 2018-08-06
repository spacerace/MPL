/*                            *** getdate.c ***                      */
/*                                                                   */
/* IBM-PC microsoft "C" under PC-DOS                                 */
/*                                                                   */
/* Function to return a string containing the date in the format     */
/* MM/DD/YY.                                                         */
/*                                                                   */
/* Written by L. Cuthbertson, May 1984                               */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define NULL	'\000'
#define DELIM	'/'

int getdate(string)
char string[];
{
	char month[3],day[3],year[5];
	int imo,iday,iyr;
	int i,j;

	/* call assembler routine to get date in integer format */
	dosdate(&imo,&iday,&iyr);

	/* convert integers into strings */
	sprintf(month,"%02d",imo);
	sprintf(day,"%02d",iday);
	sprintf(year,"%02d",iyr);

	/* build output string */
	j = 0;
	for(i=0;month[i] != NULL;i++)
		string[j++] = month[i];

	string[j++] = DELIM;

	for(i=0;day[i] != NULL;i++)
		string[j++] = day[i];

	string[j++] = DELIM;

	for(i=2;year[i] != NULL;i++)	/* skip century identifier */
		string[j++] = year[i];

	string[j] = NULL;

	/* done */
	return(0);
}
