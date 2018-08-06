/*
    avio.c -- AVIO action routines

    Implements scrollbars, sets up an AVIO Presentation Space
*/
#define  INCL_AVIO
#define	 INCL_DEV
#define  INCL_VIO
#define  INCL_WINWINDOWMGR
#define  INCL_WINSYS
#define  INCL_WINMESSAGEMGR
#define  INCL_WINTRACKRECT
#define  INCL_WINFRAMEMGR
#define  INCL_WINSCROLLBARS
#include <os2.h>
#include <string.h>	/* One strlen() call in the Blast() macro */
#include "avio.h"	/* Get Avio-prefixed routine prototypes   */
/*
    Constants
*/
char	Blank[2] = { 0x20, 0x07 };	/* <Space> + EGA white attribute */
/*
    Macros to make the code more readable
*/
/* Upper and Lower Bound Calculations */
#define	Abs(a)		(((a) > 0) ? (a) : (-(a)))
#define	Min(a,b)	(((a) < (b)) ? (a) : (b))
#define	Max(a,b)	(((a) > (b)) ? (a) : (b))
#define LowerBound(pos, disp, lbound) Max(pos - disp, lbound)
#define UpperBound(pos, disp, ubound) Min(pos + disp, ubound)

/* Scroll Bar Abbreviations */
#define DisableSB(hSB)	WinSetParent(hSB, HWND_OBJECT, TRUE)
#define EnableSB(hSB) 	WinSetParent(hSB, hWndFrame,   TRUE)
#define SetScroll(h, pos, max) \
    WinSendMsg(h, SBM_SETSCROLLBAR, MPFROM2SHORT(pos, 0), MPFROM2SHORT(0, max))

/* Scrollbar redraw macros */
#define UpdateOn(c, hsb)	if (!(++c)) WinEnableWindowUpdate(hsb, TRUE)
#define UpdateOff(c, hsb)	if (!(c--)) WinEnableWindowUpdate(hsb, FALSE)
#define	UpdateFrame(sb)	\
	WinSendMsg(hWndFrame, WM_UPDATEFRAME, MPFROMLONG(sb), 0L)

/* Scrolling Macros */
#define ClearScreen()	ScrollUp(-1)	/* Scroll up an "infinite" # lines */
#define ScrollDown(n)	VioScrollDn(0, 0, -1, -1, n, Blank, hVPS)
#define ScrollUp(n)	VioScrollUp(0, 0, -1, -1, n, Blank, hVPS)

/* RectL -> SWP conversion macros */
#define	lcx(r)		((r.xRight - r.xLeft) + 1)
#define	lcy(r)		((r.yTop - r.yBottom) + 1)

/* Miscellaneous macros */
#define Blast(l, x, y)	VioWrtCharStr(l, Min((SHORT) strlen(l), cxChScreen), x, y, hVPS)
#define CalcChars(sPg, sCh) \
    ((sCh) ? (Max(((sPg) / (sCh)), 0)) : 0)
