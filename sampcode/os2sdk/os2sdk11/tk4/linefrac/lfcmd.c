/************************************************************************
*
*   lfcmd.c -- Subroutines for menu command processing for LineFractal.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSSEMAPHORES
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include <os2.h>

#include <mt\stdio.h>
#include <mt\stdlib.h>
#include <mt\math.h>

#include "opendlg.h"

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFCMD
#define INCL_LFDLG
#define INCL_LFMAIN
#define INCL_LFTHREAD
#define INCL_LFPS
#define INCL_LFDRAW
#define INCL_LFUTIL
#define INCL_LFFILE
#include "lffuncs.h"




/************************************************************************
*
*   Macros
*
*   Helper macros for repetitious code.
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




/************************************************************************
*
*   Global Variables
*
*   global	Global data block.
*   aulRops	Array of BitBlt rops available for pasting.
*   aXform	Array of built-in transforms and default positioning
*		  and sizing information.
*
************************************************************************/

extern GLOBALDATA global;
extern XFORMDATA  aXform[];

ULONG aulRops[] =
{
      ROP_SRCCOPY,
      ROP_SRCAND,
      ROP_SRCPAINT,
      ROP_SRCINVERT,
    (~ROP_SRCCOPY)   & 0xFF,
    (~ROP_SRCAND)    & 0xFF,
    (~ROP_SRCPAINT)  & 0xFF,
    (~ROP_SRCINVERT) & 0xFF
};

char *pszAttrs = "Previous attributes not read yet.";




/************************************************************************
*
*   LfCommand
*
*   Dispatches menu commands to the proper handlers.
*
************************************************************************/

VOID
LfCommand(hwnd, id)
HWND hwnd;
USHORT id;
{
    switch (id)
    {

    /********************************************************************
    *
    *	File menu
    *
    ********************************************************************/

    case IDM_LOAD:
    case IDM_SAVE:
    case IDM_ABOUT:
	LfFileMenu(hwnd, id);
	break;

    /********************************************************************
    *
    *	Edit menu
    *
    ********************************************************************/

    case IDM_SELECT:
    case IDM_SELECTALL:
    case IDM_CUT:
    case IDM_COPY:
    case IDM_PASTE:
    case IDM_USECLIPBRD:
    case IDM_PASTES:
    case IDM_PASTEDSA:
    case IDM_PASTEDSO:
    case IDM_PASTEDSX:
    case IDM_PASTESN:
    case IDM_PASTEDSAN:
    case IDM_PASTEDSON:
    case IDM_PASTEDSXN:
	LfEditMenu(hwnd, id);
	break;

    /********************************************************************
    *
    *	Thread menu
    *
    ********************************************************************/

    case IDM_DCPOSTSCRIPT:
    case IDM_DCPROPRINTER:
    case IDM_DCDIRECT:
    case IDM_DCMEMORY:
    case IDM_EDITTHREADPARAMS:
    case IDM_THR0TOTOP:
    case IDM_THR1TOTOP:
    case IDM_THR2TOTOP:
    case IDM_THR3TOTOP:
    case IDM_THR4TOTOP:
    case IDM_THR5TOTOP:
    case IDM_THR6TOTOP:
    case IDM_THR7TOTOP:
    case IDM_THR8TOTOP:
    case IDM_THR9TOTOP:
    case IDM_KILLTOPTHREAD:
    case IDM_KILLALLTHREADS:
    case IDM_AUTOSWITCH:
    case IDM_SWITCHDELAY:
    case IDM_UPDATEMENUS:
    case IDM_THREADINHERITANCE:
	LfThreadMenu(hwnd, id);
	break;

    /********************************************************************
    *
    *	Control menu
    *
    ********************************************************************/

    case IDM_CLEARBITMAP:
    case IDM_COPYBITMAPTOSCREEN:
    case IDM_SQUAREBITMAP:
    case IDM_RESIZEBITMAPTOWINDOW:
    case IDM_MOUSECHANGESRECURSION:
    case IDM_ANIMATEPTR:
    case IDM_PTRPREFERENCES:
    case IDM_TIMERDELAY:
	if (LfControlMenu(hwnd, id))
	    goto lfcmd_redraw;
	break;

    /********************************************************************
    *
    *	Fractal menu
    *
    ********************************************************************/

    case IDM_SHARKTOOTH:
    case IDM_SAWTOOTH:
    case IDM_KOCH:
    case IDM_SPACEFILLER:
    case IDM_STOVE:
    case IDM_SQUAREWAVE:
    case IDM_HOURGLASS:
    case IDM_FRACREDRAW:
	if (LfFractalMenu(hwnd, id))
	    goto lfcmd_redraw;
	break;

    /********************************************************************
    *
    *	Primitive menu
    *
    ********************************************************************/

    case IDM_POLYLINE:
    case IDM_POLYFILLET:
    case IDM_POLYSPLINE:
    case IDM_POLYMARKER:
    case IDM_PEANO:
    case IDM_PRIMREDRAW:
	if (LfPrimitiveMenu(hwnd, id))
	    goto lfcmd_redraw;
	break;

    /********************************************************************
    *
    *	Attributes menu
    *
    ********************************************************************/

    case IDM_LINEATTRS:
    case IDM_MARKERATTRS:
    case IDM_AREAATTRS:
    case IDM_IMAGEATTRS:
    case IDM_MISCATTRS:
    case IDM_DIMENSIONS:
    case IDM_DRAGDIMS:
    case IDM_ATTRREDRAW:
	if (LfAttributesMenu(hwnd, id))
	    goto lfcmd_redraw;
	break;

    /********************************************************************
    *
    *	Environment menu
    *
    ********************************************************************/

    case IDM_AUTORESIZE:
    case IDM_AUTOSELECTDIMS:
    case IDM_CLEARONREDRAW:
    case IDM_AUTOSTARTREDRAW:
    case IDM_COLLECTBOUNDS:
	LfEnvironmentMenu(hwnd, id);
	break;

    /********************************************************************
    *
    *	Control menu, continued.  These two commands dovetail
    *	conveniently,  so they are placed out of sequence here.
    *
    ********************************************************************/

lfcmd_redraw:
    case IDM_REDRAW:
	DosSemClear(&(global.pThrTop)->lSemRedraw);

	/* Fall through to restart the drawing. */

    case IDM_ABORT:
	global.pThrTop->fInterrupted = TRUE;
	break;
    }
}




