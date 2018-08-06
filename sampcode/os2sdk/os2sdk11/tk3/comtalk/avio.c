/*
	avio.c -- AVIO handling routines
	For a cleaner implementation, look at the BROWSE application.

	Implements scrollbars, sets up an AVIO Presentation Space
	Intrinsically linked with a circular queue routine

	Created by Microsoft Corporation, 1989
*/

#define  INCL_AVIO
#define	 INCL_DEV
#define  INCL_VIO
#define	 INCL_WIN
#include <os2.h>
#include "global.h"
#include "circleq.h"	/* Get Circular Buffer routines */
#include "avio.h"	/* Prototype our routines */
#include <stdio.h>	/* Needed to open LOG file */
/*
    Constants
*/
#define AVIO_PS_ROWS	25	/* Dimensions of the AVIO PS */
#define AVIO_PS_COLUMNS	MAXLINELEN
#define	CATTRBYTES	1	/* 1 or 3 attribute bytes/cell */
#define	DEFPAGEWIDTH	5	/* Default pagesizes */
#define	DEFPAGEHEIGHT	5

char	Blank[2] = { 0x20, 0x07 };

/*
    Macros to make the code more readable
*/
/* Upper and Lower Bound Calculations */
#define	Abs(a)		(((a) > 0) ? (a) : (-(a)))
#define LowerBound(pos, disp, lbound) Max(pos - disp, lbound)
#define UpperBound(pos, disp, ubound) Min(pos + disp, ubound)

/* Scroll Bar Abbreviations */

#define	DisableSB(hSB)	WinSetParent(hSB,  HWND_OBJECT, FALSE)
#define	EnableSB(hSB)	WinSetParent(hSB, hWndSBParent, FALSE)
#define	HBarHeight()	(fNeedHorz ? lHSBHeight : 0L)
#define	VBarWidth()	(fNeedVert ? lVSBWidth  : 0L)
#define SetScroll(h, pos, max) \
    WinSendMsg(h, SBM_SETSCROLLBAR, MPFROM2SHORT(pos, 0), MPFROM2SHORT(0, max))
#define	UpdateFrame(sb)	\
    WinSendMsg(hWndSBParent, WM_UPDATEFRAME, MPFROMLONG(sb), 0L)
#define	UpdateOff(w)	WinEnableWindowUpdate(w, FALSE)
#define	UpdateOn(w)	WinEnableWindowUpdate(w, TRUE)

/* Scrolling Macros */
#define	ClearScreen()	ScrollUp(-1)
#define ScrollDown(n)	VioScrollDn(0, 0, -1, -1, n, Blank, hVPS)
#define	ScrollUp(n)	VioScrollUp(0, 0, -1, -1, n, Blank, hVPS)
#define	SetCursor(x, y)	VioSetCurPos((USHORT) x, (USHORT) y, hVPS)

/* Miscellaneous */
/*
    If partial ANSI emulation is desired, use:
	VioSetCurPos((USHORT) x, (USHORT) y, hVPS); \
	VioWrtTTY(l->szText, l->cch, hVPS)
*/
#define Blast(l, x, y)	VioWrtCharStr(l->szText, l->cch, x, y, hVPS)
/*
    Calculate the number of characters in a page
    For nicer behavior, you can do rounding here
*/
#define CalcChars(pPg, pCh, default) \
	((pCh) ? (Max((int) ((pPg) / ((SHORT) pCh)), 0)) : (default))
