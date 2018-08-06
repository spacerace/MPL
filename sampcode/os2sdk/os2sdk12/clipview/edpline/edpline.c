/*
    edpline.c -- polyline editor, for practice in mouse handling
    Created by Microsoft Corporation, 1989
*/
#define INCL_DOSMEMMGR
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINSWITCHLIST
#define INCL_WINDIALOGS
#define INCL_GPIBITMAPS
#define INCL_GPIPRIMITIVES
#define	INCL_GPITRANSFORMS
#define INCL_WINMENUS
#define	INCL_WININPUT
#define	INCL_WINFRAMEMGR
#define	INCL_WINCLIPBOARD
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>

#include "edpline.h"



#define	abs(x)			(((x) > 0) ? (x) : -(x))
#define PRIM_POLYLINE		0x0001
#define PRIM_POLYFILLET 	0x0002
#define PRIM_POLYSPLINE 	0x0004
#define PRIM_POINTARC		0x0008


/************************************************************************
*
*   Function declarations
*
************************************************************************/

/* Private functions */

VOID   cdecl main(VOID);
BOOL   InitGlobals(VOID);
BOOL   InitApp(VOID);
VOID   Close(HWND);
VOID   Command(HWND, USHORT);
VOID   Paint(HPS, BOOL);
VOID   MouseMove(HWND, MPARAM);
VOID   ButtonUp(HWND, USHORT);
VOID   ButtonDown(HWND, USHORT, MPARAM);
USHORT IsPtInList(PPOINTL);
USHORT AddPtToList(PPOINTL);
BOOL   IsPtCloseToLine(PPOINTL, PPOINTL, PPOINTL);
VOID   DrawPrimitive(HPS, USHORT);
VOID   DrawPolyLine(HPS);
VOID   DrawPolyFillet(HPS);
VOID   DrawPolySpline(HPS);
VOID   DrawPointArc(HPS);
VOID   DrawControlPoints(HPS, LONG, PPOINTL);
VOID   MyMessageBox(HWND, PSZ);
VOID   SwapLong(PLONG, PLONG);

/* Exported functions */

ULONG	CALLBACK WndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);



/************************************************************************
*
*   Global Variables
*
************************************************************************/

typedef struct
{
    HAB  hab;
    HMQ  hMsgQ;
    HWND hwndFrame;
    HWND hwnd;

    ULONG   flPrim;
    BOOL    fDisplayControlPoints;
    LONG    cptl;
    PPOINTL pptl;

    USHORT  usPtGrabbed;
    BOOL    fDragging;

    ULONG   ulHitPrecision;

    HPS     hpsMetafile;
    HDC     hdcMetafile;
    ULONG   hItem;
    SIZEL   sizlPage;
    DEVOPENSTRUC dop;

} GLOBALDATA;
GLOBALDATA global;


/************************************************************************
*
*   main
*
*   WinInitialize resizes our ring 2 stack, among other things, so
*   we won't GP fault trying to do graphics.  WinCreateMsgQueue defines
*   us as a REAL PM app. (as well as the WINDOWAPI statement in the .DEF
*   file...)   Call a sub to register our window class and create a window.
*   Loop over messages.  Exit cleanly.
*
************************************************************************/

VOID cdecl
main()
{
    QMSG qMsg;
    int iRet = 0;


    global.hab	 = WinInitialize(0);
    global.hMsgQ = WinCreateMsgQueue(global.hab, 0);

    if (InitApp())
	while (WinGetMsg( global.hab, (PQMSG)&qMsg, (HWND)NULL, 0, 0 ))
	    WinDispatchMsg( global.hab, (PQMSG)&qMsg );
    else
	iRet = -1;

    WinDestroyWindow( global.hwndFrame );
    WinDestroyMsgQueue( global.hMsgQ );
    WinTerminate( global.hab );
    DosExit(EXIT_PROCESS, iRet);
}




/****************************************************************************
*
*   InitGlobals
*
*   Initialize global variables.
*
****************************************************************************/

BOOL
InitGlobals()
{
    global.flPrim = PRIM_POLYLINE;
    global.fDisplayControlPoints = TRUE;

    global.cptl = 0L;
    global.pptl = NULL;
    if (DosAllocSeg(CPTLMAX * sizeof(POINTL),
		   ((PUSHORT)&global.pptl)+1, 0))
	return FALSE;

    global.usPtGrabbed = -1;
    global.fDragging = FALSE;
    global.ulHitPrecision = 0L;

    return TRUE;
}




