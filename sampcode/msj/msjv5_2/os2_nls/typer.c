/*
    TYPER.C -- based on the sample of the same name.

    This program correctly handles DBCS, and also demonstrates word
    wrapping.  It uses a circular buffer to store lines.

    Limitations:  This program does not recompute word breaks on
    WM_SIZE messages.
*/
#define	INCL_DOSNLS
#define INCL_PM
#define	INCL_NLS
#include <os2.h>
#include <string.h>
#include "typer.h"

//  Variable declarations
//
UCHAR	vaszBuffer[MAX_LINES][MAX_LINE_LENGTH + 1];
					// Circular queue of lines
USHORT	vcchLine = 0;			// Number of characters in current line
SHORT	vcpelyChar = 0; 		// Character height
BOOL	vfIsLeadByte[256];		// Is Byte i a DBCS lead byte?
HAB     vhab;				// Handle to the Anchor Block
HWND    vhwndTyper;			// Handle to the Client Area
HWND    vhwndTyperFrame;		// Handle to the Frame Window
USHORT	vusCurrent = 0;			// Current vaszBuffer line referenced

//  Useful macros
//
#define	LOADSTRING(id, sz) \
    WinLoadString(vhab, NULL, id, sizeof(sz), (PCH) sz)

//  Internal declarations
//
VOID	main(VOID);			// Main routine
VOID	InitLeadByteTable(VOID);	// Initialize the lead byte table
VOID	TyperChar(HWND, MPARAM, MPARAM);// WM_CHAR processing subroutine
VOID	TyperCreate(HWND); 		// WM_CREATE processing subroutine
VOID	TyperPaint(HWND, HPS, PRECTL);	// WM_PAINT processing subroutine

VOID main(VOID) {
    HMQ		hmqTyper;
    QMSG	qmsg;
    UCHAR	szClassName[MAX_STRING];
    UCHAR	szWindowTitle[MAX_STRING];
    ULONG	ctlData;

    // Standard initialization for a PM application
    //
    vhab     = WinInitialize(NULL);
    hmqTyper = WinCreateMsgQueue(vhab, 0);

    // Register the Typer window class -- if not successful, exit.
    //
    LOADSTRING(IDS_NAME, szClassName);
    if (!WinRegisterClass(vhab, (PCH) szClassName, (PFNWP)TyperWndProc,
                           CS_SIZEREDRAW, 0))
	DosExit(EXIT_PROCESS, 0);

    // Get the title of the application, and create the main window.
    // Notice that FCF_DBE_APPSTAT is included because the client area
    // will allow Kanji characters to be input.
    //
    LOADSTRING(IDS_TITLE, szWindowTitle);
    ctlData = FCF_STANDARD & ~(FCF_ICON | FCF_ACCELTABLE) | FCF_DBE_APPSTAT;
    vhwndTyperFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE,
			&ctlData, (PCH)szClassName, (PCH)szWindowTitle,
			0L, NULL, IDR_TYPER, (PHWND)&vhwndTyper);

    //	Initialize the lead byte table
    //
    InitLeadByteTable();

    // Poll messages from event queue
    //
    while (WinGetMsg(vhab, &qmsg, NULL, 0, 0))
        WinDispatchMsg(vhab, &qmsg);

    // Standard cleanup code for PM applications
    //
    WinDestroyWindow(vhwndTyperFrame);
    WinDestroyMsgQueue(hmqTyper);
    WinTerminate(vhab);
    DosExit(EXIT_PROCESS, 1);
}

