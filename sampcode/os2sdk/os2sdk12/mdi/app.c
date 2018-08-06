/***************************************************************************\
* app.c - MDI Sample application
*
* Created by Microsoft Corporation, 1989
*
\***************************************************************************/

#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#define INCL_WINHEAP
#define INCL_WINSCROLLBARS
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include "app.h"
#include "appdata.h"
#include "mdi.h"
#include "mdidata.h"


/*
    Function prototypes
*/
BOOL AppInit(VOID);
BOOL MDIInit(VOID);
VOID AppTerminate(VOID);
VOID MDITerminate(VOID);
BOOL AppNewDocument(USHORT, PSZ);
VOID TrackSplitbars(HWND, USHORT, SHORT, SHORT);
VOID MDIDesktopSize(HWND, MPARAM, MPARAM);
VOID MDIDesktopSetFocus(HWND, MPARAM); 
VOID MDIDesktopActivateDoc(SHORT idMenuitem);
BOOL AppNewDocument(USHORT, PSZ);
NPDOC MDINewDocument(USHORT fsStyle, PSZ pszClassName);
VOID MDISetInitialDocPos(HWND hwndNewFrame);

VOID AddToWindowMenu(NPDOC);



int cdecl main(void)
{
    QMSG qmsg;
    /*
     * Initialize the application globals
     * and create the main window.
     */
    if (AppInit() == FALSE) {
        WinAlarm(HWND_DESKTOP, WA_ERROR);
        return(0);
    }

    /*
     * Initialize the MDI globals etc..
     */
    if (MDIInit() == FALSE) {
        WinAlarm(HWND_DESKTOP, WA_ERROR);
        WinAlarm(HWND_DESKTOP, WA_ERROR);
        return(0);
    }

    /*
     * Create an initial, untitled document.
     */
    AppNewDocument(DS_HORZSPLITBAR | DS_VERTSPLITBAR, szDocClass);

    while (WinGetMsg(NULL, (PQMSG)&qmsg, NULL, 0, 0)) {
        WinDispatchMsg(NULL, (PQMSG)&qmsg);
    }

    /*
     * Do the clean-up of the MDI code.
     */
    MDITerminate();

    /*
     * Do the clean-up of the Application.
     */
    AppTerminate();

    DosExit(EXIT_PROCESS, 0);
}


