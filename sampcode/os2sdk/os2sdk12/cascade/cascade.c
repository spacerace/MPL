#define INCL_PM
#include <OS2.H>
#include "Cascade.H"

char szAppName[] = "Cascade";
char szAppTitle[] = "Cascading Menu Example";

HAB	hAB;
HMQ	hmqMsgQueue;
HWND	hWndMain,
	hWndFrame;

int cdecl main()
{
    QMSG    qmsg;
    ULONG   ctlData = FCF_STANDARD & ~FCF_ACCELTABLE;

    hAB = WinInitialize (0);

    hmqMsgQueue = WinCreateMsgQueue (hAB, 0);

    if (!WinRegisterClass (hAB,
			   szAppName,
			   WndProc,
			   CS_SYNCPAINT | CS_SIZEREDRAW,
			   0)) {
	return(0);
    }

    hWndFrame = WinCreateStdWindow ( HWND_DESKTOP,
				    WS_VISIBLE,
				    &ctlData,
				    szAppName,
				    NULL,
				    0L,
				    0,
				    ID_RESOURCE,
				    &hWndMain);
    WinSetWindowText (hWndFrame, szAppTitle);
    WinShowWindow (hWndFrame, TRUE);

    while ( WinGetMsg (hAB, &qmsg, NULL, 0, 0)) {
	WinDispatchMsg (hAB, &qmsg);
    }

    WinDestroyWindow   (hWndFrame);
    WinDestroyMsgQueue (hmqMsgQueue);
    WinTerminate       (hAB);
}

/*-------------------------------------------------------------------*/
/*								     */
/*-------------------------------------------------------------------*/

BOOL CheckAll (HWND hMenu, int item, BOOL check);
BOOL CheckAll (HWND hMenu, int item, BOOL check)
{
    int mPos,max,test;
    MENUITEM mi;
    char szText[20];
    MPARAM  mp1, mp2;

    max =(int) SHORT1FROMMR( WinSendMsg (hMenu, MM_QUERYITEMCOUNT, 0L, 0L) ); 

    for (mPos=0; mPos!=(int) max; mPos++) {
	test =(int) SHORT1FROMMR( WinSendMsg (hMenu, MM_ITEMIDFROMPOSITION, MPFROMSHORT(mPos), 0L) );
	WinSendMsg (hMenu, MM_QUERYITEMTEXT, MPFROM2SHORT(test,sizeof(szText)), MPFROMP(szText));
	if (test == item) {
	    mp1 = MPFROM2SHORT (test, TRUE);
	    if (check)
		mp2 = MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED);
	    else
		mp2 = MPFROM2SHORT(MIA_CHECKED, 0);
	    WinPostMsg (hMenu, MM_SETITEMATTR, mp1, mp2);
	    return TRUE;
	} else {
	    WinSendMsg (hMenu, MM_QUERYITEM, MPFROM2SHORT(test,FALSE), (MPARAM)&mi);
	    if (mi.hwndSubMenu) {
		if (CheckAll(mi.hwndSubMenu, item, check)) {
		    mp1 = MPFROM2SHORT (test, TRUE);
		    if (check)
			mp2 = MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED);
		    else
			mp2 = MPFROM2SHORT(MIA_CHECKED, 0);
		    WinPostMsg (hMenu, MM_SETITEMATTR, mp1, mp2);
		    return TRUE;
		}

	    }
	}
    }
    return FALSE;
}

/*-------------------------------------------------------------------*/
/*								     */
/*-------------------------------------------------------------------*/


MRESULT EXPENTRY WndProc (hWnd, msg, mp1, mp2)
    HWND    hWnd;
    USHORT  msg;
    MPARAM  mp1, mp2;
{
    HPS 	  hPS;
    HWND   hMenu;
    static int	  prevFont = 0;
    int thisItem;

    switch (msg) {

	case WM_COMMAND:
	    thisItem = SHORT1FROMMP(mp1);
	    switch (thisItem) {
		case IDM_ABOUT:
		    WinMessageBox (HWND_DESKTOP, hWnd,
			"Sample PM Application",
			szAppTitle, 1, MB_OK | MB_APPLMODAL | MB_MOVEABLE);
		    break;
		default:
		    if ((thisItem >= IDM_FIRSTFONT) && (thisItem<= IDM_LASTFONT)) {
			hMenu	= WinWindowFromID (
				    WinQueryWindow (hWnd, QW_PARENT, FALSE),
				    FID_MENU);
			CheckAll (hMenu, prevFont, FALSE);
			CheckAll (hMenu, thisItem, TRUE);
			prevFont = thisItem;
		    } else {
			DosBeep(600,60);
		    }
	    }
	    break;

	case WM_CLOSE:
	    WinPostMsg (hWnd, WM_QUIT, 0L, 0L);
	    break;

	case WM_ERASEBACKGROUND:
	    return ((MRESULT) TRUE);
	    break;

	case WM_PAINT:
	    hPS = WinBeginPaint (hWnd, NULL, (PWRECT)NULL);
	    WinEndPaint (hPS);
	    break;

	default:
	    return (WinDefWindowProc (hWnd, msg, mp1, mp2));
	    break;
    }
    return 0L;
}
