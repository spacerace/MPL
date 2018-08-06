/************************************************************************
*
*   lfdraw.c -- This file contains "line fractal" drawing routines
*
*   Created by Microsoft Corporation, 1989
*
*
*   Background fractal drawing scheme:
*
*	To enable the user to interact with the system during
*	the drawing of a complicated fractal, the fractal is
*	drawn into a bitmap by a background thread.  This thread
*	is started at the first WM_PAINT message received by the
*	application, and never terminates.
*
*	The thread's execution is controlled by a semaphore,
*	lSemRedraw.  The thread is initially blocked
*	by its semaphore.  When something changes in the environ-
*	ment such that the fractal is to be redrawn, or drawn for
*	the first time, the semaphore is cleared and the thread
*	is off and running.  Note that the environment must be
*	set up BEFORE the semaphore is cleared, otherwise the
*	fractal may be partially drawn with old parameters.
*	The thread automatically resets the semaphore, so that
*	as soon as it's done drawing, it has to wait for the signal
*	to start again.
*
*	The fractal is drawn in batches of up to 8196 points per polyline.
*	After each polyline is drawn, the background thread invalidates
*	the main client rectangle to force a WM_PAINT message to be sent.
*	All the paint procedure does is copy the bitmap, whatever
*	it's current state, to the screen.  The user therefore
*	sees bursts of n points at a time as his fractal is drawn.
*
*	The semaphore is controlled, in greater detail, as follows:
*
*	    Disable background drawing (set semaphore):
*
*		LfInit
*		    Don't let second thread start working until a
*		    transform has been defined.
*
*		LineFractalThread
*		    Don't start the next one until the user asks for it.
*
*	    Enable background drawing (clear semaphore):
*
*		WM_BUTTON1UP
*		WM_BUTTON2UP
*		WM_SIZE & fAutoSizePS
*		    The level of recursion or dimensions of bitmap have
*		    changed, so redraw the fractal.
*
*		Change of fractal
*		Change of drawing primitive
*		Change of attributes
*		    If the corresponding redraw flag is enabled for one
*		    of these events, then the semaphore is cleared.
*
*
*	 Event					  LineFractalThread
*	 -----					  -----------------
*
*    WM_BUTTON1UP				  --------<--------
*    WM_BUTTON2UP				 /		   \
*    WM_SIZE & fAutoSizePS			|		    |
*    Change of fractal/primitive/attributes	|		    |
*	   |					|		    |
*	   |					V		    |
*	   +---------clear----------> +----------------------+	    |
*				      |     lSemRedraw	     |	    |
*	   +----------set-----------> +----------------------+	    |
*	   |					|		    |
*	   |			      if semaphore is clear	    ^
*    initialization				|		    |
* done with current fractal			V		    |
*				      +----------------------+	    |
*				      |    Draw fractal      |	    |
*		       WM_PAINT <---- |    into bitmap	     |	    |
*				      +----------------------+	    |
*						|		    |
*						|		    |
*						 \ 		   /
*						  -------->--------
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS

#include <os2.h>
#include <mt\math.h>

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFTHREAD
#define INCL_LFDRAW
#include "lffuncs.h"




/************************************************************************
*
*   Global Variables
*
************************************************************************/

extern GLOBALDATA global;
extern XFORMDATA  aXform[];




/************************************************************************
*
*   LineFractalThread
*
*   Organize the drawing of the fractal.  Runs in an independent
*   thread to accumulate the points of the fractal, then calls
*   LfDraw to draw with the selected primitive onto the surface in
*   batches of a size selected by the user.  If this thread is the
*   top, then all or part of the client rectangle is invalidated
*   after drawing to force a WM_PAINT message.	When the paint message
*   is processed, the image will be copied to the display.
*
*   This function is entered via _beginthread, which takes care of
*   putting the parameter on the stack.
*
************************************************************************/