/****************************************************************************
*
*   InitApp
*
*   Register application window class and creates standard window.
*
****************************************************************************/

#define INIT_MENU_ITEM(val, var)     \
	TOGGLE_MENU_ITEM(global.hwndFrame, (val), (var))

BOOL
InitApp()
{
    char szTitle[24];
    ULONG ctldata;
    PID pid;
    TID tid;
    HSWITCH hsw;
    static SWCNTRL swctl = { 0, 0, 0, 0, 0, SWL_VISIBLE,
			     SWL_JUMPABLE, "Edit Polyline", 0 };

    if (!InitGlobals())
	return FALSE;


    /*  Register Application Window Class  */

    WinLoadString( global.hab, (HMODULE) NULL, IDS_TITLE, sizeof(szTitle), (PCH)szTitle );
    if ( !WinRegisterClass( global.hab, (PCH)szTitle, (PFNWP)WndProc,
	    CS_SIZEREDRAW, 0 ))
	return FALSE;



    /* Create a window instance of class "PolyLine Editor" */

    ctldata = FCF_STANDARD &
     ~(ULONG)(FCF_ICON | FCF_TASKLIST);

    if (global.hwndFrame = WinCreateStdWindow(
	HWND_DESKTOP,		   /* specify desktop as parent window	    */
	WS_VISIBLE,		   /* window styles			    */
	&ctldata,		   /* frame creation flags		    */
	(PCH)szTitle,		   /* window class name 		    */
	(PCH)szTitle,		   /* name appearing in window caption	    */
	0L,			   /*					    */
	(HMODULE)NULL,		   /* use current executable module id	    */
	IDR_EDPLINE,		   /* menu id				    */
	(HWND FAR *)&global.hwnd   /* window handle			    */
	))
    {
	INIT_MENU_ITEM(IDM_CTLPOINTS, global.fDisplayControlPoints);

	if (global.flPrim & PRIM_POLYLINE)
	    CHECK_MENU_ITEM(global.hwndFrame, IDM_POLYLINE);
	if (global.flPrim & PRIM_POLYFILLET)
	    CHECK_MENU_ITEM(global.hwndFrame, IDM_POLYFILLET);
	if (global.flPrim & PRIM_POLYSPLINE)
	    CHECK_MENU_ITEM(global.hwndFrame, IDM_POLYSPLINE);
	if (global.flPrim & PRIM_POINTARC)
	    CHECK_MENU_ITEM(global.hwndFrame, IDM_POINTARC);


	/* Add ourselves to the switch list. */

	WinQueryWindowProcess(global.hwndFrame, &pid, &tid);
	swctl.hwnd	= global.hwndFrame;
	swctl.idProcess = pid;
	hsw = WinAddSwitchEntry(&swctl);

	return TRUE;
    }
    return FALSE;
}




/************************************************************************
*
*   WndProc
*
*   Process messages for the window class.
*
************************************************************************/

