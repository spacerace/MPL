/*      msgbox.c  - Message box control sample application	*/
/*      Created by Microsoft Corp., 1989			*/
/*                                                              */
#define INCL_WIN
#include <os2.h>
#include "msgbox.h"

/* Procedure prototypes */
MPARAM EXPENTRY MsgBoxDlgProc(HWND, USHORT, MPARAM, MPARAM);
void cdecl main(void);


/* Main routine -- creates a dialog box window */
void cdecl main(void)
{
    HAB habMsgBox;
    HMQ hmqMsgBox;

    /* Initialize Anchor Block, Message Queue */
    habMsgBox = WinInitialize(0);
    hmqMsgBox = WinCreateMsgQueue(habMsgBox, 0);

    /* Summon the dialog box */
    WinDlgBox(HWND_DESKTOP, NULL, MsgBoxDlgProc, (HMODULE) NULL, IDD_MSGBOX, NULL);

    /* Clean up */
    WinDestroyMsgQueue(hmqMsgBox);
    WinTerminate(habMsgBox);
}

/* Message box control routine */
MRESULT EXPENTRY MsgBoxDlgProc(hWnd, msg, mp1, mp2)
HWND hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    USHORT flStyle;
    SHORT  rc;

    switch (msg) {
    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case DID_OK:
            WinDismissDlg(hWnd, TRUE);
            break;

	case IDD_SHOWBOX: /* Show Box */
	    /* Buttons? */
	    rc = SHORT1FROMMR( WinSendDlgItemMsg(hWnd, IDD_OK0,
		    BM_QUERYCHECKINDEX, 0L, 0L));
	    flStyle = (rc > 0) ? (USHORT) rc : 0;

	    /* Icon style? */
	    rc = SHORT1FROMMR( WinSendDlgItemMsg(hWnd, IDD_ICON0,
		    BM_QUERYCHECKINDEX, 0L, 0L));
	    if (rc > 0) flStyle = (flStyle & 0xff0f) | ((USHORT) rc << 4);

	    /* Default style? */
	    rc = SHORT1FROMMR( WinSendDlgItemMsg(hWnd, IDD_DEF0,
		    BM_QUERYCHECKINDEX, 0L, 0L));
	    if (rc > 0) flStyle = (flStyle & 0xf0ff) | ((USHORT) rc << 8);

            /* Get modality */
	    if (WinSendDlgItemMsg(hWnd, IDD_SYSTEMMODAL, BM_QUERYCHECK, 0L, 0L))
                flStyle |= MB_SYSTEMMODAL;

            /* Get help button attribute */
	    if (WinSendDlgItemMsg(hWnd, IDD_HELP, BM_QUERYCHECK, 0L, 0L))
                flStyle |= MB_HELP;

	    /* Display the Message Box Type */
	    WinSetDlgItemShort(hWnd, IDD_MSGBOXSTYLE, flStyle, FALSE);

	    /* Pop up the message box */
	    rc = WinMessageBox(HWND_DESKTOP, hWnd,
                    (PSZ)"Message Box Text Body\n(can contain several lines)",
                    (PSZ)"This is the title bar", 1, flStyle);

	    /* Update the return code box */
	    WinSetDlgItemShort(hWnd, IDD_RETURNCODE, rc, FALSE);
            break;

        default:
            break;
        }
        break;

    case WM_INITDLG: /* Push these three buttons */
	WinSendDlgItemMsg(hWnd,  IDD_OK0, BM_SETCHECK, (MPARAM) TRUE, 0L);
	WinSendDlgItemMsg(hWnd,IDD_ICON0, BM_SETCHECK, (MPARAM) TRUE, 0L);
	WinSendDlgItemMsg(hWnd, IDD_DEF0, BM_SETCHECK, (MPARAM) TRUE, 0L);
        break;

    default:
        return(WinDefDlgProc(hWnd, msg, mp1, mp2));
        break;
    }
    return 0L;
}
