/************************************************************************
*
*   lfthread.c -- Subroutines for thread management for LineFractal.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSSEMAPHORES
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <os2.h>

#include <mt\process.h>
#include <mt\stddef.h>

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFCMD
#define INCL_LFMAIN
#define INCL_LFPS
#define INCL_LFUTIL
#define INCL_LFTHREAD
#define INCL_LFDRAW
#include "lffuncs.h"




/************************************************************************
*
*   Global Variables
*
************************************************************************/

extern GLOBALDATA global;




/************************************************************************
*
*   LfStartThread
*
*   Do everything needed to start up a background thread drawing
*   fractals.
*
*   This includes:
*
*	Allocate space for thread structure.
*	Initialize fields of thread structure.
*	Create a presentation space for the thread.
*	Create stack space for the thread.
*	Set the thread's semaphore to block -- i.e. don't start
*	    drawing until everything is ready.
*	Start the thread.
*	Set the thread's priority to idle time.
*
************************************************************************/

#define SIZE_ACCUM_STACK	2048

PTHR
LfStartThread(dcType)
USHORT dcType;
{
    PTHR pthr;


    pthr = NULL;	/* zero out all 32 bits */
    if (DosAllocSeg(sizeof(THR), ((PUSHORT)&pthr)+1, 0))
	return NULL;

    pthr->dcType = dcType;

    LfPreInitThread(pthr);
    if (LfCreatePS(pthr))
    {
	LfPostInitThread(pthr);
	if (DosAllocSeg(SIZE_ACCUM_STACK, &pthr->selStack, 0))
	    goto lfst_exit;

	DosSemSet(&pthr->lSemRedraw);
	pthr->tid = _beginthread(LineFractalThread, MAKEP(pthr->selStack, 0), SIZE_ACCUM_STACK, pthr);
	if (pthr->tid == -1)
	{
	    LfDeletePS(pthr);
	    goto lfst_exit;
	}
	DosSetPrty(PRTYS_THREAD, PRTYC_IDLETIME, 0, pthr->tid);

	LfSelectXform(global.hwnd, pthr, pthr->usCurXform);
	DosSemClear(&pthr->lSemRedraw);

	if (!DevQueryCaps(pthr->hdc, CAPS_HORIZONTAL_RESOLUTION, 2L, (PLONG)&(pthr->AspectRatio)))
	{
	    pthr->AspectRatio.lHorz = 1L;
	    pthr->AspectRatio.lVert = 1L;
	}

	return pthr;
    }
lfst_exit:
    DosFreeSeg(*(((PUSHORT)&pthr)+1));
    return NULL;
}




/************************************************************************
*
*   LfPreInitThread
*
*   Initialize constant values in the thread structure needed from
*   the moment the thread starts.  If thread inheritance is enabled,
*   then copy as much as possible from the top thread, if there is
*   one.
*
************************************************************************/