ULONG CALLBACK
WndProc( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM  mp1;
MPARAM  mp2;
{
    HPS   hps;

    switch (usMsg)
    {
    case WM_CLOSE:
	Close(hwnd);
	break;

    case WM_COMMAND:
	Command(hwnd, LOUSHORT(mp1));
	break;

    case WM_PAINT:
	hps = WinBeginPaint(global.hwnd, NULL, NULL);

	if (global.ulHitPrecision == 0L)
	{
	    HDC hdc;
	    LONG cx;

	    if (hdc = WinQueryWindowDC(global.hwnd)) {
		DevQueryCaps(hdc, CAPS_MARKER_WIDTH,  1L,  &cx);
		global.ulHitPrecision = (cx >> 17) + 1L;
	    } else {
		global.ulHitPrecision = 6L;
	    }
	}
	Paint(hps, TRUE);
	WinEndPaint(hps);
	break;

    case WM_BUTTON1DOWN:
    case WM_BUTTON2DOWN:
	ButtonDown(hwnd, usMsg, mp1);
	break;

    case WM_BUTTON1UP:
    case WM_BUTTON2UP:
	ButtonUp(hwnd, usMsg);
	break;

    case WM_MOUSEMOVE:
	MouseMove(hwnd, mp1);
	return( (ULONG)WinDefWindowProc(hwnd, usMsg, mp1, mp2));
	break;

    default:
	return( (ULONG)WinDefWindowProc(hwnd, usMsg, mp1, mp2));
	break;
    }

    return FALSE;
}




/************************************************************************
*
*   MouseMove
*
************************************************************************/

VOID
MouseMove(hwnd, mp1)
HWND hwnd;
MPARAM mp1;
{
    POINTL ptl;
    HPS hps;

    if (hwnd == global.hwnd)
	if (global.fDragging)
	{
	    ptl.x = (LONG) LOUSHORT(mp1);
	    ptl.y = (LONG) HIUSHORT(mp1);

	    if (global.usPtGrabbed != -1)
	    {
		hps = WinGetPS(hwnd);
		Paint(hps, FALSE);

		*(global.pptl+global.usPtGrabbed) = ptl;

		Paint(hps, FALSE);
		WinReleasePS(hps);
	    }
	}
}




/************************************************************************
*
*   ButtonUp
*
************************************************************************/

VOID
ButtonUp(hwnd, usMsg)
HWND hwnd;
USHORT usMsg;
{
    int i;
    HPS hps;


    if (hwnd == global.hwnd)
	if (global.fDragging)
	{
	    global.fDragging = FALSE;
	    if (global.usPtGrabbed != -1)
	    {
		if (usMsg == WM_BUTTON2UP)
		{
		    hps = WinGetPS(hwnd);
		    Paint(hps, FALSE);

		    if ((i = global.usPtGrabbed) < (int) global.cptl-1)
			while (i < (int) global.cptl-1)
			{
			    global.pptl[i] = global.pptl[i+1];
			    ++i;
			}

		    --global.cptl;
		    global.usPtGrabbed = -1;

		    Paint(hps, FALSE);
		    WinReleasePS(hps);
		}
		else	/* WM_BUTTON1UP */
		    global.usPtGrabbed = -1;
	    }
	}
}




/************************************************************************
*
*   ButtonDown
*
************************************************************************/

VOID
ButtonDown(hwnd, usMsg, mp1)
HWND hwnd;
USHORT usMsg;
MPARAM mp1;
{
    POINTL ptl;
    HPS hps;
    USHORT usNewPtGrabbed;


    if (hwnd == global.hwnd)
	if (!global.fDragging)
	{
	    global.fDragging = TRUE;

	    ptl.x = (LONG) LOUSHORT(mp1);
	    ptl.y = (LONG) HIUSHORT(mp1);

	    if ((usNewPtGrabbed = IsPtInList(&ptl)) != -1)
		global.usPtGrabbed = usNewPtGrabbed;

	    if (usMsg == WM_BUTTON1DOWN)
	    {
		hps = WinGetPS(hwnd);
		Paint(hps, FALSE);

		if (usNewPtGrabbed == -1)
		    global.usPtGrabbed = AddPtToList(&ptl);
		else
		    global.usPtGrabbed = usNewPtGrabbed;

		Paint(hps, FALSE);
		WinReleasePS(hps);

		if (global.usPtGrabbed == -1)
		    MyMessageBox(global.hwnd, "Cannot add any more points.");
	    }
	}
}




/************************************************************************
*
*   IsPtInList
*
************************************************************************/

USHORT
IsPtInList(pptl)
PPOINTL pptl;
{
    int i;


    /* try to find pptl in the points we already have */
    for (i = 0; i < (int) global.cptl; ++i)
	if (((abs(pptl->x - global.pptl[i].x))
		<= (LONG) global.ulHitPrecision)
	 && ((abs(pptl->y - global.pptl[i].y))
		<= (LONG) global.ulHitPrecision))
		return i;

    /* couldn't find it */
    return -1;
}




/************************************************************************
*
*   AddPtToList
*
************************************************************************/

USHORT
AddPtToList(pptl)
PPOINTL pptl;
{
    int i, j;

    if (global.cptl < CPTLMAX)
    {
	/* check for new points lying on a line segment */
	for (i = 0; i < (int) (global.cptl - 1L); ++i)
	    if (IsPtCloseToLine(&global.pptl[i], &global.pptl[i+1], pptl))
	    {
		for (j = (int) global.cptl; j > i+1; --j)
		    global.pptl[j] = global.pptl[j - 1];
		global.pptl[i+1] = *pptl;
		++global.cptl;
		return i+1;
	    }

	/* append the point */

	i = (int) global.cptl;
	global.pptl[i] = *pptl;
	++global.cptl;
	return i;
    }

    return -1;
}




/************************************************************************
*
*   IsPtCloseToLine
*
************************************************************************/

BOOL
IsPtCloseToLine(pptl1, pptl2, pptlTest)
PPOINTL pptl1;
PPOINTL pptl2;
PPOINTL pptlTest;
{
    POINTL ptlLL, ptlUR;
    LONG dx, dy, yIntercept, result;


    /* find the bounding box of the line segment */

    ptlLL = *pptl1;	/* assume line goes lower left to upper right */
    ptlUR = *pptl2;
    if (pptl1->x > pptl2->x)
	SwapLong(&ptlLL.x, &ptlUR.x);
    if (pptl1->y > pptl2->y)
	SwapLong(&ptlLL.y, &ptlUR.y);


    /* adjust the bounding box if it's too narrow */

    dx = pptl2->x - pptl1->x;
    if (abs(dx) <= (LONG) (global.ulHitPrecision >> 1))
    {
	ptlLL.x -= (LONG) (global.ulHitPrecision >> 1);
	ptlUR.x += (LONG) (global.ulHitPrecision >> 1);
    }
    dy = pptl2->y - pptl1->y;
    if (abs(dy) <= (LONG) (global.ulHitPrecision >> 1))
    {
	ptlLL.y -= (LONG) (global.ulHitPrecision >> 1);
	ptlUR.y += (LONG) (global.ulHitPrecision >> 1);
    }


    /* see if the test point is in the bounding box of the line segment */

    if ((pptlTest->x >= ptlLL.x) &&
	(pptlTest->x <= ptlUR.x) &&
	(pptlTest->y >= ptlLL.y) &&
	(pptlTest->y <= ptlUR.y))
    {
	/* test for special cases */

	if (dx == 0)
	{
	    if (abs(pptlTest->x - pptl1->x) <= (LONG) global.ulHitPrecision)
		return TRUE;
	    else
		return FALSE;
	}

	if (dy == 0)
	{
	    if (abs(pptlTest->y - pptl1->y) <= (LONG) global.ulHitPrecision)
		return TRUE;
	    else
		return FALSE;
	}


	/* test for general case */

	yIntercept = pptl1->y - (pptl1->x * dy) / dx;

	result = pptlTest->y - (pptlTest->x * dy / dx) - yIntercept;
	if (abs(result) <= (LONG) global.ulHitPrecision)
	    return TRUE;
    }

    return FALSE;
}




/************************************************************************
*
*   SwapLong
*
************************************************************************/

VOID
SwapLong(pl1, pl2)
PLONG pl1, pl2;
{
    LONG lTmp;

    lTmp = *pl1;
    *pl1 = *pl2;
    *pl2 = lTmp;
}




/************************************************************************
*
*   Close
*
************************************************************************/

VOID
Close(hwnd)
HWND hwnd;
{
    WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
}




/************************************************************************
*
*   Command
*
*   Dispatches menu commands to the proper handlers.
*
************************************************************************/

#define UPDATE_MENU_BOOL(var, val)				\
	{							\
	    TOGGLE_BOOL((var)); 				\
	    TOGGLE_MENU_ITEM(global.hwndFrame, (val), (var));	\
	}

#define UPDATE_MENU_LIST(var, val)				\
	{							\
	    UNCHECK_MENU_ITEM(global.hwndFrame, (var)); 	\
	    (var) = (val);					\
	    CHECK_MENU_ITEM(global.hwndFrame, (var));		\
	}

VOID
Command(hwnd, id)
HWND hwnd;
USHORT id;
{
    HPS hps;
    BOOL fRedraw = FALSE;
    int rc;

    switch (id)
    {
    case IDM_ABOUT:
	rc = WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc, (HMODULE) NULL, IDD_ABOUT, NULL);
	fRedraw = FALSE;
	break;

    case IDM_NOPRIM:
	global.flPrim = 0L;
	TOGGLE_MENU_ITEM(global.hwndFrame, IDM_POLYLINE, 0);
	TOGGLE_MENU_ITEM(global.hwndFrame, IDM_POLYFILLET, 0);
	TOGGLE_MENU_ITEM(global.hwndFrame, IDM_POLYSPLINE, 0);
	fRedraw = TRUE;
	break;

    case IDM_POLYLINE:
	global.flPrim ^= PRIM_POLYLINE;
	TOGGLE_MENU_ITEM(global.hwndFrame, id, (global.flPrim & PRIM_POLYLINE));
	fRedraw = TRUE;
	break;

    case IDM_POLYFILLET:
	global.flPrim ^= PRIM_POLYFILLET;
	TOGGLE_MENU_ITEM(global.hwndFrame, id, (global.flPrim & PRIM_POLYFILLET));
	fRedraw = TRUE;
	break;

    case IDM_POLYSPLINE:
	global.flPrim ^= PRIM_POLYSPLINE;
	TOGGLE_MENU_ITEM(global.hwndFrame, id, (global.flPrim & PRIM_POLYSPLINE));
	fRedraw = TRUE;
	break;

    case IDM_POINTARC:
	global.flPrim ^= PRIM_POINTARC;
	TOGGLE_MENU_ITEM(global.hwndFrame, id, (global.flPrim & PRIM_POINTARC));
	fRedraw = TRUE;
	break;

    case IDM_CTLPOINTS:
	UPDATE_MENU_BOOL(global.fDisplayControlPoints, IDM_CTLPOINTS);
	fRedraw = TRUE;
	break;

    case IDM_CLEARALL:
	global.cptl = 0L;
	fRedraw = TRUE;
	break;

    case IDM_COPY:
	/*
	    To put this image on the clipboard, create a Metafile DC.

	    Associate a presentation space with the DC, then play the
	    drawing orders into the metafile.
	*/
	global.dop.pszLogAddress = NULL;
	global.dop.pszDriverName = "DISPLAY";
	global.dop.pdriv = NULL;
	global.dop.pszDataType = NULL;

	global.hdcMetafile = DevOpenDC(global.hab, OD_METAFILE,
			    "*", 4L, (PDEVOPENDATA) &global.dop, NULL);
	global.hpsMetafile = GpiCreatePS(global.hab, global.hdcMetafile,
				    &global.sizlPage, PU_PELS | GPIA_ASSOC);

	Paint(global.hpsMetafile, TRUE);
	/*
	    Clean up.  A handle to the metafile is obtained when
	    calling DevCloseDC().
	*/
	GpiAssociate(global.hpsMetafile, NULL);
	GpiDestroyPS(global.hpsMetafile);
	global.hItem = (ULONG) DevCloseDC(global.hdcMetafile);
	/*
	    Be sure to empty the clipboard of other data.  This will
	    also empty previous data stored in other formats.
	    Then, set the clipboard data with type METAFILE, passing
	    the handle to our metafile.
	*/
	if (WinOpenClipbrd(global.hab)) {
	    WinEmptyClipbrd(global.hab);
	    WinSetClipbrdData(global.hab,global.hItem, CF_METAFILE, CFI_HANDLE);
	    WinCloseClipbrd(global.hab);
	}
	break;
    }

    if (fRedraw)
    {
	hps = WinGetPS(hwnd);
	Paint(hps, TRUE);
	WinReleasePS(hps);
    }
}




