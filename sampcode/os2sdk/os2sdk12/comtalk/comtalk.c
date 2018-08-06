/*
   comtalk.c -- Main routines
   Created by Microsoft Corporation, 1989

   This file contains the sources for the dialog box manipulation, and menu
   managment, and other aspects of interfacing with the user.
*/
#define INCL_WIN
#include <os2.h>
#include "comtalk.h"	/* definition of COM from Global, and Resource IDs */
#include "avio.h"	/* Routines needed to manage AVIO Presentation Space */
#include "threads.h"	/* Thread initialization and control routines */
#include <stdio.h>	/* Only needed for file I/O */
#include <string.h>	/* one strcpy call */
/*
    Variables
*/
CHAR 		szCaption[] = "";
HAB		hAB;
COM		comTerm;
COM		comTemp;
HWND		hWndMenu;
CLASSINFO	clsi;
PFNWP		pfnOldFrameWndProc;
BOOL		fConnected	= FALSE;
BOOL		fPaging;
int		iUpdate;
BOOL		fFreeze		= TRUE;
int		iError;
/*
    Macros
*/
#define InRange(x, a, b) ((x >= a) && (x <= b))

/*
    Shorthand for sending messages, querying
*/
#define Parent(h) \
    WinQueryWindow(h, QW_PARENT, FALSE)

#define EnableMenuItem(id) \
    WinSendMsg(hWndMenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
	       MPFROM2SHORT(MIA_DISABLED,0))

#define DisableMenuItem(id) \
    WinSendMsg(hWndMenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
	       MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED))

#define CheckMenuItem(id) \
    WinSendMsg(hWndMenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
	       MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED)) 

#define UnCheckMenuItem(id) \
    WinSendMsg(hWndMenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
	       MPFROM2SHORT(MIA_CHECKED, 0))

#define PushButton(h, id) \
    WinSendDlgItemMsg(h, id, BM_SETCHECK, MPFROM2SHORT(TRUE, 0), 0L) 

#define Valid(bData, bStop) \
    (((bData == IDD_FIVE) && (bStop != IDD_TWOSTOP)) \
    || ((bData != IDD_FIVE) && (bStop != IDD_ONEFIVE)))

#define ErrMsg(h, s) \
    WinMessageBox(HWND_DESKTOP, h, s, NULL, 0, MB_OK | MB_ICONEXCLAMATION)

char Ctrl(char ch) {
    return  (CHAR) ((('a' <= ch) && (ch <= 'z')) ?  (ch - 'a' + '\001') :
	  ((('A' <= ch) && (ch <= 'Z')) ? (ch - 'A' + '\001') : ch));
}

/*
    Local/Private routines
*/
void ReadOpts(HWND);
void InitTerm(void);
void Initialize(HWND);
void ChangeSystemMenu(HWND);
BOOL Filter(USHORT, char, USHORT);

void main (void) {
     static CHAR szClientClass[] = "Terminal";
     HMQ	hmq;
     HWND	hWndClient, hWndFrame;
     QMSG	qmsg;
     ULONG	flFrameFlags = FCF_STANDARD | FCF_HORZSCROLL | FCF_VERTSCROLL;
     ULONG 	flFrameStyle = WS_VISIBLE | FS_SCREENALIGN;
     
     hAB = WinInitialize(0);
     hmq = WinCreateMsgQueue(hAB, 0);

     WinRegisterClass(hAB, szClientClass, ClientWndProc, CS_SYNCPAINT, 0);

     hWndFrame = WinCreateStdWindow(HWND_DESKTOP, flFrameStyle,
                                    &flFrameFlags, szClientClass, szCaption,
                                     0L, (HMODULE) NULL, ID_RESOURCE, &hWndClient);

     /* Setup AVIO PS and force a paint */
     AvioInit(hWndFrame, hWndClient);
     WinSendMsg(hWndClient, WM_PAINT, NULL, NULL);

     /* Try to subclass the Frame window... */
     pfnOldFrameWndProc = WinSubclassWindow(hWndFrame, NewFrameWndProc);

     while (WinGetMsg(hAB, &qmsg, NULL, 0, 0)) WinDispatchMsg(hAB, &qmsg);

     /* Blast the AVIO PS */
     AvioClose();

     WinDestroyWindow(hWndFrame);
     WinDestroyMsgQueue(hmq);
     WinTerminate(hAB);
     DosExit(EXIT_PROCESS, 0);
}

