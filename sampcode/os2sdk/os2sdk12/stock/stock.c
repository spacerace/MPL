/***	stock - draw a stock chart
 *
 *	Author:
 *	    Benjamin W. Slivka
 *
 *	History:
 *	    27-Mar-1989 bens	Initial version.
 *	    06-Apr-1989 bens	Always repaint on WM_SIZE.
 *	    26-Apr-1990 bens	Get it limping along
 *	    03-May-1990 bens	Add file open
 *	    11-May-1990 bens	Printing works!
 *	    06-Jun-1990 bens	Finish job property handling
 *	    07-Jun-1990 bens	Fix bugs in JP stuff, spiff up user interface
 */

#define INCL_DEV

#define INCL_GPIPRIMITIVES

#define INCL_WINDIALOGS
#define INCL_WINERRORS
#define INCL_WINFRAMEMGR
#define INCL_WINLISTBOXES
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINWINDOWMGR

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ea.h"
#include "ids.h"
#include "file.h"
#include "limits.h"
#include "mem.h"
#include "print.h"


#define cchDate    10
#define cchMax	   80

#define iH	    0	    // index of High
#define iL	    1	    // index of Low
#define iC	    2	    // index of Close
#define NV	    3	    // Number of vectors
#define cptMax	 1200	    // maximum data points


#define ENABLE_MENU_ITEM(x, y)				    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_DISABLED,NULL))

#define DISABLE_MENU_ITEM(x, y) 			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_DISABLED,MIA_DISABLED))

#define CHECK_MENU_ITEM(x, y)				    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED))

#define UNCHECK_MENU_ITEM(x, y) 			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,NULL))


MRESULT EXPENTRY ClientWndProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY PrinterSetupDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2);


HPRINTER    HPRTFromListBox(HWND hwnd);
BOOL	    InteractSaveFile(HWND hwnd);
int cdecl   main(int cArg,char **ppszArg);
VOID	    Message(char *psz, HWND hwnd);
VOID	    PlotPrice(HPS hps,RECTL rcl);
BOOL	    QueryHCI(HDC hdc,HCINFO *phci);
BOOL	    PrintChart(HWND hwnd);
BOOL	    ReadData(HWND hwnd);
VOID	    SaveJobProperties(VOID);
VOID	    SetOriginMenu(VOID);
VOID	    SetFileMenu(VOID);
VOID	    ShowWinError(char *psz);


/***	GLOBAL - global data
 *
 */
typedef struct {    /* glb */
    char	 achAppTitle[CCHMAXPATH]; // Application title
    USHORT	 cpt;		// Number of points
    BOOL	 fNewJobProp;	// TRUE => job properties changed
    BOOL	 fZeroOrigin;	// TRUE => Y origin is 0; else is minimum price
    HAB 	 hab;
    HWND	 hwndFrame;
    HWND	 hwndClient;
    USHORT	 pMax;		// Maximum price
    USHORT	 pMin;		// Minimum price
    char *	 pszFile;	// Data file
    HPRINTER	 hprt;		// Current printer handle
    HPRINTERLIST hprtlist;	// Handle of printer list
} GLOBAL;

char	    achMisc[CCHMAXPATH];
USHORT	    apt[NV][cptMax];
GLOBAL	    glb;
HPOINTER    hptrNormal;
HPOINTER    hptrWait;
char	    szClientClass[]="Stock";
char	    pszJobProp[]="StockJobProp";


/***	main - main program
 *
 */