MRESULT EXPENTRY TyperWndProc(HWND hwnd, USHORT usMsg, MPARAM mp1, MPARAM mp2) {
/*
    Note:  This scrolling behavior may not be extensible if the characters
	   are to be output from top to bottom, right to left, as can be
	   seen in traditional Chinese and Japanese scripts.
*/
    HPS    	hps;
    RECTL	rclPaint;
    RECTL	rclWindow;

    switch (usMsg) {
	case WM_CREATE:
	    TyperCreate(hwnd);
	    break;

	case WM_CLOSE:
	    WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
	    break;

	case WM_ERASEBACKGROUND:

	    // Erase the background if this message is received
	    //
	    return TRUE;
	    break;

	case WM_CHAR:

	    if (!(SHORT1FROMMP(mp1) & KC_KEYUP)) {

		// Process the downstrokes entered.
		//
		TyperChar(hwnd, mp1, mp2);

		// Invalidate the updated line.
		//
		WinQueryWindowRect(hwnd, &rclWindow);
		rclWindow.yTop = vcpelyChar - 1;
		WinInvalidateRect(hwnd, &rclWindow, TRUE);
		WinPostMsg(hwnd, WM_PAINT, 0L, 0L);
	    }
	    break;

	case WM_QUERYCONVERTPOS:
	    WinQueryWindowRect(hwnd, &rclWindow);

	    hps = WinGetPS(hwnd);
	    WinDrawText(hps, 0xFFFF, vaszBuffer[vusCurrent],
		&rclWindow, 0L, 0L, DT_LEFT | DT_BOTTOM | DT_QUERYEXTENT);
	    WinReleasePS(hps);

	    ((PRECTL) mp1)->xLeft = rclWindow.xRight;
	    ((PRECTL) mp1)->yBottom = rclWindow.yBottom;

	    return QCP_CONVERT;
	    break;

	case WM_PAINT:

	    // Repaint the invalid region
	    // Side effect:  May scroll up one line as needed.
	    //
	    hps = WinBeginPaint(hwnd, NULL, &rclPaint);
	    TyperPaint(hwnd, hps, &rclPaint);
	    WinEndPaint(hps);
            break;

	case WM_COMMAND:

	    switch (COMMANDMSG(&usMsg)->cmd) {

		// Trap the About... menu item, and put up the dialog box
		//
		case IDM_ABOUT:
		    WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc,
			  NULL, IDD_ABOUT, NULL);
		    break;

		default: break;
	    }
	    break;

	default:
	    return WinDefWindowProc(hwnd, usMsg, mp1, mp2);
	    break;
    }
    return 0L;
}

MRESULT EXPENTRY AboutDlgProc(HWND hwndDlg,USHORT usMsg,MPARAM mp1,MPARAM mp2) {
/*
    About... dialog procedure
*/
    switch(usMsg) {
	case WM_COMMAND:
	    switch(COMMANDMSG(&usMsg)->cmd) {
		case DID_OK:	WinDismissDlg(hwndDlg, TRUE);
		default:	break;
	    }
	default: return WinDefDlgProc(hwndDlg, usMsg, mp1, mp2);
    }
    return FALSE;
}

