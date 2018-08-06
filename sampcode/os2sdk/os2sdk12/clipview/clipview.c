/*
 * CLIPVIEW.C -- Clipboard Viewing application
 * Created by Microsoft Corporation, 1989
 *
 * This program registers itself as the clipboard viewer, if no clipboard
 * viewer exists.  Then, it intercepts WM_DRAWCLIPBOARD messages.
 *
 * This file contains the routines which handle the client/frame windows,
 * the dialog routines, and the clipboard rendering code.
 */
#define	INCL_GPIBITMAPS
#define	INCL_GPIMETAFILES
#define INCL_WINATOM
#define	INCL_WINCLIPBOARD
#define	INCL_WINFRAMEMGR
#define	INCL_WINLISTBOXES
#define	INCL_WINMENUS
#define	INCL_WINMLE
#define	INCL_WINSCROLLBARS
#define	INCL_WINSYS
#define	INCL_WINWINDOWMGR
#include <os2.h>
#include <string.h>
#include "clipview.h"
/*
 * Globals
 */
BITMAPINFOHEADER vbmp;			// Dimensions of current BITMAP
BOOL	vfUpdate	= FALSE;	// Are we updating the clipboard?
BOOL	vfViewBitmap	= FALSE;	// Are we currently viewing a...?
HAB	vhab;				// Anchor block
HDC	vhdcMemory;			// A memory DC for BitBlt-ing images
HDC	vhdcWindow	= NULL; 	// Client window DC
HMQ	vhmqClip;			// Message queue
HPS	vhpsMemory;			// A PS associated with vhdcMemory
HWND	vhwndClient;			// Main client area
HWND	vhwndClipFrame = NULL;		// Main frame window
HWND	vhwndHSB	= NULL; 	// Horizontal scroll bar
HWND	vhwndMLE	= NULL; 	// Handle to the MLE
HWND	vhwndTitlebar	= NULL; 	// Title-bar handle
HWND	vhwndVSB	= NULL; 	// Vertical scroll bar
SHORT	vcMaxHSB;			// Maximum scroll range for HSB
SHORT	vcMaxVSB;			// ...and for the VSB
SHORT	vcUpdate	= -1;		// Counter for scroll bar updating
USHORT	vausFormats[MAXFORMATS];	// All available formats
USHORT	vcFmts; 			// How many formats?
USHORT	vusFormat;			// What is the current format?
USHORT	vfsFmtInfo;			// Clipboard Format Information
/*
    Macros
*/
#define LOADSTRING(id, sz) WinLoadString(vhab, (HMODULE) NULL, id, MAXLEN, sz)
#define MESSAGE(sz) WinMessageBox(HWND_DESKTOP, vhwndClient, sz, NULL, 0, \
			MB_OK | MB_ICONASTERISK | MB_SYSTEMMODAL);
/*
 * Main routine...initializes window and message queue
 */
