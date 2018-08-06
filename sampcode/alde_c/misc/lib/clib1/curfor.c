/*                        *** curfor.c ***                           */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to move the cursor forward x relative columns.           */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define NULL	'\000'
#define POUND	'#'

curfor(x)
int x;
{
	extern char CUF[];
	char xrel[3],command[20];
	int i,inpos,outpos;

	/* initialize screen controls */
	scrinit();

	/* convert integer relative motion into string */
	sprintf(xrel,"%d",x);

	/* build command line */
	inpos = 0;	/* position within control string */
	outpos = 0;	/* position within command string */

	while (CUF[inpos] != POUND)
		command[outpos++] = CUF[inpos++];

	for (i=0;xrel[i] != NULL;i++)
		command[outpos++] = xrel[i];

	inpos++;
	while (CUF[inpos] != NULL)
		command[outpos++] = CUF[inpos++];

	command[outpos] = NULL;

	/* write command to screen */
	writes(command);
}
