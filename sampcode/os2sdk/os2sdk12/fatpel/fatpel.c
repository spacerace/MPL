/************************************************************************
*
*   fatpel.c -- The Diamond Metric, Theory vs. Practice
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/
 
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define	INCL_WINPOINTERS
#define INCL_WINSWITCHLIST
#define INCL_WINTRACKRECT
#define INCL_WINDIALOGS
#define INCL_WINBUTTONS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIBITMAPS
#define	INCL_GPITRANSFORMS
#define INCL_DOSMEMMGR
#define INCL_DOSFILEMGR
#define INCL_BITMAPFILEFORMAT
#define INCL_GPIPRIMITIVES
#define INCL_WINMENUS
#define INCL_GPIREGIONS
#define INCL_WINPOINTERS
#define INCL_WININPUT
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opendlg.h"
#include "fatpel.h"




/************************************************************************
*
*   Function declarations
*
************************************************************************/

/* Private functions */

VOID  cdecl main(VOID);
BOOL  FAR InitGlobals(VOID);
BOOL  FAR InitApp(VOID);
VOID  Close(HWND);
VOID  Command(HWND, USHORT);
VOID  Paint(HPS, USHORT);
VOID  EraseBackground(HPS);
VOID  DrawGrid(HPS);
VOID  DisplayRenderedPels(HPS, USHORT);
VOID  DisplayControlPoints(HPS, LONG, PPOINTL, USHORT);
VOID  DisplayMathematicalObject(HPS, USHORT);
VOID  DrawFatPels(HPS);
VOID  DrawOneFatPel(HPS, PPOINTL, COLOR);
VOID  GetFatPelFromPt(PPOINTL, PPOINTL);
VOID  SetFatPel(PPOINTL, COLOR);
VOID  RoundControlPoints(HPS, LONG, PPOINTL, PPOINTL, LONG, LONG);
VOID  ComputeTransform(PRECTL, PRECTL);
VOID  DrawPrimitive(HPS, LONG, PPOINTL);
VOID  UpdateSurfaceDims(VOID);
VOID  MySetWindowLong	(HWND, USHORT, LONG);
VOID  MySetWindowLongHex(HWND, USHORT, LONG);
LONG  MyGetWindowLong	(HWND, USHORT);
VOID  MouseMove(HWND, MPARAM);
VOID  ButtonUp(HWND, USHORT);
VOID  ButtonDown(HWND, USHORT, MPARAM);
VOID  DragPelSize(HWND, POINTS);
VOID  WriteFile(HWND, HPS);
BOOL  WriteBMP(HFILE, HPS, PBITMAPINFOHEADER);
VOID  MyMessageBox(HWND, PSZ);
VOID  SaveWindowToFile(HWND);
SHORT IsPtInList(PPOINTL);
SHORT AddPtToList(PPOINTL);
BOOL  IsPtCloseToLine(PPOINTL, PPOINTL, PPOINTL);
VOID  SwapLong(PLONG, PLONG);


/* Exported functions */