int cdecl main( ) {
    QMSG    qmsg;		    /* Message queue */
    ULONG   ctldata;		    /* FCF_ flags */
    BOOL    fViewer;		    /* Does a viewer already exist? */
    UCHAR   szAlready[MAXLEN];	    /* Already extant... message */
    UCHAR   szClassName[MAXLEN];    /* New class name */
    /*
	Start up our PM application
    */
    vhab = WinInitialize(0);
    vhmqClip = WinCreateMsgQueue(vhab, 0);
    /*
	We create the client window first to try to avoid
	synchronization problems.
    */
    LOADSTRING(IDS_CLIPCLASS, szClassName);
    if (!WinRegisterClass( vhab, (PCH)szClassName, (PFNWP)ClipWndProc,
		CS_SIZEREDRAW, 0))
	return( 0 );
    /*
	Create the window (hidden)
    */
    ctldata = (FCF_STANDARD | FCF_HORZSCROLL | FCF_VERTSCROLL)
			    & ~(FCF_ACCELTABLE);

    vhwndClipFrame = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &ctldata,
					 szClassName, "",
					 WS_VISIBLE, (HMODULE) NULL, ID_RESOURCE,
					 (PHWND) &vhwndClient );
    /*
	If there is no other clipboard viewer...
    */
    if (fViewer = !WinQueryClipbrdViewer(vhab, FALSE)) {
	/*
	    ...we'll be the viewer.  Show the clipboard window.
	*/
	WinSetClipbrdViewer(vhab, vhwndClient);
	/*
	    Poll messages from event queue
	*/
	while( WinGetMsg( vhab, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
	    WinDispatchMsg( vhab, (PQMSG)&qmsg );
	/*
	    Stop being the clipboard viewer.
	*/
	if (vhwndMLE)
	    WinDestroyWindow(vhwndMLE);
	WinSetClipbrdViewer(vhab, NULL);
    } else {
	/*
	    ...otherwise, notify the user, then terminate.
	*/
	LOADSTRING(IDS_ALREADY, szAlready);
	MESSAGE(szAlready);
    }
    /*
	Clean up
    */
    WinDestroyWindow( vhwndClipFrame );
    WinDestroyMsgQueue( vhmqClip );
    WinTerminate( vhab );

    return !fViewer;
}

MRESULT CALLBACK ClipWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*

 * This routine processes WM_COMMAND, WM_CREATE, WM_DRAWCLIPBOARD, WM_PAINT.
 * Everything else is passed to the Default Window Procedure.
 */
    HPS		hpsWindow;
    RECTL	rcl;
    SWP 	swp;
    SIZEL	sizl;
    UCHAR	szMessage[MAXLEN];

    switch (msg) {

	case WM_CREATE:
	    /*
		Create a memory DC/PS to BitBlt BITMAPs around.
	    */
	    sizl.cx = sizl.cy = 0L;
	    vhdcMemory = DevOpenDC(vhab, OD_MEMORY, "*", 0L, NULL, NULL);
	    vhpsMemory = GpiCreatePS(vhab, vhdcMemory, &sizl,
		GPIA_ASSOC | GPIF_DEFAULT | GPIT_MICRO | PU_PELS);
	    break;

	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {
		/*
		    About... dialog box
		*/
		case IDM_ABOUT:
		    WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc,
			      (HMODULE) NULL, IDD_ABOUT, NULL);
		    return 0;
		/*
		    Render... dialog box
		*/
		case IDM_RENDER:
		    WinDlgBox(HWND_DESKTOP, hwnd, RenderDlgProc,
			      (HMODULE) NULL, IDD_RENDER, NULL);
		    return 0;
		/*
		    Save... dialog box
		*/
		case IDM_SAVE:
		    if (!SaveClipboard(hwnd, vusFormat)) {
			LOADSTRING(IDS_NOTSAVED, szMessage);
			MESSAGE(szMessage);
		    }
		    return 0;

		default: break;
	    }
	    break;

	case WM_ERASEBACKGROUND:
	    return (MRESULT) TRUE;
	    break;

	case WM_PAINT:
	    /* Open the presentation space */
	    hpsWindow = WinBeginPaint(hwnd, NULL, &rcl);

	    /* Fill in the background */
	    WinFillRect(hpsWindow, &rcl, CLR_BACKGROUND);

	    /* Paint in the clipboard */
	    UpdateScreen(hwnd, vusFormat);

	    /* Finish painting */
	    WinEndPaint(hpsWindow);
	    break;

	case WM_DRAWCLIPBOARD:
	    /* Update the clipboard contents */
	    GetAllFormats();
	    vfUpdate = TRUE;
	    WinPostMsg(hwnd, WM_PAINT, 0L, 0L);
	    break;

	case WM_HSCROLL:
	    if (vfViewBitmap) {
		/*
		    Handle the appropriate scrolling messages
		*/
		DoScrolling(hwnd, TRUE, HIUSHORT(mp2));
	    } else
		/*
		    If an ownerdraw format, let the owner handle it.
		*/
		SendOwnerMsg(WM_HSCROLLCLIPBOARD, (MPARAM) hwnd, mp2);
	    break;

	case WM_VSCROLL:
	    if (vfViewBitmap) {
		/*
		    Handle the appropriate scrolling messages
		*/
		DoScrolling(hwnd, FALSE, HIUSHORT(mp2));
	    } else
		/*
		    If an ownerdraw format, let the owner handle it.
		*/
		SendOwnerMsg(WM_VSCROLLCLIPBOARD, (MPARAM) hwnd, mp2);
	    break;

	case WM_SIZE:
	    /*
		If the MLE is processing a text selector,
		tell it to resize itself.  If we have
		owner-draw data, tell the clipboard owner.
		If we have a BITMAP, readjust the scroll
		bar ranges.
	    */
	    if (vhwndMLE) {
		WinQueryWindowPos(vhwndMLE, &swp);
		swp.cx = SHORT1FROMMP(mp2);
		swp.cy = SHORT2FROMMP(mp2);
		WinSetMultWindowPos(vhab, &swp, 1);
	    } else if (vfViewBitmap) {
		WinQueryWindowPos(hwnd, &swp);
		if ((vcMaxHSB = vbmp.cx - swp.cx) < 0)
		    vcMaxHSB = 0;
		if ((vcMaxVSB = vbmp.cy - swp.cy) < 0)
		    vcMaxVSB = 0;
		WinSendMsg(vhwndHSB, SBM_SETSCROLLBAR,
		    0L, MPFROM2SHORT(0, vcMaxHSB));
		WinSendMsg(vhwndVSB, SBM_SETSCROLLBAR,
		    MPFROMSHORT(vcMaxVSB),
		    MPFROM2SHORT(0, vcMaxVSB));
	    } else {
		rcl.xLeft = rcl.yBottom = 0L;
		rcl.xLeft = (LONG) SHORT1FROMMP(mp2) - 1;
		rcl.yTop  = (LONG) SHORT2FROMMP(mp2) - 1;
		SendOwnerMsg(WM_SIZECLIPBOARD, (MPARAM) hwnd, &rcl);
	    }
	    break;

	default:
	    return WinDefWindowProc(hwnd, msg, mp1, mp2);
	    break;
    }
    return 0L;
}