MRESULT CALLBACK ClientWndProc(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
     Window Procedure which traps messages to the Client area
*/
     switch (msg) {
	  case WM_AVIOUPDATE:
		fNoUpdate = AvioUpdateLines(FALSE, &fPaging);
		if (fConnected && fPaging) {
		    CheckMenuItem(IDM_PAGING);
		}
		break;

	  case WM_MSGBOX:
		iUpdate = (int)SHORT1FROMMP( mp2);
		switch ((int)SHORT1FROMMP(mp1)) {
		    case (int) MBE_COMREAD:
			if (iError = iUpdate) EnableMenuItem(IDM_ERRORS);
			iUpdate = 0;
			break;

		    default:
			ErrMsg(hWnd, aszMessage[SHORT1FROMMP(mp1)]);
			break;
		}
		if (iUpdate) {	/* Page down because queue is full */
		    fNoUpdate = AvioUpdateLines(TRUE, &fPaging);
		    if (fConnected && fPaging) CheckMenuItem(IDM_PAGING);
		    else UnCheckMenuItem(IDM_PAGING);
		    ThdReset();
		}
		break;

	  case WM_CREATE:
		ChangeSystemMenu(hWnd);
		/*
		    Initialize the Dialog Options
		*/
		Initialize(hWnd);
		/*
		    Get the Handle so you can enable/disable menu items
		    Thanks again to Charles Petzold
		*/
		hWndMenu = WinWindowFromID(Parent(hWnd), FID_MENU);
		/*
		    Disable some entries (can do this in the resource file)
		*/
		DisableMenuItem(IDM_CLOSE);
		DisableMenuItem(IDM_BREAK);
		DisableMenuItem(IDM_COMMANDMENU);
		break;

	  case WM_PAINT:		/* Paint the AVIO way! */
		AvioPaint(hWnd);
		break;

	  case WM_SIZE:			/* Size the AVIO way!  */
		fNoUpdate = AvioUpdateLines(FALSE, &fPaging);
		if (fConnected && fPaging) {
		    CheckMenuItem(IDM_PAGING);
		}
		return AvioSize(hWnd, msg, mp1, mp2);
		break;

	  case WM_HSCROLL:
		AvioScroll(HIUSHORT(mp2), LOUSHORT(mp2), TRUE);
		break;

	  case WM_VSCROLL:
		AvioScroll(HIUSHORT(mp2), LOUSHORT(mp2), FALSE);
		break;

	  case WM_ERASEBACKGROUND:
		return 0;
		break;

	  case WM_COMMAND:
		switch (COMMANDMSG(&msg)->cmd) {
		    case IDM_ABOUT:
			WinDlgBox(HWND_DESKTOP, hWnd, AboutDlgProc,
				  (HMODULE) NULL, IDD_ABOUT, NULL);
                        return 0;

		    case IDM_HELP:
			WinDlgBox(HWND_DESKTOP, hWnd, AboutDlgProc,
				  (HMODULE) NULL, IDD_MAINHELPBOX, NULL);
			return 0;

		    case IDM_SETTINGS:
			WinDlgBox(HWND_DESKTOP, hWnd, SetDlgProc,
				  (HMODULE) NULL, IDD_SET, NULL);
			return 0;

		    case IDM_CONNECT:
			AvioStartup(hWnd);
			ThdInitialize(hWnd, comTerm);	/* Spawn 3 threads  */
			/*
			    Disable/Enable Menu Items
			*/
			DisableMenuItem(IDM_CONNECT);
			DisableMenuItem(IDM_SETTINGS);
			DisableMenuItem(IDM_ERRORS);

			EnableMenuItem(IDM_CLOSE);
			EnableMenuItem(IDM_BREAK);
			EnableMenuItem(IDM_COMMANDMENU);
			fConnected = TRUE;
			return 0;

		    case IDM_CLOSE:
			fConnected = FALSE;
			ThdTerminate();		/* Might have to wait? */
			/*
			    Update menu items
			*/
			UnCheckMenuItem(IDM_BREAK);

			DisableMenuItem(IDM_CLOSE);
			DisableMenuItem(IDM_BREAK);
			DisableMenuItem(IDM_COMMANDMENU);

			EnableMenuItem(IDM_CONNECT);
			EnableMenuItem(IDM_SETTINGS);

			return 0;

		    case IDM_BREAK:
			ThdDoBreak();
			return 0;

		    case IDM_ERRORS:
			if (iError & 1)
			    ErrMsg(hWnd, "Receive Queue Overrun");
			if (iError & 2)
			    ErrMsg(hWnd, "Receive Hardware Overrun");
			if (iError & 4)
			    ErrMsg(hWnd, "Parity Error");
			if (iError & 8)
			    ErrMsg(hWnd, "Framing Error");
			DisableMenuItem(IDM_ERRORS);
			return 0;

		    case IDM_PAGE:
			fNoUpdate = AvioUpdateLines(TRUE, &fPaging);
			if (fPaging) CheckMenuItem(IDM_PAGING);
			else UnCheckMenuItem(IDM_PAGING);
			return 0;

		    case IDM_UP:
			AvioPageUp();
			return 0;

		    case IDM_PAGING:
			if (fPaging = !fPaging) {
			    CheckMenuItem(IDM_PAGING);
			} else {
			    UnCheckMenuItem(IDM_PAGING);
			}
			return 0;

		    default: return 0;
		}

	  case WM_CHAR:		/* Put characters in typeahead buffer */
		if (fConnected && !(CHARMSG(&msg)->fs & KC_KEYUP)) 
		    if (Filter( CHARMSG(&msg)->fs,
			 (char)	CHARMSG(&msg)->chr,
				CHARMSG(&msg)->vkey))
			ErrMsg(hWnd, "Error Writing COM Port");
		break;

	  case WM_TRACKFRAME:
		AvioTrackFrame(hWnd, mp1);
		break;

	  case WM_MINMAXFRAME: /* Trap MAXIMIZE messages */
		AvioMinMax((PSWP) mp1);

	  default: return WinDefWindowProc(hWnd, msg, mp1, mp2);
     }
     return 0;
}