/************************************************************************
*
*   LfFileMenu
*
*   These commands allow the loading and saving of bitmaps from threads
*   with memory DC's.  The about box shows the version number, title,
*   and author.
*
************************************************************************/

VOID
LfFileMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    switch (id)
    {
    case IDM_LOAD:
	if (global.pThrTop->dcType == IDM_DCMEMORY)
	    LfReadFile(hwnd, global.pThrTop);
	else
	    MyMessageBox(hwnd, "Cannot load bitmap into a\nnon-memory DC.");
	break;

    case IDM_SAVE:
	if (global.pThrTop->dcType == IDM_DCMEMORY)
	    LfWriteFile(hwnd, global.pThrTop);
	else
	    MyMessageBox(hwnd, "Cannot save bitmap from a\nnon-memory DC.");
	break;

    case IDM_ABOUT:
	WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)AboutDlg, NULL,
		       IDR_ABOUTDLG, NULL );
	break;
    }
}




/************************************************************************
*
*   LfEditMenu
*
*   These commands allow for cutting, copying, and pasting of rectangles
*   of bits.  The user can drag a rectangle or select the entire surface.
*   The GpiBitBlt rop to use when pasting can be any of those listed.
*   The user can specify that the system clipboard be used for storing
*   the cut or copied bits, or that a private bitmap be used.
*
************************************************************************/

VOID
LfEditMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    switch (id)
    {
    case IDM_SELECT:
	global.fSelecting = TRUE;
	break;

    case IDM_SELECTALL:
	LfSelectAll(hwnd);
	break;

    case IDM_CUT:
	if (global.fShowSelectRc)
	    LfCut(hwnd);
	break;

    case IDM_COPY:
	if (global.fShowSelectRc)
	    LfCopy(hwnd);
	break;

    case IDM_PASTE:
	if (global.fHaveSelectRc)
	{
	    HPS hps;

	    global.fPasting = TRUE;
	    if (global.fShowSelectRc && global.fTempSelect)
	    {
		hps = WinGetPS(hwnd);
		LfShowSelectRc(hps, &global.rclSelect);
		WinReleasePS(hps);
		global.fShowSelectRc = FALSE;
		global.fTempSelect = FALSE;
	    }
	    if (!global.fShowSelectRc)
	    {
		global.fShowSelectRc = TRUE;
		hps = WinGetPS(hwnd);
		LfShowSelectRc(hps, &global.rclCutCopy);
		WinReleasePS(hps);
	    }
	}
	break;

    case IDM_USECLIPBRD:
	UPDATE_MENU_BOOL(global.fUseClipbrd, IDM_USECLIPBRD);
	break;

    /********************************************************************/

    case IDM_PASTES:
    case IDM_PASTEDSA:
    case IDM_PASTEDSO:
    case IDM_PASTEDSX:
    case IDM_PASTESN:
    case IDM_PASTEDSAN:
    case IDM_PASTEDSON:
    case IDM_PASTEDSXN:
	UNCHECK_MENU_ITEM(global.hwndFrame, global.usRopIndex+IDM_PASTES);
	global.usRopIndex = id - IDM_PASTES;
	CHECK_MENU_ITEM(global.hwndFrame, global.usRopIndex+IDM_PASTES);
	global.ulPasteROP = aulRops[global.usRopIndex];
	break;
    }
}




/************************************************************************
*
*   LfThreadMenu
*
*   These commands allow the creation, viewing, editing, and deletion
*   of up to 10 independently drawing threads.	The middle group
*   allow for simple animation by cycling through the threads at
*   a user-selectable rate.  The updating of menus can be disabled to
*   allow a faster thread-switch rate.	The last group determine how
*   new threads will look and act.  If inheritance is enabled, a new
*   thread will be drawn with the attributes of the top thread, otherwise
*   it will use the program's default values.
*
************************************************************************/