#define	SetCellSize(h,w) VioSetDeviceCellSize(h, w, hVPS)
#define	Value(value)	WinQuerySysValue(HWND_DESKTOP, value)
/*
    File-Local Variables
*/
HDC	hDC;		/* Device Context */
HVPS	hVPS;		/* Virtual PS */
int	iTopLine;	/* PS Line of window corner */
int	iCurCol; 	/* Current column of window corner */
int	cxChPage;	/* Width and height of our window, in characters */
int	cyChPage;
int	iMaxHorz;	/* Scroll bar upper bounds */
int	iMaxVert;	
BOOL	fNeedHorz;	/* Do we need the scroll bars or not? */
BOOL	fNeedVert;
HWND	hWndHorzSB;	/* Window handles of ScrollBar windows */
HWND	hWndVertSB;
extern	HWND	hWndFrame;	/* Client, frame windows */
extern	HWND	hWndClient;
PFNWP	pfnOldClient;	/* Old Client Window Procedure pointer */
PFNWP	pfnOldFrame;	/* Old Frame  Window Procedure pointer */
SHORT	cyChPS;		/* Number of rows in AVIO PS */
SHORT	cxChPS;		/* Number of cols in AVIO PS */
SHORT	cyChScreen;		/* Number of rows in display space */
SHORT	cxChScreen;		/* Number of cols in display space */
PFNQL	pfnQueryLine;
/*
    Measurements used to help make the window look nice
*/
LONG	cxConstant, cyConstant;			/* Miscellaneous frame lens */
int	cxMaxFrame, cyMaxFrame;			/* Maximum frame widths */
LONG	lChWidth,   lChHeight;
SHORT	cxMaxClient, cyMaxClient;		/* Client area bounds  */
BOOL	fCreatedPS;				/* AVIO PS created */
int	cHUpdate = -1;				/* Keep track of updates */
int	cVUpdate = -1;
/*
   Local prototypes
*/
void FixScroll(BOOL, BOOL, HWND, ULONG, int *, int, int *);
void UpdateScrollBars(RECTL);
void Refresh(void);
void Update(USHORT, USHORT, USHORT);
/*
    The actual routines
*/
void AvioInit(PLBINFO plbi) {
/*
    Initialize Presentation Space, Device Context, Scroll Bars
*/
    VIOCURSORINFO vci;
    /*
	Initialize the line buffer info 
    */
    cyChScreen	= plbi->sRows;
    cxChScreen	= plbi->sCols;
    cyChPS	= plbi->sPSrows;
    cxChPS	= plbi->sPScols;
    pfnQueryLine = plbi->pfnQL;
    /*
	One Time Initializations...
    */
    if (!fCreatedPS) {
	/*
	   Create the AVIO Presentation Space, with one attribute byte
	*/
	hDC = WinOpenWindowDC(hWndClient);	/* Open the device context */
	VioCreatePS(&hVPS, cyChPS, cxChPS + 1, 0, 1, 0);
	VioAssociate(hDC, hVPS);		/* Link the PS with the DC */
	/*
	    Turn off the cursor (set invisible attribute)
	*/
	VioGetCurType(&vci, hVPS);
	vci.attr = -1;
	VioSetCurType(&vci, hVPS);
	/*
	    Measure the frame components
	*/
	cxConstant = 0;
	cyConstant = Value(SV_CYTITLEBAR) + Value(SV_CYMENU);
        /*
	    Snag scroll bar info
	*/
	hWndHorzSB	= WinWindowFromID(hWndFrame,  FID_HORZSCROLL);
	hWndVertSB	= WinWindowFromID(hWndFrame,  FID_VERTSCROLL);
	fNeedHorz	= fNeedVert	= TRUE;
	/*
	    Setup the Client and Frame routines
	*/
	pfnOldFrame	= WinSubclassWindow(hWndFrame,  AvioFrameWndProc);
	pfnOldClient	= WinSubclassWindow(hWndClient, AvioClientWndProc);
	fCreatedPS	= TRUE;
    }
    /*
	Repaint the screen
    */
    iTopLine = iCurCol = 0;
    AvioStartup(plbi->fLargeFont);
}

void AvioStartup(BOOL fLargeFont) {
/*
    Clear the screen, set the font, redraw the area
*/
    RECTL rclFrame;

    ClearScreen();
    AvioLargeFont(fLargeFont);
    WinQueryWindowRect(hWndFrame, &rclFrame);
    UpdateScrollBars(rclFrame);
    Update(0, cyChPS, 0);
}

void AvioScroll(USHORT SB_Command, USHORT Position, BOOL Horizontal) {
/*
    Process the scroll bar messages

    These routines are symmetric; in fact, SB_LINELEFT = SB_LINEUP, etc...
    so one might note that this could be condensed.  It's left expanded for
    speed and clarity.  The scrollbar values are bounded to stay inside
    the Presentation Space.
*/
    if (Horizontal) {  /* Horizontal Scroll Bar */
	switch (SB_Command) {
	    case SB_LINELEFT:
		iCurCol = LowerBound(iCurCol, 1, 0); break;
	    case SB_LINERIGHT:
		iCurCol = UpperBound(iCurCol, 1, iMaxHorz); break;
	    case SB_PAGELEFT:
		iCurCol = LowerBound(iCurCol, cxChPage, 0); break;
	    case SB_PAGERIGHT:
		iCurCol = UpperBound(iCurCol, cxChPage, iMaxHorz); break;
	    case SB_SLIDERTRACK:
		iCurCol = (SHORT) Position;
	    default: break;
	}
	if (SB_Command != SB_SLIDERTRACK)
	    SetScroll(hWndHorzSB, iCurCol, iMaxHorz);

    } else { /* Vertical Scroll Bar */
	switch (SB_Command) {
	    case SB_LINEUP:
		iTopLine = LowerBound(iTopLine, 1, 0); break;
	    case SB_LINEDOWN:
		iTopLine = UpperBound(iTopLine, 1, iMaxVert); break;
	    case SB_PAGEUP:
		iTopLine = LowerBound(iTopLine, cyChPage, 0); break;
	    case SB_PAGEDOWN:
		iTopLine = UpperBound(iTopLine, cyChPage, iMaxVert);break;
	    case SB_SLIDERTRACK:
		iTopLine = (SHORT) Position;
	    default: break;
	}
	if (SB_Command != SB_SLIDERTRACK)
	    SetScroll(hWndVertSB, iTopLine, iMaxVert);
    }
    Refresh();
}