#define	Value(value)	WinQuerySysValue(HWND_DESKTOP, value)
/*
    File-Local Variables
*/
HDC	hDC;		/* Device Context */
HVPS	hVPS;		/* Virtual PS */
int	iTopLine;	/* PS Line of window corner */
int	iCurCol; 	/* Current column of window corner */
int	cchPgWidth;	/* Width and height of our window */
int	cchPgHeight;
int	cchMaxHorz;	/* Scroll bar upper bounds */
int	cchMaxVert;	
BOOL	fNeedHorz;	/* Do we need the scroll bars or not? */
BOOL	fNeedVert;
HWND	hWndHScroll;	/* Window handles of ScrollBar windows */
HWND	hWndVScroll;
HWND	hWndSBParent;	/* Could mooch off the value in main(), but won't */
/*
    Measurements used to help make the window look nice
*/
LONG	lChWidth,   lChHeight;			/* Character size */
LONG	lHSBHeight, lVSBWidth;			/* Scrollbar measurements */
LONG	lMiscWidth, lMiscHeight;		/* Border, titlebar, ... */
int	iMaxWidth,  iMaxHeight;			/* Client area bounds  */
int	iMaxFrameWidth, iMaxFrameHeight;	/* Frame window bounds */
BOOL	fCreated;				/* AVIO PS created */
int	rc;					/* Return code */
VIOCURSORINFO vci;
/*
   Local prototypes
*/
void GetMeasurements(void);
void Update(USHORT, USHORT, USHORT, BOOL);
void Refresh(BOOL);
void WantCursor(BOOL);
void SetScrollPos(void);
void SetScrollPosHorz(void);
void SetScrollPosVert(void);
/*
    The actual routines
*/
void GetMeasurements(void) {
/*
    Get display parameters
*/
    /*
	Scroll bar widths and heights
    */
    lHSBHeight	= Value(SV_CYHSCROLL);
    lVSBWidth	= Value(SV_CXVSCROLL);
    /*
	Non-PS widths and heights
    */ 
    lMiscHeight	= (Value(SV_CYSIZEBORDER) << 1)	/* A border on each side */
		+ Value(SV_CYTITLEBAR)		/* The title bar...      */
		+ Value(SV_CYMENU)		/* ...and the menu bar   */
		+ Value(SV_CYBYTEALIGN);	/* ...and alignment	 */
		
    lMiscWidth	= (Value(SV_CXSIZEBORDER) << 1);/* A border on each side */
    /*
	Height and width of characters
    */
    rc = DevQueryCaps(hDC, CAPS_CHAR_HEIGHT, 1L, &lChHeight);
    rc = DevQueryCaps(hDC, CAPS_CHAR_WIDTH,  1L, &lChWidth);
    /*
	Compute size of client and frame windows
    */
    iMaxWidth		= (AVIO_PS_COLUMNS	* (int) lChWidth);
    iMaxHeight		= (AVIO_PS_ROWS		* (int) lChHeight);
    iMaxFrameWidth	= (iMaxWidth		+ (int) lMiscWidth);
    iMaxFrameHeight	= (iMaxHeight		+ (int) lMiscHeight);
    /*
	Compute cursor attributes
    */
    vci.yStart	= (USHORT) 0;
    vci.cEnd	= (USHORT) lChHeight - 1;
    vci.cx	= 0;
}

void AvioInit(HWND hWndFrame, HWND hWndClient) {
/*
    Initialize Presentation Space, Device Context, Scroll Bars
*/
    /*
	Create the AVIO Presentation Space
    */
    hDC = WinOpenWindowDC(hWndClient);
    VioCreatePS(&hVPS, AVIO_PS_ROWS, AVIO_PS_COLUMNS, 0, CATTRBYTES, 0);
    VioAssociate(hDC, hVPS);
    fCreated = TRUE;
    /*
	Turn on the cursor and home it
    */
    WantCursor(TRUE);
    SetCursor(0, 0);
    /*
	Snag scroll bar info
    */
    hWndHScroll  = WinWindowFromID(hWndFrame,  FID_HORZSCROLL);
    hWndVScroll  = WinWindowFromID(hWndFrame,  FID_VERTSCROLL);
    hWndSBParent = WinQueryWindow(hWndHScroll, QW_PARENT, FALSE);
    fNeedHorz	 = fNeedVert  = TRUE;
    /*
	Get character height in pixels, etc...
    */
    GetMeasurements();
}

void AvioStartup(HWND hWndClient) {
    SWP swp;
    /*
	Initialize the queue
    */
    QueInit();
    /*
	Initialize the screen
    */
    ClearScreen();
    WinQueryWindowPos(hWndClient, &swp);
    AvioSize(hWndClient, WM_NULL, NULL, MPFROM2SHORT(swp.cx, swp.cy));
}