VOID
LfThreadMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    static char *szError1 = "Could not start thread.";

    switch (id)
    {
    case IDM_DCPOSTSCRIPT:
    case IDM_DCPROPRINTER:
	break;	/* not implemented yet */

    case IDM_DCDIRECT:
	{
	    PTHR pthr;

	    if (global.cThr < MAX_THREADS)
		if (pthr = LfStartThread(IDM_DCDIRECT))
		{
		    (global.aThr)[global.cThr] = pthr;
		    ++global.cThr;
		    pthr->dcType = id;

		    LfBringThreadToTop(pthr);
		    break;
		}
	    MyMessageBox(hwnd, szError1);
	}
	break;

    case IDM_DCMEMORY:

	if (global.cThr < MAX_THREADS)
	{
	    if (global.fFirstThread)
	    {
		global.bm.cx	    = (USHORT) (global.rcl.xRight - global.rcl.xLeft);
		global.bm.cy	    = (USHORT) (global.rcl.yTop   - global.rcl.yBottom);
		global.bm.cPlanes   = 0;
		global.bm.cBitCount = 0;
		global.fFirstThread = FALSE;
	    }
	    else if (global.fThreadInheritance)
	    {
		global.bm.cx	    = (USHORT) global.pThrTop->rcl.xRight;
		global.bm.cy	    = (USHORT) global.pThrTop->rcl.yTop;
		global.bm.cPlanes   = global.pThrTop->cPlanes;
		global.bm.cBitCount = global.pThrTop->cBitCount;
	    }
	    else
	    {
		global.bm.cx	    = (USHORT) (global.rcl.xRight - global.rcl.xLeft);
		global.bm.cy	    = (USHORT) (global.rcl.yTop   - global.rcl.yBottom);
		global.bm.cPlanes   = 0;
		global.bm.cBitCount = 0;
		if (!WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)BitmapParamsDlg, NULL,
			       IDR_BITMAPPARAMSDLG, NULL ))
		    break;	/* if user hit "cancel", don't start thread */
	    }

	    {
		PTHR pthr;

		if (pthr = LfStartThread(IDM_DCMEMORY))
		{
		    (global.aThr)[global.cThr] = pthr;
		    ++global.cThr;
		    pthr->dcType = id;

		    LfBringThreadToTop(pthr);
		    break;
		}
	    }
	}
	MyMessageBox(hwnd, szError1);
	break;

    case IDM_EDITTHREADPARAMS:
	if (global.pThrTop->dcType == IDM_DCMEMORY)
	{
	    USHORT cxT, cyT, cpT, cbT;

	    cxT = global.bm.cx	      = (USHORT) global.pThrTop->rcl.xRight;
	    cyT = global.bm.cy	      = (USHORT) global.pThrTop->rcl.yTop;
	    cpT = global.bm.cPlanes   = global.pThrTop->cPlanes;
	    cbT = global.bm.cBitCount = global.pThrTop->cBitCount;

	    if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)BitmapParamsDlg, NULL,
			   IDR_BITMAPPARAMSDLG, NULL ))
		if ((cxT != global.bm.cx)      ||
		    (cyT != global.bm.cy)      ||
		    (cpT != global.bm.cPlanes) ||
		    (cbT != global.bm.cBitCount))
		    LfResizePS(global.pThrTop);
	}
	else
	    MyMessageBox(hwnd, "No editable DC parameters.");
	break;

    case IDM_THR0TOTOP:
    case IDM_THR1TOTOP:
    case IDM_THR2TOTOP:
    case IDM_THR3TOTOP:
    case IDM_THR4TOTOP:
    case IDM_THR5TOTOP:
    case IDM_THR6TOTOP:
    case IDM_THR7TOTOP:
    case IDM_THR8TOTOP:
    case IDM_THR9TOTOP:
	{
	    USHORT i;

	    if ((i = id-IDM_THR0TOTOP) < global.cThr)
		LfBringThreadToTop(global.aThr[i]);
	}
	break;

    case IDM_KILLTOPTHREAD:
	{
	    PTHR pthr;

	    pthr = global.pThrTop;
	    if (global.cThr > 1)
	    {
		SHORT i;

		if ((i = global.iThrTop+1) == global.cThr)
		    i -= 2;
		LfBringThreadToTop(global.aThr[i]);
	    }
	    else
		LfBringThreadToTop(NULL);

	    LfKillThread(pthr);
	    LfBringThreadToTop(global.pThrTop);     /* update global.iThrTop and menu */

	    /* If no threads left, then start over. */
	    if (global.cThr == 0)
	    {
		global.fFirstThread = TRUE;
		WinSendMsg(hwnd, WM_COMMAND, (MPARAM)IDM_DCMEMORY, 0);
	    }
	}
	break;

    case IDM_KILLALLTHREADS:
	{
	    SHORT i;

	    /* Delete them all and start over. */
	    LfBringThreadToTop(NULL);
	    for (i = global.cThr-1; i >= 0; --i)
		LfKillThread(global.aThr[i]);
	    global.fFirstThread = TRUE;
	    WinSendMsg(hwnd, WM_COMMAND, (MPARAM)IDM_DCMEMORY, 0);
	}
	break;

    /********************************************************************/

    case IDM_AUTOSWITCH:
	UPDATE_MENU_BOOL(global.fAutoSwitch, IDM_AUTOSWITCH);
	if (global.fAutoSwitch)
	    WinStartTimer(global.hab, global.hwnd, IDT_AUTOSWITCH, global.usSwitchDelay);
	else
	    WinStopTimer(global.hab, global.hwnd, IDT_AUTOSWITCH);
	break;

    case IDM_SWITCHDELAY:
	WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)SwitchDelayDlg, NULL,
		       IDR_SWITCHDELAYDLG, NULL );
	break;

    case IDM_UPDATEMENUS:
	UPDATE_MENU_BOOL(global.fUpdateMenusOnThreadSwitch, IDM_UPDATEMENUS);
	break;

    /********************************************************************/

    case IDM_THREADINHERITANCE:
	UPDATE_MENU_BOOL(global.fThreadInheritance, IDM_THREADINHERITANCE);
	break;

    }
}