MRESULT AvioSize(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
    Do the default AVIO sizing, and kyfe a few values
*/
    RECTL rclFrame;

    if (!fCreatedPS) return 0L;
    /*
	Update the scroll bars, and the screen
    */
    WinQueryWindowRect(hWndFrame, &rclFrame);
    UpdateScrollBars(rclFrame);
    /*
	Now, do the normal AVIO processing
    */
    return WinDefAVioWindowProc(hWnd, msg, mp1, mp2);
}

void Update(USHORT usLineNum, USHORT usHowMany, USHORT usStartLine) {
/*
    Updates N lines starting from START line on screen.
    Starts at saved line LINENUM.
*/
    USHORT	i;				/* Loop index variable */
    USHORT	usWhichLine = usLineNum;	/* Line number to be queried */
    char	*szLine;

    for (i = usStartLine; i < (usStartLine + usHowMany); i++) {
	szLine = (*pfnQueryLine)(usWhichLine++);	/* Get the line */
	if (szLine) Blast(szLine, i, 0);		/* Print it out */
    }
}

void Refresh(void) {
/*
    Do the origin shifting and screen updating
*/
    SHORT  Delta;
    int static iOldTopLine = 0;

    VioSetOrg(0, iCurCol, hVPS); /* Get the free AVIO horizontal shift */
    Delta = iTopLine - iOldTopLine; /* Compute vertical shift */
    if (Abs(Delta) < cyChPS) {
	if (Delta < 0) { 	/* Scroll Up -- make Delta positive*/
	    ScrollDown(-Delta);
	    Update(iTopLine, -Delta, 0);
	} else {		/* Scroll Down by Delta */
	    ScrollUp(Delta);
	    Update(iTopLine + cyChPS - Delta, Delta, cyChPS - Delta);
	}
    } else AvioRedraw();	/* Redo the entire screen */
    iOldTopLine = iTopLine;
}

void AvioClose(void) {
/*
    Termination routines
*/
    /*
	Destroy the Presentation Space
    */
    VioAssociate(NULL, hVPS);
    VioDestroyPS(hVPS);
    fCreatedPS = FALSE;
}

void AvioPaint(HWND hWnd) {
/*
    Paint the AVIO presentation space by telling it to show itself.
    A possible optimization here is to repaint only the update region.
*/
    static HPS	 hPS;
    static RECTL RectL;

    hPS = WinBeginPaint(hWnd, (HPS) NULL, &RectL);
    VioShowPS(cyChPS, cxChPS, 0, hVPS);
    WinEndPaint(hPS);
} 

MRESULT AvioMinMax(PSWP pSWP) {
/*
    Handle WM_MINMAX messages, to make sure frame doesn't get too big
*/
    if (pSWP->fs & (SWP_MAXIMIZE | SWP_RESTORE)) {
	if (pSWP->fs & SWP_MAXIMIZE) {
	    /*
		Save cx, cy values for later origin displacement
	    */
	    int Oldcx = pSWP->cx;
	    int Oldcy = pSWP->cy;
	    /*
		Displace, and change to maximum size
	    */
	    pSWP->x += (Oldcx -
		(pSWP->cx = cxMaxFrame + (int) (Value(SV_CXSIZEBORDER) << 1)));
	    pSWP->y += (Oldcy -
		(pSWP->cy = cyMaxFrame + (int) (Value(SV_CYSIZEBORDER) << 1)));
	}
	/*
	    Now, fix the scroll bars
	*/
	AvioAdjustFramePos(pSWP);
	return (MRESULT) TRUE;
    }
    return FALSE;
}

void AvioClear(void) { ClearScreen(); }