int cdecl main(int cArg, char **ppszArg)
{
    ULONG	    flFrameFlags= FCF_TITLEBAR	    | FCF_SYSMENU  |
				  FCF_SIZEBORDER    | FCF_MINMAX   |
				  FCF_SHELLPOSITION | FCF_TASKLIST |
				  FCF_MENU;
    HMQ 	    hmq;
    QMSG	    qmsg;

    glb.hprt = NULL;
    glb.cpt = 0;
    glb.fZeroOrigin = TRUE;	    // Use a zero origin
    glb.fNewJobProp = FALSE;	    // No new job properties

    // If at least one argument, take that as data file name
    if (cArg >= 2)
	glb.pszFile = MemStrDup(ppszArg[1]);
    else
	glb.pszFile = NULL;

    glb.hab = WinInitialize(0);
    hmq = WinCreateMsgQueue(glb.hab,0);

    hptrWait = WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
    hptrNormal = WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE);

    WinRegisterClass(glb.hab,szClientClass,ClientWndProc,0L,0);

    glb.hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
			WS_VISIBLE | FS_ICON | FS_TASKLIST, // Window styles
			&flFrameFlags,
			szClientClass,
			NULL,
			0L,
			(HMODULE) NULL,
			IDR_APP,
			&glb.hwndClient);

    if (glb.hwndFrame == 0) {
	Message("Cannot create main window",NULL);
	WinDestroyMsgQueue(hmq);
	WinTerminate(glb.hab);
	return 1;
    }

    // Get printer list
    glb.hprtlist = PrintCreatePrinterList(glb.hab);

    // Get application title
    WinQueryWindowText(glb.hwndFrame, sizeof(glb.achAppTitle),glb.achAppTitle);

    if (glb.pszFile != NULL)
	ReadData(glb.hwndClient);

    // Get printer, if not already set by ReadData

    if (glb.hprt == NULL) {		// Need to find it
	if (glb.hprtlist != NULL) {	// Printers exists
	    glb.hprt = PrintQueryDefaultPrinter(glb.hprtlist); // Get default
	}
    }

    // Set up valid menu options

    SetFileMenu();
    SetOriginMenu();

    // Message loop

    while (WinGetMsg(glb.hab,&qmsg,NULL,0,0)) {
	WinDispatchMsg(glb.hab,&qmsg);
    }

    // Destroy printer list

    if (glb.hprtlist != NULL)
	PrintDestroyPrinterList(glb.hprtlist);

    WinDestroyWindow(glb.hwndFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(glb.hab);
    return 0;
}


/***	ClientWndProc - window proc for client window
 *
 */
MRESULT EXPENTRY ClientWndProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
    HPS     hps;
    char *  psz;
    RECTL   rcl;

    switch (msg) {

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1)) {

	case IDM_ABOUT:
	    WinDlgBox(HWND_DESKTOP,hwnd,AboutDlgProc,NULL,IDD_ABOUT,NULL);
	    return FALSE;

	case IDM_FILE_CLOSE:
	    // Make sure current file is saved
	    if (!InteractSaveFile(hwnd)) // User cancelled
		return FALSE;

	    // Empty data

	    glb.cpt = 0;
	    if (glb.pszFile != NULL) {
		MemFree(glb.pszFile);
		glb.pszFile = NULL;
	    }
	    WinInvalidateRect(hwnd,NULL,FALSE); // Force repaint


	    SetFileMenu();		// Set valid menu options
	    return FALSE;

	case IDM_FILE_OPEN:
	    // Make sure current file is saved
	    if (!InteractSaveFile(hwnd)) // User cancelled
		    return FALSE;

	    psz = FileOpen(hwnd, "Open", "*.dat");
	    if (psz != NULL) {
		if (glb.pszFile != NULL) // Old file data present
		    MemFree(glb.pszFile); // Free old file name
		glb.pszFile = psz;	// Set new file name
		if (ReadData(hwnd))	// If data was read successfully
		    WinInvalidateRect(hwnd,NULL,FALSE); // Force repaint
	    }
	    return FALSE;

	case IDM_FILE_PRINT:

	    PrintChart(hwnd);
	    break;

	case IDM_FILE_PRINTER_SETUP:
	    WinDlgBox(HWND_DESKTOP,hwnd,PrinterSetupDlgProc,NULL,
						 IDD_PRINTER_SETUP,NULL);
	    return FALSE;

	case IDM_FILE_SAVE:
	    SaveJobProperties();
	    SetFileMenu();
	    return FALSE;

	case IDM_FILE_EXIT:
	    WinSendMsg(hwnd,WM_QUIT,NULL,NULL);
	    break;

	case IDM_OPTIONS_ORIGIN:
	    glb.fZeroOrigin = !glb.fZeroOrigin; // Flip state
	    SetOriginMenu();
	    return FALSE;
	}
	break;

    case WM_CREATE:
	break;

    case WM_QUIT:
	if (!InteractSaveFile(hwnd))	    // User cancelled
	    return FALSE;		    //	Do not quit
	break;				    // Do standard quit processing

    case WM_SIZE:
	WinInvalidateRect(hwnd,NULL,FALSE);
	return 0;

    case WM_PAINT:
	hps = WinBeginPaint(hwnd,NULL,NULL);
	GpiErase(hps);
	WinQueryWindowRect(hwnd,&rcl);
	PlotPrice(hps,rcl);
	WinEndPaint(hps);
	return 0;
    }
    return WinDefWindowProc(hwnd,msg,mp1,mp2);
}


