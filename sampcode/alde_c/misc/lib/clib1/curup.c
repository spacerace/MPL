/*                        *** curup.c ***                            */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to move the cursor up y relative lines.                  */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#define NULL	'\000'
#define POUND	'#'

curup(y)
int y;
{
	extern char CUU[];
	char yrel[3],command[20];
	int i,inpos,outpos;

	/* initialize screen controls */
	scrinit();

	/* convert integer relative motion into string */
	sprintf(yrel,"%d",y);

	/* build command line */
	inpos = 0;	/* position within control string */
	outpos = 0;	/* position within command string */

	while (CUU[inpos] != POUND)
		command[outpos++] = CUU[inpos++];

	for (i=0;yrel[i] != NULL;i++)
		command[outpos++] = yrel[i];

	inpos++;
	while (CUU[inpos] != NULL)
		command[outpos++] = CUU[inpos++];

	command[outpos] = NULL;

	/* write command to screen */
	writes(command);
}