ULONG CALLBACK WndProc	 (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK AboutDlg  (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK ColorsDlg (HWND, USHORT, MPARAM, MPARAM);
ULONG CALLBACK PelSizeDlg(HWND, USHORT, MPARAM, MPARAM);




/************************************************************************
*
*   Global Variables
*
************************************************************************/

/* compute absolute value for arbitrary (in my case, LONG) number */
/* this is to avoid compiler warnings about data conversion */
#define L_ABS(x)	(((x) > 0) ? (x) : (-(x)))

typedef struct
{
    HAB      hab;
    HMQ      hMsgQ;
    HWND     hwndFrame;
    HWND     hwnd;

    BOOL     fFirstTime;   /* TRUE --> first time initialization of rcl */
    RECTL    rcl;	   /* dimensions of client rectangle */

    HPS      hpsFat;
    HDC      hdcFat;
    HBITMAP  hbmFat;
    HPS      hpsFatShadow;
    HDC      hdcFatShadow;
    HBITMAP  hbmFatShadow;

    RECTL    rclFatBM;	   /* dimensions of fatbits bitmap */
    RECTL    rclFat;	   /* dimensions of active fat bits grid */
    LONG     cxFatPel;	    /* width of fat pel */
    LONG     cyFatPel;	    /* height of fat pel */
    LONG     cxHalfFatPel;
    LONG     cyHalfFatPel;
    USHORT   usPelShape;

    MATRIXLF matlf;	/* goes from window coords to fatpel coords */

    BOOL     fRGB;	   /* TRUE --> color mode is RGB */
    COLOR    clrMathObj;
    COLOR    clrRenderedObj;
    COLOR    clrField;
    COLOR    clrCrossHair;
    COLOR    clrInterstice;
    COLOR    clrControlPoints;

    COLOR    clrBlackIndex;
    COLOR    clrEditPel;

    USHORT   usControlPointSymbol;

    BOOL     fDisplayRenderedObj;
    BOOL     fDisplayMathObj;
    BOOL     fDisplayControlPoints;
    BOOL     fDisplayCrossHairs;
    BOOL     fDisplayPelBorder;
    BOOL     fRoundControlPoints;
    BOOL     fAutoRedraw;
    USHORT   usCurPrim;
    USHORT   usMix;

    LONG     cptl;
    PPOINTL  pptl;
    PPOINTL  pptlTmp;

    BOOL     fDraggingPelSize;
    HPOINTER hptrDragSize;

    BOOL     fDraggingPelColor;
    HPOINTER hptrDragColor;

    SHORT    sPtGrabbed;
    BOOL     fDraggingControlPoint;
    LONG     lHitPrecision;

    BOOL     fEditPelColors;

} GLOBALDATA;
GLOBALDATA global;




/************************************************************************
*
*   main
*
*   WinInitialize resizes our ring 2 stack, among other things, so
*   we won't GP fault trying to do graphics.  WinCreateMsgQueue defines
*   us as a REAL PM app. (WINDOWAPI in .DEF file does also).
*   Call a sub to register our window class and create a window.
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

BOOL FAR
InitGlobals()
{
    global.fFirstTime		 = TRUE;

    global.rcl.xLeft		 = 0L;
    global.rcl.yBottom		 = 0L;
    global.rcl.xRight		 = 0L;
    global.rcl.yTop		 = 0L;

    global.hpsFat		 = NULL;
    global.hdcFat		 = NULL;
    global.hbmFat		 = NULL;
    global.hpsFatShadow 	 = NULL;
    global.hdcFatShadow 	 = NULL;
    global.hbmFatShadow 	 = NULL;
    global.rclFatBM.xLeft	 = 0L;
    global.rclFatBM.yBottom	 = 0L;
    global.rclFatBM.xRight	 = 0L;
    global.rclFatBM.yTop	 = 0L;

    global.cxFatPel		 = 32L;
    global.cyFatPel		 = 32L;
    global.cxHalfFatPel 	 = global.cxFatPel / 2L;
    global.cyHalfFatPel 	 = global.cyFatPel / 2L;
    global.usPelShape		 = IDD_CIRCLE;

    global.fRGB 		 = FALSE;
    global.clrMathObj		 = CLR_BLUE;
    global.clrRenderedObj	 = CLR_NEUTRAL;
    global.clrField		 = CLR_CYAN;
    global.clrCrossHair 	 = CLR_DARKCYAN;
    global.clrInterstice	 = CLR_BACKGROUND;
    global.clrControlPoints	 = CLR_YELLOW;

    global.clrBlackIndex	 = CLR_ERROR;
    global.clrEditPel		 = CLR_ERROR;

    global.usControlPointSymbol  = MARKSYM_SOLIDDIAMOND;

    global.fDisplayRenderedObj	 = TRUE;
    global.fDisplayMathObj	 = TRUE;
    global.fDisplayControlPoints = TRUE;
    global.fDisplayCrossHairs	 = TRUE;
    global.fDisplayPelBorder	 = TRUE;
    global.fRoundControlPoints	 = FALSE;
    global.fAutoRedraw		 = TRUE;
    global.usCurPrim		 = IDM_POLYLINE;
    global.usMix		 = FM_OVERPAINT;

    global.fDraggingPelSize	 = FALSE;
    global.fDraggingPelColor	 = FALSE;
    global.fDraggingControlPoint = FALSE;
    global.sPtGrabbed		 = NO_POINT;
    global.lHitPrecision	 = 0L;

    global.fEditPelColors	 = FALSE;


    global.cptl = 0L;
    global.pptl = NULL;
    if (DosAllocSeg(CPTLMAX * sizeof(POINTL),
		   ((PUSHORT)&global.pptl)+1, 0))
	return FALSE;
    global.pptlTmp = NULL;
    if (DosAllocSeg(CPTLMAX * sizeof(POINTL),
		   ((PUSHORT)&global.pptlTmp)+1, 0))
	return FALSE;

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

BOOL FAR
InitApp()
{
    char szTitle[24];
    ULONG ctldata;
    PID pid;
    TID tid;
    HSWITCH hsw;
    static SWCNTRL swctl = { 0, 0, 0, 0, 0, SWL_VISIBLE,
			     SWL_JUMPABLE, "FatPels", 0 };

    if (!InitGlobals())
	return FALSE;


    /*  Register Application Window Class  */

    WinLoadString( global.hab, (HMODULE) NULL, IDS_TITLE, sizeof(szTitle), (PCH)szTitle );
    if ( !WinRegisterClass( global.hab, (PCH)szTitle, (PFNWP)WndProc,
	    CS_SIZEREDRAW, 0 ))
	return FALSE;


    /* Load the pointer to use when dragging pel size. */
    if (!(global.hptrDragSize = WinLoadPointer( HWND_DESKTOP, (HMODULE) NULL, IDR_DRAGSIZEPTR )))
	return FALSE;

    /* Load the pointer to use when dragging pel color. */
    if (!(global.hptrDragColor = WinLoadPointer( HWND_DESKTOP, (HMODULE) NULL, IDR_DRAGCOLORPTR )))
	return FALSE;


    /* Create a window instance of class "FatPel" */

    ctldata = FCF_STANDARD &
     ~(ULONG)(FCF_ICON | FCF_ACCELTABLE | FCF_TASKLIST);

    if (global.hwndFrame = WinCreateStdWindow(
	HWND_DESKTOP,		   /* specify desktop as parent window	    */
	WS_VISIBLE,		   /* window styles			    */
	&ctldata,		   /* frame creation flags		    */
	(PCH)szTitle,		   /* window class name 		    */
	(PCH)szTitle,		   /* name appearing in window caption	    */
	0L,			   /*					    */
	(HMODULE)NULL,		   /* use current executable module id	    */
	IDR_FATPEL,		   /* menu id				    */
	(HWND FAR *)&global.hwnd   /* window handle			    */
	))
    {
	INIT_MENU_ITEM(IDM_RENDEREDOBJ,	 global.fDisplayRenderedObj);
	INIT_MENU_ITEM(IDM_MATHOBJ,	 global.fDisplayMathObj);
	INIT_MENU_ITEM(IDM_CTLPOINTS,	 global.fDisplayControlPoints);
	INIT_MENU_ITEM(IDM_CROSSHAIRS,	 global.fDisplayCrossHairs);
	INIT_MENU_ITEM(IDM_PELBORDER,	 global.fDisplayPelBorder);
	INIT_MENU_ITEM(IDM_ROUNDPOINTS,	 global.fRoundControlPoints);
	INIT_MENU_ITEM(IDM_AUTOREDRAW,	 global.fAutoRedraw);
	INIT_MENU_ITEM(IDM_EDITPELCOLORS, global.fEditPelColors);

	CHECK_MENU_ITEM(global.hwndFrame, global.usCurPrim);


	/* Add ourselves to the switch list. */

	WinQueryWindowProcess(global.hwndFrame, &pid, &tid);
	swctl.hwnd	= global.hwndFrame;
	swctl.idProcess = pid;
	hsw = WinAddSwitchEntry(&swctl);

	return TRUE;
    }
    return FALSE;
}




/*************************************************************************
*
*   WndProc
*
*   Process messages for the window class.
*
************************************************************************/

ULONG CALLBACK
WndProc( hwnd, usMsg, mp1, mp2 )
HWND	hwnd;
USHORT	usMsg;
MPARAM  mp1;
MPARAM  mp2;
{
    switch (usMsg)
    {
    case WM_CLOSE:
	Close(hwnd);
	break;

    case WM_COMMAND:
	Command(hwnd, LOUSHORT(mp1));
	break;

    case WM_PAINT:
	{
	    HPS   hps;

	    if (global.fFirstTime)
	    {
		SIZEF sizfx;

		hps = WinGetPS(hwnd);
		GpiQueryMarkerBox(hps, &sizfx);
		global.lHitPrecision = sizfx.cx / 0x20000L + 1L;
		WinReleasePS(hps);

		UpdateSurfaceDims();
		global.fFirstTime = FALSE;
	    }

	    /* The small bitmap may have been resized since we last
	     * painted, in which case it will have been initialized to
	     * the field color.  Therefore, we will render the mathematical
	     * object to make sure the right fatpels are there.
	     */
	    global.usMix = FM_OVERPAINT;
	    hps = WinBeginPaint(global.hwnd, NULL, NULL);
	    Paint(hps, CLEAR_BACKGROUND|RENDER_MATH_OBJ);
	    WinEndPaint(hps);
	}
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
	break;

    case WM_SIZE:
	UpdateSurfaceDims();
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


    /* make sure we still have our pointer */
    /* notice the hierarchy of pointer modes */

    if (global.fDraggingPelSize)
    {
	if (global.hptrDragSize)
	    WinSetPointer(HWND_DESKTOP,global.hptrDragSize);
    }
    else if (global.fEditPelColors)
    {
	if (global.hptrDragColor)
	    WinSetPointer(HWND_DESKTOP,global.hptrDragColor);
    }
    else
	WinSetPointer(HWND_DESKTOP,
		  WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));


    if (global.fDraggingPelColor)
    {
	POINTL ptl, ptlFat;
	HPS hps;


	ptl.x = (LONG) LOUSHORT(mp1);
	ptl.y = (LONG) HIUSHORT(mp1);

	/* letting the point go negative causes overflow errors */
	if (ptl.x < 0)
	    ptl.x = 0;
	if (ptl.y < 0)
	    ptl.y = 0;

	GetFatPelFromPt(&ptl, &ptlFat);
	SetFatPel(&ptlFat, global.clrEditPel);

	hps = WinGetPS(hwnd);
	Paint(hps, OVERRIDE_RENDERED_OBJ);
	Paint(hps, IGNORED);	/* this call just copies fatpels to the screen */
	WinReleasePS(hps);
    }
    else if (global.fDraggingControlPoint)
    {
	ptl.x = (LONG) LOUSHORT(mp1);
	ptl.y = (LONG) HIUSHORT(mp1);

	/* letting the point go negative causes overflow errors */
	if (ptl.x < 0)
	    ptl.x = 0;
	if (ptl.y < 0)
	    ptl.y = 0;

	if (global.sPtGrabbed != NO_POINT)
	{
	    hps = WinGetPS(hwnd);
	    Paint(hps, OVERRIDE_RENDERED_OBJ);

	    global.pptl[global.sPtGrabbed] = ptl;

	    Paint(hps, CLEAR_FAT_BITMAP|RENDER_MATH_OBJ);
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
    SHORT i;
    HPS hps;


    if (global.fDraggingPelColor)
    {
	global.fDraggingPelColor = FALSE;
	WinSetCapture(HWND_DESKTOP, NULL);
    }
    else if (global.fDraggingControlPoint)
    {
	global.fDraggingControlPoint = FALSE;
	WinSetCapture(HWND_DESKTOP, NULL);
	if (global.sPtGrabbed != NO_POINT)
	{
	    if (usMsg == WM_BUTTON2UP)	/* remove point? */
	    {
		hps = WinGetPS(hwnd);
		Paint(hps, OVERRIDE_RENDERED_OBJ);

		/* squeeze out selected point */
		if ((i = global.sPtGrabbed) < (SHORT)(global.cptl-1))
		    while (i < (SHORT)(global.cptl-1))
		    {
			global.pptl[i] = global.pptl[i+1];
			++i;
		    }

		--global.cptl;
		global.sPtGrabbed = NO_POINT;

		Paint(hps, CLEAR_FAT_BITMAP|RENDER_MATH_OBJ);
		WinReleasePS(hps);
	    }
	    else    /* WM_BUTTON1UP */
		global.sPtGrabbed = NO_POINT;
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
    if (global.fDraggingPelSize)
    {
	POINTS pt;
	HPS hps;

	pt.x = LOUSHORT(mp1);
	pt.y = HIUSHORT(mp1);
	DragPelSize(hwnd, pt);
	global.fDraggingPelSize = FALSE;

	WinSetPointer(HWND_DESKTOP,
		      WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));

	hps = WinGetPS(hwnd);
	global.usMix = FM_OVERPAINT;
	Paint(hps, CLEAR_BACKGROUND|CLEAR_FAT_BITMAP|RENDER_MATH_OBJ);
	WinReleasePS(hps);
    }
    else if (global.fEditPelColors)
    {
	POINTL ptl, ptlFat;
	HPS hps;

	global.fDraggingPelColor = TRUE;
	WinSetCapture(HWND_DESKTOP, hwnd);

	ptl.x = (LONG) LOUSHORT(mp1);
	ptl.y = (LONG) HIUSHORT(mp1);

	if (global.usMix != FM_XOR)
	{
	    hps = WinGetPS(hwnd);
	    global.usMix = FM_XOR;
	    Paint(hps, CLEAR_BACKGROUND);
	    WinReleasePS(hps);
	}

	if (usMsg == WM_BUTTON1DOWN)
	    global.clrEditPel = global.clrRenderedObj;
	else
	    global.clrEditPel = global.clrField;

	GetFatPelFromPt(&ptl, &ptlFat);
	SetFatPel(&ptlFat, global.clrEditPel);

	hps = WinGetPS(hwnd);
	Paint(hps, OVERRIDE_RENDERED_OBJ);
	Paint(hps, IGNORED);	/* this call just copies fatpels to the screen */
	WinReleasePS(hps);
    }
    else if (!global.fDraggingControlPoint)
    {
	POINTL ptl;
	SHORT sNewPtGrabbed;
	HPS hps;

	global.fDraggingControlPoint = TRUE;
	WinSetCapture(HWND_DESKTOP, hwnd);

	ptl.x = (LONG) LOUSHORT(mp1);
	ptl.y = (LONG) HIUSHORT(mp1);

	sNewPtGrabbed = IsPtInList(&ptl);

	if (global.usMix != FM_XOR)
	{
	    hps = WinGetPS(hwnd);
	    global.usMix = FM_XOR;
	    Paint(hps, CLEAR_BACKGROUND);
	    WinReleasePS(hps);
	}

	if (usMsg == WM_BUTTON1DOWN)	/* add/move point? */
	{
	    hps = WinGetPS(hwnd);

	    if (sNewPtGrabbed != NO_POINT)
		global.sPtGrabbed = sNewPtGrabbed;
	    Paint(hps, OVERRIDE_RENDERED_OBJ);

	    if (sNewPtGrabbed == NO_POINT)
		global.sPtGrabbed = AddPtToList(&ptl);
	    else
		global.sPtGrabbed = sNewPtGrabbed;

	    Paint(hps, CLEAR_FAT_BITMAP|RENDER_MATH_OBJ);
	    WinReleasePS(hps);

	    if (global.sPtGrabbed == NO_POINT)
		MyMessageBox(global.hwnd, "Cannot add any more points.");
	}
	else if (sNewPtGrabbed != NO_POINT)
	    global.sPtGrabbed = sNewPtGrabbed;
    }
}




/************************************************************************
*
*   GetFatPelFromPt
*
************************************************************************/

VOID
GetFatPelFromPt(pptl, pptlFat)
PPOINTL pptl;
PPOINTL pptlFat;
{
    pptlFat->x = pptl->x / global.cxFatPel;
    pptlFat->y = pptl->y / global.cyFatPel;
}




/************************************************************************
*
*   SetFatPel
*
************************************************************************/

VOID
SetFatPel(pptl, clr)
PPOINTL pptl;
COLOR clr;
{
    LINEBUNDLE lb;

    if (global.hpsFat)
    {
	lb.lColor = clr;
	GpiSetAttrs(global.hpsFat, PRIM_LINE, LBB_COLOR, 0L, &lb);
	GpiSetPel(global.hpsFat, pptl);
    }
}




/************************************************************************
*
*   IsPtInList
*
************************************************************************/

SHORT
IsPtInList(pptl)
PPOINTL pptl;
{
    SHORT i;


    /* try to find pptl in the points we already have */
    for (i = 0; i < (SHORT)global.cptl; ++i)
	if (((L_ABS(pptl->x - global.pptl[i].x)) <= global.lHitPrecision) &&
	    ((L_ABS(pptl->y - global.pptl[i].y)) <= global.lHitPrecision))
		return i;

    /* couldn't find it */
    return NO_POINT;
}




/************************************************************************
*
*   AddPtToList
*
************************************************************************/

SHORT
AddPtToList(pptl)
PPOINTL pptl;
{
    SHORT i, j;

    if (global.cptl < CPTLMAX)
    {
	/* check for new points lying on a line segment */
	for (i = 0; i < (SHORT)(global.cptl-1L); ++i)
	    if (IsPtCloseToLine(&global.pptl[i], &global.pptl[i+1], pptl))
	    {
		/* insert point between endpoints of nearest line segment */
		for (j = (SHORT)global.cptl; j > i+1; --j)
		    global.pptl[j] = global.pptl[j - 1];
		global.pptl[i+1] = *pptl;
		++global.cptl;
		return i+1;
	    }

	/* append the point */

	i = (SHORT) global.cptl;
	global.pptl[i] = *pptl;
	++global.cptl;
	return i;
    }

    return NO_POINT;
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
    LONG dx, dy, yIntercept, error;
    LONG lBoxAdjustment;


    /* find the bounding box of the line segment */

    ptlLL = *pptl1;	/* assume line goes lower left to upper right */
    ptlUR = *pptl2;
    if (pptl1->x > pptl2->x)
	SwapLong(&ptlLL.x, &ptlUR.x);
    if (pptl1->y > pptl2->y)
	SwapLong(&ptlLL.y, &ptlUR.y);


    /* adjust the bounding box if it's too narrow */

    lBoxAdjustment = global.lHitPrecision/2L;

    dx = pptl2->x - pptl1->x;
    if (L_ABS(dx) <= global.lHitPrecision)
    {
	ptlLL.x -= lBoxAdjustment;
	ptlUR.x += lBoxAdjustment;
    }
    dy = pptl2->y - pptl1->y;
    if (L_ABS(dy) <= global.lHitPrecision)
    {
	ptlLL.y -= lBoxAdjustment;
	ptlUR.y += lBoxAdjustment;
    }


    /* see if the test point is in the bounding box of the line segment */

    if ((pptlTest->x >= ptlLL.x) &&
	(pptlTest->x <= ptlUR.x) &&
	(pptlTest->y >= ptlLL.y) &&
	(pptlTest->y <= ptlUR.y))
    {
	/* test for special cases */

	if (dx == 0)	/* vertical line */
	{
	    return (L_ABS(pptlTest->x - pptl1->x) <= global.lHitPrecision);
	}

	if (dy == 0)	/* horizontal line */
	{
	    return (L_ABS(pptlTest->y - pptl1->y) <= global.lHitPrecision);
	}


	/* test for general case */

	yIntercept = pptl1->y - (pptl1->x * dy) / dx;

	error = pptlTest->y - (pptlTest->x * dy / dx) - yIntercept;
	if (L_ABS(error) <= global.lHitPrecision)
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
*   DragPelSize
*
*   Set the dimensions of a fat pel by dragging a rectangle
*   on the screen.
*
************************************************************************/

VOID
DragPelSize(hwnd, pt)
HWND hwnd;
POINTS pt;
{
    TRACKINFO ti;

    WinSendMsg(global.hwndFrame, WM_QUERYTRACKINFO, (MPARAM)TF_MOVE, (MPARAM)&ti);

    ti.cxBorder   = 1;
    ti.cyBorder   = 1;
    ti.rclTrack.xLeft	= (LONG)pt.x;
    ti.rclTrack.yBottom = (LONG)pt.y;
    ti.rclTrack.xRight	= (LONG)pt.x;
    ti.rclTrack.yTop	= (LONG)pt.y;
    ti.fs = TF_RIGHT | TF_TOP;
    ti.ptlMinTrackSize.x = 1L;
    ti.ptlMinTrackSize.y = 1L;

    if (WinTrackRect(hwnd, NULL, &ti))
    {
	global.cxFatPel = (ti.rclTrack.xRight - ti.rclTrack.xLeft)  ;
	global.cyFatPel = (ti.rclTrack.yTop   - ti.rclTrack.yBottom);

	if (global.cxFatPel < 1L)
	    global.cxFatPel = 1L;

	if (global.cyFatPel < 1L)
	    global.cyFatPel = 1L;

	global.cxHalfFatPel = global.cxFatPel / 2L;
	global.cyHalfFatPel = global.cyFatPel / 2L;

	UpdateSurfaceDims();
    }
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
    if (global.hptrDragSize)
	WinDestroyPointer(global.hptrDragSize);
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
    BOOL fRedraw = FALSE;
    USHORT fsCmd = IGNORED;


    switch (id)
    {
    case IDM_SAVE:
	SaveWindowToFile(hwnd);
	break;

    case IDM_ABOUT:
	WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)AboutDlg, (HMODULE) NULL,
		       IDR_ABOUTDLG, NULL );
	break;

    case IDM_REDRAW:
	fsCmd = CLEAR_BACKGROUND|CLEAR_FAT_BITMAP|RENDER_MATH_OBJ;
	break;

    case IDM_SETPELSIZE:
	{
	    LONG cxFatPel, cyFatPel;

	    cxFatPel = global.cxFatPel;
	    cyFatPel = global.cyFatPel;

	    if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)PelSizeDlg, (HMODULE) NULL,
			   IDR_PELSIZEDLG, NULL ))
	    {
		if ((cxFatPel == global.cxFatPel) &&
		    (cyFatPel == global.cyFatPel))
		    fsCmd = CLEAR_BACKGROUND;
		else
		    fsCmd = CLEAR_BACKGROUND|CLEAR_FAT_BITMAP|RENDER_MATH_OBJ;
		fRedraw = TRUE;
	    }
	}
	break;

    case IDM_DRAGPELSIZE:
	global.fDraggingPelSize = TRUE;
	break;

    case IDM_RENDEREDOBJ:
	UPDATE_MENU_BOOL(global.fDisplayRenderedObj, IDM_RENDEREDOBJ);
	fsCmd = CLEAR_BACKGROUND|CLEAR_FAT_BITMAP|RENDER_MATH_OBJ;
	fRedraw = TRUE;
	break;

    case IDM_MATHOBJ:
	UPDATE_MENU_BOOL(global.fDisplayMathObj, IDM_MATHOBJ);
	fsCmd = CLEAR_BACKGROUND;
	fRedraw = TRUE;
	break;

    case IDM_CTLPOINTS:
	UPDATE_MENU_BOOL(global.fDisplayControlPoints, IDM_CTLPOINTS);
	fsCmd = CLEAR_BACKGROUND;
	fRedraw = TRUE;
	break;

    case IDM_CROSSHAIRS:
	UPDATE_MENU_BOOL(global.fDisplayCrossHairs, IDM_CROSSHAIRS);
	fsCmd = CLEAR_BACKGROUND;
	fRedraw = TRUE;
	break;

    case IDM_PELBORDER:
	UPDATE_MENU_BOOL(global.fDisplayPelBorder, IDM_PELBORDER);
	fsCmd = CLEAR_BACKGROUND;
	fRedraw = TRUE;
	break;

    case IDM_ROUNDPOINTS:
	UPDATE_MENU_BOOL(global.fRoundControlPoints, IDM_ROUNDPOINTS);
	fsCmd = CLEAR_BACKGROUND;
	fRedraw = TRUE;
	break;

    case IDM_AUTOREDRAW:
	UPDATE_MENU_BOOL(global.fAutoRedraw, IDM_AUTOREDRAW);
	break;

    case IDM_NOPRIM:
    case IDM_POLYLINE:
    case IDM_POLYFILLET:
    case IDM_POLYSPLINE:
    case IDM_POINTARC:
	UPDATE_MENU_LIST(global.usCurPrim, id);
	fsCmd = CLEAR_BACKGROUND|CLEAR_FAT_BITMAP|RENDER_MATH_OBJ;
	fRedraw = TRUE;
	break;

    case IDM_SETCOLORS:
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)ColorsDlg, (HMODULE) NULL,
		       IDR_COLORSDLG, NULL ))
	{
	    fsCmd = CLEAR_BACKGROUND|RENDER_MATH_OBJ;
	    fRedraw = TRUE;
	}
	break;

    case IDM_EDITPELCOLORS:
	UPDATE_MENU_BOOL(global.fEditPelColors, IDM_EDITPELCOLORS);
	break;

    case IDM_CLEARALL:
	global.cptl = 0L;
	fsCmd = CLEAR_BACKGROUND|CLEAR_FAT_BITMAP|RENDER_MATH_OBJ;
	fRedraw = TRUE;
	break;
    }

    if ((global.fAutoRedraw && fRedraw) || (id == IDM_REDRAW))
    {
	HPS hps;

	hps = WinGetPS(hwnd);
	global.usMix = FM_OVERPAINT;
	Paint(hps, fsCmd);
	WinReleasePS(hps);
    }
}