MRESULT CALLBACK RenderDlgProc(HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
/*
    Render... dialog procedure
*/
    HWND	hwndListbox;		/* Listbox of possible formats */
    UCHAR	szFmtName[MAXLEN];	/* Format name */
    UCHAR	szMessage[MAXLEN];
    USHORT	i;
    USHORT	usFormat;		/* Format to render */
    MRESULT	mrItem; 		/* Which listbox item selected? */

    switch(msg) {

	case WM_INITDLG:
	    /*
		Put all the possible formats into the listbox, and
		select the first item by default.
	    */
	    hwndListbox = WinWindowFromID(hwndDlg, IDL_RENDER);
	    WinSendMsg(hwndListbox, LM_DELETEALL, 0L, 0L);
	    for (i = 0; i < vcFmts; i++) {
		GetFormatName(vausFormats[i], szFmtName);
		WinSendMsg(hwndListbox, LM_INSERTITEM,
			MPFROMSHORT(LIT_END), MPFROMP((PVOID) szFmtName));
	    }
	    WinSendMsg(hwndListbox, LM_SELECTITEM, 0L, MPFROMSHORT(TRUE));
	    break;

	case WM_CONTROL:
	    /*
		If the user makes a selection, quit!
	    */
	    if ((SHORT1FROMMP(mp1) == IDL_RENDER)
		&& (SHORT2FROMMP(mp1) == LN_ENTER))
		    WinPostMsg(hwndDlg, WM_COMMAND, MPFROMSHORT(DID_OK), 0L);
	    break;

	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK:
		    /*
			Since the user chose a selection, try to render it.
		    */
		    hwndListbox = WinWindowFromID(hwndDlg, IDL_RENDER);
		    mrItem = WinSendMsg(hwndListbox, LM_QUERYSELECTION, 0L, 0L);
		    if (mrItem != (MRESULT) LIT_NONE) {
			usFormat = vausFormats[SHORT1FROMMR(mrItem)];
			if (usFormat != vusFormat) {
			    /*
				If the clipboard format is not rendered,
				tell the user.
			    */
			    vfUpdate = TRUE;
			    if (!UpdateScreen(vhwndClient, usFormat)) {
				LOADSTRING(IDS_NODISPLAY, szMessage);
				MESSAGE(szMessage);
			    }
			}
		    }

		    /* fall through */

		case DID_CANCEL:
		    WinDismissDlg(hwndDlg, TRUE);

		default: break;
	    }
	default: return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    }
    return FALSE;
}

