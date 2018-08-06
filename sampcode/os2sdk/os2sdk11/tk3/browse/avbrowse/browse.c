/*
    browse.c -- AVIO File Browsing Utility

    Created by Microsoft Corporation, 1989
*/
#define	INCL_WINTRACKRECT
#define	INCL_WINWINDOWMGR
#define	INCL_WINPOINTERS
#define INCL_WINFRAMEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avio.h"
#include "browse.h"
#include <opendlg.h>
/*
    Constants
*/
#define MAXLINELEN	120
#define AVIO_PS_ROWS	25
#define	AVIO_PS_COLS	80
/*
    Global Variables
*/
FILE	*pfInput;
PFNWP	pfnOldClient;
char	*aszLines[NUM_DATA_LINES];
SHORT	sTopLine = 0;
DLF	dlfInput;
HFILE	hfInput;
USHORT	usAction;
LBINFO	lbiData;
HPOINTER hptrWait;
HPOINTER hptrArrow;
HWND	hWndClient;
HWND	hWndFrame;
BOOL	fLargeFont = FALSE;
SHORT	sMaxLine;
/*
    Open the input file
*/
int cdecl main(int argc, char *argv[]) {
     static CHAR szClientClass[] = "Browse";
     static CHAR szCaption[]	 = "";
     HAB	hAB;
     HMQ	hmq;
     QMSG	qmsg;
     ULONG	flFrameFlags = FCF_STANDARD | FCF_HORZSCROLL | FCF_VERTSCROLL;
     ULONG 	flFrameStyle = WS_VISIBLE | FS_SCREENALIGN;
     char	*szInFile;

     hAB = WinInitialize(0);
     hmq = WinCreateMsgQueue(hAB, 0);

     WinRegisterClass(hAB, szClientClass, BrowseWndProc, CS_SYNCPAINT, 0);

     hWndFrame = WinCreateStdWindow(HWND_DESKTOP, flFrameStyle,
                                    &flFrameFlags, szClientClass, szCaption,
                                     0L, NULL, ID_RESOURCE, &hWndClient);
     /*
	Get the hourglass and arrow pointers
     */
     hptrWait  = WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT,  FALSE);
     hptrArrow = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE);

     if (argc == 1) pfInput = stdin;
     else {
	if (!(pfInput = fopen(argv[1], "r"))) {
	    fprintf(stderr, "***Error:  Could not open %s", szInFile);
	    return(-1);
	}
     }
     ReadFile();
     /*
	Setup AVIO PS and force a paint
	Note:  This subclasses the client and frame windows
     */
     lbiData.sPSrows	= AVIO_PS_ROWS;
     lbiData.sPScols	= AVIO_PS_COLS;
     lbiData.sRows	= sTopLine;
     lbiData.sCols	= sMaxLine;
     lbiData.pfnQL	= (PFNQL) RetrieveLine;
     lbiData.fLargeFont	= FALSE;
     AvioInit(&lbiData);
     /*
	Process messages
     */
     while (WinGetMsg(hAB, &qmsg, NULL, 0, 0)) WinDispatchMsg(hAB, &qmsg);

     /* Blast the AVIO PS */
     AvioClose();

     WinDestroyWindow(hWndFrame);
     WinDestroyMsgQueue(hmq);
     WinTerminate(hAB);
     return 0;
}

void ReadFile(void) {
/*
    Reads in a file using <stdio.h> fgets() calls.
    It might be wise to put better word wrap facilities here
*/
    char	szLine[MAXLINELEN];

    /* Put up the hourglass */
    WinSetPointer(HWND_DESKTOP, hptrWait);

    /* Reinitialize buffer, MaxLineLength */
    for (; sTopLine > 0; ) free(aszLines[--sTopLine]);
    sMaxLine = 0;

    /* Read in the file */
    while (fgets(szLine, MAXLINELEN, pfInput)) {

	/* Convert LF (\n) into NULL (\0) */
	if (szLine[strlen(szLine) - 1] == '\n') {
	    szLine[strlen(szLine) - 1] = 0;
	} else szLine[MAXLINELEN - 1] = 0;

	if (StoreLine(szLine)) {
	    fprintf(stderr,"***Error:  Line buffer full\n");
	    return;
	}
    }
    fclose(pfInput);

    /* Reset the mouse pointer */
    WinSetPointer(HWND_DESKTOP, hptrArrow);

    return;
}