/************************************************************************
*
*   Paint
*
************************************************************************/

VOID
Paint(hps, fsCmd)
HPS  hps;
USHORT fsCmd;
{
    HRGN hrgn, hrgnClipOld, hrgnT;


    /* Clear the unused part of the client rectangle to a hatch pattern. */
    if (fsCmd & CLEAR_BACKGROUND)
	EraseBackground(hps);


    /* Set up the color mode as the user has requested */

    if (global.fRGB)
    {
	GpiCreateLogColorTable(hps, LCOL_RESET, LCOLF_RGB, 0L, 0L, NULL);
	if (global.hpsFat)
	{
	    GpiCreateLogColorTable(global.hpsFat, LCOL_RESET, LCOLF_RGB, 0L, 0L, NULL);
	    GpiCreateLogColorTable(global.hpsFatShadow, LCOL_RESET, LCOLF_RGB, 0L, 0L, NULL);
	}
    }
    else
	if (global.hpsFat)
	{
	    GpiCreateLogColorTable(global.hpsFat, LCOL_RESET, LCOLF_INDRGB, 0L, 0L, NULL);
	    GpiCreateLogColorTable(global.hpsFatShadow, LCOL_RESET, LCOLF_INDRGB, 0L, 0L, NULL);
	    global.clrBlackIndex = GpiQueryColorIndex(hps, 0L, 0x000000L);
	}


    if (global.usPelShape == IDD_CIRCLE)
    {
	ARCPARAMS arcp;

	arcp.lP = global.cxFatPel / 2L;
	arcp.lQ = global.cyFatPel / 2L;
	arcp.lR = 0L;
	arcp.lS = 0L;

	GpiSetArcParams(hps, &arcp);
    }


    /* set clipping rectangle to the fatbit surface */

    if ((hrgn = GpiCreateRegion(hps, 1L, &global.rcl)) != HRGN_ERROR)
	GpiSetClipRegion(hps, hrgn, &hrgnClipOld);


    if (fsCmd & CLEAR_BACKGROUND)
    {
	DrawGrid(hps);

	if (global.hpsFatShadow)
	{
	    AREABUNDLE ab;

	    /* clear shadow fatpel surface to background color */
	    ab.lColor = global.clrField;
	    GpiSetAttrs(global.hpsFatShadow, PRIM_AREA, ABB_COLOR, 0L, &ab);
	    GpiBitBlt(global.hpsFatShadow, NULL, 2L, (PPOINTL)&global.rclFat, ROP_PATCOPY,(ULONG) 0);
	}
    }

    if (global.fDisplayRenderedObj && !(fsCmd & OVERRIDE_RENDERED_OBJ))
	DisplayRenderedPels(hps, fsCmd);

    if (global.fDisplayControlPoints)
    {
	/* when rubberbanding with the rendered obj, newly drawn fatpels
	 * can wipe out stationary control point markers, so we have to
	 * redraw them all each time
	 */

	if (global.fDisplayRenderedObj || (fsCmd & CLEAR_BACKGROUND))
	    DisplayControlPoints(hps, global.cptl, global.pptl, global.usMix);
	else if (global.sPtGrabbed != NO_POINT)
	    /* draw just the control point that moved */
	    DisplayControlPoints(hps, 1L, global.pptl+global.sPtGrabbed, global.usMix);
    }

    if (global.fDisplayMathObj)
	DisplayMathematicalObject(hps, global.usMix);

    /* delete the clip region we set up */

    if (hrgnClipOld != HRGN_ERROR)
	GpiSetClipRegion(hps, hrgnClipOld, &hrgnT);
    if (hrgn != HRGN_ERROR)
	GpiDestroyRegion(hps, hrgn);
}