MRESULT CALLBACK AboutDlgProc(HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
/*
    About... dialog procedure
*/
    switch(msg) {
	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK: WinDismissDlg(hwndDlg, TRUE);
		default: break;
	    }
	default: return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    }
    return FALSE;
}

VOID ReadSelector(HWND hwndMLE, PSZ pszText) {
/*
    Compute the length of the text selector, in bytes.
    Allocate space, and copy the text selector into an MLE.
*/
    IPT ipt;
    ULONG ulcch = 0;
    PSZ	pszCounter;

    pszCounter = pszText;
    while (*pszCounter++) ulcch++;
    WinSendMsg(hwndMLE, MLM_FORMAT, MPFROMSHORT(MLFIE_CFTEXT), 0L);
    WinSendMsg(hwndMLE, MLM_SETIMPORTEXPORT, pszText, (MPARAM) ulcch);
    WinSendMsg(hwndMLE, MLM_IMPORT, &ipt, (MPARAM) ulcch);
}

VOID FixFrame(VOID) {
/*
    This routine tells the frame to update the scroll bars.

    First, make it so that the scroll bars cannot update themselves.
    Let the frame update the controls.  Then, re-enable the scroll bars.
*/
    if (!(vcUpdate--)) {
	WinEnableWindowUpdate(vhwndHSB, FALSE);
	WinEnableWindowUpdate(vhwndVSB, FALSE);
    }

    WinSendMsg(vhwndClipFrame, WM_UPDATEFRAME, MPFROMLONG(FCF_HORZSCROLL), 0L);
    WinSendMsg(vhwndClipFrame, WM_UPDATEFRAME, MPFROMLONG(FCF_VERTSCROLL), 0L);

    if (!(++vcUpdate)) {
	WinEnableWindowUpdate(vhwndHSB, TRUE);
	WinEnableWindowUpdate(vhwndVSB, TRUE);
    }
}

VOID NeedScrollBars(BOOL fNeed) {
/*
    This routine hides changes the scroll bar state to correspond with
    fNeed, showing or hiding them as necessary.
*/
    static BOOL fNeeded = TRUE;		/* The last scroll bar state */

    /*
	Get the scroll bar handles, if we haven't already.
    */
    if (!vhwndHSB) {
	vhwndHSB = WinWindowFromID(vhwndClipFrame, FID_HORZSCROLL);
	vhwndVSB = WinWindowFromID(vhwndClipFrame, FID_VERTSCROLL);
    }
    /*
	Case 1:  We need scroll bars, so enable them.
    */
    if (fNeed) {
	if (!fNeeded) {
	    WinSetParent(vhwndHSB, vhwndClipFrame, TRUE);
	    WinSetParent(vhwndVSB, vhwndClipFrame, TRUE);
	    FixFrame();
	}
    /*
	Case 2:  We don't need scroll bars, so hide them.
    */
    } else {
	if (fNeeded) {
	    WinSetParent(vhwndHSB, HWND_OBJECT, TRUE);
	    WinSetParent(vhwndVSB, HWND_OBJECT, TRUE);
	    FixFrame();
	}
    }
    /*
	Save state for next invocation
    */
    fNeeded = fNeed;
}