VOID FAR cdecl
LineFractalThread(pthr)
PTHR pthr;
{
    HAB hab;
    int cFracSegs;
    ULONG cptReq;
    BOOL fCached;
    BOOL fCacheable;
    BOOL fModelXformsValid;
    PLINEFRAC pXform;	       /* linked list of fractal segments */


    hab = WinInitialize(NULL);	      /* initialize ring 2 stack for thread */

    fModelXformsValid = FALSE;
    fCached	      = FALSE;
    fCacheable	      = FALSE;

    pthr->pptl	      = NULL;
    pthr->pmatlf      = NULL;

    while (!pthr->fTimeToDie)
    {
	/****************************************************************
	*
	*  Clear the busy flag to indicate we're at the semaphore.
	*  If we happen to be the top thread, then force the pointer
	*  to be what we expect to see (if we didn't do this, it
	*  might stay as an hour glass until the user moves the mouse).
	*
	****************************************************************/

	pthr->fBusy = FALSE;
	if (LfIsThreadTop(pthr))
	    if ((global.hptr)[global.usCurPtr])
		WinSetPointer(HWND_DESKTOP,(global.hptr)[global.usCurPtr]);


	/****************************************************************
	*
	*  Wait for permission to redraw.
	*  See if we're supposed to exit.  If not, clear the suicide
	*  flag, and set the busy flag.  If we're the top thread, then
	*  set the pointer to an hour glass to let the user know we're
	*  working on something.
	*
	****************************************************************/

	DosSemRequest(&pthr->lSemRedraw, -1L);

	if (pthr->fTimeToDie)
	    goto lfthread_exit;
	pthr->fInterrupted = FALSE;
	pthr->fBusy	   = TRUE;
	if (LfIsThreadTop(pthr))
	    if (global.hptrWait)
		WinSetPointer(HWND_DESKTOP,global.hptrWait);

	/****************************************************************
	*
	*  Check for changes of attributes.  If anything has changed,
	*  this subroutine copies the new stuff over within a critical
	*  section.
	*
	****************************************************************/

	LfUpdateAttrs(pthr);


	/****************************************************************
	*
	*  Check the buffers for points and model transforms.  If we
	*  don't have them, or the appropriate attributes have changed
	*  such that the ones we have are invalid, allocate for them.
	*
	****************************************************************/

	if ((pthr->pptl == NULL) || (pthr->flMiscAttrs & LFA_CPTMAX))
	{
	    if (pthr->pptl != NULL)
		DosFreeSeg(*(((PUSHORT)&pthr->pptl)+1));
	    if (DosAllocSeg(pthr->cptMax * sizeof(POINTL),
			   ((PUSHORT)&pthr->pptl)+1, 0))
		goto lfthread_exit;
	}

	if ((pthr->pmatlf == NULL) || (pthr->flMiscAttrs & LFA_POLYGONSIDES))
	{
	    if (pthr->pmatlf != NULL)
		DosFreeSeg(*(((PUSHORT)&pthr->pmatlf)+1));
	    if (DosAllocSeg(pthr->usPolygonSides * sizeof(MATRIXLF),
			   ((PUSHORT)&pthr->pmatlf)+1, 0))
		goto lfthread_exit;
	    fModelXformsValid = FALSE;
	}


	/****************************************************************
	*
	*  See if we can cache the whole lot of points.  This depends
	*  on the fractal, the size of the point cache, and the level
	*  of recursion.  Note that we must execute this code the first
	*  time through, or fCacheable will be undefined.  We're sure
	*  to come here, though, because threads are initialized with
	*  all attributes "changed".
	*
	****************************************************************/

	if (pthr->flMiscAttrs & (LFA_CPTMAX | LFA_RECURSION | LFA_CURXFORM))
	{
	    if (pthr->flMiscAttrs & LFA_CURXFORM)
	    {
		PLINEFRAC p;

		cFracSegs = 0;
		pXform = aXform[pthr->usCurXform - IDM_SHARKTOOTH].pXform;
		p = pXform;
		while (p != EOLIST)
		{
		    ++cFracSegs;
		    p = p->next;
		}
	    }

	    cptReq = (ULONG) exp((double)pthr->usRecursion *
				 log((double)cFracSegs));

	    if ((ULONG)pthr->cptMax > cptReq)
		fCacheable = TRUE;
	    else
		fCacheable = FALSE;

	    fCached = FALSE;
	}


	/****************************************************************
	*
	*  If the model transforms are invalid, then recompute them.
	*  Check first to see if any attributes have changed that
	*  would invalidate the transforms.
	*
	****************************************************************/

	if (fModelXformsValid)
	    if (pthr->flMiscAttrs & (LFA_ROTATION | LFA_POLYGONSIDES |
		   LFA_XSCALE | LFA_YSCALE | LFA_XOFF | LFA_YOFF))
		fModelXformsValid = FALSE;

	if (!fModelXformsValid)
	{
	    LfComputeModelXforms(pthr);
	    fModelXformsValid = TRUE;
	}


	/****************************************************************
	*
	*  Clear the change-of-attributes flags that have been examined
	*  by the time we get here.
	*
	****************************************************************/

	pthr->flMiscAttrs &=
	  ~(
	     LFA_CPTMAX    | LFA_RECURSION | LFA_POLYGONSIDES | LFA_CURXFORM  |
	     LFA_XSCALE    | LFA_YSCALE    | LFA_XOFF	      | LFA_YOFF      |
	     LFA_ROTATION
	   );


	/****************************************************************
	*
	*  Clear the surface if fClearOnRedraw is enabled.
	*  If the points are cached, then redraw straight from the cache.
	*  Otherwise, anchor the fractal at the left endpoint of the
	*  unit interval, then draw it to the specified depth of recursion.
	*  If we are able to cache all the points, then nothing will have
	*  been drawn when LineFractal returns, so draw the fractal without
	*  flushing the cache.	If we were not able to cache all the
	*  points, and the buffer is not empty, flush the last batch.
	*
	****************************************************************/

	if (pthr->fClearOnRedraw)
	    LfClearRect(pthr, NULL);

	if (fCached)
	    LfDraw(pthr, FALSE);
	else
	{
	    pthr->x    =  0.0;
	    pthr->y    =  0.0;

	    pthr->cptl = 0L;
	    LfAddPoint(pthr);

	    LineFractal(pthr, pthr->usRecursion, (double)pthr->cxWCS,
			0.0, FALSE, pXform);

	    if (!pthr->fInterrupted)
	    {
		if (fCacheable)
		{
		    LfDraw(pthr, FALSE);
		    fCached = TRUE;
		}
		else
		{
		    fCached = FALSE;
		    if (pthr->cptl > 1L)
			LfDraw(pthr, TRUE);
		}
	    }
	}
    }


    /****************************************************************
    *
    *  Common exit point for thread.  Free up memory allocated
    *  by this thread.
    *
    ****************************************************************/

lfthread_exit:

    if (pthr->pmatlf != NULL)
	DosFreeSeg(*(((PUSHORT)&pthr->pmatlf)+1));
    if (pthr->pptl != NULL)
	DosFreeSeg(*(((PUSHORT)&pthr->pptl)+1));
}