/***	InteractSaveFile - If new job properties, let user save them
 *
 *	Entry
 *	    hwnd - parent window for message box
 *	    glb.fNewJobProp
 *
 *	Exit-Success
 *	    returns TRUE, file is saved
 *
 *	Exit-Failure
 *	    returns FALSE, user cancelled save
 */
BOOL InteractSaveFile(HWND hwnd)
{
    static char ach[CCHMAXPATH];
    USHORT	rc;

    if (!glb.fNewJobProp)		// No job properties to save
	return TRUE;

    sprintf(ach,"Save printer setup in '%s'?",glb.pszFile);
    rc = WinMessageBox(
	    HWND_DESKTOP,		// Parent
	    hwnd,			// Owner
	    ach,			// Message
	    szClientClass,		// Caption (use default)
	    NULL,			// Window id
	    MB_QUERY | MB_YESNOCANCEL | MB_APPLMODAL // window style
    );
    switch (rc) {
	case MBID_YES:			// Save job properties
	    SaveJobProperties();
	    SetFileMenu();
	    return TRUE;

	case MBID_NO:			// Do not save job properties
	    glb.fNewJobProp = FALSE;
	    SetFileMenu();
	    return TRUE;

	case MBID_CANCEL:		// Cancel save
	    return FALSE;
    }
    return FALSE;
}


/***	PrintChart - Draw chart on printer
 *
 *	Entry
 *	    hwnd - hwnd for status
 *
 *	    glb.hab  - Anchor block
 *	    glb.hprt - Printer destination
 *	    glb.pszFile - File name (for job description)
 *
 *	    Same variables as PlotPrice;
 *
 *	Exit-Success
 *	    returns TRUE
 *
 *	Exit-Failure
 *	    returns FALSE
 */
BOOL PrintChart(HWND hwnd)
{
    LONG	    cb;
    USHORT	    jobid;
    HDC 	    hdc=NULL;
    HPS 	    hps=NULL;
    static HCINFO   hci;
    LONG	    rc;
    RECTL	    rcl;
    SIZEL	    sizl;

    WinSetPointer(HWND_DESKTOP,hptrWait); // Tell user we are being slow

    // Create DC

    hdc = PrintOpenDC(glb.hab,glb.hprt,"PM_Q_STD");
    if (hdc == NULL) {
	ShowWinError("PrintOpenDC failed");
	rc = DEV_ERROR;
	goto Exit;
    }

    if (!QueryHCI(hdc,&hci)) {
	Message("QueryHCI failed",NULL);
	rc = DEV_ERROR;
	goto Exit;
    }

    // Create and associate PS
    sizl.cx = 0L;
    sizl.cy = 0L;
    hps = GpiCreatePS(glb.hab,hdc,&sizl,GPIA_ASSOC | PU_PELS | GPIT_NORMAL);
    if (hps == NULL) {
	ShowWinError("GpiCreatePS failed");
	rc = DEV_ERROR;
	goto Exit;
    }

    // Set dimensions of drawing area
    rcl.xLeft	= 0;
    rcl.yBottom = 0;
    rcl.xRight	= hci.xPels;
    rcl.yTop	= hci.yPels;

    // Time to draw
    rc = DevEscape(
	    hdc,			// Display context
	    DEVESC_STARTDOC,		// Start document
	    (LONG)strlen(glb.pszFile),	// Length of print job name
	    glb.pszFile,		// Print job name
	    0L, 			// Ignored
	    0L				// Ignored
	 );
    if (rc != DEV_OK) {
	ShowWinError("STARTDOC failed");
	goto Exit;
    }

    PlotPrice(hps,rcl);

    cb = sizeof(jobid); 		// Size of output buffer
    rc = DevEscape(
	    hdc,			// Display context
	    DEVESC_ENDDOC,		// End document
	    0L, 			// Length of print job name
	    0L, 			// Print job name
	    &cb,			// Size of jobid
	    (PBYTE)&jobid		// Jobid
	 );
    if (rc != DEV_OK)
	ShowWinError("ENDDOC failed");

    // Free resources
Exit:
    if (hps != NULL) {
	GpiAssociate(hps,NULL);
	GpiDestroyPS(hps);
    }
    if (hdc != NULL)
	DevCloseDC(hdc);
    WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
    return (rc == DEV_OK);
}


