/************************************************************************
*
*   lfps.c -- Subroutines for PS/DC/BM management for LineFractal.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFPS
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
*   Data structures for opening the PostScript DC.
*
************************************************************************/

typedef struct _randomdata
{
    long data[12];
} PSRANDOMDATA;

typedef struct _psdevopendata
{
    char *device;
    char *name;
    PSRANDOMDATA *data;
    char *type;
} PSDEVOPENDATA;

PSRANDOMDATA psrnddata =
{
    48, 0, 0x65656565, 0, 0, 0, 0, 0, 0, 0, 0, 1
};

PSDEVOPENDATA dvdata =
{
    "ps.txt",
    "PSCRIPT",
    &psrnddata,
    "IBM_Q_STD"
};




/************************************************************************
*
*   LfOpenDC
*
*   Open a device context for the thread.  Read the DC type from the
*   thread structure.
*
************************************************************************/

BOOL fhdcInUse = FALSE; /* TRUE --> a direct dc thread is active */
HDC hdcPrivate = NULL;	/* non NULL --> it's the window DC */

BOOL
LfOpenDC(pthr)
PTHR  pthr;
{
    switch (pthr->dcType)
    {
    case IDM_DCDIRECT:
	/* only one direct dc allowed */
	if (fhdcInUse)
	    pthr->hdc = NULL;
	else
	{
	    if (!hdcPrivate)
		hdcPrivate = WinOpenWindowDC(global.hwnd);
	    pthr->hdc = hdcPrivate;
	    fhdcInUse = TRUE;
	}
	break;

    case IDM_DCMEMORY:
	pthr->hdc = DevOpenDC(global.hab, OD_MEMORY, "*", 0L, NULL, NULL);
	break;

    case IDM_DCPOSTSCRIPT:
	pthr->hdc = DevOpenDC(global.hab, OD_DIRECT, "*", 4L, (PDEVOPENDATA)&dvdata, NULL);
	break;

    case IDM_DCPROPRINTER:

    default:
	pthr->hdc = NULL;
    }

    return (pthr->hdc ? TRUE : FALSE);
}




/************************************************************************
*
*   LfCloseDC
*
*   Close the device context for the thread.
*
************************************************************************/

VOID
LfCloseDC(pthr)
PTHR  pthr;
{
    if (pthr->dcType == IDM_DCDIRECT)
	fhdcInUse = FALSE;
    else
    {
	if (pthr->hbm)
	    LfDeleteBitmap(pthr);
	if (pthr->hdc)
	    DevCloseDC(pthr->hdc);
    }
    pthr->hdc = NULL;
}




/************************************************************************
*
*   LfCreateBitmap
*
*   Create a bitmap of the format specified in the thread structure.
*   If the bitmap is to be automatically sized, then read the window
*   size, otherwise use the values in the thread structure.
*
************************************************************************/

BOOL
LfCreateBitmap(pthr)
PTHR  pthr;
{
    BITMAPINFOHEADER bminfo;

    bminfo.cbFix = sizeof bminfo;

    bminfo.cx = (USHORT) (pthr->rcl.xRight - pthr->rcl.xLeft);
    bminfo.cy = (USHORT) (pthr->rcl.yTop   - pthr->rcl.yBottom);
    bminfo.cPlanes   = pthr->cPlanes;
    bminfo.cBitCount = pthr->cBitCount;

    pthr->hbm = GpiCreateBitmap(pthr->hps, &bminfo, 0L, 0L, 0L);

    return (pthr->hbm ? TRUE : FALSE);
}




/************************************************************************
*
*   LfDeleteBitmap
*
*   Delete the thread's bitmap and set the bitmap handle to null.
*
************************************************************************/

VOID
LfDeleteBitmap(pthr)
PTHR  pthr;
{
    GpiDeleteBitmap(pthr->hbm);
    pthr->hbm = NULL;
}




