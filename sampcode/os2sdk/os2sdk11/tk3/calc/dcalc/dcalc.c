/****************************** Module Header ******************************\
* Module Name:	dcalc.c - Dialog form of the Calc application
*
* OS/2 Presentation Manager version of Calc, ported from Windows version
*
* Created by Microsoft Corporation, 1989
*
\***************************************************************************/

#define INCL_DEV
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSNLS
#define INCL_ERRORS
#define INCL_WINBUTTONS
#define INCL_WINCLIPBOARD
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WINSWITCHLIST
#define INCL_WINTRACKRECT
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dcalc.h"

/************* GLOBAL VARIABLES         */

char chLastKey, currkey;
char szCalcClass[] = "Calculator";
char szTitle[30];
char szreg1[20], szreg2[20], szmem[20], szregx[20];
/* hope 20 is enough for kanji error string */
char szErrorString[20], szPlusMinus[2];
short charwidth, charheight;
int aspectx, aspecty, nchszstr;
extern BOOL fError  = FALSE;
BOOL fValueInMemory = FALSE;
BOOL fMDown = FALSE;
UCHAR mScan = 0;

#define TOLOWER(x)   ( (((x) >= 'A') && ((x) <= 'Z')) ? (x)|0x20 : (x))
#define WIDTHCONST  28
#define CXCHARS     37
#define CYCHARS     13

HAB hab;
HDC hdcLocal;			    /* Local used for button bitmap */
HPS hpsLocal;
HDC hdcSqr;			    /* Sqr used for square-root bitmap */
HPS hpsSqr;
HBITMAP hbmLocal, hbmSqr;
HMQ  hmqCalc	    = NULL;

HWND hwndCalc	    = NULL,
     hwndMenu	    = NULL;

HPOINTER hptrFinger = NULL,
	 hptrIcon   = NULL;

DEVOPENSTRUC dop =		    /* used by DevOpenDC */
{
    NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

static char bButtonValues[] =	    /* Button values */
{
    0xBC, 0xBB, 0xBA, 0xB9,  '0',  '1',  '2',  '3',  '4',
     '5',  '6',  '7',  '8',  '9',  '.',  '/',  '*',  '-',
     '+',  'q',  '%',  'c',  '=', 0xB1, NULL
};

/************* PROCEDURE DECLARATIONS   */

MPARAM CALLBACK AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
BOOL CalcInit(VOID);
VOID CalcPaint( HWND, HPS);
MRESULT CALLBACK fnDlgCalc(HWND, USHORT, MPARAM, MPARAM);
VOID cdecl main(VOID);
VOID DataXCopy( VOID);
VOID DataXPaste( VOID);
VOID DrawNumbers( HPS);
VOID Evaluate(BYTE);
VOID InitCalc( VOID);
BOOL InterpretChar( CHAR);
VOID ProcessKey(HWND, WPOINT *);
char Translate(WPOINT *);
VOID UpdateDisplay( VOID);



/********************************************************************
   Write the appropriate number or error string to the display area
   and mark memory-in-use if appropriate.
 */

BYTE aszDisplayBuff[20];

VOID UpdateDisplay()
{
    strcpy(aszDisplayBuff, fError? "Error" :szreg1);
    strcat(aszDisplayBuff, fValueInMemory? " M" : "  ");

    WinSetDlgItemText(hwndCalc, TXT_RESULT_DISPLAY, aszDisplayBuff);
}


/**********************************************************************
    Display helpful info
 */

MPARAM CALLBACK AboutDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    if (msg == WM_COMMAND)
    {
        WinDismissDlg(hwnd, TRUE);
	return(MPFROMSHORT(TRUE));
    }
    else return(WinDefDlgProc(hwnd, msg, mp1, mp2));
}


/**********************************************************************
    General initialization
 */

BOOL CalcInit()
{
    hab = WinInitialize(0);

    hmqCalc = WinCreateMsgQueue( hab, 0);

    return(TRUE);
}

/**********************************************************************
    main procedure
 */

VOID cdecl main()
{
    QMSG qmsg;

    if (!CalcInit()) {			    /* general initialization */
        WinAlarm(HWND_DESKTOP, 0xffff);
        goto exit;
    }

    WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP, fnDlgCalc, NULL, CALCDLG, NULL);

    if (hwndCalc)
	while (WinGetMsg( hab, (PQMSG)&qmsg, NULL, 0, 0))
	    WinDispatchMsg( hab, (PQMSG)&qmsg);