/************************************************************************
*
*   LfControlMenu
*
*   These commands cause global effects.  The top set cause immediate
*   results.  The bottom set determine how the program reacts to
*   certain events (mouse movement and button messages, timer messages).
*
************************************************************************/

BOOL
LfControlMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    BOOL fRedraw = FALSE;

    switch (id)
    {
    case IDM_CLEARBITMAP:
	LfClearRect(global.pThrTop, NULL);
	break;

    case IDM_COPYBITMAPTOSCREEN:
	if (global.pThrTop->hps)
	    WinInvalidateRect(global.hwnd, &(global.pThrTop)->rcl, FALSE);
	break;

    case IDM_SQUAREBITMAP:
	LfSquareBitmap(hwnd);
	break;

    case IDM_RESIZEBITMAPTOWINDOW:
	LfResizeBitmapToWindow();
	fRedraw = TRUE;
	break;

    /********************************************************************/

    case IDM_MOUSECHANGESRECURSION:
	UPDATE_MENU_BOOL(global.fMouseChangesRecursion, IDM_MOUSECHANGESRECURSION);
	break;

    case IDM_ANIMATEPTR:
	UPDATE_MENU_BOOL(global.fAnimatePtr, IDM_ANIMATEPTR);
	if (global.fAnimatePtr)
	    global.usPtrIncr = 1;
	else
	{
	    global.usPtrIncr = 0;
	    global.usCurPtr  = global.usUserPtr;
	}
	break;

    case IDM_PTRPREFERENCES:
	WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)PtrPreferencesDlg, NULL,
		       IDR_PTRPREFERENCESDLG, NULL );
	break;

    case IDM_TIMERDELAY:
	WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)TimerDelayDlg, NULL,
		       IDR_TIMERDELAYDLG, NULL );
	break;
    }

    return fRedraw;
}




/************************************************************************
*
*   LfFractalMenu
*
*   These commands select the fractal similarity transform, i.e. the
*   basic fractal shape, for a thread.
*
************************************************************************/

BOOL
LfFractalMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    BOOL fRedraw = FALSE;

    switch (id)
    {
    case IDM_SHARKTOOTH:
    case IDM_SAWTOOTH:
    case IDM_KOCH:
    case IDM_SPACEFILLER:
    case IDM_STOVE:
    case IDM_SQUAREWAVE:
    case IDM_HOURGLASS:
	LfSelectXform(hwnd, global.pThrTop, id);
	if (global.pThrTop->fFracRedraw)
	    fRedraw = TRUE;
	break;

    /********************************************************************/

    case IDM_FRACREDRAW:
	UPDATE_MENU_BOOL(global.pThrTop->fFracRedraw, IDM_FRACREDRAW);
	break;
    }

    return fRedraw;
}




/************************************************************************
*
*   LfPrimitiveMenu
*
*   These commands select the drawing primitive used to output the
*   fractal for a thread.
*
************************************************************************/

BOOL
LfPrimitiveMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    BOOL fRedraw = FALSE;

    switch (id)
    {
    case IDM_POLYLINE:
    case IDM_POLYFILLET:
    case IDM_POLYSPLINE:
    case IDM_POLYMARKER:
    case IDM_PEANO:
	LfSelectPrim(hwnd, global.pThrTop, id);
	if (global.pThrTop->fPrimRedraw)
	    fRedraw = TRUE;
	break;

    /********************************************************************/

    case IDM_PRIMREDRAW:
	UPDATE_MENU_BOOL(global.pThrTop->fPrimRedraw, IDM_PRIMREDRAW);
	break;
    }

    return fRedraw;
}




/************************************************************************
*
*   LfAttributesMenu
*
*   These commands allow the user to alter all the drawing attributes
*   of a fractal (color, mix mode, marker symbol, etc.), as well as
*   some fractal-related attributes such as width, height, offset,
*   number of sides to polygonal frame, and rotation.  Each thread
*   has an independent set of all these attributes.
*
************************************************************************/