/************************************************************************
*
*   DisplayMathematicalObject
*
************************************************************************/

VOID
DisplayMathematicalObject(hps, usMix)
HPS hps;
USHORT usMix;
{
    PPOINTL pptl;
    LINEBUNDLE lb;

    if (global.cptl > 0)
    {
	if (global.fRoundControlPoints)
	{
	    RoundControlPoints(hps, global.cptl, global.pptl, global.pptlTmp,
			       global.cxFatPel, global.cyFatPel);
	    pptl = global.pptlTmp;
	}
	else
	    pptl = global.pptl;

	/* draw line */
	lb.lColor    = global.clrMathObj;
	lb.usMixMode = usMix;
	GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR|LBB_MIX_MODE, 0L, &lb);
	DrawPrimitive(hps, global.cptl, pptl);
    }
}




/************************************************************************
*
*   DisplayControlPoints
*
************************************************************************/

VOID
DisplayControlPoints(hps, cptl, pptl, usMix)
HPS hps;
LONG cptl;
PPOINTL pptl;
USHORT usMix;
{
    PPOINTL pptlT;
    MARKERBUNDLE mb;

    if (cptl > 0)
    {
	if (global.fRoundControlPoints)
	{
	    RoundControlPoints(hps, cptl, pptl, global.pptlTmp,
			       global.cxFatPel, global.cyFatPel);
	    pptlT = global.pptlTmp;
	}
	else
	    pptlT = pptl;


	mb.lColor    = global.clrControlPoints;
	mb.usMixMode = usMix;
	mb.usSymbol  = global.usControlPointSymbol;
	GpiSetAttrs(hps, PRIM_MARKER, MBB_COLOR|MBB_MIX_MODE|MBB_SYMBOL, 0L, &mb);

	GpiPolyMarker(hps, cptl, pptlT);
    }
}




/************************************************************************
*
*   EraseBackground
*
*   Erase the unused part of the window to a hatch pattern.
*
************************************************************************/

VOID
EraseBackground(hps)
HPS hps;
{
    RECTL rclClient, rclT;
    AREABUNDLE ab;


    WinQueryWindowRect(global.hwnd, &rclClient);

    ab.lColor	  = CLR_BLACK;
    ab.lBackColor = CLR_WHITE;
    ab.usSymbol   = PATSYM_DIAG1;
    GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR|ABB_BACK_COLOR|ABB_SYMBOL,
		0L, (PBUNDLE)&ab);

    if (global.rcl.yTop < rclClient.yTop)
    {
	rclT.xLeft   = rclClient.xLeft;
	rclT.yBottom = global.rcl.yBottom;
	rclT.xRight  = rclClient.xRight;
	rclT.yTop    = rclClient.yTop;
	GpiBitBlt(hps, NULL, 2L, (PPOINTL)&rclT, ROP_PATCOPY, (ULONG) 0);
    }

    if (global.rcl.xRight < rclClient.xRight)
    {
	rclT.xLeft   = global.rcl.xRight;
	rclT.yBottom = rclClient.yBottom;
	rclT.xRight  = rclClient.xRight;
	rclT.yTop    = global.rcl.yTop;
	GpiBitBlt(hps, NULL, 2L, (PPOINTL)&rclT, ROP_PATCOPY, (ULONG) 0);
    }

    ab.usSymbol   = PATSYM_SOLID;
    GpiSetAttrs(hps, PRIM_AREA, ABB_SYMBOL, 0L, (PBUNDLE)&ab);
}




/************************************************************************
*
*   DrawGrid
*
************************************************************************/

VOID
DrawGrid(hps)
HPS  hps;
{
    AREABUNDLE ab;
    POINTL ptl;
    POINTL aptl[3];


    /* clear fatpel surface to background color */
    ab.lColor = global.clrInterstice;
    GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0L, &ab);
    GpiBitBlt(hps, NULL, 2L, (PPOINTL)&global.rcl, ROP_PATCOPY, (ULONG) 0);


    /* draw one pel in lower left corner */

    ptl.x = global.cxFatPel / 2L;
    ptl.y = global.cyFatPel / 2L;
    DrawOneFatPel(hps, &ptl, global.clrField);


    /* blt up first column then across -- we don't have to worry
     * about the edges because a clip region has been setup to do that.
     */

    aptl[0].x = 0L;
    aptl[0].y = global.cyFatPel;
    aptl[1].x = global.cxFatPel;
    aptl[2].x = 0L;
    aptl[2].y = 0L;

    while (aptl[0].y <= global.rcl.yTop)
    {
	aptl[1].y  = aptl[0].y + aptl[0].y;
	GpiBitBlt(hps, hps, 3L, aptl, ROP_SRCCOPY, (LONG)NULL);
	aptl[0].y += aptl[1].y - aptl[0].y;
    }

    aptl[0].x = global.cxFatPel;
    aptl[0].y = 0L;
    aptl[1].y = global.rcl.yTop;
    aptl[2].x = 0L;
    aptl[2].y = 0L;

    while (aptl[0].x <= global.rcl.xRight)
    {
	aptl[1].x  = aptl[0].x + aptl[0].x;
	GpiBitBlt(hps, hps, 3L, aptl, ROP_SRCCOPY, (LONG)NULL);
	aptl[0].x += aptl[1].x - aptl[0].x;
    }
}