void AvioAdjustFramePos(PSWP pSWP) {
/*
    Trap WM_ADJUSTWINDOWPOS messages to the frame with this routine.
    Keep the window sized right, and control scrollbar visibility.
*/
    RECTL rclFrame;

    if (!(pSWP->cx && pSWP->cy)) return; 	/* Null area... */
    if (pSWP->fs & SWP_MINIMIZE) return;	/* Iconic... */
    /*
	Make sure the dimensions are in range
    */
    pSWP->cx = Min(pSWP->cx, (cxMaxFrame + (SHORT)(Value(SV_CXSIZEBORDER)<<1)));
    pSWP->cy = Min(pSWP->cy, (cyMaxFrame + (SHORT)(Value(SV_CYSIZEBORDER)<<1)));
    /*
	Update the scroll bars
    */
    rclFrame.xLeft	= (LONG) pSWP->x;
    rclFrame.xRight	= (LONG) (pSWP->x + pSWP->cx - 1);
    rclFrame.yBottom	= (LONG) pSWP->y;
    rclFrame.yTop	= (LONG) (pSWP->y + pSWP->cy - 1);
    UpdateScrollBars(rclFrame);

    return; 
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
    Routine which processes WM_QUERYTRACKINFO messages to the frame.
    Call this routine after the default one to change various parameters.

    Note:  In reality, since we have a menu bar, we should make the
    minimum width of the window something such that it does not "fold."
*/
    BOOL fMove;
    /*
	Get the grid set up for byte alignment (unless moving)

	cxGrid is set to half character width so that arrow keys
	will function when sizing (they try to size by half a
	character)
    */
    fMove = ((pTI->fs & TF_MOVE) == TF_MOVE);
    pTI->fs     |= TF_GRID;
    pTI->cxGrid  = (fMove) ? 1 : ((SHORT) lChWidth);
    pTI->cyGrid  = (fMove) ? 1 : ((SHORT) lChHeight);
    pTI->cxKeyboard	=	(SHORT) lChWidth;
    pTI->cyKeyboard	=	(SHORT) lChHeight;
    /*
	Bound the frame.
	Maximum:	Sizing Border, Scrollbars, Title, Menus, client region
    */
    pTI->ptlMaxTrackSize.x = (LONG) (pTI->cxBorder << 1) + (LONG) cxMaxFrame;
    pTI->ptlMaxTrackSize.y = (LONG) (pTI->cyBorder << 1) + (LONG) cyMaxFrame;
}

void AvioRedraw(void) {
/*
    Clear, then redraw the entire Presentation Space
*/
    ClearScreen();
    Update(iTopLine, cyChPS, 0);
}

MRESULT CALLBACK AvioClientWndProc
	(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
     Window Procedure which traps messages to the Client area
*/
     switch (msg) {
	  case WM_PAINT:		/* Paint the AVIO way! */
		AvioPaint(hWnd);
		break;

	  case WM_SIZE:			/* Size the AVIO way!  */
		return AvioSize(hWnd, msg, mp1, mp2);
		break;

	  case WM_HSCROLL:
		AvioScroll(HIUSHORT(mp2), LOUSHORT(mp2), TRUE);
		break;

	  case WM_VSCROLL:
		AvioScroll(HIUSHORT(mp2), LOUSHORT(mp2), FALSE);
		break;

	  case WM_ERASEBACKGROUND:
		break;

	  case WM_TRACKFRAME:
		AvioTrackFrame(hWnd, mp1);
		break;

	  case WM_MINMAXFRAME:		/* Limit Maximized window size */
		AvioMinMax((PSWP) mp1);

		/* fall through */

	  default: return (*pfnOldClient)(hWnd, msg, mp1, mp2);
     }
     return 0;
}

MRESULT CALLBACK AvioFrameWndProc(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2)
/*
    Force the frame to stay small enough (no larger than the PS)
*/
{
    BOOL rc;		/* Return code from WM_QUERYTRACKINFO */

    switch(msg) {
	case WM_ADJUSTWINDOWPOS:	/* Calculate scroll bar adjustments */
	    AvioAdjustFramePos(mp1);
	    break;

	case WM_QUERYTRACKINFO:		/* Get default, then process message */
	    rc = (BOOL) SHORT1FROMMR((*pfnOldFrame)(hWnd, msg, mp1, mp2));
            AvioQueryTrackInfo((PTRACKINFO) mp2);
	    return (MRESULT) rc;

	default: break;
    }
    return (*pfnOldFrame)(hWnd, msg, mp1, mp2);
}