BOOL
LfAttributesMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    BOOL fRedraw = FALSE;

    switch (id)
    {
    case IDM_LINEATTRS:
	if (global.fUpdateAttrs && (global.flLineAttrs & LFA_LINEALL))
	    MyMessageBox(hwnd, pszAttrs);
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)LineAttrsDlg, NULL,
		       IDR_LINEATTRSDLG, NULL ))
	    fRedraw = TRUE;
	break;

    case IDM_MARKERATTRS:
	if (global.fUpdateAttrs && (global.flMarkerAttrs & LFA_MARKALL))
	    MyMessageBox(hwnd, pszAttrs);
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)MarkerAttrsDlg, NULL,
		       IDR_MARKERATTRSDLG, NULL ))
	    fRedraw = TRUE;
	break;

    case IDM_AREAATTRS:
	if (global.fUpdateAttrs && (global.flAreaAttrs & LFA_AREAALL))
	    MyMessageBox(hwnd, pszAttrs);
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)AreaAttrsDlg, NULL,
		       IDR_AREAATTRSDLG, NULL ))
	    fRedraw = TRUE;
	break;

    case IDM_IMAGEATTRS:
	if (global.fUpdateAttrs && (global.flImageAttrs & LFA_IMAGEALL))
	    MyMessageBox(hwnd, pszAttrs);
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)ImageAttrsDlg, NULL,
		       IDR_IMAGEATTRSDLG, NULL ))
	    fRedraw = TRUE;
	break;

    case IDM_MISCATTRS:
	if (global.fUpdateAttrs && (global.flMiscAttrs & (LFA_RECURSION |
		LFA_CPTMAX | LFA_POLYGONSIDES | LFA_ROTATION)))
	    MyMessageBox(hwnd, pszAttrs);
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)MiscAttrsDlg, NULL,
		       IDR_MISCATTRSDLG, NULL ))
	    fRedraw = TRUE;
	break;

    case IDM_DIMENSIONS:
	if (global.fUpdateAttrs && (global.flMiscAttrs & (LFA_XOFF |
		LFA_YOFF | LFA_XSCALE | LFA_YSCALE)))
	    MyMessageBox(hwnd, pszAttrs);
	if (WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP)DimensionsDlg, NULL,
		       IDR_DIMENSIONSDLG, NULL ))
	    fRedraw = TRUE;
	break;

    case IDM_DRAGDIMS:
	global.fTracking = TRUE;
	break;

    /********************************************************************/

    case IDM_ATTRREDRAW:
	UPDATE_MENU_BOOL(global.pThrTop->fAttrRedraw, IDM_ATTRREDRAW);
	break;
    }

    if (fRedraw)
	if (global.pThrTop->fAttrRedraw)
	    return TRUE;
    return FALSE;
}




/************************************************************************
*
*   LfEnvironmentMenu
*
*   These commands alter the way a thread reacts to system and program
*   events by toggling certain flags.  Each thread has its own
*   environment flags.
*
************************************************************************/

VOID
LfEnvironmentMenu(hwnd, id)
HWND hwnd;
USHORT id;
{
    switch (id)
    {
    case IDM_AUTORESIZE:
	UPDATE_MENU_BOOL(global.pThrTop->fAutoSizePS, IDM_AUTORESIZE);
	break;

    case IDM_AUTOSELECTDIMS:
	UPDATE_MENU_BOOL(global.pThrTop->fAutoSelectDims, IDM_AUTOSELECTDIMS);
	break;

    case IDM_CLEARONREDRAW:
	UPDATE_MENU_BOOL(global.pThrTop->fClearOnRedraw, IDM_CLEARONREDRAW);
	break;

    case IDM_AUTOSTARTREDRAW:
	UPDATE_MENU_BOOL(global.pThrTop->fAutoStartRedraw, IDM_AUTOSTARTREDRAW);
	if (global.pThrTop->fAutoStartRedraw)
	    LfStartRedrawTimer();
	break;

    case IDM_COLLECTBOUNDS:
	UPDATE_MENU_BOOL(global.pThrTop->fCollectBounds, IDM_COLLECTBOUNDS);
	break;

    }

    hwnd;	/* this prevents an "unused variable" warning message */
}




/************************************************************************
*
*   LfSelectDimension
*
*   Set the dimensions of the next fractal by dragging a rectangle
*   on the screen.
*
************************************************************************/

VOID
LfSelectDimension(hwnd, pt)
HWND hwnd;
POINTS pt;
{
    TRACKINFO ti;
    LONG lXExt, lYExt;


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
	lXExt = (global.pThrTop->rcl).xRight - (global.pThrTop->rcl).xLeft;
	lYExt = (global.pThrTop->rcl).yTop   - (global.pThrTop->rcl).yBottom;

	global.dblXScale = (double)(ti.rclTrack.xRight - ti.rclTrack.xLeft)   / (double)lXExt;
	global.dblYScale = (double)(ti.rclTrack.yTop   - ti.rclTrack.yBottom) / (double)lYExt;
	global.dblXOff	 = (double) ti.rclTrack.xLeft	/ (double)lXExt;
	global.dblYOff	 = (double) ti.rclTrack.yBottom / (double)lYExt;

	global.flMiscAttrs |= LFA_XSCALE | LFA_YSCALE | LFA_XOFF | LFA_YOFF;

	global.pThrTop->fUpdateAttrs = TRUE;
	global.fUpdateAttrs	     = TRUE;
    }
}




