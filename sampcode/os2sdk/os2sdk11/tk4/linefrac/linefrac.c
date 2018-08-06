/************************************************************************
*
*   linefrac.c -- Main window procedure for LineFractal window class.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSSEMAPHORES
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include <os2.h>
#include <mt\stdlib.h>

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFMAIN
#define INCL_LFINIT
#define INCL_LFTHREAD
#define INCL_LFPS
#define INCL_LFCMD
#define INCL_LFDRAW
#include "lffuncs.h"




/************************************************************************
*
*   Global Variables
*
************************************************************************/

GLOBALDATA global;




/************************************************************************
*
*   main
*
*   WinInitialize resizes our ring 2 stack, among other things, so
*   we won't GP fault trying to do graphics.  WinCreateMsgQueue defines
*   us as a REAL PM app. (as does the WINDOWAPI in the .DEF file).
*   Call a subroutine to register our window class and create a window.
*   Loop over messages.  Exit cleanly.
*
************************************************************************/

VOID cdecl
main( VOID )
{
    QMSG qMsg;
    int iRet = 0;

    global.hab	 = WinInitialize(NULL);
    global.hMsgQ = WinCreateMsgQueue(global.hab, 0);

    if (LfInitApp())
	while (WinGetMsg( global.hab, (PQMSG)&qMsg, (HWND)NULL, 0, 0 ))
	    WinDispatchMsg( global.hab, (PQMSG)&qMsg );
    else
	iRet = -1;

    WinDestroyWindow( global.hwndFrame );
    WinDestroyMsgQueue( global.hMsgQ );
    WinTerminate( global.hab );
    DosExit(EXIT_PROCESS, iRet);
}




/************************************************************************
*
*   LineFracWndProc
*
*   Process messages for the LineFractal window class.
*
************************************************************************/

ULONG CALLBACK
LineFracWndProc( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM  mp1;
MPARAM  mp2;
{
    HPS     hps;
    USHORT  iNewTop;
    int     i;
    PTHR pthr;
    RECTL rcl;
    BOOL  fIsTimerUsed;

    switch (usMsg)
    {
    case WM_CREATE:
	if ((global.hptr)[global.usCurPtr])
	    WinSetPointer(HWND_DESKTOP,(global.hptr)[global.usCurPtr]);
	break;

    case WM_CLOSE:
	LfClose(hwnd);
	break;

    case WM_COMMAND:
	LfCommand(hwnd, LOUSHORT(mp1));
	break;

    case WM_TIMER:
	if (LOUSHORT(mp1) == IDT_AUTOSTARTREDRAW)
	{
	    fIsTimerUsed = FALSE;
	    for (i = 0; i < global.cThr; ++i)
		if (global.aThr[i]->fAutoStartRedraw)
		{
		    DosSemClear(&(global.aThr[i])->lSemRedraw);
		    fIsTimerUsed = TRUE;
		}
	    if (!fIsTimerUsed)
		LfStopRedrawTimer();
	    else
	    {
		LfStopRedrawTimer();
		LfStartRedrawTimer();
	    }
	}
	else if (LOUSHORT(mp1) == IDT_AUTOSWITCH)
	{
	    DosEnterCritSec();
	    iNewTop = (global.iThrTop + 1) % global.cThr;
	    DosExitCritSec();
	    LfBringThreadToTop(global.aThr[iNewTop]);
	}
	else
	    goto pass_it_on;
	break;

    case WM_ERASEBACKGROUND:
	LfEraseBackground(hwnd, NULL, NULL, NULL);
	return FALSE;
	break;

    case WM_PAINT:
	if (!global.pThrTop)
	    WinSendMsg(hwnd, WM_COMMAND, (MPARAM)IDM_DCMEMORY, 0);

	hps = WinBeginPaint(global.hwnd, NULL, &rcl);
	if (global.pThrTop)   /* only try to paint if we have a drawing */
	    LfPaint(hps, &rcl);
	WinEndPaint(hps);
	break;

    case WM_CHAR:
	if (LOUSHORT(mp1) & KC_CHAR)
	    LfChar(hwnd, (char)mp2);
	break;

    case WM_BUTTON1DOWN:
    case WM_BUTTON2DOWN:
	LfButtonDown(hwnd, mp1);
	WinSetFocus(HWND_DESKTOP, hwnd);
	break;

    case WM_BUTTON1UP:
    case WM_BUTTON2UP:
	LfButtonUp(usMsg);
	break;

    case WM_MOUSEMOVE:
	LfMouseMove();
	break;

    case WM_SIZE:
	/* Resize each PS that has fAutoSizePS set to TRUE. */

	WinQueryWindowRect(global.hwnd, &global.rcl);
	global.bm.cx = (USHORT) (global.rcl.xRight - global.rcl.xLeft);
	global.bm.cy = (USHORT) (global.rcl.yTop   - global.rcl.yBottom);

	for (i = 0; i < global.cThr; ++i)
	    if (pthr = global.aThr[i])
		if (pthr->hps)
		    if (pthr->fAutoSizePS)
		    {
			global.bm.cPlanes   = pthr->cPlanes;
			global.bm.cBitCount = pthr->cBitCount;

			if (LfResizePS(pthr))
			    DosSemClear(&pthr->lSemRedraw);
		    }

	/* fall through -- we might want to restart point accumulation,
	   but don't want to process the resize message */

    default:
pass_it_on:
	return( (ULONG)WinDefWindowProc(hwnd, usMsg, mp1, mp2));
	break;
    }

    return FALSE;
}




/************************************************************************
*
*   LfClose
*
*   Kill all the threads still running and delete all our fancy pointers.
*   In general, prepare to terminate the program.
*
************************************************************************/

VOID
LfClose(hwnd)
HWND hwnd;
{
    int i;

    WinSendMsg(hwnd, WM_COMMAND, (MPARAM)IDM_KILLALLTHREADS, 0);
    WinSetPointer(HWND_DESKTOP,
		  WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));
    for (i = 0; i < NUM_PTR_SHAPES; ++i)
	if ((global.hptr)[i])
	    WinDestroyPointer((global.hptr)[i]);
    if (global.hptrTrack)
	WinDestroyPointer(global.hptrTrack);
    if (global.hptrPaste)
	WinDestroyPointer(global.hptrPaste);
    WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
}