/************************************************************************
*
*   LfUpdateAttrs
*
*   Update any changed attributes from the global attributes.
*
************************************************************************/

VOID
LfUpdateAttrs(pthr)
PTHR pthr;
{

    DosEnterCritSec();
    if (pthr->fUpdateAttrs)
    {
	if (global.flLineAttrs	 & LFA_LINEALL)
	{
	    pthr->lb		 = global.lb;
	    pthr->flLineAttrs	|= global.flLineAttrs;
	    global.flLineAttrs	 = 0L;
	}
	if (global.flMarkerAttrs & LFA_MARKALL)
	{
	    pthr->mb		 = global.mb;
	    pthr->flMarkerAttrs |= global.flMarkerAttrs;
	    global.flMarkerAttrs = 0L;
	}
	if (global.flAreaAttrs	 & LFA_AREAALL)
	{
	    pthr->ab		 = global.ab;
	    pthr->flAreaAttrs	|= global.flAreaAttrs;
	    global.flAreaAttrs	 = 0L;
	}
	if (global.flImageAttrs  & LFA_IMAGEALL)
	{
	    pthr->ib		 = global.ib;
	    pthr->flImageAttrs	|= global.flImageAttrs;
	    global.flImageAttrs  = 0L;
	}
	if (global.flMiscAttrs	 & LFA_MISCALL)
	{
	    if (global.flMiscAttrs & LFA_CURPRIM)
		pthr->usCurPrim      = global.usCurPrim;
	    if (global.flMiscAttrs & LFA_CURXFORM)
		pthr->usCurXform     = global.usCurXform;
	    if (global.flMiscAttrs & LFA_RECURSION)
		pthr->usRecursion    = global.usRecursion;
	    if (global.flMiscAttrs & LFA_POLYGONSIDES)
		pthr->usPolygonSides = global.usPolygonSides;
	    if (global.flMiscAttrs & LFA_CPTMAX)
		pthr->cptMax	     = global.cptMax;
	    if (global.flMiscAttrs & LFA_XOFF)
		pthr->dblXOff	     = global.dblXOff;
	    if (global.flMiscAttrs & LFA_YOFF)
		pthr->dblYOff	     = global.dblYOff;
	    if (global.flMiscAttrs & LFA_XSCALE)
		pthr->dblXScale      = global.dblXScale;
	    if (global.flMiscAttrs & LFA_YSCALE)
		pthr->dblYScale      = global.dblYScale;
	    if (global.flMiscAttrs & LFA_ROTATION)
		pthr->dblRotation    = global.dblRotation;
	    if (global.flMiscAttrs & LFA_CXWCS)
		pthr->cxWCS	     = global.cxWCS;
	    if (global.flMiscAttrs & LFA_CYWCS)
		pthr->cyWCS	     = global.cyWCS;
	    pthr->flMiscAttrs  |= global.flMiscAttrs;
	    global.flMiscAttrs	= 0L;
	}

	pthr->fUpdateAttrs  = FALSE;
	global.fUpdateAttrs = FALSE;

    }
    DosExitCritSec();


    /* Take care of the attribute bundles now. The miscellaneous attributes
     * require more processing, so don't clear their flags yet.
     */

    if (pthr->flLineAttrs & LFA_LINEALL)
    {
	GpiSetAttrs(pthr->hps, PRIM_LINE, pthr->flLineAttrs, 0L, &pthr->lb);
	pthr->flLineAttrs = 0L;
    }
    if (pthr->flMarkerAttrs & LFA_MARKALL)
    {
	GpiSetAttrs(pthr->hps, PRIM_MARKER, pthr->flMarkerAttrs, 0L, &pthr->mb);
	pthr->flMarkerAttrs = 0L;
    }
    if (pthr->flAreaAttrs & LFA_AREAALL)
    {
	GpiSetAttrs(pthr->hps, PRIM_AREA, pthr->flAreaAttrs, 0L, &pthr->ab);
	pthr->flAreaAttrs = 0L;
    }
    if (pthr->flImageAttrs)
    {
	GpiSetAttrs(pthr->hps, PRIM_IMAGE, pthr->flImageAttrs, 0L, &pthr->ib);
	pthr->flImageAttrs = 0L;
    }
}