VOID TyperChar(HWND hwnd, MPARAM mp1, MPARAM mp2) {
/*
    This routine does simple input processing.

    Assumptions:  That there are no DBCS deadkeys, or invalid composites.
    This also implies that a deadkey followed by a DBCS space results in
    a DBCS space (without WinAlarm() being called).
*/
    BOOL	fDone;
    PUCHAR	pszLast;
    UCHAR	ch;
    USHORT	fs;
    USHORT	vkey;

    ch = (UCHAR) CHAR1FROMMP(mp2);
    fs = (USHORT) SHORT1FROMMP(mp1);
    vkey = (USHORT) SHORT2FROMMP(mp2);
    fDone = FALSE;

    if (fs & KC_VIRTUALKEY) {

	// Receiving a backspace...
	//
	if ((fDone = (vkey == VK_BACKSPACE || vkey == VK_DELETE))
	    && vcchLine > 0) {

	    pszLast = WinPrevChar(vhab, 0, 0,
		    &vaszBuffer[vusCurrent][0], &vaszBuffer[vusCurrent][vcchLine]);

	    // If DBCS, zero out the trailing byte.
	    //
	    if (vfIsLeadByte[*pszLast]) {
		vaszBuffer[vusCurrent][--vcchLine] = '\0';
	    }

	    // Zero out one character (SBCS, or DBCS lead byte)
	    //
	    vaszBuffer[vusCurrent][--vcchLine] = '\0';

	// Receiving a carriage return (CR)...
	//
	} else if (vkey == VK_NEWLINE || vkey == VK_ENTER) {
	    fDone = TRUE;

	    // Save the line in the vaszBuffer
	    //
	    vusCurrent = (vusCurrent + 1) % MAX_LINES;

	    // Initialize the next text line
	    //
	    vcchLine = 0; vaszBuffer[vusCurrent][0] = '\0';

	    // Scroll window upwards, for efficient updating.
	    //
	    WinScrollWindow(hwnd,0,vcpelyChar,NULL,NULL,NULL,NULL,SW_INVALIDATERGN);
	}
    }

    // If we haven't encountered VK_BACKSPACE, VK_DELETE, VK_NEWLINE, or
    // VK_ENTER, process KC_CHAR values.  We do this because some valid
    // characters are also valid vkeys (like Space).
    //
    if (!fDone) {
	// If DBCS character fits, add it.
	//
	if (fs & KC_CHAR) {
	    if (vfIsLeadByte[ch] && (vcchLine + 2 < MAX_LINE_LENGTH)) {

		// Add DBCS to line array
		//
		vaszBuffer[vusCurrent][vcchLine++]	= ch;
		vaszBuffer[vusCurrent][vcchLine++]	= CHAR2FROMMP(mp2);
		vaszBuffer[vusCurrent][vcchLine]	= '\0';

	    // If SBCS character fits, add it.
	    //
	    } else if (vcchLine + 1 < MAX_LINE_LENGTH) {

		if (fs & KC_INVALIDCOMP) {

		    // If we have a space, advance over the current character.
		    //
		    if (ch == ' ')
			vcchLine++;

		    // Otherwise, complain audibly.
		    //
		    else
			WinAlarm(HWND_DESKTOP, WA_WARNING);

		} else {

		    // Add character to line array
		    //
		    vaszBuffer[vusCurrent][vcchLine++]	= ch;
		}
		vaszBuffer[vusCurrent][vcchLine]	= '\0';

		// If it's a deadkey, reposition so that we'll overwrite
		// the deadkey on KC_INVALIDCOMP or KC_COMPOSITE.
		//
		if (fs & KC_DEADKEY)
		    vcchLine--;
	    }
	}
    }
}

VOID TyperCreate(HWND hwnd) {
    HPS		hps;
    FONTMETRICS fmTyper;
    USHORT	i;

    // Initialize text buffer
    //
    for (i = 0; i < MAX_LINES; i++)
	vaszBuffer[i][0] = '\0';

    // Get the character height
    //
    hps = WinGetPS(hwnd);
    GpiQueryFontMetrics(hps, (LONG) sizeof(FONTMETRICS), &fmTyper);
    vcpelyChar = (SHORT) fmTyper.lMaxBaselineExt + 1;
    WinReleasePS(hps);
}