/************************************************************************
*
*   DisplayRenderedPels
*
************************************************************************/

VOID
DisplayRenderedPels(hps, fsCmd)
HPS hps;
USHORT fsCmd;
{
    LINEBUNDLE lb;
    AREABUNDLE ab;
    POINTL aptl[3];

    /*	Call GPI to draw the current primitive into the small bitmap,
     *	then fatbit it to the display.
     */

    if (global.hbmFat)
    {
	if (fsCmd & CLEAR_FAT_BITMAP)
	{
	    /* clear fatpel surface to background color */
	    ab.lColor = global.clrField;
	    GpiSetAttrs(global.hpsFat, PRIM_AREA, ABB_COLOR, 0L, &ab);
	    GpiBitBlt(global.hpsFat, NULL, 2L, (PPOINTL)&global.rclFat, ROP_PATCOPY, (ULONG) 0);
	}

	if (fsCmd & RENDER_MATH_OBJ)
	{
	    if (global.cptl > 0)
	    {
		/* draw line */
		lb.lColor = global.clrRenderedObj;
		GpiSetAttrs(global.hpsFat, PRIM_LINE, LBB_COLOR, 0L, &lb);
		GpiSetModelTransformMatrix(global.hpsFat, 9L,
					  &global.matlf, TRANSFORM_REPLACE);
		DrawPrimitive(global.hpsFat, global.cptl, global.pptl);
		GpiSetModelTransformMatrix(global.hpsFat, 0L, NULL, TRANSFORM_REPLACE);
	    }
	}

	/* xor the new rendered bitmap into the shadow bitmap */
	*((PRECTL)&aptl[0]) = global.rclFat;
	aptl[2].x = 0L;
	aptl[2].y = 0L;
	GpiBitBlt(global.hpsFatShadow, global.hpsFat, 3L, aptl, ROP_SRCINVERT, (ULONG) 0);

	/* fatbit object to the display */
	DrawFatPels(hps);

	/* get the new shadow bitmap */
	GpiBitBlt(global.hpsFatShadow, global.hpsFat, 3L, aptl, ROP_SRCCOPY, (ULONG) 0);
    }
}




/************************************************************************
*
*   DrawFatPels
*
************************************************************************/

VOID
DrawFatPels(hps)
HPS hps;
{
    POINTL ptl, ptlCenter;
    LONG i, j;
    COLOR clr;


    /* if the pel size is 1,1, then just blt the small bitmap to the
     * display.
     */

    if ((global.cxFatPel == 1L) && (global.cyFatPel == 1L))
    {
	POINTL aptl[3];

	*((PRECTL)&aptl[0]) = global.rcl;
	aptl[2].x = 0L;
	aptl[2].y = 0L;
	GpiBitBlt(hps, global.hpsFat, 3L, aptl, ROP_SRCCOPY, 0L);

	return;
    }

    for (i = 0; i < global.rclFat.xRight; ++i)
	for (j = 0; j < global.rclFat.yTop; ++j)
	{
	    ptl.x = i;
	    ptl.y = j;

	    clr = GpiQueryPel(global.hpsFatShadow, &ptl);
	    if ((global.fRGB && (clr != 0x000000L)) ||
	       (!global.fRGB && (clr != global.clrBlackIndex)))
	    {
		clr = GpiQueryPel(global.hpsFat, &ptl);
		ptlCenter.x = (i * global.cxFatPel) + global.cxHalfFatPel;
		ptlCenter.y = (j * global.cyFatPel) + global.cyHalfFatPel;
		DrawOneFatPel(hps, &ptlCenter, clr);
	    }
	}
}




/************************************************************************
*
*   DrawOneFatPel
*
************************************************************************/

VOID
DrawOneFatPel(hps, pptl, clr)
HPS hps;
PPOINTL pptl;
COLOR clr;
{
    POINTL ptl;
    LINEBUNDLE lb;
    AREABUNDLE ab;


    if (global.fDisplayPelBorder || global.fDisplayCrossHairs)
    {
	lb.lColor = global.clrCrossHair;
	GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR, 0L, &lb);
    }

    ab.lColor = clr;
    GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0L, &ab);


    switch (global.usPelShape)
    {
    case IDD_SQUARE:
	{
	    POINTL ptlT;
	    ULONG flCmd;

	    if (global.fDisplayPelBorder)
		flCmd = DRO_OUTLINEFILL;
	    else
		flCmd = DRO_FILL;

	    ptlT.x = pptl->x - global.cxHalfFatPel;
	    ptlT.y = pptl->y - global.cyHalfFatPel;
	    GpiSetCurrentPosition(hps, &ptlT);
	    ptlT.x = pptl->x + global.cxHalfFatPel;
	    ptlT.y = pptl->y + global.cyHalfFatPel;
	    GpiBox(hps, flCmd, &ptlT, 0L, 0L);
	}
	break;

    case IDD_DIAMOND:
	{
	    POINTL aptlT[4];
	    ULONG flCmd;

	    if (global.fDisplayPelBorder)
		flCmd = BA_BOUNDARY;
	    else
		flCmd = 0L;

	    aptlT[0].x = pptl->x;
	    aptlT[0].y = pptl->y - global.cyHalfFatPel;
	    aptlT[1].x = pptl->x - global.cxHalfFatPel;
	    aptlT[1].y = pptl->y;
	    aptlT[2].x = pptl->x;
	    aptlT[2].y = pptl->y + global.cyHalfFatPel;
	    aptlT[3].x = pptl->x + global.cxHalfFatPel;
	    aptlT[3].y = pptl->y;

	    GpiSetCurrentPosition(hps, &aptlT[3]);
	    GpiBeginArea(hps, flCmd);
	    GpiPolyLine(hps, 4L, aptlT);
	    GpiEndArea(hps);
	}

	break;

    case IDD_CIRCLE:
	{
	    ULONG flCmd;

	    if (global.fDisplayPelBorder)
		flCmd = DRO_OUTLINEFILL;
	    else
		flCmd = DRO_FILL;

	    GpiSetCurrentPosition(hps, pptl);
	    GpiFullArc(hps, flCmd, 0x10000L);
	}
	break;
    }


    if (global.fDisplayCrossHairs)
    {
	/* draw cross in center of pel */

	ptl.x = pptl->x - global.cxHalfFatPel;
	ptl.y = pptl->y;
	GpiSetCurrentPosition(hps, &ptl);
	ptl.x = pptl->x + global.cxHalfFatPel;
	GpiPolyLine(hps, 1L, &ptl);

	ptl.x = pptl->x;
	ptl.y = pptl->y - global.cyHalfFatPel;
	GpiSetCurrentPosition(hps, &ptl);
	ptl.y = pptl->y + global.cyHalfFatPel;
	GpiPolyLine(hps, 1L, &ptl);
    }
}




/************************************************************************
*
*   RoundControlPoints
*
************************************************************************/

VOID
RoundControlPoints(hps, cptl, pptl1, pptl2, cx, cy)
HPS hps;
LONG cptl;
PPOINTL pptl1;
PPOINTL pptl2;
LONG cx;
LONG cy;
{
    LONG cx2, cy2;
    LONG i;
    MATRIXLF matlf;


    /* copy the input buffer to the output/scratch buffer */
    for (i = 0; i < cptl; ++i)
	pptl2[i] = pptl1[i];


    /* set the transform, transform the points to device space (i.e. to
     * hpsFat dimensions), then restore the original transform
     */
    GpiQueryModelTransformMatrix(hps, 9L, &matlf);
    GpiSetModelTransformMatrix(hps, 9L, &global.matlf, TRANSFORM_REPLACE);
    GpiConvert(hps, CVTC_WORLD, CVTC_DEVICE, cptl, pptl2);
    GpiSetModelTransformMatrix(hps, 9L, &matlf, TRANSFORM_REPLACE);


    /* position each point in the center of its fatpel */

    cx2 = cx / 2L;
    cy2 = cy / 2L;

    for (i = 0; i < cptl; ++i, ++pptl2)
    {
	pptl2->x = pptl2->x * cx + cx2;
	pptl2->y = pptl2->y * cy + cy2;
    }
}




/************************************************************************
*
*   ComputeTransform
*
************************************************************************/

VOID
ComputeTransform(prcl1, prcl2)
PRECTL prcl1;
PRECTL prcl2;
{
    LONG xExt1, yExt1;
    LONG xExt2, yExt2;
    FIXED xScale, yScale;


    xExt1 = prcl1->xRight - prcl1->xLeft;
    yExt1 = prcl1->yTop   - prcl1->yBottom;
    xExt2 = prcl2->xRight - prcl2->xLeft;
    yExt2 = prcl2->yTop   - prcl2->yBottom;


    /* If the rectangles are of exactly the same dimensions, then
     * set the unity transform.  If not, compute the x and y scale
     * factors.  Note that in world coordinates rectangles are
     * inclusive-inclusive, whereas in device coordinates they are
     * inclusive-exclusive.  The extents of the destination are
     * therefore one pel too large as computed, so we subtract one
     * in the scale factor computation.
     */

    if (xExt1 == xExt2)
	xScale = 0x10000L;
    else
	xScale = ((xExt2-1L) * 0x10000L) / xExt1;

    if (yExt1 == yExt2)
	yScale = 0x10000L;
    else
	yScale = ((yExt2-1L) * 0x10000L) / yExt1;


    /* store the transform matrix for easy access */

    global.matlf.fxM11 = xScale;
    global.matlf.fxM12 = 0L;
    global.matlf. lM13 = 0L;
    global.matlf.fxM21 = 0L;
    global.matlf.fxM22 = yScale;
    global.matlf. lM23 = 0L;
    global.matlf. lM31 = 0L;
    global.matlf. lM32 = 0L;
    global.matlf. lM33 = 1L;
}