/************************************************************************
*
*   LfComputeModelXforms
*
*   Compute the model transform matrices necessary to draw the fractal
*   on each side of the polygonal frame.  The rotation, scaling, and
*   translation are all rolled into one matrix for simplicity.
*
************************************************************************/

VOID
LfComputeModelXforms(pthr)
PTHR pthr;
{
    double dblXScale, dblYScale;
    double dblXOff, dblYOff;
    double dblTheta;
    double dblRotation, dblSinRotation, dblCosRotation;
    double dblSideLen, dblAngleDecr;
    double dblXExtDims, dblYExtDims;
    double dblHalfXDims, dblHalfYDims;
    double dx, dy;
    PMATRIXLF pmatlf;
    int i;


    dblAngleDecr = TWO_PI / (double)pthr->usPolygonSides;

    if (pthr->usPolygonSides == 1)
    {
	dblSideLen   = 1.0;
	dblRotation  = (double) pthr->dblRotation;
    }
    else
    {
	/* C 5.1 incorrectly compiles sin(temp_dbl2) in large
	 * model, where temp_dbl2 is expanded to eliminate all
	 * temporary variables, therefore I DO use the temp vars.
	 */

	double temp_dbl1, temp_dbl2;

	temp_dbl1    = (double)pthr->usPolygonSides;
	temp_dbl2    = PI / temp_dbl1;
	dblSideLen   = sin(temp_dbl2);
	dblRotation  = PI - dblAngleDecr;
	dblRotation  = 0.5 * dblRotation + pthr->dblRotation;
    }

    {
	double temp_dbl1, temp_dbl2;

	temp_dbl1  = (double) pthr->rcl.xRight;
	temp_dbl2  = (double) pthr->cxWCS;
	dblXScale  = temp_dbl1 / temp_dbl2;
	dblXScale *= pthr->dblXScale * dblSideLen;

	temp_dbl1  = (double) pthr->rcl.yTop;
	temp_dbl2  = (double) pthr->cyWCS;
	dblYScale  = temp_dbl1 / temp_dbl2;
	dblYScale *= pthr->dblYScale * dblSideLen;
    }

    dblXExtDims  = (double) pthr->rcl.xRight * pthr->dblXScale;
    dblYExtDims  = (double) pthr->rcl.yTop   * pthr->dblYScale;
    dblHalfXDims = 0.5 * dblXExtDims;
    dblHalfYDims = 0.5 * dblYExtDims;
    dblXOff	 = (double) pthr->rcl.xRight * pthr->dblXOff + dblHalfXDims;
    dblYOff	 = (double) pthr->rcl.yTop   * pthr->dblYOff + dblHalfYDims;

    dblTheta	 = PI + pthr->dblRotation;

    for (i = 0; i < pthr->usPolygonSides; ++i)
    {
	dblCosRotation = cos(dblRotation);
	dblSinRotation = sin(dblRotation);

	dx = dblHalfXDims * cos(dblTheta);
	dy = dblHalfYDims * sin(dblTheta);

	/* 0.000015 = about 1/65536 */

	pmatlf = pthr->pmatlf + i;
	pmatlf->fxM11 = (FIXED)(( dblCosRotation * dblXScale + 0.000015) * (double) 0x10000L);
	pmatlf->fxM12 = (FIXED)(( dblSinRotation * dblYScale + 0.000015) * (double) 0x10000L);
	pmatlf-> lM13 = 0L;
	pmatlf->fxM21 = (FIXED)((-dblSinRotation * dblXScale + 0.000015) * (double) 0x10000L);
	pmatlf->fxM22 = (FIXED)(( dblCosRotation * dblYScale + 0.000015) * (double) 0x10000L);
	pmatlf-> lM23 = 0L;
	pmatlf-> lM31 = (LONG) (dblXOff + dx + 0.5);
	pmatlf-> lM32 = (LONG) (dblYOff + dy + 0.5);
	pmatlf-> lM33 = 1L;

	dblRotation -= dblAngleDecr;
	dblTheta    -= dblAngleDecr;
    }
}