/***	ShowWinError - Show message box with error from Win* API call
 *
 *	Entry
 *	    psz - message text
 *
 *	Exit
 *	    none
 *
 */
VOID ShowWinError(char *psz)
{
    PERRINFO	    perri;

    perri = WinGetErrorInfo(glb.hab);
    if (perri == (PERRINFO) NULL)
	return;

    sprintf(achMisc,"%s; Error = 0x%0x",psz,perri->idError);
    Message(achMisc,NULL);
    WinFreeErrorInfo(perri);
}


/***	SetOriginMenu - Set text in origin menu
 *
 *	Entry
 *	    glb.fZeroOrgin - current state of origin plotting
 *
 *	Exit
 *	    Menu item checked/unchecked
 */
VOID SetOriginMenu(VOID)
{
    if (glb.fZeroOrigin)
	UNCHECK_MENU_ITEM(glb.hwndFrame,IDM_OPTIONS_ORIGIN);
    else
	CHECK_MENU_ITEM(glb.hwndFrame,IDM_OPTIONS_ORIGIN);

    WinInvalidateRect(glb.hwndClient,NULL,FALSE); // Force repaint
}


/***	SetFileMenu - Set file menu item states
 *
 */
VOID SetFileMenu(VOID)
{
    static char ach[CCHMAXPATH];

    strcpy(ach,glb.achAppTitle);	// Title start with app name

    if (glb.pszFile == NULL) {		// No file currently open
	DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_CLOSE);
	DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_SAVE);
	DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_PRINT);
	DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_PRINTER_SETUP);
    }
    else {
	ENABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_CLOSE);

	if (glb.fNewJobProp)		// Job properties need to be saved
	    ENABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_SAVE);
	else
	    DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_SAVE);

	if (glb.hprt != NULL) { 	// A printer exists
	    ENABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_PRINT);
	    ENABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_PRINTER_SETUP);
	}
	else {
	    DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_PRINT);
	    DISABLE_MENU_ITEM(glb.hwndFrame,IDM_FILE_PRINTER_SETUP);
	}

	strcat(ach," - ");		// Append file name
	strcat(ach,glb.pszFile);
    }
    WinSetWindowText(glb.hwndFrame,ach); // Set title
}