/************************************************************************
*
*   DrawPrimitive
*
************************************************************************/

VOID
DrawPrimitive(hps, cptl, pptl)
HPS hps;
LONG cptl;
PPOINTL pptl;
{
    switch (global.usCurPrim)
    {
    case IDM_NOPRIM:
	break;

    case IDM_POLYLINE:
	GpiSetCurrentPosition(hps, pptl);
	GpiPolyLine(hps, cptl-1L, pptl + 1);
	break;

    case IDM_POLYFILLET:
	if (cptl >= 3L)
	{
	    GpiSetCurrentPosition(hps, pptl);
	    GpiPolyFillet(hps, cptl-1L, pptl + 1);
	}
	break;

    case IDM_POLYSPLINE:
	if (cptl >= 4L)
	{
	    LONG cptSlack;    /* # points in pptl not usable by PolySpline */

	    cptSlack = ((cptl-1L) % 3) + 1;
	    GpiSetCurrentPosition( hps, pptl );
	    GpiPolySpline( hps, cptl-cptSlack, pptl+1 );
	}
	break;

    case IDM_POINTARC:
	if (cptl >= 3L)
	{
	    GpiSetCurrentPosition( hps, pptl );
	    GpiPointArc( hps, pptl+1 );
	}
	break;
    }
}




/************************************************************************
*
*   UpdateSurfaceDims
*
************************************************************************/

VOID
UpdateSurfaceDims()
{
    SIZEL size;
    BITMAPINFOHEADER bminfo;
    AREABUNDLE ab;


    WinQueryWindowRect(global.hwnd, &global.rcl);

    /* compute size of small surface */
    global.rclFat.xLeft   = 0L;
    global.rclFat.yBottom = 0L;
    global.rclFat.xRight  = global.rcl.xRight / global.cxFatPel;
    global.rclFat.yTop	  = global.rcl.yTop   / global.cyFatPel;

    /* compute size of fatpel version of small surface */
    global.rcl.xLeft   = 0L;
    global.rcl.yBottom = 0L;
    global.rcl.xRight  = global.rclFat.xRight * global.cxFatPel;
    global.rcl.yTop    = global.rclFat.yTop   * global.cyFatPel;

    ComputeTransform(&global.rcl, &global.rclFat);

    if ((global.rclFat.xRight <= global.rclFatBM.xRight) &&
	(global.rclFat.yTop   <= global.rclFatBM.yTop))
	return;



    /* The new fatbits surface doesn't fit in the bitmap, so we
     * have to make a new one.	If we don't have a DC or PS, make
     * those before making the bitmap.	If we do have a bitmap,
     * delete it before making the new one.
     */

    global.rclFatBM = global.rclFat;

    if (global.hbmFat)
    {
	GpiSetBitmap(global.hpsFat, NULL);
	GpiDeleteBitmap(global.hbmFat);
	GpiSetBitmap(global.hpsFatShadow, NULL);
	GpiDeleteBitmap(global.hbmFatShadow);
    }

    if (!global.hdcFat)
    {
	global.hdcFat = DevOpenDC(global.hab, OD_MEMORY, "*", 0L, NULL, NULL);
	if (!global.hdcFat)
	    goto usd_error;

	global.hdcFatShadow = DevOpenDC(global.hab, OD_MEMORY, "*", 0L, NULL, NULL);
	if (!global.hdcFatShadow)
	    goto usd_error;
    }

    if (!global.hpsFat)
    {
	size.cx = 0L;
	size.cy = 0L;
	global.hpsFat = GpiCreatePS(global.hab, global.hdcFat, &size,
				 PU_PELS|GPIT_MICRO|GPIA_ASSOC);
	if (!global.hpsFat)
	    goto usd_error;

	global.hpsFatShadow = GpiCreatePS(global.hab, global.hdcFatShadow, &size,
				 PU_PELS|GPIT_MICRO|GPIA_ASSOC);
	if (!global.hpsFatShadow)
	    goto usd_error;
    }

    /* create bitmap with maximum color resolution (24-bit color) */
    bminfo.cbFix = sizeof(BITMAPINFOHEADER);
    bminfo.cx = (USHORT) (global.rclFatBM.xRight - global.rclFatBM.xLeft);
    bminfo.cy = (USHORT) (global.rclFatBM.yTop	 - global.rclFatBM.yBottom);
    bminfo.cPlanes   = 1L;
    bminfo.cBitCount = 24L;
    global.hbmFat = GpiCreateBitmap(global.hpsFat, &bminfo, 0L, 0L, 0L);
    if (!global.hbmFat)
	goto usd_error;
    GpiSetBitmap(global.hpsFat, global.hbmFat);

    /* create a shadow bitmap of the one we just created */
    bminfo.cbFix = sizeof(BITMAPINFOHEADER);
    bminfo.cx = (USHORT) (global.rclFatBM.xRight - global.rclFatBM.xLeft);
    bminfo.cy = (USHORT) (global.rclFatBM.yTop	 - global.rclFatBM.yBottom);
    bminfo.cPlanes   = 1L;
    bminfo.cBitCount = 24L;
    global.hbmFatShadow = GpiCreateBitmap(global.hpsFatShadow, &bminfo, 0L, 0L, 0L);
    if (!global.hbmFat)
	goto usd_error;
    GpiSetBitmap(global.hpsFatShadow, global.hbmFatShadow);

    /* clear bitmap surface to field color */
    ab.lColor = global.clrField;
    GpiSetAttrs(global.hpsFat, PRIM_AREA, ABB_COLOR, 0L, &ab);
    GpiBitBlt(global.hpsFat, NULL, 2L, (PPOINTL)&global.rclFat, ROP_PATCOPY, (ULONG) 0);

    return;


/* error exit point */

usd_error:
    if (global.hpsFat)
	GpiDestroyPS(global.hpsFat);
    if (global.hpsFatShadow)
	GpiDestroyPS(global.hpsFatShadow);
    if (global.hdcFat)
	DevCloseDC(global.hdcFat);
    if (global.hdcFatShadow)
	DevCloseDC(global.hdcFatShadow);

    global.hpsFat	= NULL;
    global.hdcFat	= NULL;
    global.hpsFatShadow = NULL;
    global.hdcFatShadow = NULL;
}




/************************************************************************
*
*   AboutDlg
*
*   Process messages for the About box.
*
************************************************************************/

ULONG CALLBACK
AboutDlg(hwnd, usMsg, mp1, mp2)
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    switch (usMsg)
    {
    case WM_COMMAND:
	if (SHORT1FROMMP(mp1) == DID_OK)
	    WinDismissDlg(hwnd, TRUE);
	else
	    return FALSE;
	break;

    default:
	return (ULONG) WinDefDlgProc(hwnd, usMsg, mp1, mp2);
    }
    return FALSE;
}




/************************************************************************
*
*   PelSizeDlg
*
*   Process messages for the Pel Size dialog box.
*
************************************************************************/

ULONG CALLBACK
PelSizeDlg(hwnd, usMsg, mp1, mp2)
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:
	MySetWindowLong(hwnd, IDD_PELWIDTH,  global.cxFatPel);
	MySetWindowLong(hwnd, IDD_PELHEIGHT, global.cyFatPel);
	WinSendDlgItemMsg(hwnd, global.usPelShape,
			  BM_SETCHECK, (MPARAM)TRUE, 0L);
	return FALSE;
	break;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
	{
	case IDD_OK:
	    global.cxFatPel = MyGetWindowLong(hwnd, IDD_PELWIDTH);
	    global.cyFatPel = MyGetWindowLong(hwnd, IDD_PELHEIGHT);

	    if (global.cxFatPel < 1L)
		global.cxFatPel = 1L;

	    if (global.cyFatPel < 1L)
		global.cyFatPel = 1L;

	    global.cxHalfFatPel = global.cxFatPel / 2L;
	    global.cyHalfFatPel = global.cyFatPel / 2L;

	    global.usPelShape = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, IDD_SQUARE,
				   BM_QUERYCHECKINDEX, 0L, 0L) + IDD_SQUARE);


	    UpdateSurfaceDims();

	    fRet = TRUE;

	    /* fall through to some common code */

	case IDD_CANCEL:
	    WinDismissDlg(hwnd, fRet);
	    break;

	default:
	    return FALSE;
	}
	break;

    default:
	return (ULONG) WinDefDlgProc(hwnd, usMsg, mp1, mp2);
    }
    return FALSE;
}




/************************************************************************
*
*   ColorsDlg
*
*   Process messages for the Set Colors dialog box.
*
************************************************************************/

