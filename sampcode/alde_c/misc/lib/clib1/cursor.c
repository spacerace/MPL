/*                         *** cursor.c ***                          */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to move the cursor to a specific r,c co-ordinate.        */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define NULL	'\000'
#define POUND	'#'

cursor(r,c)
int r,c;
{
	extern char CUP[];
	char row[3],col[4],command[20];
	int i,inpos,outpos;

	/* initialize screen controls */
	scrinit();

	/* decode integer co-ordinates */
	sprintf(row,"%d",r);
	sprintf(col,"%d",c);

	/* build control sequence */
	inpos = 0;	/* position in control line */
	outpos = 0;	/* position in command line */

	while (CUP[inpos] != POUND)
		command[outpos++] = CUP[inpos++];

	for (i=0;row[i] != NULL;i++)
		command[outpos++] = row[i];

	inpos++;
	while(CUP[inpos] != POUND)
		command[outpos++] = CUP[inpos++];

	for (i=0;col[i] != NULL;i++)
		command[outpos++] = col[i];

	inpos++;
	while(CUP[inpos] != NULL)
		command[outpos++] = CUP[inpos++];

	command[outpos] = NULL;

	/* write command to screen */
	writes(command);
}