/************************************************************************
*
*   LfCreatePS
*
*   Create a presentation space for the thread.  If the DC type is
*   a memory bitmap, then create a bitmap, select it into the PS, and
*   initialize it to the desired background color.  Compute the scale
*   factors and offsets to use when drawing.
*
************************************************************************/

BOOL
LfCreatePS(pthr)
PTHR  pthr;
{
    SIZEL size;
    BOOL fOK = FALSE;

    if (LfOpenDC(pthr))
    {
	size.cx = 0L;
	size.cy = 0L;
	pthr->hps = GpiCreatePS(global.hab, pthr->hdc, &size,
				 PU_PELS|GPIT_MICRO|GPIA_ASSOC);
	if (pthr->hps)
	{
	    if (pthr->dcType == IDM_DCDIRECT)
	    {
		pthr->rcl = global.rcl;
		GpiSetAttrs(pthr->hps, PRIM_LINE,   LFA_LINEALL,  0L, &pthr->lb);
		GpiSetAttrs(pthr->hps, PRIM_MARKER, LFA_MARKALL,  0L, &pthr->mb);
		GpiSetAttrs(pthr->hps, PRIM_AREA,   LFA_AREAALL,  0L, &pthr->ab);
		GpiSetAttrs(pthr->hps, PRIM_IMAGE,  LFA_IMAGEALL, 0L, &pthr->ib);

	    }
	    else if (pthr->dcType == IDM_DCMEMORY)
	    {
		pthr->rcl.xLeft   = 0L;
		pthr->rcl.yBottom = 0L;
		pthr->rcl.xRight  = global.bm.cx;
		pthr->rcl.yTop	  = global.bm.cy;
		pthr->cPlanes	  = global.bm.cPlanes;
		pthr->cBitCount   = global.bm.cBitCount;
		if (LfCreateBitmap(pthr))
		    if (GpiSetBitmap(pthr->hps, pthr->hbm) != HBM_ERROR)
		    {
			GpiSetAttrs(pthr->hps, PRIM_LINE,   LFA_LINEALL,  0L, &pthr->lb);
			GpiSetAttrs(pthr->hps, PRIM_MARKER, LFA_MARKALL,  0L, &pthr->mb);
			GpiSetAttrs(pthr->hps, PRIM_AREA,   LFA_AREAALL,  0L, &pthr->ab);
			GpiSetAttrs(pthr->hps, PRIM_IMAGE,  LFA_IMAGEALL, 0L, &pthr->ib);

			LfClearRect(pthr, &pthr->rcl);
			if (pthr->fCollectBounds)
			    GpiSetDrawControl(pthr->hps, DCTL_BOUNDARY, DCTL_ON);
			fOK = TRUE;
		    }

		if (!fOK)
		{
		    LfDeletePS(pthr);
		    return FALSE;
		}
	    }
	    else
	    {
		pthr->rcl = global.rcl;
	    }

	    pthr->flMiscAttrs |= LFA_XSCALE | LFA_YSCALE | LFA_XOFF | LFA_YOFF;

	    return TRUE;
	}
    }
    return FALSE;
}




/************************************************************************
*
*   LfDeletePS
*
*   Delete the thread's presentation space.  If the DC type is
*   a memory bitmap, then unset the bitmap and delete it.
*
************************************************************************/

VOID
LfDeletePS(pthr)
PTHR  pthr;
{
    if (pthr->hbm)
    {
	GpiSetBitmap(pthr->hps, NULL);
	LfDeleteBitmap(pthr);
    }
    GpiDestroyPS(pthr->hps);
    LfCloseDC(pthr);
}




/************************************************************************
*
*   LfResizePS
*
*   Destroy the current PS and create a new one the size of the
*   current client rectangle.
*
************************************************************************/

BOOL
LfResizePS(pthr)
PTHR pthr;
{
    if (pthr->hps)
    {
	pthr->fInterrupted = TRUE;
	while (pthr->fBusy)
	    ;
	LfDeletePS(pthr);
    }

    return LfCreatePS(pthr);
}