ULONG CALLBACK
ColorsDlg(hwnd, usMsg, mp1, mp2)
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;
    BOOL fRGB;
    COLOR clrMathObj;
    COLOR clrRenderedObj;
    COLOR clrField;
    COLOR clrCrossHair;
    COLOR clrInterstice;
    COLOR clrControlPoints;

    switch (usMsg)
    {
    case WM_INITDLG:
	if (global.fRGB)
	{
	    MySetWindowLongHex(hwnd, IDD_MATHOBJ,     global.clrMathObj);
	    MySetWindowLongHex(hwnd, IDD_RENDEREDOBJ, global.clrRenderedObj);
	    MySetWindowLongHex(hwnd, IDD_FIELD,	     global.clrField);
	    MySetWindowLongHex(hwnd, IDD_CROSSHAIRS,  global.clrCrossHair);
	    MySetWindowLongHex(hwnd, IDD_INTERSTICE,  global.clrInterstice);
	    MySetWindowLongHex(hwnd, IDD_CTLPOINTS,   global.clrControlPoints);
	}
	else
	{
	    MySetWindowLong   (hwnd, IDD_MATHOBJ,     global.clrMathObj);
	    MySetWindowLong   (hwnd, IDD_RENDEREDOBJ, global.clrRenderedObj);
	    MySetWindowLong   (hwnd, IDD_FIELD,	     global.clrField);
	    MySetWindowLong   (hwnd, IDD_CROSSHAIRS,  global.clrCrossHair);
	    MySetWindowLong   (hwnd, IDD_INTERSTICE,  global.clrInterstice);
	    MySetWindowLong   (hwnd, IDD_CTLPOINTS,   global.clrControlPoints);
	}
	WinSendDlgItemMsg(hwnd, IDD_RGB, BM_SETCHECK, MPFROM2SHORT(global.fRGB,0), 0L);
	return FALSE;
	break;

    case WM_CONTROL:
	if ((SHORT1FROMMP(mp1) == IDD_RGB) && (SHORT2FROMMP(mp1)== BN_CLICKED))
	{
	    fRGB = !SHORT1FROMMR(WinSendDlgItemMsg(hwnd, IDD_RGB, BM_QUERYCHECK, 0L, 0L));
	    WinSendDlgItemMsg(hwnd, IDD_RGB, BM_SETCHECK, MPFROM2SHORT(fRGB,0), 0L);

	    clrMathObj	     = MyGetWindowLong(hwnd, IDD_MATHOBJ);
	    clrRenderedObj   = MyGetWindowLong(hwnd, IDD_RENDEREDOBJ);
	    clrField	     = MyGetWindowLong(hwnd, IDD_FIELD);
	    clrCrossHair     = MyGetWindowLong(hwnd, IDD_CROSSHAIRS);
	    clrInterstice    = MyGetWindowLong(hwnd, IDD_INTERSTICE);
	    clrControlPoints = MyGetWindowLong(hwnd, IDD_CTLPOINTS);

	    if (fRGB)
	    {
		HPS hps;

		/* for each color, get rgb value from index */

		hps = WinGetPS(hwnd);

		clrMathObj	 = GpiQueryRGBColor(hps, 0L, clrMathObj);
		clrRenderedObj	 = GpiQueryRGBColor(hps, 0L, clrRenderedObj);
		clrField	 = GpiQueryRGBColor(hps, 0L, clrField);
		clrCrossHair	 = GpiQueryRGBColor(hps, 0L, clrCrossHair);
		clrInterstice	 = GpiQueryRGBColor(hps, 0L, clrInterstice);
		clrControlPoints = GpiQueryRGBColor(hps, 0L, clrControlPoints);

		WinReleasePS(hps);

		MySetWindowLongHex(hwnd, IDD_MATHOBJ,	 clrMathObj);
		MySetWindowLongHex(hwnd, IDD_RENDEREDOBJ, clrRenderedObj);
		MySetWindowLongHex(hwnd, IDD_FIELD,	 clrField);
		MySetWindowLongHex(hwnd, IDD_CROSSHAIRS,  clrCrossHair);
		MySetWindowLongHex(hwnd, IDD_INTERSTICE,  clrInterstice);
		MySetWindowLongHex(hwnd, IDD_CTLPOINTS,	 clrControlPoints);
	    }
	    else
	    {
		HPS hps;

		/* for each color, get nearest index value from rgb */

		hps = WinGetPS(hwnd);

		clrMathObj	 = GpiQueryColorIndex(hps, 0L, clrMathObj);
		clrRenderedObj	 = GpiQueryColorIndex(hps, 0L, clrRenderedObj);
		clrField	 = GpiQueryColorIndex(hps, 0L, clrField);
		clrCrossHair	 = GpiQueryColorIndex(hps, 0L, clrCrossHair);
		clrInterstice	 = GpiQueryColorIndex(hps, 0L, clrInterstice);
		clrControlPoints = GpiQueryColorIndex(hps, 0L, clrControlPoints);

		WinReleasePS(hps);

		MySetWindowLong   (hwnd, IDD_MATHOBJ,	 clrMathObj);
		MySetWindowLong   (hwnd, IDD_RENDEREDOBJ, clrRenderedObj);
		MySetWindowLong   (hwnd, IDD_FIELD,	 clrField);
		MySetWindowLong   (hwnd, IDD_CROSSHAIRS,  clrCrossHair);
		MySetWindowLong   (hwnd, IDD_INTERSTICE,  clrInterstice);
		MySetWindowLong   (hwnd, IDD_CTLPOINTS,	 clrControlPoints);
	    }
	}
	return (ULONG) WinDefDlgProc(hwnd, usMsg, mp1, mp2);
	break;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
	{
	case IDD_OK:
	    global.clrMathObj	    = MyGetWindowLong(hwnd, IDD_MATHOBJ);
	    global.clrRenderedObj   = MyGetWindowLong(hwnd, IDD_RENDEREDOBJ);
	    global.clrField	    = MyGetWindowLong(hwnd, IDD_FIELD);
	    global.clrCrossHair     = MyGetWindowLong(hwnd, IDD_CROSSHAIRS);
	    global.clrInterstice    = MyGetWindowLong(hwnd, IDD_INTERSTICE);
	    global.clrControlPoints = MyGetWindowLong(hwnd, IDD_CTLPOINTS);

	    global.fRGB = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, IDD_RGB, BM_QUERYCHECK, 0L, 0L));

	    fRet = TRUE;

	    /* fall through to some common code */

	case IDD_CANCEL:
	    WinDismissDlg(hwnd, fRet);
	    break;

	default:
	    return FALSE;
	}
	break;

    default:
	return (ULONG) WinDefDlgProc(hwnd, usMsg, mp1, mp2);
    }
    return FALSE;
}

	    


/************************************************************************
*
*   MySetWindowLong
*
*   Sets the given control id to the value specified.
*
************************************************************************/

VOID
MySetWindowLong(hWnd, id, num)
HWND hWnd;
USHORT id;
LONG num;
{
    char szStr[CCHSTR];

    sprintf((NPCH)szStr, "%ld", num);
    WinSetWindowText(WinWindowFromID(hWnd, id), (PCH)szStr);
}




/************************************************************************
*
*   MySetWindowLongHex
*
*   Sets the given control id to the value specified, in hexadecimal
*   notation.
*
************************************************************************/

VOID
MySetWindowLongHex(hWnd, id, num)
HWND hWnd;
USHORT id;
LONG num;
{
    char szStr[CCHSTR];

    sprintf((NPCH)szStr, "0x%06lX", num);
    WinSetWindowText(WinWindowFromID(hWnd, id), (PCH)szStr);
}




/************************************************************************
*
*   MyGetWindowLong
*
*   Returns the value from the given control id.
*
************************************************************************/

LONG
MyGetWindowLong(hWnd, id)
HWND hWnd;
USHORT id;
{
    char szStr[CCHSTR];
    LONG num;

    WinQueryWindowText(WinWindowFromID(hWnd, id), CCHSTR, (PCH)szStr);

    if (strchr(szStr, 'x'))
	sscanf((NPCH)szStr, "0x%lx", &num);
    else if (strchr(szStr, 'X'))
	sscanf((NPCH)szStr, "0X%lx", &num);
    else
	sscanf((NPCH)szStr, "%ld", &num);

    return num;
}




/************************************************************************
*
*   SaveWindowToFile
*
*   Copy the bits from the client rectangle (actually, just the fatpel
*   area) into a bitmap, then save that bitmap.
*
************************************************************************/

VOID
SaveWindowToFile(hwnd)
HWND hwnd;
{
    BITMAPINFOHEADER bminfo;
    HBITMAP hbm;
    HPS hps;
    POINTL aptl[3];

    /* create bitmap in display's favorite format */
    bminfo.cbFix = sizeof(BITMAPINFOHEADER);
    bminfo.cx = (USHORT) (global.rcl.xRight - global.rcl.xLeft);
    bminfo.cy = (USHORT) (global.rcl.yTop   - global.rcl.yBottom);
    bminfo.cPlanes   = 0L;
    bminfo.cBitCount = 0L;
    if (hbm = GpiCreateBitmap(global.hpsFat, &bminfo, 0L, 0L, 0L))
    {
	/* select it into the small bitmap's PS */
	GpiSetBitmap(global.hpsFat, hbm);

	/* GpiBitBlt from the window to the bitmap */
	hps = WinGetPS(hwnd);

	*((PRECTL)&aptl[0]) = global.rcl;
	aptl[2].x = 0L;
	aptl[2].y = 0L;
	GpiBitBlt(global.hpsFat, hps, 3L, aptl, ROP_SRCCOPY, 0L);

	WinReleasePS(hps);

	/* save the bitmap */
	WriteFile(hwnd, global.hpsFat);
    }

    /* deselect the bitmap and delete it */
    GpiSetBitmap(global.hpsFat, global.hbmFat);
    if (hbm)
	GpiDeleteBitmap(hbm);
}




/************************************************************************
*
*   WriteFile
*
*   Calls the OpenDlg's DlgFile function to ask the user what file name to
*   save under.
*
************************************************************************/