/***	ReadData - read stock data from file
 *
 *	Entry
 *	    hwnd	 - Hwnd for status messages
 *	    glb.pszFile  - File to read
 *	    glb.hprtlist - Printer list
 *
 *	Exit-Success
 *	    Returns TRUE
 *		apt[][]  = Filled in with price data
 *		glb.cpt  = Count of points read
 *		glb.pMax = Maximum price
 *		glb.pMin = Minimum price
 *		glb.hprt = May have change, to file job properties
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL ReadData(HWND hwnd)
{
    static char   ach[CCHMAXPATH];
    static USHORT cb;
    int 	  cf;
    USHORT	  factorSplit;
    USHORT	  fracClose;
    USHORT	  fracHigh;
    USHORT	  fracLow;
    HPRINTER	  hprt;
    int 	  i;
    USHORT	  intClose;
    USHORT	  intHigh;
    USHORT	  intLow;
    int 	  j;
    char *	  pb;
    char *	  pch;
    char *	  pch1;
    FILE *	  pfile;
    ULONG	  ulVol;

    WinSetPointer(HWND_DESKTOP,hptrWait); // Tell user we are being slow

    glb.cpt = 0;
    glb.fNewJobProp = FALSE;		// Job properties not changed
    SetFileMenu();

    pfile = fopen(glb.pszFile,"r");
    if (pfile == NULL) {
	WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
	sprintf(achMisc,"Could not open \"%s\"",glb.pszFile);
	Message(achMisc,hwnd);
	return FALSE;
    }

    fgets(ach,cchMax,pfile);   // skip two header lines
    fgets(ach,cchMax,pfile);
    fgets(ach,cchMax,pfile);   // Get first data line
    while (!feof(pfile)) {
	if (glb.cpt > cptMax) {
	    WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
	    sprintf(achMisc,"Too many data points. Max is %d.",cptMax);
	    Message(achMisc,hwnd);
// BUGBUG 03-May-1990 bens Should continue with truncated data?
//
// Should make file read a separate function, and do min/max computation
// as long as some data was read.
//
	return FALSE;
	}

	// Skip date
	pch = strchr(ach,'"')+1;	// pch	-> 04/24/60",....
	pch1 = strchr(pch,'"'); 	// pch1 ->	   ",....
	*pch1 = '\0';			// Split string at end of date
	pch1 += 2;			// Skip ",

	cf = sscanf(pch1," %d.%d, %d.%d, %d.%d, %ld",
	      &intHigh,&fracHigh,&intLow,&fracLow,&intClose,&fracClose,&ulVol);

	if (cf != 7) {
	    WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
	    // Adjust count for two header lines, plus base 0
	    sprintf(achMisc,"Line %d is bad: [%s]",glb.cpt+3,ach);
	    Message(achMisc,hwnd);
// BUGBUG 03-May-1990 bens What to do now?
//
// On par with previous BUGBUG, we should probably display data up to the
// point where it was bad.
//
// Also, it would be nice to give a hint about the problem.
//
// At a minimum, tell if too many or too few data points.
	    return FALSE;
	}

	// convert to units of 8 points
	intHigh  = (intHigh  << 3) + (fracHigh	/ 125);
	intLow	 = (intLow   << 3) + (fracLow	/ 125);
	intClose = (intClose << 3) + (fracClose / 125);

	if (stricmp(pch,"SPLIT") == 0) {
	    factorSplit = intHigh/8;
	    for (i=0; i<(int) glb.cpt; i++)	    // Process all saved records
		for (j=0; j<NV; j++)	// Process dollar values
		    apt[j][i] /= factorSplit;
		// dP(iV, j%) *= factorSplit;
	}
	else {
	    apt[iH][glb.cpt] = intHigh;
	    apt[iL][glb.cpt] = intLow;
	    apt[iC][glb.cpt] = intClose;
	    glb.cpt++;
	}
	fgets(ach,cchMax,pfile);	// Get next data line
    }
    fclose(pfile);

    // Find max and min prices

    // Speed win: prime min/max with real value.
    //	This allows the if..else if below to work reliably.

    glb.pMax = apt[0][0];
    glb.pMin = apt[0][0];

    for (i=0; i<(int)glb.cpt; i++) {
	for (j=0; j<NV; j++) {
	    if (apt[j][i] > glb.pMax)
		glb.pMax = apt[j][i];
	    else if (apt[j][i] < glb.pMin)
		glb.pMin = apt[j][i];
	}
    }

    // Select printer, based on job properties

    pb = EAQueryValue(glb.pszFile,pszJobProp,&cb);
    if (pb == NULL) {			// No job prop, use default printer
	WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
	return TRUE;			//  Done
    }
    if (cb == 0) {			// Data is not binary
	MemFree(pb);			// Free buffer
	WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
	return TRUE;			//  Done
    }

    // Try to match a printer

    hprt = PrintMatchPrinter(glb.hprtlist,cb,pb);
    if (hprt != NULL)			// Got a match!
	glb.hprt = hprt;		// Set new printer

    MemFree(pb);			// Free buffer

    SetFileMenu();			// Update menus and title bar

    WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
    return TRUE;			//  Done
}


/***	PlotPrice - plot stock price
 *
 *	Entry
 *	    hps - Presentation space to plot to
 *	    rcl - Bounding rectangle in hps
 *
 *	    apt[][]  = Filled in with price data
 *	    glb.cpt  = Count of points read
 *	    glb.pMax = Maximum price
 *	    glb.pMin = Minimum price
 *	    glb.hprt = May have change, to file job properties
 *	    glb.fZeroOrigin - TRUE => make Y-origin be 0.
 *
 *	Exit
 *	    Stock chart plotted
 */