/************************************************************************
*
*   LfEraseBackground
*
*   Erase the window background to a hatch pattern.  This enables the
*   user to see where the bitmap's edges are in case it's smaller
*   than the window.
*
************************************************************************/

VOID
LfEraseBackground(hwnd, hpsCaller, prclUpdate, prclX)
HWND hwnd;
HPS hpsCaller;
PRECTL prclUpdate;
PRECTL prclX;	/* excluded rectangle */
{
    HPS hps;
    RECTL rcl;
    RECTL rclT;
    AREABUNDLE ab;
    HRGN hrgnClipOld;
    HRGN hrgn;
    HRGN hrgnT;


    if (hpsCaller)
	hps = hpsCaller;
    else
	hps = WinGetPS(hwnd);

    if (prclUpdate)
	rcl = *prclUpdate;
    else
	WinQueryUpdateRect(hwnd, (PRECTL)&rcl);

    ab.lColor	  = CLR_BLACK;
    ab.lBackColor = CLR_WHITE;
    ab.usSymbol   = PATSYM_DIAG1;
    GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR|ABB_BACK_COLOR|ABB_SYMBOL,
		0L, (PBUNDLE)&ab);


    /* Make a region out of the nearest rectangle, then copy the real
     * region data into it.  Set this as our clip rectangle.
     */
    if ((hrgn = GpiCreateRegion(hps, 1L, &rcl)) != HRGN_ERROR)
    {
	WinQueryUpdateRegion(hwnd, hrgn);
	GpiSetClipRegion(hps, hrgn, &hrgnClipOld);
    }

    if (!prclX)
	GpiBitBlt(hps, NULL, 2L, (PPOINTL)&rcl, ROP_PATCOPY, NULL);
    else
    {
	if (prclX->yTop < rcl.yTop)
	{
	    rclT.xLeft	 = rcl.xLeft;
	    rclT.yBottom = prclX->yBottom;
	    rclT.xRight  = rcl.xRight;
	    rclT.yTop	 = rcl.yTop;
	    GpiBitBlt(hps, NULL, 2L, (PPOINTL)&rclT, ROP_PATCOPY, NULL);
	}

	if (prclX->xRight < rcl.xRight)
	{
	    rclT.xLeft	 = prclX->xRight;
	    rclT.yBottom = rcl.yBottom;
	    rclT.xRight  = rcl.xRight;
	    rclT.yTop	 = prclX->yTop;
	    GpiBitBlt(hps, NULL, 2L, (PPOINTL)&rclT, ROP_PATCOPY, NULL);
	}
    }

    if (hrgnClipOld != HRGN_ERROR)
	GpiSetClipRegion(hps, hrgnClipOld, &hrgnT);
    if (hrgn != HRGN_ERROR)
	GpiDestroyRegion(hps, hrgn);

    if (!hpsCaller)
	WinReleasePS(hps);
}




/************************************************************************
*
*   LfPaint
*
*   If we have a bitmap, blt it to the screen, no matter what state
*   it's in.  If the selection rectangle is still alive, then display
*   it, too.  Note that it goes directly to the screen, so we have to
*   redraw it each time the bitmap is thrown back up.
*
************************************************************************/