VOID
WriteFile(hwnd, hps)
HWND hwnd;
HPS hps;
{
    HFILE hfile;
    DLF dlf;
    BITMAPINFOHEADER bmih;

    dlf.rgbAction	= DLG_SAVEDLG;
    dlf.rgbFlags	= 0;
    dlf.phFile		= &hfile;
    dlf.pszExt		= "";
    dlf.pszAppName	= "FatPel";
    dlf.pszInstructions = NULL;
    dlf.szFileName[0]	= '\0';
    dlf.szOpenFile[0]	= '\0';
    dlf.pszTitle	= "Save Bitmap";


    switch (DlgFile(hwnd,&dlf))
    {
    case TDF_ERRMEM:
    case TDF_INVALID:
	MyMessageBox(hwnd, "Error opening file.");
	break;

    case TDF_NOSAVE:
	break;

    default:
	bmih.cbFix     = sizeof(BITMAPINFOHEADER);
	bmih.cx        = (USHORT) global.rcl.xRight;
	bmih.cy        = (USHORT) global.rcl.yTop;
	bmih.cPlanes   = 0L;
	bmih.cBitCount = 0L;

	if (!WriteBMP(hfile, hps, &bmih))
	    MyMessageBox(hwnd, "Error writing file.");
    }
}




/************************************************************************
*
*   WriteBMP
*
*   Write the bitmap out to a BMP format file.	Write the file
*   header first, then the bitmap bits.  Space for the header
*   and the bits is allocated.	Huge bitmaps are supported.
*   Free up memory and close the file before leaving.  The file
*   will have been opened by the time this function is called,
*   and the file handle will be in the *pdlf structure.
*
************************************************************************/

BOOL
WriteBMP(hfile, hps, pbmih)
HFILE hfile;
HPS hps;		 /* hps from which to get bitmap bits.	   */
PBITMAPINFOHEADER pbmih; /* Bitmap information. 		   */
{
    ULONG cScans;
    ULONG ulSize;	 /* Number of bytes occupied by bitmap bits.	     */
    USHORT cSegs;	 /* Number of 64K segments in ulSize.		     */
    USHORT cbExtra;	 /* Bytes in last segment of ulSize.		     */
    SEL selBits;	 /* Base selector to bitmap bits.		     */
    USHORT hugeshift;	 /* Segment index shift value.			     */
    USHORT cbBMHdr;	 /* Size of bitmap header.			     */
    PBITMAPFILEHEADER pbfh; /* Pointer to private copy of bitmap info data.	*/
    USHORT cbWrite1;	 /* Number of bytes to write first call to DosWrite  */
    USHORT cbWrite2;	 /* Number of bytes to write second call to DosWrite */
    USHORT cbWritten;	 /* Number of bytes written by DosWrite.	     */
    BOOL fRet = FALSE;	 /* Function return code.			     */
    int i;		 /* Generic loop index. 			     */
    struct
    {
	LONG cPlanes;
	LONG cBitCount;
    } bmFmt;


    /*******************************************************************
    * If the bitmap was created with either 0 planes or 0 bits per
    * pixel, then query the format to write with.  By asking for just
    * one format (two LONGs, or one instance of structure of bmFmt),
    * we'll get the device's favored format.
    *******************************************************************/

    if ((pbmih->cPlanes == 0) || (pbmih->cBitCount == 0))
    {
	if (!GpiQueryDeviceBitmapFormats(hps, 2L, (PLONG)&bmFmt))
	    goto lfwrite_error_close_file;
    }
    else
    {
	bmFmt.cPlanes	= pbmih->cPlanes;
	bmFmt.cBitCount = pbmih->cBitCount;
    }


    /*******************************************************************
    * Determine size of bitmap header.	The header consists of a
    * a fixed-size part and a variable-length color table.  The
    * latter has  2^cBitCount  entries, each of which is sizeof(RGB)
    * bytes long.  The exception is when cBitCount is 24, in which
    * case the color table is omitted because the pixels are direct
    * rgb values.
    *******************************************************************/

    i = (int) bmFmt.cBitCount;
    if (i == 24)
	cbBMHdr = 0;
    else
	for (cbBMHdr = sizeof(RGB); i > 0; --i)
	    cbBMHdr *= 2;
    cbBMHdr += sizeof(BITMAPFILEHEADER);


    /*******************************************************************
    * Copy structure from input to work buffer.  The call to
    * GpiQueryBitmapBits will have write-access to this, so we won't
    * let it have the user's data.
    *******************************************************************/

    pbfh = 0;
    if (DosAllocSeg(cbBMHdr, ((PUSHORT)&pbfh)+1, 0))
	goto lfwrite_error_close_file;
    pbfh->bmp = *pbmih;
    if ((pbmih->cPlanes == 0) || (pbmih->cBitCount))
    {
	pbfh->bmp.cPlanes   = (USHORT) bmFmt.cPlanes;
	pbfh->bmp.cBitCount = (USHORT) bmFmt.cBitCount;
    }


    /*******************************************************************
    * Allocate space for the bitmap bits -- all of them at once.
    * The extra ULONG casts are there to force all the arithmetic
    * to be done in 32 bits.
    *******************************************************************/

    ulSize = (
	       (
		 (
		   (ULONG)pbfh->bmp.cBitCount
		   * (ULONG)pbfh->bmp.cx
		   + 31L
		 ) / 32L
	       ) * (ULONG)pbfh->bmp.cPlanes * 4L
	     ) * (ULONG)pbfh->bmp.cy;

    cSegs   = (USHORT)(ulSize/0x10000L);
    cbExtra = (USHORT)(ulSize%0x10000L);
    if (DosAllocHuge(cSegs, cbExtra, (PSEL)&selBits, 0, 0))
	goto lfwrite_error_free_header;
    if (DosGetHugeShift(&hugeshift))
	goto lfwrite_error_free_bits;


    /*******************************************************************
    * Tell GPI to give us the bits. The function returns the number
    * of scan lines of the bitmap that were copied.  We want all of
    * them at once.
    *******************************************************************/

    cScans = GpiQueryBitmapBits( hps
			       , 0L
			       , (ULONG)pbfh->bmp.cy
			       , (PBYTE)MAKEP(selBits, 0)
			       , (PBITMAPINFO)&pbfh->bmp);
    if (cScans != pbfh->bmp.cy)  /* compare with original number of scans */
	goto lfwrite_error_free_bits;


    /*******************************************************************
    * Fill in the extra header fields and write the header out to
    * the file.
    *******************************************************************/

    pbfh->usType    = 0x4D42;		  /* 'MB' */
    pbfh->cbSize    = ulSize + cbBMHdr;
    pbfh->xHotspot  = pbfh->bmp.cx / 2;
    pbfh->yHotspot  = pbfh->bmp.cy / 2;
    pbfh->offBits   = cbBMHdr;

    if (DosWrite( hfile
		, (PVOID)pbfh
		, cbBMHdr
		, &cbWritten))
	goto lfwrite_error_free_bits;
    if (cbWritten != cbBMHdr)
	goto lfwrite_error_free_bits;


    /*******************************************************************
    * Write the bits out to the file. The DosWrite function allows a
    * maximum of 64K-1 bytes written at a time.  We get around this
    * by writing two 32K chunks for each 64K segment, and writing the
    * last segment in one piece.
    *******************************************************************/

    for (i = 0; i <= (SHORT) cSegs; ++i)
    {
	if (i < (SHORT) cSegs)
	{
	    /* This segment is 64K bytes long, so split it up. */
	    cbWrite1 = 0x8000;
	    cbWrite2 = 0x8000;
	}
	else
	{
	    /* This segment is less than 64K bytes long, so write it all. */
	    cbWrite1 = cbExtra;
	    cbWrite2 = 0;
	}

	/* There's a possibility that cbExtra will be 0, so check
	 * to avoid an unnecessary system call.
	 */
	if (cbWrite1 > 0)
	{
	    if (DosWrite( hfile
			, (PVOID)MAKEP(selBits+(i<<hugeshift), 0)
			, cbWrite1
			, &cbWritten))
		goto lfwrite_error_free_bits;
	    if (cbWrite1 != cbWritten)
		goto lfwrite_error_free_bits;
	}

	/* This will always be skipped on the last partial segment. */
	if (cbWrite2 > 0)
	{
	    if (DosWrite( hfile
			, (PVOID)MAKEP(selBits+(i<<hugeshift), cbWrite1)
			, cbWrite2
			, &cbWritten))
		goto lfwrite_error_free_bits;
	    if (cbWrite2 != cbWritten)
		goto lfwrite_error_free_bits;
	}
    }

    fRet = TRUE;     /* The bits are on the disk. */


    /*******************************************************************
    * Close the file, free the buffer space and leave.	This is a
    * common exit point from the function.  Since the same cleanup
    * operations need to be performed for such a large number of
    * possible error conditions, this is concise way to do the right
    * thing.
    *******************************************************************/

lfwrite_error_free_bits:
    DosFreeSeg(selBits);
lfwrite_error_free_header:
    DosFreeSeg(*((PUSHORT)&pbfh+1));
lfwrite_error_close_file:
    DosClose(hfile);
    return fRet;
}




/************************************************************************
*
*   MyMessageBox
*
*   Displays a message box with the given string.  To simplify matters,
*   the box will always have the same title ("FatPel"), will always
*   have a single button ("Ok"), will always have an exclamation point
*   icon, and will always be application modal.
*
************************************************************************/

VOID
MyMessageBox(hWnd, sz)
HWND hWnd;
PSZ sz;
{
    static char *szTitle = "FatPel Application";

    WinMessageBox(HWND_DESKTOP, hWnd, sz, szTitle, 0,
		  MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
}