SHORT StoreLine(char *szLine) {
/*
    Put a line into the line buffer; line numbers are free
    For > 64K data, add code here and in RetrieveLine
*/
    int		i, cLinePos;
    BOOL	fDone;
    /*
	Check if top line exceeded, or malloc() fails
    */
    if (sTopLine == NUM_DATA_LINES)  return -1;
    /*
	Compute line length with tabs expanded
    */
    cLinePos = 0;
    for (i = 0; i < MAXLINELEN; i++) {
	switch(szLine[i]) {
	    case '\0':
		cLinePos++; i = MAXLINELEN;
		break;
	    case '\t':
		do {
		    cLinePos++;
		} while (cLinePos % 8);
		break;

	    default:
		cLinePos++;
	}

    }
    if (cLinePos > sMaxLine) sMaxLine = cLinePos;
    if (!(aszLines[sTopLine] = malloc(cLinePos))) return -1;
    /*
	Copy szLine into the line buffer.  Expand tabs here.
    */
    i = cLinePos = 0; fDone = FALSE;
    while ((i <= MAXLINELEN) && (!fDone)) {
	switch(szLine[i]) {
	    case '\t':
		do {
		    aszLines[sTopLine][cLinePos++] = ' ';
		} while (cLinePos % 8);
		break;

	    default:
		aszLines[sTopLine][cLinePos++] = szLine[i];
		fDone = !szLine[i];
		break;
	}
	i++;
    }
    sTopLine++;
    return 0;
}

char * _loadds RetrieveLine(USHORT usLineNum) {
/*
    Return line numbered usLineNum
*/
    if (usLineNum >= sTopLine) {		/* Out of range */
	return NULL;
    }
    return aszLines[usLineNum];
}

MRESULT CALLBACK BrowseWndProc(hWnd, msg, mp1, mp2)
HWND hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
/*
    Handle the About... and Open... messages
*/
    switch(msg) {
	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {
		case IDM_ABOUT:
		    WinDlgBox(HWND_DESKTOP, hWnd, AboutDlgProc,
			      NULL, IDD_ABOUT, NULL);
                    return 0;

		case IDM_OPEN:
		    /*
			Open the file, using the file dialog
			then reopen it with stdio calls
		    */
		    SetupDLF(&dlfInput, DLG_OPENDLG, &hfInput,
			"\\*.*", NULL, "Browse Open File",
			"Select a file to be browsed.");
		    DlgFile(hWnd, &dlfInput);
		    pfInput = fopen(dlfInput.szOpenFile, "r");
		    ReadFile();
		    /*
			Close the opened handle
		    */
		    DosClose(hfInput);

		    /* Fix up the screen display */
		    lbiData.sRows = sTopLine;
		    lbiData.sCols = sMaxLine;
		    lbiData.fLargeFont = fLargeFont;
		    AvioInit(&lbiData);

		    return 0;

		case IDM_FONT:
		    AvioLargeFont(fLargeFont = !fLargeFont);
	   	    return 0;

		default: return 0;
	    }
	    break;
	default: return WinDefWindowProc(hWnd, msg, mp1, mp2);
    }
    return 0L;
}

MRESULT CALLBACK AboutDlgProc(hDlg, msg, mp1, mp2)
/*
    About... dialog procedure
*/
HWND	hDlg;
USHORT	msg;
MPARAM	mp1;
MPARAM	mp2;
{
    switch(msg) {
	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK: WinDismissDlg(hDlg, TRUE); break;
		default: break;
	    }
	default: return WinDefDlgProc(hDlg, msg, mp1, mp2);
    }
    return FALSE;
}
