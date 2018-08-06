/*                         *** mencon.c ***                          */
/*                                                                   */
/* IBM-PC microsoft "C" under PC-DOS v2.0                            */
/*                                                                   */
/* Function to open a menu file and display it on the screen.        */
/* Returns the number of entry fields found on the menu (nfield) or  */
/* -1 if an error occured.  Places the r,c coordinates of the entry  */
/* fields within the globally declared rc array.                     */
/*                                                                   */
/* To build a menu, use the text editor to create a file called      */
/* xxxx.mnu   This file contains the menu test as it should appear   */
/* on the screen.  The screen is not automatically erased when menus */
/* are displayed to allow for overlaying.  Command lines control the */
/* various display parameters.  See \comm\comm.mnu for a menu text   */
/* example and \comm\ibmtty.c for a menu call example.               */
/*                                                                   */
/* Command summary - column 1 contains a '>'.                        */
/*                                                                   */
/*>BE		- ring bell                                          */
/*>CC		- comment line                                       */
/*>DA rr cc	- display system date at given row and column        */
/*>DS xx	- toggle given screen attribute                      */
/*			0 = all attributes off                       */
/*			1 = BOLD                                     */
/*			4 = underscore                               */
/*			5 = blink                                    */
/*			7 = reverse video                            */
/*			8 = invisible                                */
/*>ES		- erase screen                                       */
/*>EL xx	- erase xx number lines                              */
/*>FR ulr ulc lrr lrc	- draw a box                                 */
/*>PA xx.	- pause xx seconds (must be a real number)           */
/*>RP min max	- set valid range of responses (used by chosit)      */
/*>ST ll	- set line counter (start diplaying) to given line   */
/*>SK xx	- increment line counter (skip) xx lines             */
/*>SC c		- change default input marker from '[' to 'c'        */
/*>TI rr cc	- display time at given row and column               */
/*                                                                   */
/* Written by L. Cuthbertson, March 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#include <stdio.h>

#define NULL		'\000'
#define COMKEY		'>'
#define SPACE		' '
#define LBRACK		'['

int rc[50][2];
int lenrc;
int vrange[2];

int mencon(menu)
char menu[];
{
	char line[81],menufile[30];
	char inchar;
	int irow,icol,nfield;
	FILE *mfp;

	/* build menu file menu */
	strcpy(menufile,menu);
	strcat(menufile,".mnu");

	/* open file */
	if ((mfp = fopen(menufile,"r")) == NULL) {
		return(-1);
	}

	/* read loop */
	irow = 0;		/* line counter */
	nfield = 0;		/* entry field counter */
	inchar = LBRACK;	/* default input position marker */
	while (getline(mfp,line,sizeof(line)) != EOF) {

		/* handle command lines */
		if (line[0] == COMKEY) {
			if (docomm(line,&irow,&inchar) == (-1)) {
				cursor(25,1);
				writes("\007*** invalid menu command ***");
				pause(2.);
				writes(line);
				pause(2.);
			}
			continue;
		}

		/* got a menu line - bump up line counter */
		irow++;

		/* check for blank line */
		if (line[0] == NULL)
			continue;

		/* locate start of actual information on line */
		for (icol=1;line[icol-1] == SPACE;icol++)
			;
		cursor(irow,icol);

		/* output information */
		for (;line[icol-1] != NULL;icol++) {
			writec(line[icol-1]);

			/* check for input position */
			if (line[icol-1] == inchar) {
				nfield++;
				rc[lenrc][0] = irow;
				rc[lenrc++][1] = (icol+1);
			}
		}

	}

	/* done */
	fclose(mfp);
	return(nfield);
}

/*********************************************************************/
/*                                                                   */
/*                        *** docomm.c ***                           */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to execute menu commands for the function mencon.        */
/* Returns a 0 if successful or a -1 if not.                         */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#include <ctype.h>
#define BELL		'\007'

int docomm(line,icrow,inchar)
char line[],*inchar;
int *icrow;
{
	char command[2],date[9],time[9];
	int i,irow,icol,irel;
	int iatt,ulrow,ulcol,lrrow,lrcol;
	float frel;

	/* get command */
	command[0] = toupper(line[1]);
	command[1] = toupper(line[2]);

	/* ring bell */
	if (command[0] == 'B' && command[1] == 'E') {
		sscanf(line,"%*4s%d",&irel);
		for(i=0;i<irel;i++) {
			writec(BELL);
		}

	/* check for comment line */
	} else if (command[0] == 'C' && command[1] == 'C') {
		return (1);

	/* display date */
	} else if (command[0] == 'D' && command[1] == 'A') {
		sscanf(line,"%*4s%d%d",&irow,&icol);
		cursor(irow,icol);
		getdate(date);
		writes(date);

	/* toggle screen attribute */
	} else if (command[0] == 'D' && command[1] == 'S') {
		sscanf(line,"%*4s%d",&iatt);
		if (scratt(iatt) == (-1))
			return(-1);

	/* erase screen */
	} else if (command[0] == 'E' && command[1] == 'S') {
		lenrc = 0;		/* initialize rc array */
		escreen(2);

	/* erase x number of lines */
	} else if (command[0] == 'E' && command[1] == 'L') {
		sscanf(line,"%*4s%d%d",&irel,&icol);
		for (i=0;i<irel;i++) {
			cursor((*icrow+i+1),icol);
			eline(0);
		}
		cursor(*icrow,1);

	/* frame */
	} else if (command[0] == 'F' && command[1] == 'R') {
		sscanf(line,"%*4s%d%d%d%d",&ulrow,&ulcol,&lrrow,&lrcol);
		if (frame(ulrow,ulcol,lrrow,lrcol) == (-1))
			return(-1);

	/* pause */
	} else if (command[0] == 'P' && command[1] == 'A') {
		sscanf(line,"%*4s%f",&frel);
		pause(frel);

	/* set valid range */
	} else if (command[0] == 'R' && command[1] == 'P') {
		sscanf(line,"%*4s%d%d",&vrange[0],&vrange[1]);

	/* set line counter directly */
	} else if (command[0] == 'S' && command[1] == 'T') {
		sscanf(line,"%*4s%d",icrow);
		*icrow -= 1;

	/* increment line counter directly */
	} else if (command[0] == 'S' && command[1] == 'K') {
		sscanf(line,"%*4s%d",&irel);
		*icrow += irel;

	/* set input position marker */
	} else if (command[0] == 'S' && command[1] == 'C') {
		*inchar = line[4];

	/* display time */
	} else if (command[0] == 'T' && command[1] == 'I') {
		sscanf(line,"%*4s%d%d",&irow,&icol);
		cursor(irow,icol);
		gettime(time);
		writes(time);

	/* error */
	} else {
		return (-1);
	} 

	/* done */
	return(0);
}