exit:					    /* clean up */

    if (hwndMenu)      WinDestroyWindow(hwndMenu);

    WinDestroyMsgQueue(hmqCalc);
    WinTerminate(hab);

    DosExit(EXIT_PROCESS, 0);		    /* exit without error */
}


/*************************************************************************
   Calc Dialog Window Procedure
 */


USHORT	idProcess, idThread;
SWCNTRL swc;
HSWITCH hsw;
USHORT	usWidthCalc, usHeightCalc;

MRESULT CALLBACK fnDlgCalc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    RECTL rectl;
    BOOL fClip;
    USHORT fi, idCtrl;
    MRESULT mresult;
    USHORT  afSWP;
    PSWP    pswp;

    static BOOL fMinimized;


    switch (msg)
    {
    case WM_INITDLG:

/* Set up the global state assumed by the dialog.
 */
	hwndCalc = hwnd;
	hwndMenu = WinLoadMenu(hwnd, NULL, IDR_CALC);

	fMinimized = FALSE;

	hptrFinger = WinLoadPointer(HWND_DESKTOP, (HMODULE)NULL, IDP_FINGER);
	hptrIcon   = WinLoadPointer(HWND_DESKTOP, (HMODULE)NULL, IDR_CALC);

	WinSetWindowULong(hwndCalc, QWL_STYLE,
			  FS_ICON | WinQueryWindowULong(hwndCalc, QWL_STYLE)
			 );

	WinSendMsg(hwndCalc, WM_SETICON,     (MPARAM) hptrIcon, 0L);
	WinSendMsg(hwndCalc, WM_UPDATEFRAME, (MPARAM) 0L,	0L);

	WinQueryWindowRect(hwndCalc, &rectl);
	 usWidthCalc= (SHORT) (rectl.xRight - rectl.xLeft);
	usHeightCalc= (SHORT) (rectl.yTop   - rectl.yBottom);

	WinQueryWindowProcess(hwndCalc, &idProcess, &idThread);

	WinLoadString(NULL, NULL, 1, 30, (PSZ)szTitle);
	WinLoadString(NULL, NULL, 2, 20, (PSZ)szErrorString);
	WinLoadString(NULL, NULL, 3, 2,  (PSZ)szPlusMinus);

	strcpy(swc.szSwtitle, szTitle);
	swc.hwnd	  = hwndCalc;
	swc.hwndIcon	  = hptrIcon;
	swc.hprog	  = (ULONG)NULL;
	swc.idProcess	  = idProcess;
	swc.idSession	  = (USHORT)0;
	swc.uchVisibility = SWL_VISIBLE;
	swc.fbJump	  = SWL_JUMPABLE;
	hsw		  = WinAddSwitchEntry((PSWCNTRL)&swc);

	InitCalc();			    /* arithmetic initialization */

	WinSetActiveWindow(HWND_DESKTOP, hwndCalc);

	WinSetFocus(HWND_DESKTOP, hwndCalc);

	break;

    case WM_MINMAXFRAME:

	pswp= PVOIDFROMMP(mp1);

	if (pswp->fs & SWP_MINIMIZE) fMinimized= TRUE;
	else
	    if (pswp->fs & SWP_RESTORE) fMinimized= FALSE;

	return(WinDefDlgProc(hwnd, msg, mp1, mp2));

	break;

    case WM_DESTROY:

	WinDestroyPointer(hptrIcon  );	hptrIcon  = NULL;
	WinDestroyPointer(hptrFinger);	hptrFinger= NULL;

	break;

    case WM_INITMENU:

        fClip = FALSE;

        if (WinOpenClipbrd(NULL))
        {
            fClip = WinQueryClipbrdFmtInfo(NULL, CF_TEXT, (USHORT FAR *)&fi);
            WinCloseClipbrd(NULL);
        }

	WinSendMsg((HWND)mp2, MM_SETITEMATTR,
		   (MPARAM) MAKELONG(CMD_PASTE, TRUE),
		   (MPARAM) MAKELONG(MIA_DISABLED, fClip ? 0 : MIA_DISABLED));
        break;

    case WM_ADJUSTWINDOWPOS:

	mresult= WinDefDlgProc(hwnd, msg, mp1, mp2);

	if (fMinimized) return(mresult);

	afSWP= (pswp= (PSWP) mp1)->fs;

	if (	 afSWP & (SWP_SIZE     | SWP_MAXIMIZE)
	    && !(afSWP &  SWP_MINIMIZE)
	   )
	{
	    pswp->y += pswp->cy - usHeightCalc;
	    pswp->cx =	usWidthCalc;
	    pswp->cy = usHeightCalc;
	}

	return(mresult);


    case WM_COMMAND:

	fError = FALSE;

	idCtrl= SHORT1FROMMP(mp1);

	if (   SHORT1FROMMP(mp2) == BN_CLICKED
	    && idCtrl >= BUTTON_MC
	    && idCtrl <= BUTTON_CHANGE_SIGN
	   )
	{
	    Evaluate(bButtonValues[idCtrl-BUTTON_MC]);
	    UpdateDisplay();
	}
	else
	    switch(idCtrl)
	    {
	    case CMD_COPY:
		DataXCopy();			/* copy to clipboard */
		break;
	    case CMD_PASTE:
		DataXPaste();			/* paste from clipboard */
		break;
	    case CMD_EXIT:
		WinPostMsg(hwndCalc, WM_QUIT, 0L, 0L);
		break;
	    case CMD_ABOUT:
		WinDlgBox(HWND_DESKTOP, hwndCalc, (PFNWP)AboutDlgProc, NULL,
			  1, (PSZ)NULL);
		break;
	    }
        break;


    case WM_CONTROLPOINTER:
	if (!fMinimized) return(hptrFinger);
	else return(WinDefDlgProc(hwnd, msg, mp1, mp2));


    case WM_MOUSEMOVE:
	if (!fMinimized) WinSetPointer(HWND_DESKTOP, hptrFinger);
        break;

    case WM_BUTTON1DOWN:

	return(WinDefDlgProc(hwnd, WM_TRACKFRAME, (MPARAM) TF_MOVE, mp2));

	break;

    case WM_CHAR:

	fError = FALSE;

	if (SHORT1FROMMP(mp1) & KC_KEYUP)
	{
	    if (CHAR4FROMMP(mp1) == mScan)
		fMDown = FALSE; 		/* 'm' key went up */
	}
	else if (SHORT1FROMMP(mp1) & KC_CHAR)
        {
	    if (InterpretChar(CHAR1FROMMP(mp2)))
		UpdateDisplay();
	    else if ((CHAR1FROMMP(mp2)=='m') || (CHAR1FROMMP(mp2)=='M'))
	    {
		mScan = CHAR4FROMMP(mp1);	/* save 'm' key scan code */
		fMDown = TRUE;			/* 'm' key went down */
	    }
        }
        break;

    case WM_SETFOCUS:
	if ((HWNDFROMMP(mp1)==hwndCalc) && !mp2);
	    fMDown = FALSE;			/* since we are losing focus */

    default:
	return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0L);
}


/*************************************************************************
    translate & interpret keys (ie. locate in logical keyboard)
 */

BOOL InterpretChar(ch)
register CHAR ch;
{
    BOOL fDone;
    CHAR *chstep;

    fDone = FALSE;
    chstep = bButtonValues;
    switch (ch)
    {
    case 'n':
	ch = szPlusMinus[0];
        break;
    case 27:                        /* xlate Escape into 'c' */
        ch = 'c';
        break;
    case '\r':                      /* xlate Enter into '=' */
        ch = '=';
        break;
    }

    if (fMDown) 		    /* Do memory keys */
    {
        switch (ch)
        {
        case 'c':
        case 'C':
            ch = '\274';
            break;
        case 'r':
        case 'R':
            ch = '\273';
            break;
        case '+':
            ch = '\272';
            break;
        case '-':
            ch = '\271';
            break;
        }
    }

    while (!fDone && *chstep)
    {
        if (*chstep++ == ch)
	    fDone = TRUE;		/* char found in logical keyboard */
    }

    if (fDone)
    {
	Evaluate(ch);
    }

    return (fDone);
}
