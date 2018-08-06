#define INCL_PM
#include <OS2.H>
#include "Expand.H"

MRESULT EXPENTRY WndProc      (HWND, USHORT, MPARAM, MPARAM );
MRESULT EXPENTRY AboutDlgProc (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY DialogProc  (HWND, USHORT, MPARAM, MPARAM);

int cdecl main(void);

char szAppName[] = "Expand";

HAB	hAB;
HMQ	hmqMsgQueue;
HWND	hWndMain,
	hWndFrame;

int cdecl main()
{
    QMSG    qmsg;
    ULONG   ctlData;

    hAB = WinInitialize (0);
    hmqMsgQueue = WinCreateMsgQueue (hAB, 0);

    if (!WinRegisterClass (hAB,
			   szAppName,
			   WndProc,
			   CS_SYNCPAINT | CS_SIZEREDRAW,
			   0)) {
	return(0);
    }


    ctlData = FCF_STANDARD;
    hWndFrame = WinCreateStdWindow ( HWND_DESKTOP,
				    WS_VISIBLE,
				    &ctlData,
				    szAppName,
				    NULL,
				    0L,
				    0,
				    ID_RESOURCE,
				    &hWndMain);
    WinSetWindowText (hWndFrame, szAppName);
    WinShowWindow (hWndFrame, TRUE);

    while ( WinGetMsg (hAB, &qmsg, NULL, 0, 0)) {
	WinDispatchMsg (hAB, &qmsg);
    }

    WinDestroyWindow   (hWndFrame);
    WinDestroyMsgQueue (hmqMsgQueue);
    WinTerminate       (hAB);
}

MRESULT EXPENTRY WndProc (hWnd, msg, mp1, mp2)
    HWND    hWnd;
    USHORT  msg;
    MPARAM  mp1, mp2;
{
    HPS    hPS;
    RECTL   rclPaint, rclWindow;
    POINTL  ptlPatternRef;

    switch (msg) {

	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {
		case IDM_ABOUT:
		    WinDlgBox(HWND_DESKTOP, hWnd, AboutDlgProc,
			(HMODULE) NULL, ID_RESOURCE, NULL);
		    break;

		case IDM_ITEM+1:
		    WinDlgBox(HWND_DESKTOP, hWnd, DialogProc,
			(HMODULE) NULL, ID_DIALOG, NULL);
		    break;

		case IDM_ITEM+2:
		case IDM_ITEM+3:
		case IDM_ITEM+4:
		    break;

		case IDM_EXIT:
		    WinPostMsg (hWnd, WM_CLOSE, 0L, 0L);
		    break;
	    }
	    break;

	case WM_HELP:
	    WinMessageBox (HWND_DESKTOP, hWnd,
		"Help Not Implemented Yet.",
		" - Help - ",
		0,
		MB_OK | MB_MOVEABLE | MB_APPLMODAL);
	    break;

	case WM_CLOSE:
	    WinPostMsg (hWnd, WM_QUIT, 0L, 0L);
	    break;

	case WM_PAINT:
	    hPS = WinBeginPaint (hWnd, NULL, &rclPaint);

	    WinQueryWindowRect(hWnd, &rclWindow);
	    ptlPatternRef.x = rclWindow.xLeft;
	    ptlPatternRef.y = rclWindow.yTop;
	    GpiSetPatternRefPoint(hPS, &ptlPatternRef);
	    WinFillRect(hPS, &rclPaint, SYSCLR_APPWORKSPACE);

	    WinEndPaint (hPS);
	    break;

	default:
	    return (WinDefWindowProc (hWnd, msg, mp1, mp2));
    }
    return 0L;
}


MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {
		case IDB_OK:
		    WinDismissDlg (hwnd, TRUE);
		    return 0;
	    }
	    break;
     }
     return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY DialogProc (HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    SWP  swpDlg, swpParent, swpOwner;
    static HWND option, button1, button2, button3, button4;

    switch (msg) {

	case WM_INITDLG:
	    WinSendDlgItemMsg (hDlg, 256, EM_SETTEXTLIMIT, MPFROMSHORT(8), 0L);
	    WinQueryWindowPos (hDlg, &swpDlg);
	    WinQueryWindowPos (WinQueryWindow(hDlg, QW_PARENT, FALSE), &swpParent);
	    WinQueryWindowPos (WinQueryWindow(hDlg, QW_OWNER, FALSE), &swpOwner);
	    swpDlg.x = swpOwner.x + ((swpOwner.cx / 2) - ((swpDlg.cx+120) / 2));
	    swpDlg.y = swpOwner.y + ((swpOwner.cy / 2) - (swpDlg.cy / 2));
	    WinSetMultWindowPos (hAB, &swpDlg, 1);
	    option  = WinWindowFromID (hDlg, IDB_OPTION);
	    button1 = WinWindowFromID (hDlg, IDB_RADIO1);
	    WinShowWindow (button1, FALSE);
	    button2 = WinWindowFromID (hDlg, IDB_RADIO2);
	    WinShowWindow (button2, FALSE);
	    button3 = WinWindowFromID (hDlg, IDB_RADIO3);
	    WinShowWindow (button3, FALSE);
	    button4 = WinWindowFromID (hDlg, IDB_RADIO4);
	    WinShowWindow (button4, FALSE);
	    break;

	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {
		case IDB_OPTION:
		    WinQueryWindowPos (hDlg, &swpDlg);
		    swpDlg.fs = SWP_SIZE;
		    swpDlg.cx += 120;
		    WinSetMultWindowPos (hAB, &swpDlg, 1);
		    WinEnableWindow (option, FALSE);
		    WinShowWindow (button1, TRUE);
		    WinShowWindow (button2, TRUE);
		    WinShowWindow (button3, TRUE);
		    WinShowWindow (button4, TRUE);
		    WinSetFocus (HWND_DESKTOP, button1);
		    return FALSE;

		case IDB_OK:
		case IDB_CANCEL:
		    WinDismissDlg (hDlg, TRUE);
		    return 0;
	    }
	    break;
     }
     return WinDefDlgProc (hDlg, msg, mp1, mp2);
}
