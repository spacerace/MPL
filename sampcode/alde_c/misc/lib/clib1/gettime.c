/*                            *** gettime.c ***                      */
/*                                                                   */
/* IBM-PC microsoft "C" under PC-DOS                                 */
/*                                                                   */
/* Function to return a string containing the time in the format     */
/* HH:MM:SS (military time).                                         */
/*                                                                   */
/* Written by L. Cuthbertson, May 1984                               */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define NULL	'\000'
#define DELIM   ':'

int gettime(string)
char string[];
{
	char hours[3],minutes[3],seconds[3];
	int ih,im,is,iths;
	int i,j;

	/* call assembler routine to get time in integer format */
	dostime(&ih,&im,&is,&iths);

	/* convert integers to strings - ignore 1/100ths of second */
	sprintf(hours,"%02d",ih);
	sprintf(minutes,"%02d",im);
	sprintf(seconds,"%02d",is);

	/* build output string */
	j = 0;
	for(i=0;hours[i] != NULL;i++)
		string[j++] = hours[i];

	string[j++] = DELIM;

	for(i=0;minutes[i] != NULL;i++)
		string[j++] = minutes[i];

	string[j++] = DELIM;

	for(i=0;seconds[i] != NULL;i++)
		string[j++] = seconds[i];

	string[j] = NULL;

	/* done */
	return(0);
}