void UpdateScrollBars(RECTL rclClient) {
/*
    This routine fixes up the scroll bars when the window is resized, or
    when the font size is changed.

    Parameters:	The dimensions of the frame window
    Result:	Updates the scrollbars, enabling/disabling as needed
*/
    BOOL    fNeededHorz = fNeedHorz;  /* Did we need the scrollbars then? */
    BOOL    fNeededVert = fNeedVert;
    /*
	Compute the client rectangle, without the scrollbars
    */
    WinCalcFrameRect(hWndFrame, &rclClient, TRUE);
    /*
	Compute page width -- do we need the horizontal scroll bar?
    */
    cxChPage	 = (int) CalcChars(lcx(rclClient), lChWidth);
    fNeedHorz = ((iMaxHorz = Max(cxChScreen - cxChPage,  0)) > 0);
    /*
	Compute page height -- do we need the vertical scroll bar?
    */
    cyChPage	 = (int) CalcChars(lcy(rclClient), lChHeight);
    fNeedVert = ((iMaxVert = Max(cyChScreen - cyChPage, 0)) > 0);
    /*
	Maintain scrollbar integrity
    */
    iCurCol	= Min(iCurCol, iMaxHorz);
    iTopLine	= Min(iTopLine, iMaxVert);
    /*
	Now, update the scrollbars as necessary
    */
    FixScroll(fNeededHorz, fNeedHorz, hWndHorzSB,
	      FCF_HORZSCROLL, &iCurCol, iMaxHorz, &cHUpdate);

    FixScroll(fNeededVert, fNeedVert, hWndVertSB,
	      FCF_VERTSCROLL, &iTopLine, iMaxVert, &cVUpdate);
    /*
	Now, update the screen to be visually consistent
    */
    Refresh();
}

void FixScroll(fNeeded, fNeed, hWnd, flScroll, piPos, iMax, pc)
/*
    This routine makes the necessary scrollbar adjustments, and
    also enables/disables them.
*/
BOOL	fNeeded;	    /* Whether we used to need the scrollbar */
BOOL	fNeed;		    /* Whether we need the scrollbar now */
HWND	hWnd;		    /* Handle to the scrollbar window */
ULONG	flScroll;	    /* FCF_xxxxSCROLL flag (for the scrollbar) */
int	*piPos; 	    /* Current location of scrollbar thumb */
int	iMax;		    /* New maximum for the scrollbar */
int	*pc;		    /* Counter for WinEnableWindowUpdate recursion */
{
    if (fNeed) {    /* Enable the scroll bar -- we didn't need it before */
	if (!fNeeded) {
	    EnableSB(hWnd);
	    UpdateOff((*pc), hWnd);
	    UpdateFrame(flScroll);
	    UpdateOn((*pc), hWnd);
	}
	SetScroll(hWnd, (*piPos) = Min((*piPos), iMax), iMax);
    } else {	    /* Disable the scroll bar, we no longer need it */
	if (fNeeded) {
	    DisableSB(hWnd);
	    UpdateOff((*pc), hWnd);
	    UpdateFrame(flScroll);
	    UpdateOn((*pc), hWnd);
	}
    }
}

void AvioLargeFont(BOOL fLargeFont) {
    static BOOL fFirst = TRUE;				    // Need to init?
    static LONG lSmallHt, lSmallWd, lLargeHt, lLargeWd;     // Font sizes
    SWP swp;

    if (fFirst) {
	/*
	    The first time through, get the small and large font sizes
	*/
	DevQueryCaps(hDC, CAPS_CHAR_HEIGHT,		1L, &lLargeHt);
	DevQueryCaps(hDC, CAPS_CHAR_WIDTH,		1L, &lLargeWd);
	DevQueryCaps(hDC, CAPS_SMALL_CHAR_HEIGHT,	1L, &lSmallHt);
	DevQueryCaps(hDC, CAPS_SMALL_CHAR_WIDTH,	1L, &lSmallWd);
	fFirst = FALSE;
    }
    /*
	Set the character size with VioSetDeviceCellSize
    */
    SetCellSize( (SHORT) (lChHeight = ((fLargeFont) ? lLargeHt : lSmallHt)),
		 (SHORT) (lChWidth  = ((fLargeFont) ? lLargeWd : lSmallWd)) );
    /*
	Compute maximum size of client area
    */
    cxMaxClient	= (cxChPS * (SHORT) lChWidth);
    cxMaxFrame	= cxMaxClient + (SHORT) cxConstant;
    cyMaxClient = (cyChPS * (SHORT) lChHeight);
    cyMaxFrame  = cyMaxClient + (SHORT) cyConstant;
    /*
	Send a WM_ADJUSTFRAMEPOS message
    */
    WinQueryWindowPos(hWndFrame, &swp);
    if (swp.fs & SWP_MAXIMIZE) {
	AvioMinMax(&swp);
	WinSetMultWindowPos(hWndFrame, &swp, 1);
    } else {
	swp.fs = SWP_ACTIVATE | SWP_MOVE | SWP_SHOW | SWP_SIZE;
	WinSetWindowPos(hWndFrame, NULL, swp.x, swp.y,
	    Min(cxMaxFrame, swp.cx), Min(cyMaxFrame, swp.cy), swp.fs);
    }
    AvioAdjustFramePos(&swp);		/* Fix up the frame, scroll bars */
    AvioPaint(hWndClient);		/* Repaint with new characters   */
}