MRESULT CALLBACK AboutDlgProc(HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
    Dialog box control for the ABOUT COMTALK... dialog box
*/
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

void WriteOpts(void) {
/*
    Write Settings to file COMTALK.INI
*/
    FILE *fp;

    fp = fopen("comtalk.ini", "w+");
    fprintf(fp, "%d %d %d %d %d %d %d %s\n", comTerm.usBaud, comTerm.bParity,
		comTerm.bData, comTerm.bStop, comTerm.fWrap,
		comTerm.fHardware, comTerm.fSoftware, comTerm.szPort);
    fclose(fp);
}

void ReadOpts(HWND hWnd) {
/*
    Read Settings from COMTALK.INI
*/
    FILE *fp;

    /* Use InitTerm() if we have reading problems */
    if ((fp = fopen("comtalk.ini", "r")) == NULL) InitTerm();
    else if (fscanf(fp, "%d%d%d%d%d%d%d%s", &comTerm.usBaud, &comTerm.bParity,
	&comTerm.bData, &comTerm.bStop, &comTerm.fWrap,
	&comTerm.fHardware, &comTerm.fSoftware, comTerm.szPort) == EOF)
	InitTerm();
    if (!Valid(comTerm.bData, comTerm.bStop)) {
	ErrMsg(hWnd, "Invalid terminal setting");
	InitTerm();
    }
    fclose(fp);
}

void InitTerm(void) {
/*
    Initialize the TERM structure to DosDevIOCtl defaults
*/
    strcpy(comTerm.szPort, "com1");
    comTerm.usBaud = 9600; comTerm.bParity = IDD_EVENP;
    comTerm.bData = IDD_SEVEN; comTerm.bStop = IDD_ONESTOP;
    comTerm.fWrap = comTerm.fSoftware = TRUE; comTerm.fHardware = FALSE;
}

MRESULT CALLBACK SetDlgProc(HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
    The Settings Dialog Box control routine
*/
    BOOL	rc;
    BYTE	bTemp;

    switch(msg) {
        case WM_INITDLG:
	    WinSetDlgItemText(hDlg, IDD_PORT, comTerm.szPort);
	    WinSetDlgItemShort(hDlg, IDD_BAUD, comTerm.usBaud, FALSE);

	    PushButton(hDlg, comTerm.bParity);
	    PushButton(hDlg, comTerm.bData);
	    PushButton(hDlg, comTerm.bStop);
	    if (comTerm.fWrap) PushButton(hDlg, IDD_WRAP);
	    if (comTerm.fHardware) PushButton(hDlg, IDD_HW);
	    if (comTerm.fSoftware) PushButton(hDlg, IDD_SW);

	    comTemp.bParity	= comTerm.bParity;
	    comTemp.bData	= comTerm.bData;
	    comTemp.bStop	= comTerm.bStop;
	    comTemp.fWrap	= comTerm.fWrap;
	    comTemp.fHardware	= comTerm.fHardware;
	    comTemp.fSoftware   = comTerm.fSoftware;
	    break; 

	case WM_HELP:
	    WinDlgBox(HWND_DESKTOP, hDlg, AboutDlgProc,
		      (HMODULE) NULL, IDD_SETHELPBOX, NULL);
	    break;

	case WM_CONTROL:
	    /*
		The fact that these are AutoRadioButtons makes life easy.
	    */
	    bTemp = (BYTE) SHORT1FROMMP(mp1);	/* Which button pushed? */
	    if InRange(bTemp, IDD_NOP, IDD_SPACEP) 
		 { 
			comTemp.bParity = bTemp; 
	    } 
		else 
		{
			if InRange(bTemp, IDD_FIVE, IDD_EIGHT) 
			{
				comTemp.bData = bTemp;
	    	} 
			else 
			{
				if InRange(bTemp, IDD_ONESTOP, IDD_TWOSTOP) 
				{
					comTemp.bStop = bTemp;
				}
	     		else 
				{
					switch (bTemp) {
					case IDD_WRAP: comTemp.fWrap     = !comTemp.fWrap;     break;
					case IDD_HW  : comTemp.fHardware = !comTemp.fHardware; break;
					case IDD_SW  : comTemp.fSoftware = !comTemp.fSoftware; break;
					default:					       break;
	    			}
				}
			}
		}
	    break;
	case WM_COMMAND:	/* Ready to exit... */
	    switch(COMMANDMSG(&msg)->cmd) {
		case IDD_SAVE:
		case DID_OK:
		    if (!Valid(comTemp.bData, comTemp.bStop)) {
			ErrMsg(hDlg,"Data and Stop Bits Incompatible");
			break;	/* No-op...Dialog not dismissed */
		    }
		    WinQueryDlgItemText(hDlg, IDD_PORT, 5, comTerm.szPort);
		    WinQueryDlgItemShort(hDlg, IDD_BAUD, &comTerm.usBaud, rc);
		    comTerm.bParity	= comTemp.bParity;
		    comTerm.bData	= comTemp.bData;
		    comTerm.bStop	= comTemp.bStop;
		    comTerm.fWrap	= comTemp.fWrap;
		    comTerm.fHardware	= comTemp.fHardware;
		    comTerm.fSoftware	= comTemp.fSoftware;
		    if (COMMANDMSG(&msg)->cmd == IDD_SAVE) WriteOpts();
		case DID_CANCEL: WinDismissDlg(hDlg, FALSE);
		default: break;
	    }
	    break;
	default: return WinDefDlgProc(hDlg, msg, mp1, mp2);
    }
    return FALSE;
}

void Initialize(HWND hWnd) {
    ReadOpts(hWnd);
    fPaging = FALSE;
}

void ChangeSystemMenu(HWND hWnd) {
/*
    Insert items into the System Menu (with thanks to Charles Petzold)
*/
    static CHAR *x[2] = { NULL, "~About ComTalk..." }; /* Items to add */
    static MENUITEM mi[2] = {	/* The RESOURCE definitions */
	MIT_END, MIS_SEPARATOR, 0x0000, 0, NULL, 0,
	MIT_END, MIS_TEXT, 0x0000, IDM_ABOUT, NULL, 0
    };
    HWND	hSM, hSSM;	/* Menu and submenu handles */
    MENUITEM	miSM;		/* System Menu Menuitem     */
    SHORT	idSM;		/* ID of the System Menu    */
    /*
	Get ahold of the system menu
    */
    hSM = WinWindowFromID(Parent(hWnd), FID_SYSMENU);
    idSM = SHORT1FROMMR( WinSendMsg(hSM, MM_ITEMIDFROMPOSITION, NULL, NULL));
    WinSendMsg(hSM, MM_QUERYITEM, MPFROM2SHORT(idSM, FALSE), MPFROMP(&miSM));
    /*
	Manipulate the System SubMenu
    */
    hSSM = miSM.hwndSubMenu;
    WinSendMsg(hSSM, MM_INSERTITEM, MPFROMP(mi), MPFROMP(x[0]));
    WinSendMsg(hSSM, MM_INSERTITEM, MPFROMP(mi+1), MPFROMP(x[1]));
}

MRESULT CALLBACK NewFrameWndProc(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
    Force the frame to stay small enough
*/
    BOOL rc;		/* Return code for WM_QueryTrackInfo */

    switch(msg) {
	case WM_ADJUSTWINDOWPOS:	/* Calculate, then show scrollbars */
	    AvioAdjustFrame(mp1);
	    break;
	case WM_QUERYTRACKINFO:
	    rc = (BOOL)SHORT1FROMMR ((*pfnOldFrameWndProc)(hWnd, msg, mp1, mp2));
            AvioQueryTrackInfo((PTRACKINFO) mp2);
	    return (MRESULT) rc;
	default: break;
    }
    return (*pfnOldFrameWndProc)(hWnd, msg, mp1, mp2);
}

BOOL Filter(USHORT fs, char ch, USHORT vkey) {
    BOOL rc = FALSE;

    if (fs & KC_VIRTUALKEY) {
	switch(vkey) {
	    case VK_HOME:
		if (fs & KC_CTRL) rc = ThdPutString("\033[2J",4);
		return (rc || ThdPutString("\033[H", 3));
	    case VK_UP:
		return ThdPutString("\033[A", 3);
	    case VK_DOWN:
		return ThdPutString("\033[B", 3);
	    case VK_RIGHT:
		return ThdPutString("\033[C", 3);
	    case VK_LEFT:
		return ThdPutString("\033[D", 3);
	    default: break;
	}
    }

    if (fs & KC_CTRL) {
	switch (ch) {
	    case 'l':
	    case 'L': AvioRedraw();
	    case '\0': return FALSE; break;
	    default: ch = Ctrl(ch); break;
	}
    } else {
	switch (ch) {
	    case '\0': return FALSE; break;
	    default: break;
	}
    }
    return(rc || ThdPutChar(ch));
}