VOID PlotPrice(HPS hps,RECTL rcl)
{
    SHORT   cxBar;
    int     i;
    POINTL  ptl;
    SHORT   x;
    float   xFactor;
    SHORT   xMargin = 4;
    SHORT   yC;
    float   yFactor;
    SHORT   yH;
    SHORT   yL;
    SHORT   yMargin = 4;
    SHORT   yOrigin;

    if (glb.cpt == 0)			// No points to draw
	return;

    if (glb.fZeroOrigin)
	yOrigin = 0;
    else
	yOrigin = glb.pMin;

    yFactor = (float)(rcl.yTop - rcl.yBottom - (LONG)(2*yMargin)) /
	      (float)(glb.pMax-yOrigin);
    xFactor = (float)(rcl.xRight - rcl.xLeft - (LONG)(2*xMargin)) /
	      (float)(glb.cpt);

    cxBar = (USHORT)(xFactor/2.0);
    if (cxBar < 1)
	cxBar = 1;

    WinSetPointer(HWND_DESKTOP,hptrWait); // Tell user we are being slow

    for (i=0; i<(int)glb.cpt; i++) {
	    x = (USHORT)(i*(float)xFactor) + xMargin;
	    yH = (USHORT)((float)(apt[iH][i]-yOrigin)*yFactor) + yMargin;
	    yL = (USHORT)((float)(apt[iL][i]-yOrigin)*yFactor) + yMargin;
	    yC = (USHORT)((float)(apt[iC][i]-yOrigin)*yFactor) + yMargin;

	    // Draw vertical (low to high) bar

	    GpiSetColor(hps,CLR_CYAN);

	    ptl.x = x;
	    ptl.y = yL;
	    GpiMove(hps,&ptl);

	    ptl.y = yH;
	    GpiLine(hps,&ptl);

	    // Draw horizontal (close) bar

	    GpiSetColor(hps,CLR_RED);

	    ptl.x = x;
	    ptl.y = yC;
	    GpiMove(hps,&ptl);

	    ptl.x += cxBar;
	    GpiLine(hps,&ptl);
    }
    WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
}


/***	AboutDlgProc - "About..." Dialog Procedure
*
*/
MRESULT EXPENTRY AboutDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
    switch (msg) {
	case WM_COMMAND:
	    switch (SHORT1FROMMP(mp1)) {
		case DID_OK:
		    WinDismissDlg(hwnd,TRUE);
		    return FALSE;
	    }
	    break;
    }
    return WinDefDlgProc(hwnd,msg,mp1,mp2);
}


/***	PrinterSetupDlgProc - "File Printer Setup..." Dialog Procedure
*
*/
MRESULT EXPENTRY PrinterSetupDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
    static BOOL     fNewJobProp;
    HPRINTER	    hprt;
    HWND	    hwndLB;
    USHORT	    i;
    char *	    pszName;
    char *	    pszNameDefault;

    switch (msg) {

    case WM_INITDLG:
	fNewJobProp = FALSE;
	hwndLB = WinWindowFromID(hwnd, IDL_PRINTER);
	hprt = 0;			// Enumerate full list of printers
	while ((hprt=PrintQueryNextPrinter(glb.hprtlist,hprt)) != NULL) {
	    pszName = (char *)PrintQueryPrinterInfo(hprt,PQPI_NAME);
	    // Insert item and store handle
	    i = SHORT1FROMMR(WinSendMsg(hwndLB, LM_INSERTITEM,
			   MPFROMSHORT(LIT_SORTASCENDING),MPFROMP(pszName)));
	    WinSendMsg(hwndLB,LM_SETITEMHANDLE,
					  MPFROMSHORT(i),MPFROMP(hprt));
	    if (hprt == glb.hprt)	// This is the default printer
		pszNameDefault = pszName; // Save name to set selection
	}

	// Find default printer and set selection

	i = SHORT1FROMMR(WinSendMsg(hwndLB,LM_SEARCHSTRING,
			 MPFROM2SHORT(0,LIT_FIRST),MPFROMP(pszNameDefault)));
	WinSendMsg(hwndLB,LM_SELECTITEM,MPFROMSHORT(i),MPFROMSHORT(TRUE));
	break;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1)) {

	case DID_OK:
	    hprt = HPRTFromListBox(hwnd); // Get selection
	    if (hprt != glb.hprt) {	// Printer destination changed
		glb.hprt = hprt;	// Remember new printer
		fNewJobProp = TRUE;	// Force new job properties
	    }
	    glb.fNewJobProp = fNewJobProp; // User made change
	    SetFileMenu();
	    WinDismissDlg(hwnd,TRUE);
	    return FALSE;

	case IDB_SETUP:
	    hprt = HPRTFromListBox(hwnd); // Get selection
	    if (PrintChangeJobProperties(hprt)) // Properties changed
		fNewJobProp = TRUE;	//  Remember JP changed
	    return FALSE;

	case IDB_RESET:
	    hprt = HPRTFromListBox(hwnd); // Get selection
	    PrintResetJobProperties(hprt); // Reset job properties
	    fNewJobProp = TRUE; 	//  Remember JP changed
	    return FALSE;

	case DID_CANCEL:
	    WinDismissDlg(hwnd,FALSE);
	    return FALSE;
	}
	break;
    }
    return WinDefDlgProc(hwnd,msg,mp1,mp2);
}


