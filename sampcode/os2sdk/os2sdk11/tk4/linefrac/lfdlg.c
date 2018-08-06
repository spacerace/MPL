/************************************************************************
*
*   lfdlg.c -- Window procedures for dialog boxes used in LineFractal.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

#include <mt\stdio.h>
#include <mt\stdlib.h>
#include <mt\math.h>

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFDLG
#define INCL_LFUTIL
#include "lffuncs.h"




/************************************************************************
*
*   Global Variables
*
*   global	Global data block.
*
************************************************************************/

extern GLOBALDATA global;




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
    case WM_COMMAND:			/* the user has pressed a button */
	switch (SHORT1FROMMP(mp1))	/* which button? */
	{
	case IDD_OK:
	case IDD_CANCEL:
	    WinDismissDlg(hwnd, TRUE);
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
*   TimerDelayDlg
*
*   Process messages for the dialog box used to set the timer delay
*   values for automatically restarting a drawing.
*
************************************************************************/

ULONG CALLBACK
TimerDelayDlg(hwnd, usMsg, mp1, mp2)
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:
	MySetWindowUShort(hwnd, IDD_MINDELAY, global.usMinTimerDelay);
	MySetWindowUShort(hwnd, IDD_MAXDELAY, global.usMaxTimerDelay);
	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:
	    MyGetWindowUShort(hwnd, IDD_MINDELAY, &global.usMinTimerDelay);
	    MyGetWindowUShort(hwnd, IDD_MAXDELAY, &global.usMaxTimerDelay);

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
*   BitmapParamsDlg
*
*   Process messages for the bitmap parameters dialog box.
*
************************************************************************/

ULONG CALLBACK
BitmapParamsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	MySetWindowUShort(hwnd, IDD_BMCX,     global.bm.cx);
	MySetWindowUShort(hwnd, IDD_BMCY,     global.bm.cy);
	MySetWindowUShort(hwnd, IDD_BMPLANES, global.bm.cPlanes);
	MySetWindowUShort(hwnd, IDD_BMBPP,    global.bm.cBitCount);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    MyGetWindowUShort(hwnd, IDD_BMCX,	  &global.bm.cx);
	    MyGetWindowUShort(hwnd, IDD_BMCY,	  &global.bm.cy);
	    MyGetWindowUShort(hwnd, IDD_BMPLANES, &global.bm.cPlanes);
	    MyGetWindowUShort(hwnd, IDD_BMBPP,	  &global.bm.cBitCount);

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
*   SwitchDelayDlg
*
*   Process messages for the dialog box used to set the switch
*   delay value for automatically switching between threads.
*
************************************************************************/

ULONG CALLBACK
SwitchDelayDlg(hwnd, usMsg, mp1, mp2)
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:
	MySetWindowUShort(hwnd, IDD_SWITCHDELAY, global.usSwitchDelay);
	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:
	    MyGetWindowUShort(hwnd, IDD_SWITCHDELAY, &global.usSwitchDelay);

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
*   LineAttrsDlg
*
*   Process messages for the line attributes dialog box.
*
************************************************************************/

#define SETLINEATTR(H, I, J)						  \
	MySetWindow##I(hwnd, J, global.lb.H)

#define GETLINEATTR(H, I, J, K, L)					  \
	{								  \
	    J temp;							  \
	    MyGetWindow##I(hwnd, K, &temp);				  \
	    if (temp != global.lb.H)					  \
	    {								  \
		global.lb.H = temp;					  \
		global.flLineAttrs |= L;				  \
	    }								  \
	}

ULONG CALLBACK
LineAttrsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	if (!global.fUpdateAttrs || !(global.flLineAttrs & LFA_LINEALL))
	    global.lb = global.pThrTop->lb;

	SETLINEATTR(lColor,	Long,	IDD_LINECOLOR);
	SETLINEATTR(usMixMode,	UShort, IDD_LINEMIX);
	SETLINEATTR(fxWidth,	Long,	IDD_LINEWIDTH);
	SETLINEATTR(lGeomWidth, Long,	IDD_LINEGEOMWIDTH);
	SETLINEATTR(usType,	UShort, IDD_LINETYPE);
	SETLINEATTR(usEnd,	UShort, IDD_LINEEND);
	SETLINEATTR(usJoin,	UShort, IDD_LINEJOIN);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETLINEATTR(lColor,     Long,   LONG,   IDD_LINECOLOR,     LFA_LINECOLOR);
	    GETLINEATTR(usMixMode,  UShort, USHORT, IDD_LINEMIX,       LFA_LINEMIX);
	    GETLINEATTR(fxWidth,    Long,   LONG,   IDD_LINEWIDTH,     LFA_LINEWIDTH);
	    GETLINEATTR(lGeomWidth, Long,   LONG,   IDD_LINEGEOMWIDTH, LFA_LINEGEOMWIDTH);
	    GETLINEATTR(usType,     UShort, USHORT, IDD_LINETYPE,      LFA_LINETYPE);
	    GETLINEATTR(usEnd,	    UShort, USHORT, IDD_LINEEND,       LFA_LINEEND);
	    GETLINEATTR(usJoin,     UShort, USHORT, IDD_LINEJOIN,      LFA_LINEJOIN);

	    if (global.flLineAttrs & LFA_LINEALL)
	    {
		global.pThrTop->fUpdateAttrs = TRUE;
		global.fUpdateAttrs = TRUE;
	    }

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
#undef SETLINEATTR
#undef GETLINEATTR




/************************************************************************
*
*   MarkerAttrsDlg
*
*   Process messages for the marker attributes dialog box.
*
************************************************************************/

#define SETMARKATTR(H, I, J)						  \
	MySetWindow##I(hwnd, J, global.mb.H)

#define GETMARKATTR(H, I, J, K, L)					  \
	{								  \
	    J temp;							  \
	    MyGetWindow##I(hwnd, K, &temp);				  \
	    if (temp != global.mb.H)					  \
	    {								  \
		global.mb.H = temp;					  \
		global.flMarkerAttrs |= L;				  \
	    }								  \
	}

ULONG CALLBACK
MarkerAttrsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	if (!global.fUpdateAttrs || !(global.flMarkerAttrs & LFA_MARKALL))
	    global.mb = global.pThrTop->mb;

	SETMARKATTR(lColor,	   Long,   IDD_MARKCOLOR);
	SETMARKATTR(lBackColor,    Long,   IDD_MARKBACKCOLOR);
	SETMARKATTR(usMixMode,	   UShort, IDD_MARKMIX);
	SETMARKATTR(usBackMixMode, UShort, IDD_MARKBACKMIX);
	SETMARKATTR(usSet,	   UShort, IDD_MARKSET);
	SETMARKATTR(usSymbol,	   UShort, IDD_MARKSYMBOL);
	SETMARKATTR(sizfxCell.cx,  Long,   IDD_MARKCELLWIDTH);
	SETMARKATTR(sizfxCell.cy,  Long,   IDD_MARKCELLHEIGHT);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETMARKATTR(lColor,        Long,   LONG,   IDD_MARKCOLOR,	  LFA_MARKCOLOR);
	    GETMARKATTR(lBackColor,    Long,   LONG,   IDD_MARKBACKCOLOR,  LFA_MARKBACKCOLOR);
	    GETMARKATTR(usMixMode,     UShort, USHORT, IDD_MARKMIX,	  LFA_MARKMIX);
	    GETMARKATTR(usBackMixMode, UShort, USHORT, IDD_MARKBACKMIX,	  LFA_MARKBACKMIX);
	    GETMARKATTR(usSet,	       UShort, USHORT, IDD_MARKSET,	  LFA_MARKSET);
	    GETMARKATTR(usSymbol,      UShort, USHORT, IDD_MARKSYMBOL,	  LFA_MARKSYMBOL);
	    GETMARKATTR(sizfxCell.cx,  Long,   LONG,   IDD_MARKCELLWIDTH,  LFA_MARKCELLWIDTH);
	    GETMARKATTR(sizfxCell.cy,  Long,   LONG,   IDD_MARKCELLHEIGHT, LFA_MARKCELLHEIGHT);

	    if (global.flMarkerAttrs & LFA_MARKALL)
	    {
		global.pThrTop->fUpdateAttrs = TRUE;
		global.fUpdateAttrs = TRUE;
	    }

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
#undef SETMARKATTR
#undef GETMARKATTR




/************************************************************************
*
*   AreaAttrsDlg
*
*   Process messages for the area attributes dialog box.
*
************************************************************************/

#define SETAREAATTR(H, I, J)						  \
	MySetWindow##I(hwnd, J, global.ab.H)

#define GETAREAATTR(H, I, J, K, L)					  \
	{								  \
	    J temp;							  \
	    MyGetWindow##I(hwnd, K, &temp);				  \
	    if (temp != global.ab.H)					  \
	    {								  \
		global.ab.H = temp;					  \
		global.flAreaAttrs |= L;				  \
	    }								  \
	}

ULONG CALLBACK
AreaAttrsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	if (!global.fUpdateAttrs || !(global.flAreaAttrs & LFA_AREAALL))
	    global.ab = global.pThrTop->ab;

	SETAREAATTR(lColor,	   Long,   IDD_AREACOLOR);
	SETAREAATTR(lBackColor,    Long,   IDD_AREABACKCOLOR);
	SETAREAATTR(usMixMode,	   UShort, IDD_AREAMIX);
	SETAREAATTR(usBackMixMode, UShort, IDD_AREABACKMIX);
	SETAREAATTR(usSet,	   UShort, IDD_AREASET);
	SETAREAATTR(usSymbol,	   UShort, IDD_AREASYMBOL);
	SETAREAATTR(ptlRefPoint.x, Long,   IDD_AREAREFX);
	SETAREAATTR(ptlRefPoint.y, Long,   IDD_AREAREFY);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETAREAATTR(lColor,        Long,   LONG,   IDD_AREACOLOR,	 LFA_AREACOLOR);
	    GETAREAATTR(lBackColor,    Long,   LONG,   IDD_AREABACKCOLOR, LFA_AREABACKCOLOR);
	    GETAREAATTR(usMixMode,     UShort, USHORT, IDD_AREAMIX,	 LFA_AREAMIX);
	    GETAREAATTR(usBackMixMode, UShort, USHORT, IDD_AREABACKMIX,	 LFA_AREABACKMIX);
	    GETAREAATTR(usSet,	       UShort, USHORT, IDD_AREASET,	 LFA_AREASET);
	    GETAREAATTR(usSymbol,      UShort, USHORT, IDD_AREASYMBOL,	 LFA_AREASYMBOL);
	    GETAREAATTR(ptlRefPoint.x, Long,   LONG,   IDD_AREAREFX,	 LFA_AREAREFX);
	    GETAREAATTR(ptlRefPoint.y, Long,   LONG,   IDD_AREAREFY,	 LFA_AREAREFY);

	    if (global.flAreaAttrs & LFA_AREAALL)
	    {
		global.pThrTop->fUpdateAttrs = TRUE;
		global.fUpdateAttrs = TRUE;
	    }

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
#undef SETAREAATTR
#undef GETAREAATTR




/************************************************************************
*
*   ImageAttrsDlg
*
*   Process messages for the image attributes dialog box.
*
************************************************************************/

#define SETIMAGEATTR(H, I, J)						  \
	MySetWindow##I(hwnd, J, global.ib.H)

#define GETIMAGEATTR(H, I, J, K, L)					  \
	{								  \
	    J temp;							  \
	    MyGetWindow##I(hwnd, K, &temp);				  \
	    if (temp != global.ib.H)					  \
	    {								  \
		global.ib.H = temp;					  \
		global.flImageAttrs |= L;				  \
	    }								  \
	}

ULONG CALLBACK
ImageAttrsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	if (!global.fUpdateAttrs || !(global.flImageAttrs & LFA_IMAGEALL))
	    global.ib = global.pThrTop->ib;

	SETIMAGEATTR(lColor,	    Long,   IDD_IMAGECOLOR);
	SETIMAGEATTR(lBackColor,    Long,   IDD_IMAGEBACKCOLOR);
	SETIMAGEATTR(usMixMode,     UShort, IDD_IMAGEMIX);
	SETIMAGEATTR(usBackMixMode, UShort, IDD_IMAGEBACKMIX);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETIMAGEATTR(lColor,	Long,	LONG,	IDD_IMAGECOLOR,	   LFA_IMAGECOLOR);
	    GETIMAGEATTR(lBackColor,	Long,	LONG,	IDD_IMAGEBACKCOLOR, LFA_IMAGEBACKCOLOR);
	    GETIMAGEATTR(usMixMode,	UShort, USHORT, IDD_IMAGEMIX,	   LFA_IMAGEMIX);
	    GETIMAGEATTR(usBackMixMode, UShort, USHORT, IDD_IMAGEBACKMIX,   LFA_IMAGEBACKMIX);

	    if (global.flImageAttrs & LFA_IMAGEALL)
	    {
		global.pThrTop->fUpdateAttrs = TRUE;
		global.fUpdateAttrs = TRUE;
	    }

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
#undef SETIMAGEATTR
#undef GETIMAGEATTR




/************************************************************************
*
*   MiscAttrsDlg
*
*   Process messages for the fractal attributes dialog box.
*
************************************************************************/

#define SETMISCATTR(H, I, J)						  \
	MySetWindow##I(hwnd, J, global.H)

#define GETMISCATTR(H, I, J, K, L)					  \
	{								  \
	    J temp;							  \
	    MyGetWindow##I(hwnd, K, &temp);				  \
	    if (temp != global.H)					  \
	    {								  \
		global.H = temp;					  \
		global.flMiscAttrs |= L;				  \
	    }								  \
	}

ULONG CALLBACK
MiscAttrsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	if (!global.fUpdateAttrs || !(global.flMiscAttrs &
		(LFA_RECURSION|LFA_CPTMAX|LFA_POLYGONSIDES|LFA_ROTATION)))
	{
	    global.usRecursion	  = global.pThrTop->usRecursion;
	    global.cptMax	  = global.pThrTop->cptMax;
	    global.usPolygonSides = global.pThrTop->usPolygonSides;
	    global.dblRotation	  = global.pThrTop->dblRotation;
	}

	SETMISCATTR(usRecursion,    UShort, IDD_RECURSION);
	SETMISCATTR(cptMax,	    UShort, IDD_CPTMAX);
	SETMISCATTR(usPolygonSides, UShort, IDD_NUMSIDES);
	SETMISCATTR(dblRotation,    Double, IDD_ROTATION);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETMISCATTR(usRecursion,	UShort, USHORT, IDD_RECURSION, LFA_RECURSION);
	    GETMISCATTR(cptMax, 	UShort, USHORT, IDD_CPTMAX,    LFA_CPTMAX);
	    GETMISCATTR(usPolygonSides, UShort, USHORT, IDD_NUMSIDES,  LFA_POLYGONSIDES);
	    GETMISCATTR(dblRotation,	Double, double, IDD_ROTATION,  LFA_ROTATION);

	    if (global.flMiscAttrs & LFA_POLYGONSIDES)
		if (global.usPolygonSides == 0)
		    global.usPolygonSides = 2;

	    if (global.flMiscAttrs & LFA_CPTMAX)
	    {
		if (global.cptMax == 0)
		    global.cptMax = 1;
		else if (global.cptMax > MAX_POINT_COUNT)
		    global.cptMax = MAX_POINT_COUNT;
	    }

	    if (global.flMiscAttrs & (LFA_RECURSION|LFA_CPTMAX|
		LFA_POLYGONSIDES|LFA_ROTATION))
	    {
		global.pThrTop->fUpdateAttrs = TRUE;
		global.fUpdateAttrs = TRUE;
	    }

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
#undef SETMISCATTR
#undef GETMISCATTR




/************************************************************************
*
*   DimensionsDlg
*
*   Process messages for the fractal dimensions dialog box.
*
************************************************************************/

#define SETDIMENSIONS(H, I, J)						  \
	MySetWindow##I(hwnd, J, global.H)

#define GETDIMENSIONS(H, I, J, K, L)					  \
	{								  \
	    J temp;							  \
	    MyGetWindow##I(hwnd, K, &temp);				  \
	    if (temp != global.H)					  \
	    {								  \
		global.H = temp;					  \
		global.flMiscAttrs |= L;				  \
	    }								  \
	}

ULONG CALLBACK
DimensionsDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	if (!global.fUpdateAttrs || !(global.flMiscAttrs &
	    (LFA_XOFF|LFA_YOFF|LFA_XSCALE|LFA_YSCALE)))
	{
	    global.dblXOff   = global.pThrTop->dblXOff;
	    global.dblYOff   = global.pThrTop->dblYOff;
	    global.dblXScale = global.pThrTop->dblXScale;
	    global.dblYScale = global.pThrTop->dblYScale;
	}

	SETDIMENSIONS(dblXOff,	 Double, IDD_XOFF);
	SETDIMENSIONS(dblYOff,	 Double, IDD_YOFF);
	SETDIMENSIONS(dblXScale, Double, IDD_XSCALE);
	SETDIMENSIONS(dblYScale, Double, IDD_YSCALE);

	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETDIMENSIONS(dblXOff,   Double, double, IDD_XOFF,	LFA_XOFF);
	    GETDIMENSIONS(dblYOff,   Double, double, IDD_YOFF,	LFA_YOFF);
	    GETDIMENSIONS(dblXScale, Double, double, IDD_XSCALE, LFA_XSCALE);
	    GETDIMENSIONS(dblYScale, Double, double, IDD_YSCALE, LFA_YSCALE);

	    if (global.flMiscAttrs & (LFA_XOFF|LFA_YOFF|LFA_XSCALE|LFA_YSCALE))
	    {
		global.pThrTop->fUpdateAttrs = TRUE;
		global.fUpdateAttrs = TRUE;
	    }

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
#undef SETDIMENSIONS
#undef GETDIMENSIONS




/************************************************************************
*
*   PtrPreferencesDlg
*
*   Process messages for the mouse pointer preferences dialog box.
*
************************************************************************/

#define SETPTRPREF(H, I, J)				    \
	MySetWindow##I(hwnd, J, global.H)

#define GETPTRPREF(H, I, J)				    \
	MyGetWindow##I(hwnd, J, &global.H)

ULONG CALLBACK
PtrPreferencesDlg( hwnd, usMsg, mp1, mp2 )
HWND   hwnd;
USHORT usMsg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL fRet = FALSE;

    switch (usMsg)
    {
    case WM_INITDLG:

	SETPTRPREF(usPtrThreshold, UShort, IDD_PTRTHRESHOLD);
	WinSendDlgItemMsg(hwnd, global.usUserPtr+IDD_USERPTR0,
			  BM_SETCHECK, (MPARAM)TRUE, 0L);
	return FALSE;

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
        {
	case IDD_OK:

	    GETPTRPREF(usPtrThreshold, UShort, IDD_PTRTHRESHOLD);
	    global.usUserPtr = (USHORT) WinSendDlgItemMsg(hwnd, IDD_USERPTR0,
						 BM_QUERYCHECKINDEX, 0L, 0L);

	    if (global.usPtrThreshold < 0)
		global.usPtrThreshold = 0;

	    if (!global.fAnimatePtr)
		global.usCurPtr = global.usUserPtr;

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
#undef SETPTRPREF
#undef GETPTRPREF