void AvioScroll(USHORT SB_Command, USHORT usPosition, BOOL fHorizontal) {
/*
    Process the scroll bar messages

    These routines are symmetric; in fact, SB_LINELEFT = SB_LINEUP, etc...
    so one might note that this could be condensed.  It's left expanded for
    speed and clarity.  I bound the values each way so that we stay inside
    the AVIO presentation space.
*/
    if (fHorizontal) {  /* Horizontal Scroll Bar */
	switch (SB_Command) {
	    case SB_LINELEFT:
		iCurCol = LowerBound(iCurCol, 1, 0); break;
	    case SB_LINERIGHT:
		iCurCol = UpperBound(iCurCol, 1, cchMaxHorz); break;
	    case SB_PAGELEFT:
		iCurCol = LowerBound(iCurCol, cchPgWidth, 0); break;
	    case SB_PAGERIGHT:
		iCurCol = UpperBound(iCurCol, cchPgWidth, cchMaxHorz); break;
	    case SB_SLIDERTRACK:
		iCurCol = (SHORT) usPosition;
	    default: break;
	}
	if (SB_Command != SB_SLIDERTRACK)
	    SetScroll(hWndHScroll, iCurCol, cchMaxHorz);

    } else { /* Vertical Scroll Bar */
	switch (SB_Command) {
	    case SB_LINEUP:
		iTopLine = LowerBound(iTopLine, 1, 0); break;
	    case SB_LINEDOWN:
		iTopLine = UpperBound(iTopLine, 1, cchMaxVert); break;
	    case SB_PAGEUP:
		iTopLine = LowerBound(iTopLine, cchPgHeight, 0); break;
	    case SB_PAGEDOWN:
		iTopLine = UpperBound(iTopLine, cchPgHeight, cchMaxVert); break;
	    case SB_SLIDERTRACK:
		iTopLine = (SHORT) usPosition;
	    default: break;
	}
	if (SB_Command != SB_SLIDERTRACK)
	    SetScroll(hWndVScroll, iTopLine, cchMaxVert);
    }
    Refresh(FALSE);
}

MRESULT AvioSize(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
    Do the default AVIO sizing, and kyfe a few values
*/
    if (!fCreated) return 0L;
    /*
	Compute height and width of page in characters

	The scrollbars have already been subtracted out,
	since we are called by the client area.
    */
    cchPgHeight = CalcChars(SHORT2FROMMP(mp2), lChHeight, DEFPAGEHEIGHT); 
    cchPgWidth  = CalcChars(SHORT1FROMMP(mp2), lChWidth,  DEFPAGEWIDTH);
    /*
	Adjust scrollbar maximums
    */
    cchMaxVert = Max(AVIO_PS_ROWS    - cchPgHeight, 0);
    cchMaxHorz = Max(AVIO_PS_COLUMNS -  cchPgWidth, 0);
    /*
	Maintain scrollbar integrity
    */
    fNeedHorz = (cchMaxHorz > 0);
    fNeedVert = (cchMaxVert > 0);
    SetScroll(hWndHScroll, iCurCol  = Min(iCurCol, cchMaxHorz), cchMaxHorz);
    SetScroll(hWndVScroll, iTopLine = Min(iTopLine,cchMaxVert), cchMaxVert);
    /*
	Do the Scroll Bar shifting
    */
    Refresh(FALSE);
    /*
	Now, do the normal AVIO processing
    */
    return WinDefAVioWindowProc(hWnd, msg, mp1, mp2);
}

void Update
    (USHORT usLineNum, USHORT usHowMany, USHORT usStartLine, BOOL fForced) {
/*
    Updates usHowMany lines starting from usStartLine on screen.
    Starts at saved line usLineNum.  If fForced is set, all lines
    in range are displayed; otherwise it's lazy.
*/
    USHORT	i;				/* Loop index */
    USHORT	usWhichLine = usLineNum;	/* Line to be queried */
    Line	l;				/* Line to be output */

    for (i = usStartLine; i < (usStartLine + usHowMany); i++) {
	l = QueQuery(usWhichLine++);		/* Get the line */
	if (!l->fDrawn || fForced) {
	    if (l->cch) Blast(l, i, 0);		/* Print it out */
	    if (!l->fComplete) SetCursor(i, l->cch);
	    l->fDrawn = TRUE;
	}
    }
}