/*
    RenderFormat()

    Input:		Clipboard format to render, and handle to client area
    Side effects:	Renders the image in the client area
*/
BOOL RenderFormat(HWND hwnd, USHORT usFormat) {
    BOOL    fRendered = TRUE;
    HMF	    hmfCopy;
    HPS     hpsWindow;
    LONG    alOptions[8];
    RECTL   rclWindow;
    SIZEL   sizl;
    SWP	    swpWindow;
    ULONG   hItem;
    POINTL  aptl[3];
    /*
	Open the clipboard
    */
    if (!WinOpenClipbrd(vhab))
	return FALSE;
    /*
	Open up the window DC and PS
    */
    if (!vhdcWindow)
	vhdcWindow = WinOpenWindowDC(hwnd);

    sizl.cx = sizl.cy = 0L;
    hpsWindow = GpiCreatePS(vhab, vhdcWindow, &sizl, GPIA_ASSOC | PU_ARBITRARY);
    /*
	Enable the scroll bars, if necessary.  This affects the size
	of the client area.
    */
    if (vfUpdate)
	NeedScrollBars( (vfViewBitmap =
		(usFormat == CF_BITMAP) || (usFormat == CF_DSPBITMAP)) );

    WinQueryWindowRect(hwnd, &rclWindow);
    /*
	Get the clipboard data
    */
    WinQueryClipbrdFmtInfo(vhab, usFormat, &vfsFmtInfo);
    if (!(hItem = WinQueryClipbrdData(vhab, usFormat))) {
	fRendered = FALSE;
    } else {
      /*
	Display the new format, as appropriate.
      */
      switch (usFormat) {
	case CF_TEXT:
	case CF_DSPTEXT:
	    if (vfUpdate) {
		/*
		    Create a new MLE and read the text into it.
		*/
		vhwndMLE = WinCreateWindow(hwnd, WC_MLE, "",
		    WS_VISIBLE | MLS_READONLY | MLS_HSCROLL | MLS_VSCROLL,
		    0, 0,
		    (SHORT) rclWindow.xRight, (SHORT) rclWindow.yTop,
		    hwnd, HWND_TOP, 0, NULL, NULL);

		ReadSelector(vhwndMLE, MAKEP((SEL) hItem, 0));
	    }
	    break;

	case CF_BITMAP:
	case CF_DSPBITMAP:
	    if (vfUpdate) {
		/*
		    Get the BITMAP dimensions, for scroll bar processing
		*/
		if (GpiQueryBitmapParameters((HBITMAP) hItem, &vbmp)
			!= GPI_OK) {
		    return FALSE;
		}
		/*
		    Set the scroll bar ranges from 0 to vbmp.max - client.max
		*/
		WinQueryWindowPos(hwnd, &swpWindow);

		if ((vcMaxHSB = vbmp.cx - swpWindow.cx) < 0)
		    vcMaxHSB = 0;
		if ((vcMaxVSB = vbmp.cy - swpWindow.cy) < 0)
		    vcMaxVSB = 0;
		WinSendMsg(vhwndHSB, SBM_SETSCROLLBAR,
		    0L, MPFROM2SHORT(0, vcMaxHSB));
		WinSendMsg(vhwndVSB, SBM_SETSCROLLBAR,
		    MPFROMSHORT(vcMaxVSB),
		    MPFROM2SHORT(0, vcMaxVSB));
	    }
	    /*
		Draw the BITMAP, based on the scroll bar settings.
	    */
	    GpiSetBitmap(vhpsMemory, (HBITMAP) hItem);

	    aptl[0].x = rclWindow.xLeft;	/* Target bottom left */
	    aptl[0].y = rclWindow.yBottom;
	    aptl[1].x = rclWindow.xRight;	/* Target top right */
	    aptl[1].y = rclWindow.yTop;
						/* Source bottom left */
	    aptl[2].x = (LONG) WinSendMsg(vhwndHSB, SBM_QUERYPOS, 0L, 0L);
	    aptl[2].y = vcMaxVSB
		- (LONG) WinSendMsg(vhwndVSB, SBM_QUERYPOS, 0L, 0L);

	    GpiBitBlt(hpsWindow, vhpsMemory, 3L, aptl, ROP_SRCCOPY, 0L);
	    GpiSetBitmap(vhpsMemory, NULL);
	    break;

	case CF_METAFILE:
	case CF_DSPMETAFILE:
	    /*
		Set up the alOptions for displaying the metafile, and
		let the system do the rest of the work.
	    */
	    alOptions[PMF_SEGBASE]	    = 0L;
	    alOptions[PMF_LOADTYPE]	    = LT_DEFAULT;
	    alOptions[PMF_RESOLVE]	    = 0L;
	    alOptions[PMF_LCIDS]	    = LC_LOADDISC;
	    alOptions[PMF_RESET]	    = RES_DEFAULT;
	    alOptions[PMF_SUPPRESS]	    = SUP_DEFAULT;
	    alOptions[PMF_COLORTABLES]	    = CTAB_NOMODIFY;
	    alOptions[PMF_COLORREALIZABLE]  = CREA_DEFAULT;
	    hmfCopy = GpiCopyMetaFile((HMF) hItem);
	    GpiPlayMetaFile(hpsWindow, hmfCopy, 8L, alOptions, 0L, 0L, NULL);
	    break;

	case CF_EMPTY:
	    /*
		Don't do anything.
	    */
	    break;

	default:
	    /*
		If it's an owner-draw format that we can display...
		...try to get the owner to paint the clipboard.
		(return if we were successful or not)
	    */
	    fRendered = SendOwnerMsg(WM_PAINTCLIPBOARD, MPFROMHWND(hwnd), 0L);
	    break;
      }
    }
    /*
	Tell everybody that the client area is valid now
    */
    WinValidateRect(hwnd, (PRECTL) NULL, FALSE);
    /*
	Clean up
    */
    GpiAssociate(hpsWindow, NULL);
    GpiDestroyPS(hpsWindow);
    WinCloseClipbrd(vhab);
    return fRendered;
}