/************************************************************************
*
*   Paint
*
************************************************************************/

VOID
Paint(hps, fClearScreen)
HPS  hps;
BOOL fClearScreen;
{
    LINEBUNDLE lb;
    RECTL rcl;
    if (fClearScreen)
    {
	/* clear the screen */
	WinQueryWindowRect(global.hwnd, &rcl);
	GpiBitBlt(hps, NULL, 2L, (PPOINTL) &rcl, ROP_ONE, 0L);
    }


    if (global.cptl > 0L)
    {
	if (global.fDisplayControlPoints)
	{
	    if (fClearScreen)
		/* draw all the control points */
		DrawControlPoints(hps, global.cptl, global.pptl);
	    else if (global.usPtGrabbed != -1)
		/* draw just the control point that moved */
		DrawControlPoints(hps, 1L, global.pptl+global.usPtGrabbed);
	}

	/* set mix mode to xor */
	lb.usMixMode = FM_XOR;
	GpiSetAttrs(hps, PRIM_LINE, LBB_MIX_MODE, 0L, &lb);

	/* draw the current primitives */

	if (global.flPrim & PRIM_POLYLINE)
	{
	    lb.lColor = CLR_BROWN;
	    GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR, 0L, &lb);
	    DrawPrimitive(hps, IDM_POLYLINE);
	}

	if (global.flPrim & PRIM_POLYFILLET)
	{
	    lb.lColor = CLR_DARKCYAN;
	    GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR, 0L, &lb);
	    DrawPrimitive(hps, IDM_POLYFILLET);
	}

	if (global.flPrim & PRIM_POLYSPLINE)
	{
	    lb.lColor = CLR_DARKPINK;
	    GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR, 0L, &lb);
	    DrawPrimitive(hps, IDM_POLYSPLINE);
	}

	if (global.flPrim & PRIM_POINTARC)
	{
	    lb.lColor = CLR_BACKGROUND;
	    GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR, 0L, &lb);
	    DrawPrimitive(hps, IDM_POINTARC);
	}
    }
}