/***	HPRTFromListBox - Get selected printer
 *
 *	Entry
 *	    hwnd - hwnd of Printer Setup dialog
 *
 *	Exit
 *	    hprt of selected printer
 */
HPRINTER HPRTFromListBox(HWND hwnd)
{
    HWND    hwndLB;
    USHORT  i;

    // Get selection
    hwndLB = WinWindowFromID(hwnd, IDL_PRINTER);
    i = SHORT1FROMMR(WinSendDlgItemMsg(hwnd,IDL_PRINTER,LM_QUERYSELECTION,0L,0L));

    // Get printer handle

    return (HPRINTER) (ULONG)WinSendMsg(hwndLB,LM_QUERYITEMHANDLE,MPFROMSHORT(i),0L);
}


/***	SaveJobProperties - Save current job properties in EA on file
 *
 *	Entry
 *	    glb.hprt	- printer handle
 *	    glb.pszFile - file name
 *
 *	Exit
 *	    Job properties saved with file.
 */
VOID SaveJobProperties(VOID)
{
    static USHORT   cb;
    char *	    pb;

    if (!glb.fNewJobProp)		// Nothing to save
	return;

    glb.fNewJobProp = FALSE;		// JP will be saved

    // Get job properties size

    WinSetPointer(HWND_DESKTOP,hptrWait); // Tell user we are being slow

    cb = 0;
    PrintQueryJobProperties(glb.hprt,&cb,pb); // Get size
    if (cb == 0)			// No job properties to save
	return; 			//  Done
    pb = MemAlloc(cb);			// Alloc buffer
    if (pb == NULL)			// Could not get memory
	return; 			//  Done

    // Get job properties

    if (!PrintQueryJobProperties(glb.hprt,&cb,pb)) { // failed
	MemFree(pb);			// Free buffer
	return; 			//  Done
    }

    // Save new Job Properties with data file

    EASetValue(glb.pszFile,pszJobProp,cb,pb);
    MemFree(pb);			// Free buffer

    WinSetPointer(HWND_DESKTOP,hptrNormal); // Tell user we are done
}


/***	Message - Put up message box
 *
 *	Entry
 *	    psz  - message
 *	    hwnd - parent window
 *
 *	Exit
 *	    None
 */
VOID Message(char *psz, HWND hwnd)
{
    HWND    hwndOwner;

    if (hwnd == NULL) {
	hwndOwner = glb.hwndClient;
	if (hwndOwner == NULL)
	    hwndOwner = HWND_DESKTOP;
    }
    else
	hwndOwner = hwnd;

    WinMessageBox(
	HWND_DESKTOP,		// parent
	hwndOwner,		// owner
	psz,			// message
	NULL,			// caption (use default)
	NULL,			// window id
	MB_OK | MB_APPLMODAL	// window style
    );
}


/***	QueryHCI - Query hardcopy capabilities for current form
 *
 *	Entry
 *	    hdc - handle to DC
 *	    phci - pointer to HCINFO structure
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL QueryHCI(HDC hdc,HCINFO *phci)
{
    LONG    cForms;
    LONG    i;
    LONG    rc;

    cForms = DevQueryHardcopyCaps(hdc,0L,0L,phci); // Get form count
    if (cForms == 0)
	return FALSE;			// No forms!

    for (i=0; i<cForms; i++) {
	rc = DevQueryHardcopyCaps(hdc,i,1L,phci); // Get info for this form
	if (rc == 1) {			// Got info
	    if (phci->flAttributes & HCAPS_CURRENT) { // Have current form
		return TRUE;		// Return data
	    }
	}
    }
    return FALSE;
}
