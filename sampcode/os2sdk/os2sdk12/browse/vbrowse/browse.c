/*
    VIO File Browsing Application
    Created by Microsoft Corporation, 1989
*/
#define	 INCL_KBD
#define	 INCL_VIO
#include <stdio.h>
#include <stdlib.h>
#include <os2.h>
#include <string.h>
#include "browse.h"
/*
    Global Variables
*/
FILE	*pfInput;
char	*aszLines[NUM_DATA_LINES];
SHORT	sTopLine= -1;
SHORT	sRows;
SHORT	HorScrollPos=0;
BYTE	abBlank[2] = { 0x20, 0x07 }; 

/*
    Macros for Vio calls
    The last parameter is zero because we're using a VIO PS
*/
#define ClearScreen()	VioScrollDn(0, 0, -1, -1, -1, abBlank, 0)
#define	Move(r,c)	VioSetCurPos(r, c, 0)
#define ScrollDown(n)	VioScrollDn(0, 0, -1, -1,  n, abBlank, 0)
#define ScrollUp(n)	VioScrollUp(0, 0, -1, -1,  n, abBlank, 0)
#define Write(s)	VioWrtTTY(s, strlen(s), 0)
/*
    Macros for bounds checking
*/
#define Abs(x)		(((x) > 0) ? (x) : (-(x)))
#define Max(x, y)	(((x) > (y)) ? (x) : (y))
#define Min(x, y)	(((x) < (y)) ? (x) : (y))
#define LowerBound(pos, disp, lbound)	Max(pos - disp, lbound)
#define UpperBound(pos, disp, ubound)	Min(pos + disp, ubound)

/*
    Functions
*/
int cdecl main(int argc, char *argv[]) {
/*
    Open the input file and initialize globals
*/
    char	*szFilename;
    VIOMODEINFO	viomiMode;

    /*
	Open the Input File
    */
    if (argc == 1)
	pfInput = stdin;
    else {
	szFilename = argv[1];
	if (!(pfInput = fopen(szFilename,"r"))) {
	    fprintf(stderr, "***Error:  Could not open %s", szFilename);
	    return(-1);
	}
    }
    /*
	Read it into the line buffer
    */
    if (ReadFile()) return(-1);
    /*
	Get the video parameters
    */
    viomiMode.cb = sizeof(viomiMode);
    VioGetMode(&viomiMode, 0);
    sRows = (SHORT) viomiMode.row;

    DisplayScreen(0, TRUE);
    ManipulateFile();

    return 0;
}

SHORT ReadFile(VOID) {
/*
    Read lines from the file into the line buffer
    If there's an error, abort the program (return -1)
*/
    char szLine[MAXLINELENGTH];

    while (fgets(szLine, MAXLINELENGTH, pfInput)) {

	/* Convert LF (\n) character to NULL (\0) */
	if (szLine[strlen(szLine)-1] == '\n')
	    szLine[strlen(szLine)-1] = 0;
	else {
	    fprintf(stderr,"***Error:  Incomplete line read\n");
	    return(-1);
	}

	/* Put the line into the line buffer */
	if (StoreLine(szLine)) {
	    fprintf(stderr,"***Error:  Line buffer full\n");
	    return(-1);
	}
    }

    /* Close the Input file */
    fclose(pfInput);
    return 0;
}

VOID ManipulateFile(VOID) {
/*
    Main loop for display processing
*/
    CHAR    ch;
    SHORT   sLine = 0;

    /* The main command loop */
    while ((ch = GetKbdInput()) != ESC) {
	/*
	    Take user input and compute new top line of screen
	    by taking appropriate jump in jumptable.

	    Note:  no horizontal scrolling.
	*/
	switch (ch) {
	case LINE_UP:	 sLine = LowerBound(sLine, 1, 0);		break;
	case LINE_DOWN:  sLine = UpperBound(sLine, 1, BOTTOM);		break;
	case PAGE_UP:	 sLine = LowerBound(sLine, sRows, 0);		break;
	case PAGE_DOWN:  sLine = UpperBound(sLine, sRows, BOTTOM);	break;
	case HOME_KEY:	 sLine = 0;					break;
	case END_KEY:	 sLine = BOTTOM;				break;
	default:							break;
	}
	DisplayScreen((USHORT) sLine, !ch);
    }

    /* Set Cursor to the bottom of the screen */
    Move((USHORT) sRows - 1, 0);
}

SHORT StoreLine(char *szLine) {
/*
    Put a line into the line buffer; line numbers are free
    For > 64K data, add code here and in RetrieveLine
*/
    /*
	Check if top line exceeded, or if malloc() fails
    */
    if ((sTopLine == NUM_DATA_LINES) ||
	((aszLines[++sTopLine] = malloc(strlen(szLine) + 1)) == NULL))

	return -1;
    /*
	Copy szLine into the line buffer
    */
    strcpy(aszLines[sTopLine], szLine);
    return 0;
}

SHORT RetrieveLine(char **pszLine , USHORT usLineNum) {
/*
    Return line numbered usLineNum
*/
    if ((SHORT) usLineNum > sTopLine) return -1;  /* Out of range */
    *pszLine = aszLines[usLineNum];
    return 0;
}

VOID DisplayScreen(USHORT usDisplayTop, BOOL fForceDraw) {
/*
    Display lines on the screen, starting at usDisplayTop
    by scrolling, then painting new information
*/
    SHORT	    sDelta;
    static USHORT   usOldDispTop;

    sDelta = usDisplayTop - usOldDispTop;
    /*
	If only a few lines need repainting...
    */
    if ((Abs(sDelta) < sRows) && !fForceDraw ) {
	/*
	    Moving to a "higher line", so:
		Scroll down by the amount (make the difference positive)
		Paint in the lines at the top
	*/
	if (sDelta < 0) {
	    ScrollDown(-sDelta);
	    Refresh(usDisplayTop, -sDelta, 0);
	} else {
	    /*
		Moving to a "lower line", so:
		Scroll the information up, and paint at the bottom
	    */
	    ScrollUp(sDelta);
	    Refresh(usDisplayTop + sRows - sDelta, sDelta, sRows - sDelta);
	}
    } else {	/* Paint the entire screen */
	ClearScreen();
	Refresh(usDisplayTop, sRows, 0);
    }
    usOldDispTop = usDisplayTop;
}

VOID Refresh (USHORT iLine, USHORT usLines, USHORT usStart) {
/*
    Updates usLines lines, starting at line iLine in the line
    buffer, and line usStart on the screen
*/
    USHORT usLine;
    char   *szLine;

    for (usLine = 0; usLine < usLines; usLine++) {
	/*
	    Read the line, set the cursor, print the line
	*/
	if (RetrieveLine(&szLine, (iLine + usLine))) break;
	Move((usStart + usLine), 0);
	Write(szLine);
    }
}

CHAR GetKbdInput(VOID) {
/*
    Get chars, then check scan codes and return our own values
*/
    KBDKEYINFO kbciKeyInfo;

    /*
	Wait for characters
    */
    KbdCharIn(&kbciKeyInfo, IO_WAIT, 0);

    switch (kbciKeyInfo.chScan) {
	case ESC: 			/* escape */
	case LINE_UP:
	case LINE_DOWN:
	case PAGE_UP:
	case PAGE_DOWN:
	case HOME_KEY:
	case END_KEY:
	    return kbciKeyInfo.chScan; break;
	default:
	   return((CHAR) NULL); break;
    }
}
