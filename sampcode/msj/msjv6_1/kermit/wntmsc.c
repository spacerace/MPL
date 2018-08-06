/* 
 * Less frequently used functions for WNTERM
 *
 * Written by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
*/

#define NOKANJI
#define NOATOM
#define NOSOUND
#include <windows.h>
#include <string.h>
#include "ttycls.h"
#include "wnterm.h"
#include "wntdlg.h"

/* local function declarations */
static void NEAR SetCommInit(HWND hDlg);
static BOOL NEAR SetCommValues(HWND hDlg);

/* procedure for the about box */
BOOL FAR PASCAL AboutBoxProc(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{

    switch (message) {
	case WM_INITDIALOG:	/* nothing to initialize */
	    break;
	case WM_COMMAND:	/* system command */
	    switch (wParam) {
		case IDOK:	/* respond only to these parameter */
		case IDCANCEL:
		    EndDialog(hDlg,TRUE);
		    break;
		default:
		    return FALSE;	/* windows will handle this */
	    }
	    break;

	default:
	    return FALSE;	/* and, windows will process this as well */
    }
    return TRUE;		/* if we process message, return TRUE */
}

/* display a simple message box according to the string id */
void ShowMessage(HWND hWnd, int msgnum)
{

    char szMessage[80];

  /* get the string corresponding to msgnum */
    LoadString(hInst, msgnum, (LPSTR)szMessage, sizeof(szMessage));
    
  /* put up this simple message box */
    MessageBox(hWnd,(LPSTR)szMessage, (LPSTR)szAppName, 
		MB_ICONASTERISK | MB_OK);

}

/* set the communications parameters selected by user */
BOOL FAR PASCAL SetCommParams(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{

    int result;

    switch (message) {
	case WM_INITDIALOG:		/* initialize the box parameters */
	    SetCommInit(hDlg);
	    break;

	case WM_COMMAND:	
	    switch (wParam) {
		case IDD_BAUDRATE: /* if nothing in baudrate box, disable OK */
		    if (HIWORD(lParam) == EN_CHANGE) {
    			result = (int)SendDlgItemMessage(hDlg,wParam,
						WM_GETTEXTLENGTH,0,0L);
			EnableWindow(GetDlgItem(hDlg, IDOK), result);
		    }
		    break;

		case IDOK:	/* set comm parameters from values in box */
		    result = SetCommValues(hDlg);
		    if (result)
		        EndDialog(hDlg, TRUE);
		    break;
		
		case IDCANCEL:		/* user wants no changes */
		    EndDialog(hDlg, FALSE);
		    break;

		case IDD_COM1:	/* highlight the button selected */
		case IDD_COM2:
		case IDD_COM3:
		case IDD_COM4:
		    CheckRadioButton(hDlg, IDD_COM1, IDD_COM4, wParam);
		    break;

		case IDD_SEVENBITS:  /* highlight the button selected */
		case IDD_EIGHTBITS:
		    CheckRadioButton(hDlg,IDD_SEVENBITS,IDD_EIGHTBITS,wParam);
		    break;

		case IDD_NOPARITY:   /* highlight the button selected */
		case IDD_ODDPARITY:
		case IDD_EVENPARITY:
		case IDD_MARKPARITY:
		case IDD_SPACEPARITY:
		    CheckRadioButton(hDlg,IDD_NOPARITY,IDD_SPACEPARITY,wParam);
		    break;

		default:
		    return FALSE;
	    }
	    break;

	default:
	    return FALSE;
    }
    return TRUE;
}

/* read the communications DCB and initialize the dialog box accordingly */
static void NEAR SetCommInit(HWND hDlg)
{

  /* enter the baud rate into the baudrate edit control */
    SetDlgItemInt(hDlg,IDD_BAUDRATE,CommData.BaudRate,FALSE);

  /* highlight the corresponding radio buttons */
    CheckRadioButton(hDlg,IDD_SEVENBITS, IDD_EIGHTBITS,
				IDD_SEVENBITS + CommData.ByteSize -7);
    CheckRadioButton(hDlg,IDD_NOPARITY, IDD_SPACEPARITY,
				IDD_NOPARITY + CommData.Parity - NOPARITY);
    CheckRadioButton(hDlg,IDD_COM1, IDD_COM4, IDD_COM1 + CommData.Id);

}

/* set communications values; if failure, try to restore the old ones */
static BOOL NEAR SetCommValues(HWND hDlg)
{

    register int i, state;
    short newcid;
    char comstr[10];
    char wintitle[50];
    BYTE newid;
    BYTE oldid = CommData.Id;			/* save the current values */
    WORD oldbaud = CommData.BaudRate;
    BYTE oldsize = CommData.ByteSize;
    BYTE oldpar = CommData.Parity;
    BYTE oldstop = CommData.StopBits;

  /* read the rate from the edit control */
    CommData.BaudRate = GetDlgItemInt(hDlg,IDD_BAUDRATE,NULL, FALSE);

    for (i = IDD_COM1; i <= IDD_COM4; i++) {
      /* get the button which is checked */
	state = (int)SendDlgItemMessage(hDlg, i, BM_GETCHECK, 0, 0L);
	if (state) {
	    newid = (BYTE)(i - IDD_COM1);	/* convert to comm cid */
	    break;
	}
    }
  /* read the button which is checked */
    for (i = IDD_SEVENBITS; i <= IDD_EIGHTBITS; i++) {
	state = (int)SendDlgItemMessage(hDlg, i, BM_GETCHECK, 0, 0L);
	if (state) {
	  /* convert to a communications byte size parameter */
	    CommData.ByteSize = (BYTE)(i - IDD_SEVENBITS + 7);
	    break;
	}
    }
  /* read the button which is checked */
    for (i = IDD_NOPARITY; i <= IDD_SPACEPARITY; i++) {
	state = (int)SendDlgItemMessage(hDlg, i, BM_GETCHECK, 0, 0L);
	if (state) {
	  /* convert to a communications parity value */
	    CommData.Parity = (BYTE)(i - IDD_NOPARITY + NOPARITY);
	    break;
	}
    }
    if (CommData.BaudRate <= 200)	/* two stop bits in this case */
	CommData.StopBits = TWOSTOPBITS;
    else 
	CommData.StopBits = ONESTOPBIT;

  /* if requesting a new comm port, try to open it first */
    if (newid != oldid) {
        LoadString(hInst, IDS_COM1 + newid, (LPSTR)comstr, sizeof(comstr));
	newcid = OpenComm((LPSTR)comstr, INQUESIZE, OUTQUESIZE);
      /* if successful, close the old one */
	if (newcid >= 0) {
	    state = CloseComm(cid);
	  /* old one closed, so update window title and cid number */
	    if (state == 0) {
		cid = newcid;
	        CommData.Id = (BYTE)cid;
	        LoadString(hInst,IDS_WINTITLE,(LPSTR)wintitle,sizeof(wintitle));
		strcat(wintitle,comstr);
		SetWindowText(MWnd.hWnd, (LPSTR)wintitle);				
	    }
	    else {
	      /* failed, go back to old port and show error */
		CloseComm(newcid);
		ShowMessage(hDlg, IDS_CANNOTOPENPORT);
		return FALSE;
	    }
	}
	else {
	 /* failed to open new port, show error */
	    ShowMessage(hDlg, IDS_CANNOTOPENPORT);
	    return FALSE;
	}
    }
  /* set the state of the comm DCB */
    state = SetCommState((DCB FAR *)&CommData);
  /* if unsuccessful, restore all the old values and show error */
    if (state) {
	CommData.BaudRate = oldbaud;
	CommData.ByteSize = oldsize;
	CommData.Parity = oldpar;
	CommData.StopBits = oldstop;
	SetCommState((DCB FAR *)&CommData);
	SetCommInit(hDlg);
        ShowMessage(hDlg, IDS_COMMSETERROR);
	return FALSE;
    }
    return TRUE;
}
