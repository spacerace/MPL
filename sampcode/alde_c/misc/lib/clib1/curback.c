/*                       *** curback.c ***                           */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to move the cursor backward x relative columns.          */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define NULL	'\000'
#define POUND	'#'

curback(x)
int x;
{
	extern char CUB[];
	char xrel[3],command[20];
	int i,inpos,outpos;

	/* initialize screen controls */
	scrinit();

	/* convert integer relative motion into string */
	sprintf(xrel,"%d",x);

	/* build command line */
	inpos = 0;	/* position within control string */
	outpos = 0;	/* position within command string */

	while (CUB[inpos] != POUND)
		command[outpos++] = CUB[inpos++];

	for (i=0;xrel[i] != NULL;i++)
		command[outpos++] = xrel[i];

	inpos++;
	while (CUB[inpos] != NULL)
		command[outpos++] = CUB[inpos++];

	command[outpos] = NULL;

	/* write command to screen */
	writes(command);
}