/************************************************************************
*
*   LfSquareBitmap
*
*   Resize the window horizontally to so that the client area is a
*   square.
*
************************************************************************/

VOID
LfSquareBitmap(hwnd)
HWND hwnd;
{
    SWP swp;
    HWND hwndDesk;
    RECTL rcl;

    if (hwnd == global.hwnd)
	if (hwndDesk = WinQueryDesktopWindow(global.hab, NULL))
	    if (WinQueryWindowPos(global.hwndFrame, (PSWP)&swp))
	    {
		rcl.xLeft   = 0L;
		rcl.yBottom = 0L;
		rcl.xRight  = swp.cx;
		rcl.yTop    = swp.cy;
		if (WinCalcFrameRect(global.hwndFrame, &rcl, TRUE))
		{
		    rcl.xRight	= (SHORT)((LONG)rcl.yTop   *
				  global.pThrTop->AspectRatio.lHorz /
				  global.pThrTop->AspectRatio.lVert);

		    if (WinCalcFrameRect(global.hwndFrame, &rcl, FALSE))
			WinSetWindowPos(global.hwndFrame, 0, swp.x, swp.y,
			    (SHORT)rcl.xRight, (SHORT)rcl.yTop,
			    SWP_SIZE);
		}
	    }
}




/************************************************************************
*
*   LfResizeBitmapToWindow
*
*   Resize the bitmap to match the windows dimensions exactly.
*
************************************************************************/

VOID
LfResizeBitmapToWindow()
{
    global.bm.cx = (USHORT) (global.rcl.xRight - global.rcl.xLeft);
    global.bm.cy = (USHORT) (global.rcl.yTop   - global.rcl.yBottom);
    global.bm.cPlanes	= global.pThrTop->cPlanes;
    global.bm.cBitCount = global.pThrTop->cBitCount;

    LfResizePS(global.pThrTop);
}




/************************************************************************
*
*   LfSelect
*
*   Select a rectangle to cut or copy to the clipboard.
*
************************************************************************/

VOID
LfSelect(hwnd, pt)
HWND hwnd;
POINTS pt;
{
    HPS hps;
    TRACKINFO ti;
    PRECTL prcl;


    if (global.fShowSelectRc)
    {
	global.fShowSelectRc = FALSE;
	hps = WinGetPS(hwnd);
	if (global.fTempSelect)
	{
	    prcl = &global.rclSelect;
	    global.fTempSelect = FALSE;
	}
	else
	    prcl = &global.rclCutCopy;
	LfShowSelectRc(hps, prcl);
	WinReleasePS(hps);
    }

    WinSendMsg(global.hwndFrame, WM_QUERYTRACKINFO, (MPARAM)TF_MOVE, (MPARAM)&ti);

    ti.cxBorder = 1;
    ti.cyBorder = 1;
    ti.rclTrack.xLeft	= (LONG)pt.x;
    ti.rclTrack.yBottom = (LONG)pt.y;
    ti.rclTrack.xRight	= (LONG)pt.x;
    ti.rclTrack.yTop	= (LONG)pt.y;
    ti.fs = TF_RIGHT | TF_TOP;
    ti.ptlMinTrackSize.x = 1L;
    ti.ptlMinTrackSize.y = 1L;

    if (WinTrackRect(hwnd, NULL, &ti))
    {
	global.rclSelect.xLeft	 = ti.rclTrack.xLeft;
	global.rclSelect.yBottom = ti.rclTrack.yBottom;
	global.rclSelect.xRight  = ti.rclTrack.xRight;
	global.rclSelect.yTop	 = ti.rclTrack.yTop;

	global.fShowSelectRc = TRUE;
	global.fTempSelect   = TRUE;
	hps = WinGetPS(hwnd);
	LfShowSelectRc(hps, &global.rclSelect);
	WinReleasePS(hps);
    }
}




/************************************************************************
*
*   LfSelectAll
*
*   Select the entire surface to cut or copy to the clipboard.
*
************************************************************************/

VOID
LfSelectAll(hwnd)
HWND hwnd;
{
    HPS hps;

    global.rclSelect = global.pThrTop->rcl;

    global.fShowSelectRc = TRUE;
    global.fTempSelect	 = TRUE;
    hps = WinGetPS(hwnd);
    LfShowSelectRc(hps, &global.rclSelect);
    WinReleasePS(hps);
}




/************************************************************************
*
*   LfCut
*
*   Cuts the selected rectangle from the bitmap and puts it in the
*   clipboard.
*
************************************************************************/

VOID
LfCut(hwnd)
HWND hwnd;
{
    if (LfCopy(hwnd))
	LfClearRect(global.pThrTop, &global.rclCutCopy);
}