/************************************************************************
*
*   LineFractal
*
*   Draw fractal with the given similarity transform.
*
*   The general idea is to define a transformation to apply to the
*   unit line segment, to get a new polyline.  This same transformation
*   is then applied to each line segment of the new polyline.  The number
*   of successive applications of the similarity transform is set by the
*   user.  It's known as the level of recursion of the fractal.
*
*   Since this is where the point accumulation process will usually
*   be, it recognizes the flag	fInterrupted  to allow the current
*   work to be abandoned.
*
************************************************************************/

VOID
LineFractal(pthr, depth, len, ang, flip, xform)
PTHR pthr;
int depth;
double len;
double ang;
BOOL flip;
PLINEFRAC xform;
{
    double newlen;
    PLINEFRAC newseg;


    if (pthr->fInterrupted)
	return;

    if (depth)
    {
	/*
	 *  We have not reached the maximum depth of recursion yet,
	 *  so apply the similarity transform to the current line
	 *  segment.
	 */

	--depth;
	newseg = xform;
	do
	{
	    newlen  = len * newseg->length;
	    ang    += newseg->angle * (flip ? -1 : 1);
	    LineFractal(pthr, depth, newlen, ang, (flip ^ newseg->flip), xform);
	} while ((newseg = newseg->next) != EOLIST);
    }
    else
    {
	/*
	 *  We have reached the maximum depth of recursion, so
	 *  draw a line segment.
	 */

	pthr->x += len * cos(ang);
	pthr->y += len * sin(ang);
	LfAddPoint(pthr);
    }
}




/************************************************************************
*
*   LfAddPoint
*
*   Applies the global coordinate transform to the point (x, y), then
*   stuffs it into the global PolyLine point array, and increments the
*   count of points in the array.
*
************************************************************************/

