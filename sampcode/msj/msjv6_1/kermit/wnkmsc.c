/*
 * WNKMSC.C
 *
 * Windows Kermit dialog box routines
 *
 * Copyright (c) 1990 by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
 *
 */

#define NOKANJI
#define NOMINMAX
#define NOATOM
#define NOSOUND
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include "ascii.h"
#include "wnkerm.h"
#include "wnkdlg.h"

#define MAXPATHLEN	128
static char *defstring = "*.*";		/* default file type */

/* local function declarations */
static	void	NEAR	krmProtocolInit(HWND hDlg);
static	void	NEAR	krmProtocolSet(HWND hDlg);
static	void	NEAR	krmPacketsInit(HWND hDlg);
static	void	NEAR	krmPacketsSet(HWND hDlg);
static	void	NEAR	krmSetCancelFlags(HWND hDlg);
static	void	NEAR	krmSendFileInit(HWND hDlg);
static	int	NEAR	krmCheckSelect(HWND hDlg);
static	void	NEAR	krmGetSendFileList(HWND hDlg, LONG lParam);
static	BOOL	NEAR	krmListSelect(HWND hDlg, LONG lParam);
static	void	NEAR	krmListBoxOK(HWND hDlg, LONG lParam);
static	void	NEAR	krmPathSelect(HWND hDlg, LONG lParam);
static	int	NEAR	GetCheckedRadioButton(HWND hDlg, WORD start, WORD end);
static  int	NEAR	GetComboboxSelection(HWND hDlg, WORD id, char * buffer);
static	int	NEAR	GetComboboxIndex(HWND hDlg, int id);
static  char*   NEAR    getnamestr(char *str, int len);

/*
 * krmOpenDlgBox
 *
 * Open a modal dialog box
 */
int FAR krmOpenDlgBox(HWND hWnd, FARPROC fpProc, WORD boxnum)
{

    FARPROC fp;
    BOOL result;

  /* make a proc instance for the about box window function */
    fp = MakeProcInstance(fpProc, Kermit.hInst);
  /* create a modal dialog box */
    result = DialogBox(Kermit.hInst, MAKEINTRESOURCE(boxnum),hWnd,fp);
    FreeProcInstance(fp);
    return result;

}

/*
 * krmProtocol
 *
 * This dialog box sets general parameters affecting the
 * Kermit protocol.
 */