/************************************************************************
*
*   LfCopy
*
*   Copies the selected rectangle from the bitmap the clipboard.
*
************************************************************************/

BOOL
LfCopy(hwnd)
HWND hwnd;
{
    HPS hps;
    PRECTL prcl;
    SIZEL size;
    BITMAPINFOHEADER bminfo;
    POINTL  aptl[4];



    global.fShowSelectRc = FALSE;
    hps = WinGetPS(hwnd);
    if (global.fTempSelect)
    {
	prcl = &global.rclSelect;
	global.fTempSelect = FALSE;
    }
    else
	prcl = &global.rclCutCopy;
    LfShowSelectRc(hps, prcl);
    WinReleasePS(hps);

    if (global.fUseClipbrd)
    {
	if (WinOpenClipbrd(global.hab))
	{
	    global.fHaveSelectRc = TRUE;
	    global.rclCutCopy.xLeft   = global.rclSelect.xLeft;
	    global.rclCutCopy.yBottom = global.rclSelect.yBottom;
	    global.rclCutCopy.xRight  = global.rclSelect.xRight;
	    global.rclCutCopy.yTop    = global.rclSelect.yTop;

	    /* copy bits here */

	    WinCloseClipbrd(global.hab);

	    return TRUE;
	}
    }
    else
    {
	global.fHaveSelectRc = TRUE;
	global.rclCutCopy.xLeft   = global.rclSelect.xLeft;
	global.rclCutCopy.yBottom = global.rclSelect.yBottom;
	global.rclCutCopy.xRight  = global.rclSelect.xRight;
	global.rclCutCopy.yTop	  = global.rclSelect.yTop;

	if (global.fHaveCutCopyDC)
	{
	    GpiSetBitmap(global.CutCopyPS, NULL);
	    GpiDeleteBitmap(global.CutCopyBM);
	    GpiDestroyPS(global.CutCopyPS);
	    DevCloseDC(global.CutCopyDC);
	    global.fHaveCutCopyDC = FALSE;
	}

	global.CutCopyDC = DevOpenDC(global.hab, OD_MEMORY, "*", 0L, NULL, NULL);
	if (!global.CutCopyDC)
	    return FALSE;

	size.cx = 0L;
	size.cy = 0L;
	global.CutCopyPS = GpiCreatePS(global.hab, global.CutCopyDC, &size,
				 PU_PELS|GPIT_MICRO|GPIA_ASSOC);
	if (!global.CutCopyPS)
	    goto lfc_error_close_dc;


	bminfo.cbFix = sizeof(BITMAPINFOHEADER);
	bminfo.cx = (USHORT) (global.rclCutCopy.xRight - global.rclCutCopy.xLeft);
	bminfo.cy = (USHORT) (global.rclCutCopy.yTop   - global.rclCutCopy.yBottom);
	bminfo.cPlanes	 = global.pThrTop->cPlanes;
	bminfo.cBitCount = global.pThrTop->cBitCount;
	global.CutCopyBM = GpiCreateBitmap(global.CutCopyPS, &bminfo, 0L, 0L, 0L);
	if (!global.CutCopyBM)
	    goto lfc_error_destroy_ps;
	GpiSetBitmap(global.CutCopyPS, global.CutCopyBM);

	aptl[0].x = 0L;
	aptl[0].y = 0L;
	aptl[1].x = global.rclCutCopy.xRight - global.rclCutCopy.xLeft;
	aptl[1].y = global.rclCutCopy.yTop   - global.rclCutCopy.yBottom;
	aptl[2].x = global.rclCutCopy.xLeft;
	aptl[2].y = global.rclCutCopy.yBottom;

	GpiBitBlt(global.CutCopyPS, global.pThrTop->hps, 3L, aptl, ROP_SRCCOPY, NULL);

	global.fHaveCutCopyDC = TRUE;
	return TRUE;


lfc_error_destroy_ps:
	GpiDestroyPS(global.CutCopyPS);
lfc_error_close_dc:
	DevCloseDC(global.CutCopyDC);
	return FALSE;
    }

    return FALSE;
}




/************************************************************************
*
*   LfPaste
*
*   Pastes the selected rectangle from the clipboard to the bitmap.
*   Hides the selection rectangle before calling WinTrackRect.
*
************************************************************************/