VOID
LfAddPoint(pthr)
PTHR pthr;
{
    if (pthr->cptl == (ULONG)pthr->cptMax)
	LfDraw(pthr, TRUE);

    if (pthr->cptl < (ULONG)pthr->cptMax)
    {
	(pthr->pptl + pthr->cptl)->x =
	    (int)(pthr->x + 0.5);
	(pthr->pptl + pthr->cptl)->y =
	    (int)(pthr->y + 0.5);
	++pthr->cptl;
    }
}




/************************************************************************
*
*   LfDraw
*
*   For each segment of the frame, set the model transform and draw the
*   cache of points in the current primitive.
*
*   Invalidate the client rectangle of the main window in case this is
*   the top thread, to cause our latest bitmap to be copied there.
*
************************************************************************/

VOID
LfDraw(pthr, fFlush)
PTHR pthr;
BOOL fFlush;
{
    int i;
    BOOL myFlush;


    /* If this is a direct DC, but is not the top thread, then
     * don't draw anything.
     */
    if (pthr->dcType == IDM_DCDIRECT)
	if (!LfIsThreadTop(pthr))
	{
	    if (fFlush)
		pthr->cptl = 0L;
	    return;
	}


    if (pthr->fCollectBounds)
	GpiResetBoundaryData(pthr->hps);


    myFlush = FALSE;

    for (i = 0; i < pthr->usPolygonSides; ++i)
    {
	if (pthr->fInterrupted)
	    return;

	/* set model transform */
	GpiSetModelTransformMatrix(pthr->hps, 9L, pthr->pmatlf+i, TRANSFORM_REPLACE);

	/* we only really flush the last time we use the cache */
	if (i == pthr->usPolygonSides - 1)
	    myFlush = fFlush;

	switch ( pthr->usCurPrim )
	{
	case IDM_POLYLINE:
	    LfDrawPolyLine(pthr, myFlush);
	    break;

	case IDM_POLYFILLET:
	    LfDrawPolyFillet(pthr, myFlush);
	    break;

	case IDM_POLYSPLINE:
	    LfDrawPolySpline(pthr, myFlush);
	    break;

	case IDM_PEANO:
	    LfDrawPolyPeano(pthr, myFlush);
	    break;

	case IDM_POLYMARKER:
	    LfDrawPolyMarker(pthr, myFlush);
	    break;
	}

	if (pthr->dcType != IDM_DCDIRECT)
	    if (LfIsThreadTop(pthr))
	    {
		if (pthr->fCollectBounds)
		{
		    GpiQueryBoundaryData(pthr->hps, &pthr->rclBounds);
		    ++(pthr->rclBounds).xRight;
		    ++(pthr->rclBounds).yTop;
		    WinInvalidateRect(global.hwnd, &pthr->rclBounds, FALSE);
		}
		else
		    WinInvalidateRect(global.hwnd, &pthr->rcl, FALSE);
	    }
    }
}




/************************************************************************
*
*   LfDrawPolyLine
*
*   Draw a polyline using the thread's point buffer.
*
************************************************************************/

VOID
LfDrawPolyLine(pthr, fFlush)
PTHR pthr;
BOOL fFlush;
{
    /* After drawing the line, save the last point to set the
       current position before the next call. */

    GpiSetCurrentPosition( pthr->hps, pthr->pptl );
    GpiPolyLine( pthr->hps, pthr->cptl-1L, pthr->pptl+1 );
    if (fFlush)
    {
	*pthr->pptl = *(pthr->pptl + pthr->cptl-1);
	pthr->cptl = 1L;
    }
}




/************************************************************************
*
*   LfDrawPolyFillet
*
*   Draw a polyfillet using the thread's point buffer.
*
************************************************************************/

VOID
LfDrawPolyFillet(pthr, fFlush)
PTHR pthr;
BOOL fFlush;
{
    /* After drawing the curve, save the last point to set the
       current position before the next call. */

    if (pthr->cptl > 2)
    {
	GpiSetCurrentPosition( pthr->hps, pthr->pptl );
	GpiPolyFillet( pthr->hps, pthr->cptl-1L, pthr->pptl+1 );
	if (fFlush)
	{
	    *pthr->pptl = *(pthr->pptl + pthr->cptl-1);
	    pthr->cptl = 1L;
	}
    }
}




/************************************************************************
*
*   LfDrawPolySpline
*
*   Draw a polyspline using the thread's point buffer.
*
************************************************************************/