void Refresh(BOOL fRedraw) {
/*
    fRedraw forces full redraw if set 
*/
    SHORT  sDelta;
    int static iOldTopLine = -AVIO_PS_ROWS;

    VioSetOrg(0, iCurCol, hVPS); /* Get the free AVIO horizontal shift */
    sDelta = iTopLine - iOldTopLine; /* Compute vertical shift */
    if ((Abs(sDelta) < AVIO_PS_ROWS) && !fRedraw) {
	if (sDelta < 0) { 	/* Scroll Up -- make sDelta positive*/
	    ScrollDown(-sDelta);
	    Update(iTopLine, -sDelta, 0, TRUE);
	} else {		/* Scroll Down by sDelta */
	    ScrollUp(sDelta);
	    Update(iTopLine + cchPgHeight - sDelta, sDelta,
				cchPgHeight - sDelta, TRUE);
	}
    } else AvioRedraw();	/* Redo the entire screen */
    iOldTopLine = iTopLine;
}

void AvioClose (void) {
/*
    Termination routines
*/
    /*
	Destroy the Presentation Space
    */
    VioAssociate(NULL, hVPS);
    VioDestroyPS(hVPS);
    fCreated = FALSE;
}

void AvioPaint(HWND hWnd) {
    static HPS   hPS;
    static RECTL rcl;

    hPS = WinBeginPaint(hWnd, NULL, &rcl);
    VioShowPS(AVIO_PS_ROWS, AVIO_PS_COLUMNS, 0, hVPS);
    WinEndPaint(hPS);
} 

MRESULT AvioMinMax(PSWP pSWP) {
/*
    Control Maximizing
*/
    if (pSWP->fs & (SWP_MAXIMIZE | SWP_RESTORE)) {
	if (pSWP->fs & SWP_MAXIMIZE) {
	    /*
		Save cx, cy values for later origin displacement
	    */
	    int iOldcx = pSWP->cx;
	    int iOldcy = pSWP->cy;
	    /*
		Displace, and change to maximum size
	    */
	    pSWP->x += (iOldcx - (pSWP->cx = iMaxFrameWidth));
	    pSWP->y += (iOldcy - (pSWP->cy = iMaxFrameHeight));
	}
	/*
	    Now, fix the scroll bars
	*/
	AvioAdjustFrame(pSWP);
	return TRUE;
    }
    return FALSE;
}

void AvioClear(void) { ClearScreen(); }

void AvioAdjustFrame(PSWP pSWP) {
/*
    Trap WM_ADJUSTWINDOWPOS messages to the frame with this routine.
    Keep the window sized right, and control scrollbar visibility.
*/
    BOOL fNeededHorz = fNeedHorz;
    BOOL fNeededVert = fNeedVert;
/*
    Do scrollbar enable/disable calculations (but don't update the screen)
*/
    if (pSWP->fs & SWP_MINIMIZE) fNeedHorz = fNeedVert = FALSE;
    if ((pSWP->cx * pSWP->cy) == 0) return;
    /*
	Do we need them?
    */
    fNeedVert = (pSWP->cy < (SHORT) (iMaxFrameHeight));
    fNeedHorz = (pSWP->cx < (SHORT) (iMaxFrameWidth  + VBarWidth()));
    fNeedVert = (pSWP->cy < (SHORT) (iMaxFrameHeight + HBarHeight()));
/*
    Do width calculations to make sure we're staying small enough.
    The Tracking Rectangle shouldn't allow us to get too big.
*/
    /*
	Check if we're stretching too far
    */
    pSWP->cx = Min(pSWP->cx, iMaxFrameWidth  + (int) VBarWidth());
    pSWP->cy = Min(pSWP->cy, iMaxFrameHeight + (int) HBarHeight());
    /*
	...if so, fix, then add them!
    */
    AvioSize(NULL, WM_NULL, NULL, MPFROM2SHORT(
	pSWP->cx - (int) (lMiscWidth + VBarWidth()),
	pSWP->cy - (int) (lMiscHeight + HBarHeight()) ));

    if (fNeedHorz) {
	if (!fNeededHorz) {
	    EnableSB(hWndHScroll);
	    UpdateOff(hWndHScroll);
	    UpdateFrame(FCF_HORZSCROLL);
	    UpdateOn(hWndHScroll);
	}
    } else {
	if (fNeededHorz) {
	    DisableSB(hWndHScroll);
	    UpdateOff(hWndHScroll);
	    UpdateFrame(FCF_HORZSCROLL);
	    UpdateOn(hWndHScroll);
	}
    }
    if (fNeedVert) {
	if (!fNeededVert) {
	     EnableSB(hWndVScroll);
	     UpdateOff(hWndVScroll);
	     UpdateFrame(FCF_VERTSCROLL);
	     UpdateOn(hWndVScroll);
	}
    } else {
	if (fNeededVert) {
	     DisableSB(hWndVScroll);
	     UpdateOff(hWndVScroll);
	     UpdateFrame(FCF_VERTSCROLL);
	     UpdateOn(hWndVScroll);
	}
    }
}