BOOL UpdateScreen(HWND hwnd, USHORT usFormat) {
/*
    Render the format, change the title bar.
    The title bar will look like:  "<appname> (<format>)"
*/
    BOOL  fRendered = TRUE;
    HPS   hpsWindow;
    RECTL rcl;
    UCHAR szFormat[MAXLEN];
    UCHAR szTitle[MAXTITLELEN];

    if (vfUpdate) {
	/* If the MLE exists, destroy it */
	if (vhwndMLE) {
	    WinDestroyWindow(vhwndMLE);
	    vhwndMLE = NULL;
	}

	/* Clear the client area */
	WinQueryWindowRect(hwnd, &rcl);
	WinInvalidateRect(hwnd, &rcl, FALSE);
	hpsWindow = WinBeginPaint(hwnd, NULL, NULL);
	WinFillRect(hpsWindow, &rcl, CLR_BACKGROUND);
	WinEndPaint(hpsWindow);
    }
    if (usFormat)			// Check that usFormat != CF_EMPTY
	fRendered = RenderFormat(hwnd, usFormat);
    /*
	Set the title bar appropriately
    */
    if (!vhwndTitlebar && vhwndClipFrame)
	vhwndTitlebar = WinWindowFromID(vhwndClipFrame, FID_TITLEBAR);

    if (vhwndTitlebar) {
	GetFormatName(usFormat, szFormat);
	LOADSTRING(IDS_APPNAME, szTitle);
	strcat(szTitle, "("); strcat(szTitle, szFormat); strcat(szTitle, ")");
	WinSetWindowText(vhwndTitlebar, szTitle);
    }
    /*
	Save the rendered format.
    */
    vusFormat = usFormat;
    return fRendered;
}

VOID GetAllFormats(VOID) {
    USHORT usFormat;		// Temporary used when enumerating
    /*
	Put ourselves into a clean state
    */
    usFormat = vcFmts = 0;
    /*
	Cycle through the available clipboard formats
    */
    while (usFormat = WinEnumClipbrdFmts(vhab, usFormat)) {
	vausFormats[vcFmts++] = usFormat;
    }
    /*
	Set the current clipboard format to the first one, if possible
	(in preparation for the WM_PAINT which will follow).
    */
    vusFormat = (vcFmts ? vausFormats[0] : CF_EMPTY);
}

VOID GetFormatName(USHORT usFormat, UCHAR szFmtName[]) {
/*
    GetFormatName()

    This routine returns a format name in szFmtName which corresponds
    to the format usFormat.  Basically, either we know the format, or
    we get the name from the system atom table.  If we can't find it,
    we set it to CF_UNKNOWN.
*/
    switch (usFormat) {
	/*
	    If we know the format, we can read it from the string table.
	*/
	case CF_EMPTY:
	case CF_TEXT:
	case CF_DSPTEXT:
	case CF_BITMAP:
	case CF_DSPBITMAP:
	case CF_METAFILE:
	case CF_DSPMETAFILE:
	    LOADSTRING(usFormat, szFmtName);
	    break;

	default:
	    /*
		Get the format name from the system atom table.
		If not found, tag it as an unknown format.
	    */
	    if (!WinQueryAtomName(WinQuerySystemAtomTable(),
		   usFormat, szFmtName, MAXLEN))

		LOADSTRING(CF_UNKNOWN, szFmtName);

	    break;
    }
}