VOID TyperPaint(HWND hwnd, HPS hps, PRECTL prclUpdate) {
    USHORT  usUpdateTop;
    USHORT  usUpdateBottom;
    RECTL   rclArea;
    RECTL   rclWindow;
    UCHAR   *pszTmp;
    USHORT  cchDrawn;
    USHORT  cchTmp;
    USHORT  usWhich;
    USHORT  usUpdate;
    SHORT   sNew = -1;

    // Compute the lines to be updated.
    // NOTE:  This assumes that screen coordinates start with y = 0.
    //
    usUpdateTop = (USHORT) (prclUpdate->yTop / vcpelyChar);
    usUpdateBottom = (USHORT) (prclUpdate->yBottom / vcpelyChar);

    // The following code is functionally equivalent to
    //     usWhich = (vusCurrent - usUpdateBottom) % MAX_LINES;
    //
    sNew = (vusCurrent - usUpdateBottom);
    usWhich = ((sNew >= 0) ?
		sNew % MAX_LINES : 
		MAX_LINES - (-sNew % MAX_LINES));

    // Initialize the width of the text line
    //
    WinQueryWindowRect(hwnd, &rclWindow);
    rclArea.xLeft  = rclWindow.xLeft;
    rclArea.xRight = rclWindow.xRight;

    // For all the relevant lines...
    //
    for (usUpdate = usUpdateBottom;
		usUpdate <= usUpdateTop;
		usUpdate++) {

	// Initialize the height of the text line
	//
	rclArea.yBottom     = vcpelyChar * usUpdate;
	rclArea.yTop	    = rclArea.yBottom + (vcpelyChar - 1);

	// Load the line to be printed
	//
	pszTmp = vaszBuffer[usWhich];
	cchTmp = strlen(pszTmp);

	// Always draw in a word wrapped fashion
	//
	if (cchTmp) {
	    cchDrawn = WinDrawText(hps, cchTmp, pszTmp, &rclArea,
		    CLR_BLACK, CLR_BACKGROUND,
		    DT_ERASERECT | DT_LEFT | DT_TOP | DT_WORDBREAK);
	    if (usWhich == vusCurrent) {
		while (cchDrawn && cchDrawn < cchTmp) {

		    // Fix up code, line's longer than expected!
		    // We must perform additional word wrapping.
		    //
		    // Add a new line to the line buffer (this
		    // code assumes that we must be on the last
		    // line to wrap.
		    //
		    sNew = (vusCurrent + 1) % MAX_LINES;
		    vaszBuffer[sNew][0] = '\0';
		    strcpy(vaszBuffer[sNew], vaszBuffer[vusCurrent] + cchDrawn);

		    // Adjust old line to reflect character deletion
		    //
		    vaszBuffer[vusCurrent][cchDrawn] = '\0';

		    // Set up new parameters for TyperChar()
		    //
		    vcchLine -= cchDrawn;
		    vusCurrent = sNew;

		    // Scroll the window, then fill in the invalid region
		    //
		    WinScrollWindow(hwnd, 0, vcpelyChar,
				    NULL, NULL, NULL, &rclArea, 0);

		    pszTmp = vaszBuffer[vusCurrent];
		    cchTmp -= cchDrawn;
		    cchDrawn = WinDrawText(hps, cchTmp, pszTmp, &rclArea,
			    CLR_BLACK, CLR_BACKGROUND,
			    DT_ERASERECT | DT_LEFT | DT_TOP | DT_WORDBREAK);

		    // We increment the update pointer because there's one
		    // fewer line to update.
		    usUpdate++;
		}
	    }
	} else {
	    // Clear the rectangle
	    //
	    WinFillRect(hps, &rclArea, CLR_BACKGROUND);
	}
	// Point to the next line to be drawn
	// This is functionally equivalent to
	//	usWhich = (usWhich - 1) % MAX_LINES;
	//
	usWhich = (usWhich ? (usWhich - 1) : MAX_LINES - 1);
    }
}

VOID InitLeadByteTable(VOID) {
/*
   This routine initializes the array which tells if index "i" is
   a valid leading byte in the current codepage.
*/
    COUNTRYCODE ctryc;			    // Used with DosGetDBCSEv() call
    UCHAR	vachDBCSEv[TABLE_SIZE];     // Lead Byte range table
    SHORT	i, j;			    // Temporary variables

    // Initialize the array
    //
    for (i = 0; i <= 0xFF; i++)
	vfIsLeadByte[i] = FALSE;

    // Get the valid lead byte ranges
    // Use (country, codepage) = (0, 0) for current process settings
    //
    ctryc.country = ctryc.codepage = 0;
    DosGetDBCSEv(TABLE_SIZE, &ctryc, vachDBCSEv);

    // Fill in the array, "blacking out" all the returned ranges
    //
    while (vachDBCSEv[j] && vachDBCSEv[j + 1]) {
	for (i = vachDBCSEv[j]; i <= vachDBCSEv[j + 1]; i++)
	    vfIsLeadByte[i] = TRUE;
	j += 2;
    }
}