VOID
LfPaste(hwnd)
HWND hwnd;
{
    HPS hps;
    PRECTL prcl;
    TRACKINFO ti;
    POINTL    aptl[4];


    if (global.fShowSelectRc)
    {
	global.fShowSelectRc = FALSE;
	hps = WinGetPS(hwnd);
	if (global.fTempSelect)
	{
	    prcl = &global.rclSelect;
	    global.fTempSelect = FALSE;
	}
	else
	    prcl = &global.rclCutCopy;
	LfShowSelectRc(hps, prcl);
	WinReleasePS(hps);
    }

    WinSendMsg(global.hwndFrame, WM_QUERYTRACKINFO, (MPARAM)TF_MOVE, (MPARAM)&ti);

    ti.cxBorder = 1;
    ti.cyBorder = 1;
    ti.rclTrack.xLeft	= global.rclCutCopy.xLeft;
    ti.rclTrack.yBottom = global.rclCutCopy.yBottom;
    ti.rclTrack.xRight	= global.rclCutCopy.xRight;
    ti.rclTrack.yTop	= global.rclCutCopy.yTop;
    ti.fs = TF_MOVE;
    ti.ptlMinTrackSize.x = 1L;
    ti.ptlMinTrackSize.y = 1L;

    if (WinTrackRect(hwnd, NULL, &ti))
    {
	if (global.fUseClipbrd)
	{
	    if (WinOpenClipbrd(global.hab))
	    {
		/* copy bits here */
		WinCloseClipbrd(global.hab);
	    }
	}
	else
	{
	    aptl[0].x = ti.rclTrack.xLeft;
	    aptl[0].y = ti.rclTrack.yBottom;
	    aptl[1].x = ti.rclTrack.xLeft   + (global.rclCutCopy.xRight - global.rclCutCopy.xLeft);
	    aptl[1].y = ti.rclTrack.yBottom + (global.rclCutCopy.yTop	- global.rclCutCopy.yBottom);
	    aptl[2].x = 0L;
	    aptl[2].y = 0L;

	    GpiBitBlt(global.pThrTop->hps, global.CutCopyPS, 3L, aptl, global.ulPasteROP, NULL);
	    WinInvalidateRect(global.hwnd, &(ti.rclTrack), FALSE);
	}
    }
}




/************************************************************************
*
*   LfSelectXform
*
*   Select a similarity transform.
*
************************************************************************/

VOID
LfSelectXform(hwnd, pthr, id)
HWND hwnd;
PTHR pthr;
USHORT id;
{
    BOOL fAlreadySaidIt = FALSE;


    if ((id != pthr->usCurXform) ||
       ((id != global.usCurXform) && global.fUpdateAttrs))
    {
	if (global.fUpdateAttrs && (global.flMiscAttrs & LFA_CURXFORM))
	{
	    MyMessageBox(hwnd, pszAttrs);
	    fAlreadySaidIt = TRUE;
	}

	global.flMiscAttrs |= LFA_CURXFORM;

	if (LfIsThreadTop(pthr))
	{
	    if (fAlreadySaidIt)
		UNCHECK_MENU_ITEM(global.hwndFrame, global.usCurXform);
	    else
		UNCHECK_MENU_ITEM(global.hwndFrame, pthr->usCurXform);
	    global.usCurXform = id;
	    CHECK_MENU_ITEM(global.hwndFrame, id);
	}
	else
	    global.usCurXform = id;

	pthr->fUpdateAttrs  = TRUE;
	global.fUpdateAttrs = TRUE;
    }

    if (pthr->fAutoSelectDims)
    {
	if (global.fUpdateAttrs && (global.flMiscAttrs & (LFA_XSCALE |
		 LFA_YSCALE | LFA_XOFF | LFA_YOFF | LFA_POLYGONSIDES)))
	    if (!fAlreadySaidIt)
		MyMessageBox(hwnd, pszAttrs);

	global.dblXScale      = aXform[id - IDM_SHARKTOOTH].xScale;
	global.dblYScale      = aXform[id - IDM_SHARKTOOTH].yScale;
	global.dblXOff	      = aXform[id - IDM_SHARKTOOTH].xOff;
	global.dblYOff	      = aXform[id - IDM_SHARKTOOTH].yOff;
	global.usPolygonSides = aXform[id - IDM_SHARKTOOTH].usSides;

	global.flMiscAttrs |= LFA_XSCALE | LFA_YSCALE | LFA_XOFF | LFA_YOFF |
			      LFA_POLYGONSIDES;
	pthr->fUpdateAttrs  = TRUE;
	global.fUpdateAttrs = TRUE;
    }
}




/************************************************************************
*
*   LfSelectPrim
*
*   Select a drawing primitive.
*
************************************************************************/

VOID
LfSelectPrim(hwnd, pthr, id)
HWND hwnd;
PTHR pthr;
USHORT id;
{
    BOOL fAlreadySaidIt = FALSE;


    if ((id != pthr->usCurPrim) ||
       ((id != global.usCurPrim) && global.fUpdateAttrs))
    {
	if (global.fUpdateAttrs && (global.flMiscAttrs & LFA_CURPRIM))
	{
	    MyMessageBox(hwnd, "Previous attributes not read yet.");
	    fAlreadySaidIt = TRUE;
	}

	if (LfIsThreadTop(pthr))
	{
	    if (fAlreadySaidIt)
		UNCHECK_MENU_ITEM(global.hwndFrame, global.usCurPrim);
	    else
		UNCHECK_MENU_ITEM(global.hwndFrame, pthr->usCurPrim);
	    global.usCurPrim = id;
	    CHECK_MENU_ITEM(global.hwndFrame, id);
	}
	else
	    global.usCurPrim = id;

	global.flMiscAttrs |= LFA_CURPRIM;
	pthr->fUpdateAttrs  = TRUE;
	global.fUpdateAttrs = TRUE;
    }
}