BOOL FAR PASCAL krmProtocol(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch(message) {
	case WM_INITDIALOG:
	krmProtocolInit(hDlg);
	break;

	case WM_COMMAND:
	    switch(wParam) {
		case IDOK:
		    krmProtocolSet(hDlg);
		    EndDialog(hDlg, TRUE);
		    break;
		case IDCANCEL:
		    EndDialog(hDlg, FALSE);
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

/*
 * krmProtocolSet
 *
 * In response to values chosen by the user, this
 * routine sets the various protocol data structures.
 * If requested, values are saved to WIN.INI
 */
static void NEAR krmProtocolSet(HWND hDlg)
{

    char szAppStr[80];
    char szKeyStr[80];
    char szValStr[80];

    KermParams.Bell = IsDlgButtonChecked(hDlg, IDD_KRM_BELL);
    KermParams.Timer = IsDlgButtonChecked(hDlg, IDD_KRM_TIMER);
    KermParams.DiscardPartialFile = IsDlgButtonChecked(hDlg, IDD_KRM_DISCARD);
    KermParams.FileWarning = IsDlgButtonChecked(hDlg, IDD_KRM_FILEWARNING);
    KermParams.verbose = IsDlgButtonChecked(hDlg, IDD_KRM_VERBOSE);

    KermParams.BlockCheckType = GetCheckedRadioButton(hDlg, 
					IDD_KRM_1BYTECHK,
					IDD_KRM_3BYTECHK)
				- IDD_KRM_1BYTECHK + 1;

    GetComboboxSelection(hDlg, IDD_KRM_RETRYLIMIT, szValStr);
    KermParams.RetryLimit = atoi(szValStr);

    GetComboboxSelection(hDlg, IDD_KRM_SENDDELAY, szValStr);
    KermParams.SendDelay = atoi(szValStr);

  /* user wants to save the values, so write them to WIN.INI */
    if (IsDlgButtonChecked(hDlg, IDD_KRM_SAVE)) {
	LoadString(Kermit.hInst,IDS_KRM_KERMIT,(LPSTR)szAppStr,sizeof(szAppStr));

        LoadString(Kermit.hInst, IDS_KRM_TIMER, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.Timer, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_VERBOSE, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.verbose, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_DISCARD, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.DiscardPartialFile, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_BELL, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.Bell, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_FILEWARN, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.FileWarning, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_BLOCKCHECKTYPE, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.BlockCheckType, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RETRYLIMIT, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.RetryLimit, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDDELAY, szKeyStr, sizeof(szKeyStr));
        itoa(KermParams.SendDelay, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

	SendMessage((HWND)(-1), WM_WININICHANGE, 0, (LONG)(LPSTR)szAppStr);
    }
}

/*
 * krmProtocolInit
 *
 * Initialize the Kermit protocol settings
 * dialog box.
 */
static void NEAR krmProtocolInit(HWND hDlg)
{
    register int i;
    char buf[80];

    CheckDlgButton(hDlg, IDD_KRM_BELL, KermParams.Bell);
    CheckDlgButton(hDlg, IDD_KRM_TIMER, KermParams.Timer);
    CheckDlgButton(hDlg, IDD_KRM_DISCARD, KermParams.DiscardPartialFile);
    CheckDlgButton(hDlg, IDD_KRM_FILEWARNING, KermParams.FileWarning);
    CheckDlgButton(hDlg, IDD_KRM_VERBOSE, KermParams.verbose);
    CheckRadioButton(hDlg, IDD_KRM_1BYTECHK, IDD_KRM_3BYTECHK,
			   IDD_KRM_1BYTECHK + KermParams.BlockCheckType - 1);

    for (i = 1; i <= 63; i++)
	SendDlgItemMessage(hDlg, IDD_KRM_RETRYLIMIT, CB_INSERTSTRING,
				-1, (LONG)(LPSTR)itoa(i, buf, 10));
    SendDlgItemMessage(hDlg, IDD_KRM_RETRYLIMIT, CB_SELECTSTRING,
			-1, (LONG)(LPSTR)itoa(KermParams.RetryLimit, buf, 10));
				
    for (i = 0; i <= 63; i++)
	SendDlgItemMessage(hDlg, IDD_KRM_SENDDELAY, CB_INSERTSTRING,
				-1, (LONG)(LPSTR)itoa(i, buf, 10));
    SendDlgItemMessage(hDlg, IDD_KRM_SENDDELAY, CB_SELECTSTRING,
			-1, (LONG)(LPSTR)itoa(KermParams.SendDelay, buf, 10));
}

/*
 * krmPackets
 *
 * This dialog box sets the various Kermit packet parameters.
 */
BOOL FAR PASCAL krmPackets(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch(message) {
	case WM_INITDIALOG:
	krmPacketsInit(hDlg);
	break;

	case WM_COMMAND:
	    switch(wParam) {
		case IDOK:
		    krmPacketsSet(hDlg);
		    EndDialog(hDlg, TRUE);
		    break;
		case IDCANCEL:
		    EndDialog(hDlg, FALSE);
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

/*
 * krmPacketsSet
 *
 * In response to values chosen by the user, this
 * routine sets the various packet data structures.
 * If requested, values are saved to WIN.INI
 */
static void NEAR krmPacketsSet(HWND hDlg)
{
    char szAppStr[80];
    char szKeyStr[80];
    char szValStr[80];

    sndparams.mark = (BYTE)GetComboboxIndex(hDlg, IDD_KRM_SENDMARK);
    rcvparams.mark = (BYTE)GetComboboxIndex(hDlg, IDD_KRM_RECEIVEMARK);
    sndparams.maxpktsize = (short)GetComboboxIndex(hDlg, IDD_KRM_SENDMAXSIZE)
			   + KRM_MINPACKETSIZE;
    rcvparams.maxpktsize = (short)GetComboboxIndex(hDlg, IDD_KRM_RECEIVEMAXSIZE)
			   + KRM_MINPACKETSIZE;
    sndparams.timeout = (short)GetComboboxIndex(hDlg, IDD_KRM_SENDTIMEOUT)
			   + KRM_MINTIMEOUT;
    rcvparams.timeout = (short)GetComboboxIndex(hDlg, IDD_KRM_RECEIVETIMEOUT)
			   + KRM_MINTIMEOUT;
    sndparams.padcount = (short)GetComboboxIndex(hDlg, IDD_KRM_SENDPADCOUNT);
    rcvparams.padcount = (short)GetComboboxIndex(hDlg, IDD_KRM_RECEIVEPADCOUNT);
    sndparams.padchar = (BYTE)GetComboboxIndex(hDlg, IDD_KRM_SENDPADCHAR);
    if (sndparams.padchar >= ' ')
	sndparams.padchar = DEL;
    rcvparams.padchar = (BYTE)GetComboboxIndex(hDlg, IDD_KRM_RECEIVEPADCHAR);
    if (rcvparams.padchar >= ' ')
	sndparams.padchar = DEL;
    sndparams.eol = (BYTE)GetComboboxIndex(hDlg, IDD_KRM_SENDEOL);
    rcvparams.eol = (BYTE)GetComboboxIndex(hDlg, IDD_KRM_RECEIVEEOL);
    sndparams.quote = (BYTE)(GetComboboxIndex(hDlg, IDD_KRM_SENDQUOTE) + '!');
    if (sndparams.quote >= '?')
	sndparams.quote += 30;
    rcvparams.quote = (BYTE)(GetComboboxIndex(hDlg, IDD_KRM_RECEIVEQUOTE) + '!');
    if (rcvparams.quote >= '?')
	rcvparams.quote += 30;

    if (IsDlgButtonChecked(hDlg, IDD_KRM_SAVE)) {
	LoadString(Kermit.hInst,IDS_KRM_KERMIT,(LPSTR)szAppStr,sizeof(szAppStr));

        LoadString(Kermit.hInst, IDS_KRM_SENDMARK, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.mark, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVMARK, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.mark, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDMAXLEN, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.maxpktsize, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVMAXLEN, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.maxpktsize, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDTIMEOUT, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.timeout, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVTIMEOUT, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.timeout, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDPADCOUNT, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.padcount, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVPADCOUNT, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.padcount, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDPADCHAR, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.padchar, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVPADCHAR, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.padchar, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDEOL, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.eol, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVEOL, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.eol, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_SENDQUOTE, szKeyStr, sizeof(szKeyStr));
        itoa(sndparams.quote, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

        LoadString(Kermit.hInst, IDS_KRM_RCVQUOTE, szKeyStr, sizeof(szKeyStr));
        itoa(rcvparams.quote, szValStr, 10);
        WriteProfileString(szAppStr, szKeyStr, szValStr);

	SendMessage((HWND)(-1), WM_WININICHANGE, 0, (LONG)(LPSTR)szAppStr);
    }
}

/*
 * krmPacketsInit
 *
 * Initialize the Kermit packets settings
 * dialog box.
 */
static void NEAR krmPacketsInit(HWND hDlg)
{

    register int i;
    char buf[80];

    for (i = 0; i < ' '; i++) {
	sprintf(buf, "CTRL-%c", '@'+i);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDMARK, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEMARK, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    SendDlgItemMessage(hDlg,IDD_KRM_SENDMARK, CB_SETCURSEL,sndparams.mark, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVEMARK,CB_SETCURSEL,rcvparams.mark, 0L);

    for (i = KRM_MINPACKETSIZE; i <= KRM_MAXPACKETSIZE; i++) {
	itoa(i, buf, 10);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDMAXSIZE, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEMAXSIZE, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    SendDlgItemMessage(hDlg,IDD_KRM_SENDMAXSIZE, CB_SETCURSEL,
		       sndparams.maxpktsize - KRM_MINPACKETSIZE, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVEMAXSIZE, CB_SETCURSEL,
		       rcvparams.maxpktsize - KRM_MINPACKETSIZE, 0L);

    for (i = KRM_MINTIMEOUT; i <= KRM_MAXTIMEOUT; i++) {
	itoa(i, buf, 10);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDTIMEOUT, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVETIMEOUT, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    SendDlgItemMessage(hDlg,IDD_KRM_SENDTIMEOUT, CB_SETCURSEL,
		       sndparams.timeout - KRM_MINTIMEOUT, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVETIMEOUT, CB_SETCURSEL,
		       rcvparams.timeout - KRM_MINTIMEOUT, 0L);

    for (i = 0; i <= KRM_MAXPADCOUNT; i++) {
	itoa(i, buf, 10);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDPADCOUNT, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEPADCOUNT, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    SendDlgItemMessage(hDlg,IDD_KRM_SENDPADCOUNT,CB_SETCURSEL,
		       sndparams.padcount, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVEPADCOUNT,CB_SETCURSEL,
		       rcvparams.padcount, 0L);
    for (i = 0; i < ' '; i++) {
	sprintf(buf, "CTRL-%c", '@'+i);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDPADCHAR, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEPADCHAR, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    strcpy(buf, "CTRL ?");
    SendDlgItemMessage(hDlg, IDD_KRM_SENDPADCHAR, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEPADCHAR, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    SendDlgItemMessage(hDlg,IDD_KRM_SENDPADCHAR,CB_SETCURSEL,
		       sndparams.padchar != DEL ? sndparams.padchar : 32, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVEPADCHAR,CB_SETCURSEL,
		       rcvparams.padchar != DEL ? rcvparams.padchar : 32, 0L);

    for (i = 0; i < 32; i++) {
	sprintf(buf, "CTRL-%c", '@'+i);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDEOL, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEEOL, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    SendDlgItemMessage(hDlg,IDD_KRM_SENDEOL, CB_SETCURSEL,sndparams.eol, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVEEOL, CB_SETCURSEL,rcvparams.eol, 0L);

    for (i = 33; i <= 126; i++) {
	if ((i >= 63) && (i <= 95))
	    continue;
	sprintf(buf, "%c", i);
	SendDlgItemMessage(hDlg, IDD_KRM_SENDQUOTE, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
	SendDlgItemMessage(hDlg, IDD_KRM_RECEIVEQUOTE, CB_INSERTSTRING,
			   -1, (LONG)(LPSTR)buf);
    }
    SendDlgItemMessage(hDlg,IDD_KRM_SENDQUOTE, CB_SETCURSEL,
			(33 <= sndparams.quote) && (sndparams.quote < 63) ? 
			sndparams.quote - 33 : sndparams.quote - 63, 0L);
    SendDlgItemMessage(hDlg,IDD_KRM_RECEIVEQUOTE, CB_SETCURSEL,
			(33 <= rcvparams.quote) && (rcvparams.quote < 63) ? 
			rcvparams.quote - 33 : rcvparams.quote - 63, 0L);
    
}

/* 
 * krmSendFileProc
 *
 * This dialog box allows the user to choose a list of
 * files to send to a remote Kermit.
 */
BOOL FAR PASCAL krmSendFileProc(HWND hDlg,unsigned message,WORD wParam,LONG lParam)
{

    switch (message) {
	case WM_INITDIALOG:
	    krmSendFileInit(hDlg);
	    break;

	case WM_COMMAND:
	    switch (wParam) {

		/* turn off SEND button if no files selected */
		case IDD_KRM_LISTSELECT:
		    if (HIWORD(lParam) == LBN_SELCHANGE)
			EnableWindow(GetDlgItem(hDlg, IDD_KRM_SEND),
				      krmListSelect(hDlg, lParam));
		    break;

		/* in this dialog box, IDOK sets selections */
		case IDOK:		    
		    krmListBoxOK(hDlg, lParam);
		    EnableWindow(GetDlgItem(hDlg, IDD_KRM_SEND),
				 krmListSelect(hDlg, lParam));
		    break;
		
		case IDCANCEL:
		    EndDialog(hDlg, FALSE);
		    break;

		/* get selected files into a buffer */
		case IDD_KRM_SEND:
		    krmGetSendFileList(hDlg, lParam);
		    break;

		/* move to another directory */
		case IDD_KRM_PATHSELECT:
		    krmPathSelect(hDlg, lParam);
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

/*
 * krmListSelect
 *
 * Look through the file name list box for a selection.
 */
static BOOL NEAR krmListSelect(HWND hDlg, LONG lParam)
{
    BOOL select = FALSE;
    register int i;
    register int num = (int)SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT,
     					       LB_GETCOUNT, 0, 0L);

    for (i = 0; i < num; i++) {
	select = (BOOL)SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT, 
					  LB_GETSEL, i, 0L);
	if (select)
	    break;
    }
    return select;
}

/*
 * krmPathSelect
 *
 * Move to a new directory in the Send File dialog box.
 */
static void NEAR krmPathSelect(HWND hDlg, LONG lParam)
{

    char buf[MAXPATHLEN];
    char str[MAXPATHLEN];
    int len;

    if (HIWORD(lParam) == LBN_SELCHANGE) {
	len = GetDlgItemText(hDlg, IDD_KRM_EDITSELECT,(LPSTR)str,80);
        DlgDirSelect(hDlg, (LPSTR)buf, IDD_KRM_PATHSELECT);
	if (strchr(str,'*') || strchr(str,'?'))
	    strcat(buf,getnamestr(str,len));
	else
	    strcat(buf, defstring);
        SetDlgItemText(hDlg, IDD_KRM_EDITSELECT, (LPSTR)buf);
    }
    else if (HIWORD(lParam) == LBN_DBLCLK) {
	GetDlgItemText(hDlg, IDD_KRM_EDITSELECT,(LPSTR)buf,80);
	DlgDirList(hDlg, (LPSTR)buf, IDD_KRM_LISTSELECT,IDD_KRM_PATHDISPLAY, 0);
	DlgDirList(hDlg, (LPSTR)buf, IDD_KRM_PATHSELECT,IDD_KRM_PATHDISPLAY, 0xC010);
	SetDlgItemText(hDlg, IDD_KRM_EDITSELECT, (LPSTR)buf);
    }
}

/*
 * krmSendFileInit
 *
 * Initialize the Send File Select dialog box
 */
static void NEAR krmSendFileInit(HWND hDlg)
{

    char buf[10];
    strcpy(buf, defstring);

    DlgDirList(hDlg, (LPSTR)buf, IDD_KRM_LISTSELECT,IDD_KRM_PATHDISPLAY, 0);
    DlgDirList(hDlg, (LPSTR)buf, IDD_KRM_PATHSELECT,IDD_KRM_PATHDISPLAY, 0xC010);
    SetDlgItemText(hDlg, IDD_KRM_EDITSELECT, (LPSTR)buf);
    SendDlgItemMessage(hDlg, IDD_KRM_EDITSELECT,EM_LIMITTEXT,128,0L);
    EnableWindow(GetDlgItem(hDlg, IDD_KRM_SEND), FALSE);

}

/*
 * krmCheckSelect
 *
 * Count the number of files selected in the Send File Selection
 * dialog box.
 */
static int NEAR krmCheckSelect(HWND hDlg)
{

    register int num, i;
    register int count = 0;

    num = (int)SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT,LB_GETCOUNT,0,0L);

    for (i = 0; i < num; i++)
        if (SendDlgItemMessage(hDlg,IDD_KRM_LISTSELECT,LB_GETSEL,(WORD)i,0L))
	    count += 1;

    return count;
}

/*
 * krmGetSendFileList
 *
 * Read the list box for the names of the files selected
 * to be sent by Kermit and store them in a locally allocated buffer
 * as a single string delimited by spaces.
 */
static void NEAR krmGetSendFileList(HWND hDlg, LONG lParam)
{

    char buf[MAXPATHLEN];
    int count, i, num;

    count = krmCheckSelect(hDlg);
    if (count > 0) {
        Kermit.hFilelist = LocalAlloc(LPTR, count * 13);
	if (Kermit.hFilelist != NULL) {
	    Kermit.pFilelist = LocalLock(Kermit.hFilelist);
	    num = (int)SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT,
						LB_GETCOUNT,0,0L);
	    for (i = 0; i < num; i++) {
	        if (SendDlgItemMessage(hDlg,IDD_KRM_LISTSELECT,LB_GETSEL,(WORD)i,0L)) {
	       	    SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT,LB_GETTEXT, 
						(WORD)i, (LONG)(LPSTR)buf);
		    strcat(Kermit.pFilelist, buf);
		    strcat(Kermit.pFilelist, " ");
	        }
            }
        }
        else
	    count = 0;
    }
    if (count == 0) {
	/* no files selected or local alloc error */
    }
    EndDialog(hDlg,count);
}

/*
 * krmListBoxOK
 *
 * If user chooses the SELECT button (IDOK), highlight those files
 * which match the specification in the associated edit select box.
 * If nothing selected, remove all highlighting.
 */
static void NEAR krmListBoxOK(HWND hDlg, LONG lParam)
{

    char buf[MAXPATHLEN];
    int validsspec, count;
    OFSTRUCT myoff;
    int hfile;
    char oldpath[MAXPATHLEN];
    char *ptr;

/* if nothing in the edit box */
    if (GetDlgItemText(hDlg, IDD_KRM_EDITSELECT,(LPSTR)buf,sizeof(buf)) == 0)
/* remove highlighting from file list box */
        SendDlgItemMessage(hDlg,IDD_KRM_LISTSELECT,LB_SETSEL,FALSE,(LONG)(-1));
    else {
/* otherwise, save the path */
    	GetDlgItemText(hDlg, IDD_KRM_PATHDISPLAY,(LPSTR)oldpath,128);
/* see if valid search spec */
	SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT, WM_SETREDRAW, FALSE, 0L);
	SendDlgItemMessage(hDlg, IDD_KRM_PATHSELECT, WM_SETREDRAW, FALSE, 0L);
	validsspec = DlgDirList(hDlg,(LPSTR)buf,IDD_KRM_LISTSELECT,IDD_KRM_PATHDISPLAY,0);
	SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT, WM_SETREDRAW, TRUE, 0L);
	SendDlgItemMessage(hDlg, IDD_KRM_PATHSELECT, WM_SETREDRAW, TRUE, 0L);
/* if valid, get the new path */
	if (validsspec) {
/* buf modified, so update it */
	    SetDlgItemText(hDlg, IDD_KRM_EDITSELECT, (LPSTR)buf);
	    GetDlgItemText(hDlg, IDD_KRM_PATHDISPLAY, (LPSTR)buf, sizeof(buf));
/* if the same as before */
	    if (strcmp(oldpath, buf) == 0)
/* select the whole list box */
                SendDlgItemMessage(hDlg,IDD_KRM_LISTSELECT,
				   LB_SETSEL,TRUE,(LONG)(-1));
	    else {
/* update the directory change box */
                SendDlgItemMessage(hDlg,IDD_KRM_LISTSELECT,
				   LB_SETSEL,FALSE,(LONG)(-1));
	        DlgDirList(hDlg,(LPSTR)buf,IDD_KRM_PATHSELECT,
			   IDD_KRM_PATHDISPLAY, 0xC010);
	    }
/* not a valid search spec */
	} else {
/* clear box */
            SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT, LB_RESETCONTENT,0,0L);
/* see if it exists */
	    hfile = OpenFile((LPSTR)buf, (OFSTRUCT FAR *)&myoff, OF_EXIST);
	    if (hfile != -1) {
/* get the file name and add it to list box */
	        ptr = strrchr(myoff.szPathName,'\\') + 1;
	        SendDlgItemMessage(hDlg, IDD_KRM_LISTSELECT,LB_ADDSTRING,0,
							(LONG)(LPSTR)ptr);
/* get the pathname */
	        strcpy(buf, myoff.szPathName);
	        count = strlen(buf) - strlen(ptr);
	        buf[count] = NUL;
/* select it */
                SendDlgItemMessage(hDlg,IDD_KRM_LISTSELECT,LB_SETSEL,TRUE,-1L);
/* update the path select box */
	        SetDlgItemText(hDlg, IDD_KRM_EDITSELECT, (LPSTR)ptr);
	        DlgDirList(hDlg, (LPSTR)buf, IDD_KRM_PATHSELECT,IDD_KRM_PATHDISPLAY, 0xC010);
	    }
        }
    }
/* select text in edit box */
    SendDlgItemMessage(hDlg, IDD_KRM_EDITSELECT, EM_SETSEL, 0, MAKELONG(0,32767));
}

/* 
 * krmXferDlgBox
 *
 * Show the progress of the file transfer
 */
BOOL FAR PASCAL krmXferDlgBox(HWND hDlg,unsigned message,WORD wParam,LONG lParam)
{

    char buf[40];
    FARPROC fp;
    register HWND hctl;

    switch (message) {

	case WM_INITDIALOG:
	    if (Kermit.mode == IDM_KRM_SEND) {
    	        LoadString(Kermit.hInst,IDS_KRM_SENDING, (LPSTR)buf, sizeof(buf));
		SetDlgItemText(hDlg, IDD_KRM_OPERATION, (LPSTR)buf);
	    }
	    break;

	case WM_COMMAND:
	    switch (wParam) {
		case IDCANCEL:
		    hctl = GetDlgItem(hDlg, IDCANCEL);
		    fp = MakeProcInstance((FARPROC)krmCancelBoxProc, Kermit.hInst);
		    DialogBox(Kermit.hInst,MAKEINTRESOURCE(DT_KRM_CANCEL),hDlg,fp);
		    FreeProcInstance(fp);
		    SetFocus(hctl);
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

/*
 * krmCancelBoxProc
 *
 * Check the file transfer cancel mode
 * and set the corresponding cancel flag
 */
BOOL FAR PASCAL krmCancelBoxProc(HWND hDlg, unsigned message,
				 WORD wParam, LONG lParam)
{

    switch (message) {
	case WM_INITDIALOG:
	    CheckRadioButton(hDlg,IDD_KRM_CANCELFILE, IDD_KRM_CANCELPROTOCOL,
				  IDD_KRM_CANCELFILE);
	    break;

	case WM_COMMAND:	/* system command */
	    switch (wParam) {

		case IDD_KRM_CANCELFILE:
		case IDD_KRM_CANCELBATCH:
		case IDD_KRM_CANCELERROR:
		case IDD_KRM_CANCELPROTOCOL:
		    CheckRadioButton(hDlg, IDD_KRM_CANCELFILE, 
					   IDD_KRM_CANCELPROTOCOL, wParam);
		    break;

		case IDOK:
		    krmSetCancelFlags(hDlg);
		    EndDialog(hDlg,TRUE);
		    break;

		case IDCANCEL:
		    EndDialog(hDlg, FALSE);
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

/*
 * krmSetCancelFlags
 *
 * Set the file transfer cancel flags
 * chosen by the user
 */
static void NEAR krmSetCancelFlags(HWND hDlg)
{
    register int i;
    register WORD param;

    for (i = IDD_KRM_CANCELFILE; i <= IDD_KRM_CANCELPROTOCOL; i++) {
	if (SendDlgItemMessage(hDlg, i, BM_GETCHECK, 0, 0L)) {
	    switch(i) {
		case IDD_KRM_CANCELFILE:
		    param = IDM_KRM_FILEABORT;
		    break;
	        case IDD_KRM_CANCELBATCH:
		    param = IDM_KRM_BATCHABORT;
		    break;
		case IDD_KRM_CANCELERROR:
		    param = IDM_KRM_ERRORABORT;
		    break;
		case IDD_KRM_CANCELPROTOCOL:
		    param = IDM_KRM_CANCEL;
		    break;
	    }
	    PostMessage(Kermit.hWnd, WM_COMMAND, param, 0L);
	    break;
	}
    }
}

/*
 * GetComboboxSelection
 *
 * Utility function for combo boxes
 * Finds the highlighted string and returns its value.
 *
 */
static int NEAR GetComboboxSelection(HWND hDlg, WORD id, char * buffer)
{
    register int index;

    index = (int)SendDlgItemMessage(hDlg, id, CB_GETCURSEL, 0, 0L);
    if (index != CB_ERR)
	index = (int)SendDlgItemMessage(hDlg, id, CB_GETLBTEXT,
					index, (LONG)(LPSTR)buffer);
    return index;
}

/*
 * GetComboBoxIndex
 *
 * Utility function for combo boxes
 * Get the index of the current selection
 */
static int NEAR GetComboboxIndex(HWND hDlg, int id)
{
    return (int)SendDlgItemMessage(hDlg, id, CB_GETCURSEL, 0, 0L);
}

/*
 * GetChecked RadioButton
 *
 * Utility function for radio buttons
 * Finds the checked button, if any, in a group
 * of radio buttons.  If none, then returns highest value
 * of the range plus 1
 */
static int NEAR GetCheckedRadioButton(HWND hDlg, WORD start, WORD end)
{

    register WORD i;

    for (i = start; i <= end; i++)
	if (SendDlgItemMessage(hDlg, i, BM_GETCHECK, 0, 0L))
	    break;

    return i;
}

/*
 * getnamestr
 *
 * Utility function to parse a path name for a file name
 */
static char* NEAR getnamestr(char *str, int len)
{
    char *tmp;

    tmp = str + len;

    while ((*tmp != ':') && (*tmp != '\\') && (tmp > str))
	tmp--;
    if (*tmp != ':' && *tmp != '\\')
	return(tmp);
    else
	return(tmp + 1);

}