BOOL SendOwnerMsg(USHORT msg, MPARAM mp1, MPARAM mp2) {
    BOOL    rc;
    HWND    hwndOwner;
    /*
	If we are an OWNERDISPLAY format,
	    lock the owner window, tell it to perform the operation, return
    */
    if ( rc = ( (vfsFmtInfo & CFI_OWNERDISPLAY)
	 && (hwndOwner = WinQueryClipbrdOwner(vhab, TRUE)) ) ) {

	WinSendMsg(hwndOwner, msg, mp1, mp2);
	WinLockWindow(hwndOwner, FALSE);
    }
    return rc;
}

BOOL DoScrolling(HWND hwnd, BOOL fHorz, USHORT sbCmd) {
/*
    This routine depends on the fact that the thumb cannot be set past the
    range of the scroll bar.  Since this is handled in the system SBM_SETPOS
    code already, we need not worry about it.

    We return TRUE if the scroll bar message is processed.
*/
    HWND   hwndSB;		/* Scroll bar handle */
    USHORT cpels;		/* Page length/width for PAGExxxx commands */
    SWP    swp;			/* Dimensions of the client area */
    USHORT usOld;		/* The current scroll bar position */
    USHORT usNew;		/* The new scroll bar position */
    /*
	Set the scroll bar-specific parameters
    */
    WinQueryWindowPos(hwnd, &swp);
    if (fHorz) {	/* Horizontal scroll bar */
	hwndSB = vhwndHSB;
	cpels = swp.cx;
    } else {		/* Vertical scroll bar */
	hwndSB = vhwndVSB;
	cpels = swp.cy;
    }
    /*
	Handle both scroll bars with one common routine

	Basically, the scroll bar has been set so that 
	the thumb value corresponds to the offset that
	the bitmap is drawn from.  So, to scroll by a
	page, compute the number of pels of the page,
	and move the thumb by that amount.

	This code is simplified by the fact that SB_SETPOS
	will not allow the thumb to be set outside of the
	range of the scroll bar, but will "stop" it at the
	appropriate bound.
    */
    usOld = SHORT1FROMMR( WinSendMsg(hwndSB, SBM_QUERYPOS, 0L, 0L));

    switch (sbCmd) {
	case SB_PAGERIGHT:	/* SB_PAGEDOWN */
	    WinSendMsg(hwndSB, SBM_SETPOS, MPFROMSHORT(usOld + cpels), 0L);
	    break;

	case SB_PAGELEFT:	/* SB_PAGEUP */
	    WinSendMsg(hwndSB, SBM_SETPOS, MPFROMSHORT(usOld - cpels), 0L);
	    break;

	case SB_LINERIGHT:	/* SB_LINEDOWN */
	    WinSendMsg(hwndSB, SBM_SETPOS, MPFROMSHORT(usOld + LINE), 0L);
	    break;

	case SB_LINELEFT:	/* SB_LINEUP */
	    WinSendMsg(hwndSB, SBM_SETPOS, MPFROMSHORT(usOld - LINE), 0L);
	    break;

	case SB_SLIDERPOSITION:
	    /*
		It would be nice to be consistent with the other
		SB_ cases, but the problem is that when this message
		is sent, the position is *already* set to "usPosition".

		So, just invalidate the entire region, and hope that most
		of these types of operations will be large scrolls.
	    */
	    // WinSendMsg(hwndSB, SBM_SETPOS, MPFROMSHORT(LOUSHORT(mp2)), 0L);
	    WinInvalidateRect(hwnd, NULL, TRUE);
	    break;

	default:
	    return FALSE;
    }
    /*
	Now, we find out where the new thumb position is,
	scroll the window contents appropriately, and specify
	SW_INVALIDATERGN so that the remainder will be
	invalidated/repainted.
    */
    usNew = SHORT1FROMMR( WinSendMsg(hwndSB, SBM_QUERYPOS, 0L, 0L));
    if (fHorz)
	WinScrollWindow(hwnd, (SHORT) (usOld - usNew), 0, 
	    NULL, NULL, NULL, NULL, SW_INVALIDATERGN);
    else
	WinScrollWindow(hwnd, 0, (SHORT) (usNew - usOld),
	    NULL, NULL, NULL, NULL, SW_INVALIDATERGN);

    return TRUE;
}