void AvioTrackFrame(HWND hWnd, MPARAM mpTrackFlags) {
/*
    Takes action on WM_TRACKFRAME message
*/
    static TRACKINFO tiTrackInfo;
    /*
	Get the tracking information in the TrackInfo structure
    */
    WinSendMsg(hWnd, WM_QUERYTRACKINFO, mpTrackFlags, &tiTrackInfo);
    WinTrackRect(hWnd, NULL, &tiTrackInfo);
}

void AvioQueryTrackInfo(PTRACKINFO pTI) {
/*
    Forces the frame to be byte aligned and bounded
*/
    BOOL fMove;
    /*
	Get the grid set up for byte alignment

	Set cxGrid to half a character width, because sizing
	from the keyboard tries to move by half characters.
	Also, make sure we can move the window freely.
    */
    fMove = ((pTI->fs & TF_MOVE) == TF_MOVE);
    pTI->fs     |= TF_GRID;
    pTI->cxGrid  = (fMove) ? 1 : ((SHORT) (lChWidth << 1));
    pTI->cyGrid  = (fMove) ? 1 : ((SHORT) lChHeight);
    /*
	Bound the frame now
    */
    pTI->ptlMinTrackSize.x = (pTI->cxBorder << 1) + lMiscWidth;
    pTI->ptlMinTrackSize.y = (pTI->cyBorder << 1) + lMiscHeight;
    pTI->ptlMaxTrackSize.x = iMaxFrameWidth  + lVSBWidth +  (pTI->cxBorder <<1);
    pTI->ptlMaxTrackSize.y = iMaxFrameHeight + lHSBHeight + (pTI->cyBorder <<1);
}

BOOL AvioUpdateLines(BOOL fPage, BOOL *fPaging) {
/*
    Update the display
*/
    int	cLines;

    cLines = QueUpdateHead(AVIO_PS_ROWS, fPage, *fPaging);
    if (cLines == AVIO_PS_ROWS) *fPaging = TRUE;
    if (cLines > 0) {
	ScrollUp(cLines);
        Update(iTopLine + AVIO_PS_ROWS - cLines, cLines,
			AVIO_PS_ROWS - cLines, TRUE);
    }
    Update(iTopLine, cchPgHeight, 0, FALSE);
    return TRUE;
}

void AvioRedraw(void) {
/*
    Clear, then redraw the entire Presentation Space
*/
    ClearScreen();
    Update(iTopLine, cchPgHeight, 0, TRUE);
}

void WantCursor(BOOL fYes) {
/*
    Do the underscore cursor
*/
    vci.attr	= (USHORT) (fYes ? 0 : -1);
    vci.yStart	= 0;
    vci.cEnd	= (USHORT) lChHeight - 1;
    vci.cx	= 0;
    VioSetCurType(&vci, hVPS);
}

void AvioPageUp(void) {
/*
    Execute the Page Up instruction
*/
    int cLines;

    cLines = QuePageUp(AVIO_PS_ROWS);
    ScrollDown(cLines);
    Update(iTopLine, cLines, 0, TRUE);
}