MRESULT EXPENTRY MDIWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    HPS hps;
    RECTL rclPaint, rclWindow;
    POINTL ptlPatternRef;

    switch (msg) {

    case WM_PAINT:
        hps = WinBeginPaint(hwnd, (HPS)NULL, &rclPaint);

        /*
         * Set the pattern to be at the top-left
         * since we're top-left aligning the bits.
         */
        WinQueryWindowRect(hwnd, (PRECTL)&rclWindow);
        ptlPatternRef.x = rclWindow.xLeft;
        ptlPatternRef.y = rclWindow.yTop;
        GpiSetPatternRefPoint(hps, &ptlPatternRef);

        WinFillRect(hps, &rclPaint, SYSCLR_APPWORKSPACE);

        WinEndPaint(hps);
        break;

#if 0
    case WM_SIZE:

        /* HACK -- only reposition the windows if it is not going to or coming
        from a minimized position, it would be better to what
        WM_WINDOWPOSCHANGED and pay attention to the fs fields of the SWP
        structure */

        if ( SHORT1FROMMP(mp1) && SHORT2FROMMP(mp1) &&
             SHORT1FROMMP(mp2) && SHORT2FROMMP(mp2) ) {
            MDIDesktopSize ( hwnd, mp1, mp2 );
        }
    break;

#else
    case WM_SIZE:
        MDIDesktopSize ( hwnd, mp1, mp2 );
        break;
#endif

    case WM_SETFOCUS:
        MDIDesktopSetFocus(hwnd, mp2);
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {

        /*
         * Pass these accelerators onto the active document's
         * frame so it can process it.
         *
         * These are the CMD_ values from the document system
         * menu.
         */
        case CMD_DOCRESTORE:
            WinSendMsg(hwndActiveDoc, WM_SYSCOMMAND, (MPARAM)SC_RESTORE, mp2);
            break;

        case CMD_DOCNEXT:
            WinSendMsg(hwndActiveDoc, WM_SYSCOMMAND, (MPARAM)SC_NEXT, mp2);
            break;

        case CMD_DOCMINIMIZE:
            WinSendMsg(hwndActiveDoc, WM_SYSCOMMAND, (MPARAM)SC_MINIMIZE, mp2);
            break;

        case CMD_DOCCLOSE:
            WinSendMsg(hwndActiveDoc, WM_SYSCOMMAND, (MPARAM)SC_CLOSE, mp2);
            break;

        case CMD_DOCSPLIT:
            /*
             * Call TrackSplitbars() with -1 for xMouse to tell
             * it to reposition the pointer to where the
             * splitbars currently are.
             */
            WinSetPointer(HWND_DESKTOP, hptrHVSplit);
            TrackSplitbars(WinWindowFromID(hwndActiveDoc, FID_CLIENT),
                    SPS_VERT | SPS_HORZ, -1, -1);
            WinSetPointer(HWND_DESKTOP, hptrArrow);
            break;

        case CMD_NEW:
            if (AppNewDocument(DS_HORZSPLITBAR | DS_VERTSPLITBAR, szDocClass) == FALSE)
                WinAlarm(HWND_DESKTOP, WA_ERROR);
            break;

        case CMD_CLOSE:
            /*
             * Close the active document.
             */
            if (hwndActiveDoc)
                WinSendMsg(hwndActiveDoc, WM_CLOSE, 0L, 0L);
            break;

	case CMD_ABOUT:
	    /*
	     * Put up the About... dialog box
	     */
	    WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc, NULL, IDD_ABOUT, NULL);
	    break;
	
        case CMD_EXIT:
            WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
            break;

        case CMD_ARRANGETILED:
            ArrangeWindows(AWP_TILED);
            break;

        case CMD_ARRANGECASCADED:
            ArrangeWindows(AWP_CASCADED);
            break;

        default:
            /*
             * The means a window title was selected from
             * the window menu.  Have the MDI code activate
             * the correct window based on the menuitem ID.
             *
             * WARNING: Be sure to keep you applications
             * menuitem IDs < CMD_WINDOWITEMS.
             */

            /* MULTIPLEMENU */
            /*  Also in here we need to pass document unique WM_COMMAND
                messages on down to the document's client procs */

            if (SHORT1FROMMP(mp1) >= CMD_WINDOWITEMS)
                MDIDesktopActivateDoc(SHORT1FROMMP(mp1));
            break;
        }
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }

    return (0L);
}


BOOL AppNewDocument(USHORT fsStyle, PSZ pszClassName)
{
    register NPDOC npdocNew;
    HWND hwndFrame, hwndClient;
    HWND hwndHScroll, hwndVScroll;

    npdocNew = MDINewDocument(fsStyle, pszClassName);

    npdocNew->clrBackground = clrNext++;
    if (clrNext > CLR_PALEGRAY)
        clrNext = CLR_BACKGROUND;

    hwndFrame = npdocNew->hwndFrame;
    hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

    /*
     * Setup the scrollbars.
     */
    hwndHScroll = WinWindowFromID(hwndFrame, FID_HORZSCROLL);
    WinSendMsg(hwndHScroll, SBM_SETSCROLLBAR, MPFROMSHORT(0),
            MPFROM2SHORT(0, 600));
    hwndHScroll = WinWindowFromID(hwndFrame, ID_HORZSCROLL2);
    WinSendMsg(hwndHScroll, SBM_SETSCROLLBAR, MPFROMSHORT(0),
            MPFROM2SHORT(0, 600));

    hwndVScroll = WinWindowFromID(hwndFrame, FID_VERTSCROLL);
    WinSendMsg(hwndVScroll, SBM_SETSCROLLBAR, MPFROMSHORT(0),
            MPFROM2SHORT(0, 600));
    hwndVScroll = WinWindowFromID(hwndFrame, ID_VERTSCROLL2);
    WinSendMsg(hwndVScroll, SBM_SETSCROLLBAR, MPFROMSHORT(0),
            MPFROM2SHORT(0, 600));

    /*
     * Set the focus the client so the new window will be
     * active when we show it.
     */
    WinSetFocus(HWND_DESKTOP, hwndClient);

    AddToWindowMenu(npdocNew);    /* Moved here from end of
                                  MdiNewDocument routine so that the doc has
                                  been activated, and therefore the main
                                  window has a menu before attempting to add
                                  the doc to the main window's menu */


    /*
     * Set the initial position of the frame window and make it visible.
     */
    MDISetInitialDocPos(hwndFrame);

    return (TRUE);
}

MRESULT EXPENTRY AboutDlgProc(HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
/*
    About... dialog procedure
*/
{
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