VOID
LfPreInitThread(pthr)
PTHR pthr;
{
    pthr->hps		= NULL;
    pthr->hdc		= NULL;
    pthr->hbm		= NULL;
    pthr->fTimeToDie	= FALSE;
    pthr->fUpdateAttrs	= FALSE;
    pthr->fBusy 	= FALSE;
    pthr->pptl		= NULL;
    pthr->cptl		= 0;
    pthr->pmatlf	= NULL;

    if (global.fThreadInheritance && global.pThrTop)
    {
	pthr->cPlanes	       = global.pThrTop->cPlanes	  ;
	pthr->cBitCount        = global.pThrTop->cBitCount	  ;

	pthr->cxWCS	       = global.pThrTop->cxWCS		  ;
	pthr->cyWCS	       = global.pThrTop->cyWCS		  ;

	pthr->fAutoSizePS      = global.pThrTop->fAutoSizePS	  ;
	pthr->fAutoSelectDims  = global.pThrTop->fAutoSelectDims  ;
	pthr->fClearOnRedraw   = global.pThrTop->fClearOnRedraw   ;
	pthr->fAutoStartRedraw = global.pThrTop->fAutoStartRedraw ;
	pthr->fCollectBounds   = global.pThrTop->fCollectBounds   ;

	pthr->fFracRedraw      = global.pThrTop->fFracRedraw	  ;
	pthr->fPrimRedraw      = global.pThrTop->fPrimRedraw	  ;
	pthr->fAttrRedraw      = global.pThrTop->fAttrRedraw	  ;

	pthr->lb	       = global.pThrTop->lb		  ;
	pthr->flLineAttrs      = LFA_LINEALL			  ;
	pthr->mb	       = global.pThrTop->mb		  ;
	pthr->flMarkerAttrs    = LFA_MARKALL			  ;
	pthr->ab	       = global.pThrTop->ab		  ;
	pthr->flAreaAttrs      = LFA_AREAALL			  ;
	pthr->ib	       = global.pThrTop->ib		  ;
	pthr->flImageAttrs     = LFA_IMAGEALL			  ;

	pthr->usCurPrim        = global.pThrTop->usCurPrim	  ;
	pthr->usCurXform       = global.pThrTop->usCurXform	  ;
	pthr->usRecursion      = global.pThrTop->usRecursion	  ;
	pthr->usPolygonSides   = global.pThrTop->usPolygonSides   ;
	pthr->cptMax	       = global.pThrTop->cptMax 	  ;
	pthr->dblXOff	       = global.pThrTop->dblXOff	  ;
	pthr->dblYOff	       = global.pThrTop->dblYOff	  ;
	pthr->dblXScale        = global.pThrTop->dblXScale	  ;
	pthr->dblYScale        = global.pThrTop->dblYScale	  ;
	pthr->dblRotation      = global.pThrTop->dblRotation	  ;
	pthr->flMiscAttrs      = LFA_MISCALL			  ;
    }
    else
    {
	pthr->cPlanes	       = global.bm.cPlanes;
	pthr->cBitCount        = global.bm.cBitCount;

	pthr->cxWCS	       = 10000L;
	pthr->cyWCS	       = 10000L;

	pthr->fAutoSizePS      = TRUE;
	pthr->fAutoSelectDims  = TRUE;
	pthr->fClearOnRedraw   = TRUE;
	pthr->fAutoStartRedraw = FALSE;
	pthr->fCollectBounds   = TRUE;

	pthr->fFracRedraw      = TRUE;
	pthr->fPrimRedraw      = TRUE;
	pthr->fAttrRedraw      = TRUE;

	pthr->lb.lColor        = CLR_NEUTRAL;
	pthr->lb.usMixMode     = FM_OVERPAINT;
	pthr->lb.fxWidth       = LINEWIDTH_NORMAL;
;	 pthr->lb.lGeomWidth	= 1L;
	pthr->lb.usType        = LINETYPE_SOLID;
;	 pthr->lb.usEnd 	= LINEEND_FLAT;
;	 pthr->lb.usJoin	= LINEJOIN_BEVEL;
	pthr->flLineAttrs      = LFA_LINEALL;

	pthr->mb.lColor        = CLR_NEUTRAL;
	pthr->mb.lBackColor    = CLR_BACKGROUND;
	pthr->mb.usMixMode     = FM_OVERPAINT;
	pthr->mb.usBackMixMode = BM_LEAVEALONE;
	pthr->mb.usSet	       = LCID_DEFAULT;
	pthr->mb.usSymbol      = MARKSYM_DIAMOND;
;	 pthr->mb.sizfxCell.cx	= 0L;
;	 pthr->mb.sizfxCell.cy	= 0L;
	pthr->flMarkerAttrs    = LFA_MARKALL;

	pthr->ab.lColor        = CLR_NEUTRAL;
	pthr->ab.lBackColor    = CLR_BACKGROUND;
	pthr->ab.usMixMode     = FM_OVERPAINT;
	pthr->ab.usBackMixMode = BM_OVERPAINT;
	pthr->ab.usSet	       = LCID_DEFAULT;
	pthr->ab.usSymbol      = PATSYM_NOSHADE;
;	 pthr->ab.ptlRefPoint.x = 0L;
;	 pthr->ab.ptlRefPoint.y = 0L;
	pthr->flAreaAttrs      = LFA_AREAALL;

	pthr->ib.lColor        = CLR_NEUTRAL;
	pthr->ib.lBackColor    = CLR_BACKGROUND;
	pthr->ib.usMixMode     = FM_OVERPAINT;
	pthr->ib.usBackMixMode = FM_OVERPAINT;
	pthr->flImageAttrs     = LFA_IMAGEALL;

	pthr->usCurPrim        = IDM_POLYLINE;
	pthr->usCurXform       = IDM_SAWTOOTH;
	pthr->usRecursion      = 1;
	pthr->usPolygonSides   = 3;
	pthr->cptMax	       = MAX_POINT_COUNT;
	pthr->dblXOff	       = 0.125;
	pthr->dblYOff	       = 0.5;
	pthr->dblXScale        = 0.75;
	pthr->dblYScale        = 0.75;
	pthr->dblRotation      = 0.0;
	pthr->flMiscAttrs      = LFA_MISCALL;
    }

}