/************************************************************************
*
*   DrawPrimitive
*
************************************************************************/

VOID
DrawPrimitive(hps, usPrim)
HPS hps;
USHORT usPrim;
{
    switch ( usPrim )
    {
    case IDM_POLYLINE:
	DrawPolyLine(hps);
	break;

    case IDM_POLYFILLET:
	DrawPolyFillet(hps);
	break;

    case IDM_POLYSPLINE:
	DrawPolySpline(hps);
	break;

    case IDM_POINTARC:
	DrawPointArc(hps);
	break;
    }
}




/************************************************************************
*
*   DrawPolyLine
*
************************************************************************/

VOID
DrawPolyLine(hps)
HPS hps;
{
    GpiSetCurrentPosition( hps, global.pptl );
    GpiPolyLine( hps, global.cptl-1L, global.pptl+1 );
}




/************************************************************************
*
*   DrawPolyFillet
*
************************************************************************/

VOID
DrawPolyFillet(hps)
HPS hps;
{
    if (global.cptl > 2)
    {
	GpiSetCurrentPosition( hps, global.pptl );
	GpiPolyFillet( hps, global.cptl-1L, global.pptl+1 );
    }
}




/************************************************************************
*
*   DrawPolySpline
*
************************************************************************/

VOID
DrawPolySpline(hps)
HPS hps;
{
    USHORT cptSlack;	/* # points in pptl not usable by PolySpline */

    /* GpiPolySpline expects the number of points to be a
       multiple of 3.  If we have a non-multiple of three,
       (excluding the first point, which we've used to set
       the current position), only pass the largest multiple
       of three, saving the rest for the next go-round. */

    cptSlack = (int)((global.cptl-1L) % 3) + 1;
    GpiSetCurrentPosition( hps, global.pptl );
    GpiPolySpline( hps, global.cptl-cptSlack,
		   global.pptl+1 );
}