VOID
LfPaint(hps, prcl)
HPS  hps;
PRECTL prcl;
{
    POINTL  aptl[4];

    if (global.pThrTop)
    {
	switch (global.pThrTop->dcType)
	{
	case IDM_DCDIRECT:	/* all drawing is already on the screen */
	    break;

	case IDM_DCPOSTSCRIPT:
	case IDM_DCPROPRINTER:
	    break;

	case IDM_DCMEMORY:

	    GpiSetAttrs(hps, PRIM_IMAGE, IBB_COLOR|IBB_BACK_COLOR, 0L, &global.pThrTop->ib);

	    aptl[0].x = 0L;
	    aptl[0].y = 0L;
	    aptl[1].x = (global.pThrTop->rcl).xRight;
	    aptl[1].y = (global.pThrTop->rcl).yTop;
	    aptl[2].x = 0L;
	    aptl[2].y = 0L;

	    GpiBitBlt(hps, global.pThrTop->hps, 3L, aptl, ROP_SRCCOPY, (LONG)NULL);
	    LfEraseBackground(global.hwnd, hps, prcl, &(global.pThrTop->rcl));
	    break;

	default:
	    break;
	}
	if (global.fShowSelectRc)
	    LfShowSelectRc(hps, global.fTempSelect ? &global.rclSelect : &global.rclCutCopy);
    }
}




/************************************************************************
*
*   LfShowSelectRc
*
*   Draw the selection rectangle in the given presentation space.  The
*   rectangle is drawn in xor-mode so this can be called to remove it
*   as well as show it.
*
************************************************************************/

VOID
LfShowSelectRc(hps, lprc)
HPS hps;
PRECTL lprc;
{
    LINEBUNDLE lb;


    lb.lColor	 = CLR_TRUE;
    lb.usMixMode = FM_XOR;
    lb.usType	 = LINETYPE_ALTERNATE;
    GpiSetAttrs(hps, PRIM_LINE, LBB_COLOR|LBB_MIX_MODE|LBB_TYPE,
		0L, (PBUNDLE)&lb);
    GpiSetCurrentPosition(hps, (POINTL *)&(lprc->xLeft));
    GpiBox(hps, DRO_OUTLINE, (POINTL *)&(lprc->xRight), 0L, 0L);
}




/************************************************************************
*
*   LfChar
*
*   Handle LineFractal's keyboard interface.  This consists of:
*
*   <space>	Cancels the selection rectangle.
*   0-9 	Brings thread i to the top, if it exists.
*
************************************************************************/

VOID
LfChar(hwnd, ch)
HWND hwnd;
char ch;
{
    HPS hps;
    PRECTL lprc;
    int i;

    if (ch == ' ')
    {
	if (global.fShowSelectRc)
	{
	    global.fShowSelectRc = FALSE;
	    hps = WinGetPS(hwnd);
	    if (global.fTempSelect)
	    {
		lprc = &global.rclSelect;
		global.fTempSelect = FALSE;
	    }
	    else
		lprc = &global.rclCutCopy;
	    LfShowSelectRc(hps, lprc);
	    WinReleasePS(hps);
	}
    }
    else if (ch >= '0' && ch <= '9')
    {
	i = ch - '0';
	if (i < global.cThr)
	    LfBringThreadToTop(global.aThr[i]);
    }
}




/************************************************************************
*
*   LfMouseMove
*
*   Handle actions necessary upon each move of the mouse pointer.
*   This consists of resetting the mouse pointer -- if we pass
*   this on to WinDefWindowProc, it will reset it to the arrow.
*   As long as we're setting the pointer so often, we can do a
*   little animation.
*
************************************************************************/

VOID
LfMouseMove()
{
    /* Do this so that if in tracking mode, the correct pointer
       shape appears immediately. */
    if (global.fTracking || global.fSelecting)
    {
	if (global.hptrTrack)
	    WinSetPointer(HWND_DESKTOP,global.hptrTrack);
	return;
    }
    if (global.fPasting)
    {
	if (global.hptrPaste)
	    WinSetPointer(HWND_DESKTOP,global.hptrPaste);
	return;
    }
    if (global.pThrTop)
    {
	 if (global.pThrTop->fBusy)
	 {
	    if (global.hptrWait)
		WinSetPointer(HWND_DESKTOP,global.hptrWait);
	    return;
	 }
    }
    global.usPtrCounter = (global.usPtrCounter+1) % global.usPtrThreshold;
    if (global.usPtrCounter == 0)
    {
	global.usCurPtr += global.usPtrIncr;
	if (global.usCurPtr <= 0)
	    global.usCurPtr = 0;
	else
	    global.usCurPtr %= NUM_PTR_SHAPES;
	if ((global.usCurPtr == NUM_PTR_SHAPES - 1) ||
	    (global.usCurPtr == 0))
	    global.usPtrIncr *= -1;
    }
    if ((global.hptr)[global.usCurPtr])
	WinSetPointer(HWND_DESKTOP,(global.hptr)[global.usCurPtr]);
}