VOID
LfDrawPolySpline(pthr, fFlush)
PTHR pthr;
BOOL fFlush;
{
    int i;		/* loop counter */
    USHORT cptSlack;	/* # points in pptl not usable by PolySpline */

    /* GpiPolySpline expects the number of points to be a
       multiple of 3.  If we have a non-multiple of three,
       (excluding the first point, which we've used to set
       the current position), only pass the largest multiple
       of three, saving the rest for the next go-round. */

    cptSlack = (int)((pthr->cptl-1L) % 3) + 1;
    GpiSetCurrentPosition( pthr->hps, pthr->pptl );
    GpiPolySpline( pthr->hps, pthr->cptl-cptSlack,
		   pthr->pptl+1 );
    if (fFlush)
    {
	for (i = 0; i < cptSlack; ++i)
	    *(pthr->pptl + i) = *(pthr->pptl + pthr->cptl-cptSlack+i);
	pthr->cptl = cptSlack;
    }
}




/************************************************************************
*
*   LfDrawPolyPeano
*
*   Draw a chain of Peano primitives using the thread's point buffer.
*
************************************************************************/

VOID
LfDrawPolyPeano(pthr, fFlush)
PTHR pthr;
BOOL fFlush;
{
    LONG a, b, c, d;	/* temporary vars for Peano curvelet calculations */
    int cptPeano;	/* current point in pptl in use by Peano curve */
    POINTL ptPeano[2];	/* Peano curvelet array to pass to PolyLine */

    for (cptPeano = 0; cptPeano < (int)(pthr->cptl-1L); ++cptPeano)
    {
	ptPeano[0] = *(pthr->pptl + cptPeano);
	ptPeano[1] = *(pthr->pptl + cptPeano+1);
	a = (pthr->pptl + cptPeano+1)->x - (pthr->pptl + cptPeano)->x;
	b = (pthr->pptl + cptPeano+1)->y - (pthr->pptl + cptPeano)->y;
	c = (a + b)/2;
	d = (a - b)/2;
	if (labs(a) > labs(b))
	{
	    ptPeano[0].x +=  d;
	    ptPeano[0].y +=  c;
	}
	else
	{
	    ptPeano[0].x +=  c;
	    ptPeano[0].y += -d;
	}

	GpiSetCurrentPosition( pthr->hps, pthr->pptl + cptPeano);
	GpiPolyLine( pthr->hps, 2L, (PPOINTL)ptPeano);
    }
    if (fFlush)
    {
	*pthr->pptl = *(pthr->pptl + pthr->cptl-1);
	pthr->cptl = 1L;
    }
}




/************************************************************************
*
*   LfDrawPolyMarker
*
*   Draw a list of markers using the thread's point buffer.
*
************************************************************************/

VOID
LfDrawPolyMarker(pthr, fFlush)
PTHR pthr;
BOOL fFlush;
{
    /* I want to draw markers at every point in the array, but
       GpiPolyMarker won't draw at the last point!  So, GpiMarker
       does the job instead. */

    GpiSetCurrentPosition( pthr->hps, pthr->pptl + pthr->cptl-1);
    GpiPolyMarker( pthr->hps, pthr->cptl, pthr->pptl );
    GpiMarker	 ( pthr->hps,		  pthr->pptl + pthr->cptl-1 );

    if (fFlush)
	pthr->cptl = 0L;
}




/************************************************************************
*
*   LfClearRect
*
*   Set the area attributes if needed and fill the bitmap with them.
*
************************************************************************/

VOID
LfClearRect(pthr, prcl)
PTHR pthr;
PRECTL prcl;
{
    PRECTL prclT;

    if (pthr->hps)
    {
	if (prcl)
	    prclT = prcl;
	else
	    prclT = &pthr->rcl;

	if (pthr->dcType == IDM_DCDIRECT)
	{
	    /* If direct DC, only blt if top thread. */
	    if (LfIsThreadTop(pthr))
		GpiBitBlt(pthr->hps, NULL, 2L, (PPOINTL)prclT, ROP_PATCOPY, NULL);
	}
	else
	{
	    GpiBitBlt(pthr->hps, NULL, 2L, (PPOINTL)prclT, ROP_PATCOPY, NULL);
	    if (LfIsThreadTop(pthr))
		WinInvalidateRect(global.hwnd, prclT, FALSE);
	}
    }
}