/************************************************************************
*
*   LfPostInitThread
*
*   Initialize various values in the thread structure which required
*   that a PS/DC/BM had been created.  If thread inheritance is enabled,
*   then copy as much as possible from the top thread, if there is
*   one.
*
************************************************************************/

VOID
LfPostInitThread(pthr)
PTHR pthr;
{
    if (!(global.fThreadInheritance && global.pThrTop))
    {
	pthr->lb.lGeomWidth = GpiQueryLineWidthGeom(pthr->hps);
	pthr->lb.usEnd	    = (USHORT) GpiQueryLineEnd(pthr->hps);
	pthr->lb.usJoin     = (USHORT) GpiQueryLineJoin(pthr->hps);

	DevQueryCaps(pthr->hdc, CAPS_MARKER_WIDTH,  1L, (PLONG)&(pthr->mb.sizfxCell.cx));
	DevQueryCaps(pthr->hdc, CAPS_MARKER_HEIGHT, 1L, (PLONG)&(pthr->mb.sizfxCell.cy));

	GpiQueryPatternRefPoint(pthr->hps,&(pthr->ab.ptlRefPoint));
    }
}




/************************************************************************
*
*   LfKillThread
*
*   Force the given thread to die and release its resources.
*
************************************************************************/

VOID
LfKillThread(pthr)
PTHR pthr;
{
    int i, j;


    /* This does not do anything if there aren't any threads. */

    if (global.cThr && pthr)
    {
	/****************************************************************
	*  Tell the thread to die when it can.
	*  If blocked on lSemRedraw, then it will recognize fTimeToDie
	*  as soon as it's unblocked.  If it's currently drawing,
	*  it will recognize fInterrupted, kick out of the recursion,
	*  loop back up to check lSemRedraw, which will be clear, so
	*  it'll do as already mentioned.  Set the priority to time-
	*  critical to speed things up.
	****************************************************************/

	pthr->fTimeToDie   = TRUE;
	pthr->fInterrupted = TRUE;
	DosSetPrty(PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, pthr->tid);
	DosSemClear(&pthr->lSemRedraw);


	/****************************************************************
	*  Once it's decided it must die, it will clean up after
	*  itself and exit quietly.  To detect when it has actually
	*  exited, so that we can safely free up the stack, we will
	*  loop asking for the thread's priority until an error occurs.
	*  We will assume that an error implies the thread has exited.
	****************************************************************/

	{
	    USHORT usT;
	    USHORT usRet;
	    while (!(usRet = DosGetPrty(PRTYS_THREAD, &usT, pthr->tid)));
	    if (usRet != ERROR_INVALID_THREADID)
	    {
		MyMessageBox(global.hwnd,
		    "Cannot kill thread");
		return;
	    }
	}


	/****************************************************************
	*  Extract the given thread from the group. This is done
	*  inside a critical section because the drawing threads
	*  call LfIsThreadTop, which reads global.aThr.
	****************************************************************/

	i = 0;
	while ((i < global.cThr) && (pthr != global.aThr[i]))
	    ++i;    /* if i == global.cThr we have a problem */
	DosEnterCritSec();
	for (j = i; j < global.cThr-1; ++j)
	    global.aThr[j] = global.aThr[j+1];
	global.aThr[j] = NULL;	/* clear out all copies of pthr */
	--global.cThr;
	DosExitCritSec();


	/****************************************************************
	*  Free up resources allocated for the thread.
	****************************************************************/

	LfDeletePS(pthr);
	DosFreeSeg(pthr->selStack);
	DosFreeSeg(*(((PUSHORT)&pthr)+1));
    }
}




/************************************************************************
*
*   LfBringThreadToTop
*
*   Bring the specified thread to the top, and invalidate the window
*   to force a WM_PAINT message to make the thread's image visible.
*   The given thread can be NULL, in which case just set pThrTop to
*   NULL.
*
************************************************************************/