/************************************************************************
*
*   DrawPointArc
*
************************************************************************/

VOID
DrawPointArc(hps)
HPS hps;
{
    if (global.cptl >= 3L)
    {
	GpiSetCurrentPosition( hps, global.pptl );
	GpiPointArc( hps, global.pptl+1 );
    }
}




/************************************************************************
*
*   DrawControlPoints
*
************************************************************************/

VOID
DrawControlPoints(hps, cptl, pptl)
HPS hps;
LONG cptl;
PPOINTL pptl;
{
    MARKERBUNDLE mb;

    mb.lColor = CLR_TRUE;
    mb.usMixMode = FM_XOR;
    GpiSetAttrs(hps, PRIM_MARKER, MBB_COLOR | MBB_MIX_MODE, 0L, &mb);

    GpiPolyMarker(hps, cptl, pptl);
}




/************************************************************************
*
*   MyMessageBox
*
*   Displays a message box with the given string.  To simplify matters,
*   the box will always have the same title ("PolyLine Editor"), will always
*   have a single button ("Ok"), will always have an exclamation point
*   icon, and will always be application modal.
*
************************************************************************/

VOID
MyMessageBox(hWnd, sz)
HWND hWnd;
PSZ sz;
{
    static char *szTitle = "PolyLine Editor";

    WinMessageBox(HWND_DESKTOP, hWnd, sz, szTitle, 0,
		  MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
}

MRESULT CALLBACK AboutDlgProc(HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
    About... dialog procedure
*/
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
