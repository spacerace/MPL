/*                              *** chosit.c ***                     */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to display a menu, prompt for a response, and validate.  */
/* Calls function mencon to display the menu.  Returns the integer   */
/* response, or -1 if invalid response or error occured.              */
/*                                                                   */
/* Written by L. Cuthbertson, April 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

int chosit(menu,ans,lenans)
char menu[],ans[];
int lenans;
{
	extern int rc[][2];
	extern int vrange[2];
	int i,j,ians,nfield;

	/* display menu */
	if (strcmp(menu,"skip") != 0) {
		nfield = mencon(menu);
		if (nfield == (-1))
			return(-1);	/* error in mencon */
	}

	/* move cursor to entry position */
	i=0;			/* position within rc array */
	if (i > (nfield-1)) {
		return(-1);	/* no entry position on menu */
	}

	loop:
	cursor(rc[i][0],rc[i][1]);

	/* accept answer */
	reads(ans,lenans);

	/* convert answer to integer */
	sscanf(ans,"%d",&ians);

	/* always exit if 99 entered */
	if (ians == 99)
		exit();

	/* validate answer */
	if ((ians < vrange[0]) || (ians > vrange[1])) {
		cursor(25,19);
		writes("\007*** invalid response - please try again ***");
		pause(5.);
		cursor(25,19);
		eline(0);
		cursor(rc[i][0],rc[i][1]);
		for (j=1;j<lenans;j++)
			writec(' ');
		goto loop;
	}

	/* done */
	return(ians);
}