VOID
LfBringThreadToTop(pthr)
PTHR pthr;
{
    int i;


    /********************************************************************
    *  Find the given thread in the group.
    ********************************************************************/

    if (pthr)
    {
	i = 0;
	while ((i < global.cThr) && (pthr != global.aThr[i]))
	    ++i;    /* if i == global.cThr, we have problems */
    }
    else
	i = 32767; /* should cause gp fault if used */


    /********************************************************************
    *  If the user wants the menu items updated, and we're switching
    *  away from an active thread, then uncheck the current thread's
    *  settings.
    ********************************************************************/

    if (global.fUpdateMenusOnThreadSwitch)
    {
	if (global.pThrTop)
	{
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_AUTORESIZE);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_AUTOSELECTDIMS);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_CLEARONREDRAW);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_AUTOSTARTREDRAW);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_COLLECTBOUNDS);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_FRACREDRAW);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_PRIMREDRAW);
	    UNCHECK_MENU_ITEM(global.hwndFrame, IDM_ATTRREDRAW);
	    UNCHECK_MENU_ITEM(global.hwndFrame, global.pThrTop->usCurPrim);
	    UNCHECK_MENU_ITEM(global.hwndFrame, global.pThrTop->usCurXform);
	    UNCHECK_MENU_ITEM(global.hwndFrame, global.pThrTop->dcType);
	}
    }


    /********************************************************************
    *  Switch the given thread to the top.
    ********************************************************************/

    DosEnterCritSec();
    global.iThrTop = i;
    global.pThrTop = pthr;
    DosExitCritSec();


    /********************************************************************
    *  If we switched in a living thread, then invalidate the entire
    *  window.	This will cause a WM_PAINT message to be sent, which
    *  will display the image from the new top thread.	If the user
    *  has not disabled updating of the menus, then do that.
    ********************************************************************/

    if (pthr)
    {
	WinInvalidateRect(global.hwnd, NULL, FALSE);

	/* If the user wants menu items updated, then do it now. */
	if (global.fUpdateMenusOnThreadSwitch)
	{
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_AUTORESIZE,	   pthr->fAutoSizePS);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_AUTOSELECTDIMS,  pthr->fAutoSelectDims);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_CLEARONREDRAW,   pthr->fClearOnRedraw);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_AUTOSTARTREDRAW, pthr->fAutoStartRedraw);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_COLLECTBOUNDS,   pthr->fCollectBounds);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_FRACREDRAW,	   pthr->fFracRedraw);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_PRIMREDRAW,	   pthr->fPrimRedraw);
	    TOGGLE_MENU_ITEM(global.hwndFrame, IDM_ATTRREDRAW,	   pthr->fAttrRedraw);
	    CHECK_MENU_ITEM (global.hwndFrame, pthr->usCurPrim);
	    CHECK_MENU_ITEM (global.hwndFrame, pthr->usCurXform);
	    CHECK_MENU_ITEM (global.hwndFrame, pthr->dcType);

	    LfUpdateThreadMenu();
	}
    }
}




/************************************************************************
*
*   LfIsThreadTop
*
*   Returns TRUE if the given thread is the top one.  Putting this
*   test in a function isolates it and makes it easier to maintain
*   a critical section.
*
************************************************************************/

BOOL
LfIsThreadTop(pthr)
PTHR pthr;
{
    BOOL f;

    f = FALSE;
    DosEnterCritSec();
    if (pthr == global.pThrTop)
	f = TRUE;
    DosExitCritSec();

    return f;
}




/************************************************************************
*
*   LfUpdateThreadMenu
*
*   Brings the "Thread Bring thread to top" submenu in line with the
*   currently active threads.
*
************************************************************************/

VOID
LfUpdateThreadMenu()
{
    /*
     * Enable the numbers for threads that do exist.
     * Disable the numbers for threads that don't exist.
     * Uncheck everything, then check the right one.
     */

    int i;

    for (i = 0; i < global.cThr; ++i)
    {
	UNCHECK_MENU_ITEM(global.hwndFrame, IDM_THR0TOTOP+i);
	ENABLE_MENU_ITEM (global.hwndFrame, IDM_THR0TOTOP+i);
    }

    for (     ; i < MAX_THREADS; ++i)
    {
	UNCHECK_MENU_ITEM(global.hwndFrame, IDM_THR0TOTOP+i);
	DISABLE_MENU_ITEM(global.hwndFrame, IDM_THR0TOTOP+i);
    }

    CHECK_MENU_ITEM(global.hwndFrame, IDM_THR0TOTOP + global.iThrTop);
}