/************************************************************************
*
*   LfButtonUp
*
*   Handle up clicks of the mouse buttons.  This consists of:
*
*   left button up	increase the depth of recursion
*   right button up	decrease the depth of recursion
*
*			In both cases, clear the semaphore so the
*			drawing can restart at the new level of
*			recursion.
*
************************************************************************/

VOID
LfButtonUp(usMsg)
USHORT usMsg;
{
    if (global.fMouseChangesRecursion)
    {
	if (usMsg == WM_BUTTON1UP)
	    global.pThrTop->usRecursion = ++global.pThrTop->usRecursion;
	else if (global.pThrTop->usRecursion > 0)
	    global.pThrTop->usRecursion = --global.pThrTop->usRecursion;

	global.pThrTop->flMiscAttrs |= LFA_RECURSION;
	global.pThrTop->fUpdateAttrs = TRUE;
	global.fUpdateAttrs = TRUE;

	if (global.pThrTop->hps)
	{
	    global.pThrTop->fInterrupted = TRUE;
	    DosSemClear(&(global.pThrTop)->lSemRedraw);
	}
    }
}




/************************************************************************
*
*   LfButtonDown
*
*   Handle down clicks of the mouse buttons.  This consists of
*   changing the mouse pointer depending upon which mode we're in
*   and calling off to the subroutine to do the real work given
*   the position at which the mouse button was clicked.  The modes
*   are "tracking", "selecting", and "pasting".  Tracking means the
*   user is defining the new dimensions of the fractal.  Selecting
*   means the user is dragging a rectangle to cut or copy.  Pasting
*   means the user is positioning the cut or copied rectangle in
*   the window.
*
************************************************************************/

VOID
LfButtonDown(hwnd, mp1)
HWND hwnd;
MPARAM mp1;
{
    POINTS pt;

    if (global.fTracking)
    {
	if (global.hptrTrack)
	    WinSetPointer(HWND_DESKTOP,global.hptrTrack);
	pt.x = LOUSHORT(mp1);
	pt.y = HIUSHORT(mp1);
	LfSelectDimension(hwnd, pt);
	global.fTracking = FALSE;
	if ((global.hptr)[global.usCurPtr])
	    WinSetPointer(HWND_DESKTOP,(global.hptr)[global.usCurPtr]);
	if (global.pThrTop->fAttrRedraw)
	{
	    global.pThrTop->fInterrupted = TRUE;
	    DosSemClear(&(global.pThrTop)->lSemRedraw);
	}
    }
    else if (global.fSelecting)
    {
	if (global.hptrTrack)
	    WinSetPointer(HWND_DESKTOP,global.hptrTrack);
	pt.x = LOUSHORT(mp1);
	pt.y = HIUSHORT(mp1);
	LfSelect(hwnd, pt);
	global.fSelecting = FALSE;
	if ((global.hptr)[global.usCurPtr])
	    WinSetPointer(HWND_DESKTOP,(global.hptr)[global.usCurPtr]);
    }
    else if (global.fPasting)
    {
	if (global.hptrPaste)
	    WinSetPointer(HWND_DESKTOP,global.hptrPaste);
	pt.x = LOUSHORT(mp1);
	pt.y = HIUSHORT(mp1);
	LfPaste(hwnd);
	global.fPasting = FALSE;
	if ((global.hptr)[global.usCurPtr])
	    WinSetPointer(HWND_DESKTOP,(global.hptr)[global.usCurPtr]);
    }
}




/************************************************************************
*
*   LfStartRedrawTimer
*
************************************************************************/

VOID
LfStartRedrawTimer()
{
    USHORT timeout;

    if (!global.fTimerOn)
    {
	timeout = (USHORT) ((rand()/32767.0) *
		  (global.usMaxTimerDelay  - global.usMinTimerDelay) +
		   global.usMinTimerDelay);
	WinStartTimer(global.hab, global.hwnd, IDT_AUTOSTARTREDRAW, timeout);
	global.fTimerOn = TRUE;
    }
}




/************************************************************************
*
*   LfStopRedrawTimer
*
************************************************************************/

VOID
LfStopRedrawTimer()
{
    WinStopTimer(global.hab, global.hwnd, IDT_AUTOSTARTREDRAW);
    global.fTimerOn = FALSE;
}
