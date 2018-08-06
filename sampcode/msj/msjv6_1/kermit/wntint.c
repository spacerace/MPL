/*
 * WNTERM initialization module
 *
 * Written by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
*/

#define NOMINMAX
#define NOATOM
#define NOKANJI
#define NOSOUND
#include <windows.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <ascii.h>
#include "ttycls.h"
#include "wnterm.h"

/* local function declarations */
static BOOL NEAR RegisterMainWindowClass(HANDLE);
static void NEAR GetPrevInstanceData(HANDLE);
static HWND NEAR MakeAndShowMainWnd(HANDLE, int);
static BOOL NEAR OpenAndSetCommPort(HANDLE);
static BOOL NEAR SearchKey(char *str, char *key, int len);
short FAR PASCAL FindSmallFont(LPLOGFONT, LPTEXTMETRIC, short, LPSTR);

/* this function calls all initialization routines */
BOOL FAR InitProgram(hInstance,hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{

    hInst = hInstance;		// need this value in various places

  // if first instance, register windows and get string for icon.
  // otherwise, just load data normally obtained during registration.
    if (!hPrevInstance) {
        LoadString(hInstance,IDS_ICONSTRING,(LPSTR)szIconTitle,
			sizeof(szIconTitle));
	if (!RegisterMainWindowClass(hInstance))
	    return FALSE;
    }
    else
	GetPrevInstanceData(hPrevInstance);

  // now create and show the main window.
    if (!MakeAndShowMainWnd(hInstance,cmdShow))
	return FALSE;

  // Activate the local/line menu selection to put the terminal on line
    SendMessage(MWnd.hWnd, WM_COMMAND, IDM_OFFLINE,0L);

  // open the communications port.
    if (!OpenAndSetCommPort(hInstance))
	PostMessage(MWnd.hWnd, WM_SYSCOMMAND, SC_CLOSE, 0L);

    return TRUE;
}

/* register the main window */
static BOOL NEAR RegisterMainWindowClass(HANDLE hInstance)
{

    WNDCLASS WndClass;

  // get the class name from the resource segment.
    LoadString(hInstance,IDS_APPNAME,(LPSTR)szAppName,sizeof(szAppName));
 
  // clear the WndClass data area
    memset(&WndClass,0, sizeof(WndClass));

  // fill in the data.
    WndClass.hCursor	= LoadCursor(NULL, IDC_ARROW);
    WndClass.hIcon	= NULL;			// no icon bitmap.
    WndClass.lpszMenuName = (LPSTR)szAppName;	// use this menu.
    WndClass.lpszClassName = (LPSTR)szAppName;	// this class
    WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // let Windows do it
    WndClass.hInstance = hInstance;		// this instance
    WndClass.style = CS_VREDRAW | CS_HREDRAW;	// basic style
    WndClass.lpfnWndProc = MainWndProc;		// window proc
    WndClass.cbWndExtra = sizeof(PTTYWND);	// data associated with window
    if (!RegisterClass((LPWNDCLASS)&WndClass))
	return FALSE;

  // show success.
    return TRUE;

}

/* if we don't need to register the window, we have to load these strings */
static void NEAR GetPrevInstanceData(HANDLE hPrevInstance)
{

    GetInstanceData(hPrevInstance, (PSTR)szAppName, sizeof(szAppName));
    GetInstanceData(hPrevInstance, (PSTR)szIconTitle, sizeof(szIconTitle));
}

/* make the main window */
static HWND NEAR MakeAndShowMainWnd(hInstance, cmdShow)
HANDLE hInstance;
int cmdShow;
{

    char szWinTitle[50];

  // get the title from the resource segment.
    LoadString(hInstance, IDS_WINTITLE, (LPSTR)szWinTitle,sizeof(szWinTitle));

  // create the window.
    MWnd.hWnd = CreateWindow((LPSTR)szAppName,		// class
			     (LPSTR)szWinTitle,		// title
			     WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			     CW_USEDEFAULT,0,		// default position
			     CW_USEDEFAULT,0,		// and size
			     (HWND)NULL,		// no parent
			     (HMENU)NULL,		// class menu
			     (HANDLE)hInstance,		// instance
			     (LPSTR)&MWnd);		// extra create data

  // continue if window created and text buffer initialized.
    if (MWnd.hWnd && MWnd.hVidBuf) {
	ShowWindow(MWnd.hWnd, cmdShow);		// show window
        UpdateWindow(MWnd.hWnd);		// paint it
	return MWnd.hWnd;			// show success
    }
    return NULL;			// show failure
}

static LOGFONT *pttyfat; // used to find a small font if requested in win.ini

/* 
 * this function is called by Windows until all fonts of the specified
 * type have been enumerated.
 */
short FAR PASCAL FindSmallFont(lpLF, lpTM, type, lpData)
LPLOGFONT lpLF;
LPTEXTMETRIC lpTM;
short type;
LPSTR lpData;
{

    register int i;
    static short refheight = 0;
    short refwidth = (short)LOWORD(lpData);
    short height = lpTM->tmHeight + lpTM->tmExternalLeading;
    short width = lpTM->tmAveCharWidth;

  // lpData contains the width and height of the system font, which is used
  // as a reference value.  On the first pass, it must be initialized.
    if (refheight == 0)
	refheight = HIWORD(lpData);

  // if we find a smaller font, get its paramters.
    if ((height < refheight) && (width <= refwidth)) {
	refheight = height;
	pttyfat->lfHeight = lpLF->lfHeight;
	pttyfat->lfWidth = lpLF->lfWidth;
	pttyfat->lfWeight = lpLF->lfWeight;
	pttyfat->lfCharSet = lpLF->lfCharSet;
	pttyfat->lfOutPrecision = lpLF->lfOutPrecision;
	pttyfat->lfClipPrecision = lpLF->lfClipPrecision;
	pttyfat->lfQuality = lpLF->lfQuality;
	pttyfat->lfPitchAndFamily = lpLF->lfPitchAndFamily;
	for (i = 0; i < LF_FACESIZE; i++)
	    pttyfat->lfFaceName[i] = lpLF->lfFaceName[i];
    }
    return refheight;
}

/* called when main window created */
void WndCreate(HWND hWnd, LPCREATESTRUCT pCS)
{

    short width, height, cwidth, cheight;
    TEXTMETRIC TM;
    HDC hDC;
    PTTYWND pMWnd;
    FARPROC fp;
    short result;
    char valstr[80], keystr[20], defstr[20];
    int initlen;
    HFONT holdfont;

  // If the communications port is open, cid >= 0.  If we see that cid 
  // is a large negative number, then we know the comm port is not open.
    cid = INT_MIN;

  // get off/on-line menu strings for later use.
    LoadString(hInst, IDS_OFFLINE, (LPSTR)szOffLine,sizeof(szOffLine));
    LoadString(hInst, IDS_ONLINE, (LPSTR)szOnLine,sizeof(szOnLine));

  // Create a farproc to the main window procedure.
    fpMainWndProc = MakeProcInstance((FARPROC)MainWndProc, hInst);

  // we are going to create a buffer for a window this large.
    width = GetSystemMetrics(SM_CXFULLSCREEN);
    height = GetSystemMetrics(SM_CYFULLSCREEN) - GetSystemMetrics(SM_CYMENU);

  // Read win.ini to see if the small font should be used.
  // If there is no win.ini entry, make one first.
    initlen = GetProfileString(szAppName, NULL, "", valstr, sizeof(valstr));
    itoa(smallfont, defstr, 10);    
    LoadString(hInst,IDS_SMALLFONT,(LPSTR)keystr,sizeof(keystr));
    if (!SearchKey(valstr, keystr, initlen)) {
        WriteProfileString(szAppName, keystr, defstr);
        SendMessage((HWND)-1, WM_WININICHANGE, 0, (LONG)(LPSTR)szAppName);
    }
    smallfont = GetProfileInt(szAppName, keystr, smallfont);

  // Get screen device context
    hDC = GetDC(hWnd);

  // If the small font is requested, enumerate all fonts
  // and pick the one with fixed pitch and the smallest height.
    if (smallfont) {
	pttyfat = (LOGFONT *)LocalAlloc(LPTR, sizeof(LOGFONT));
        LoadString(hInst, IDS_FONTFACE, (LPSTR)valstr, sizeof(valstr));
        fp = MakeProcInstance((FARPROC)FindSmallFont, hInst);
        result = EnumFonts(hDC, valstr, fp, (LPSTR)MAKELONG(cwidth, cheight));
        if ((result > 0) && (cheight - result))		// found one
	    hfont = CreateFontIndirect(pttyfat);
	FreeProcInstance(fp);
	LocalFree((HANDLE)pttyfat);
    }
    else
      // use the system font
        hfont = GetStockObject(SYSTEM_FIXED_FONT);

  //  Get size
    holdfont = SelectObject(hDC, hfont);
    GetTextMetrics(hDC, &TM);
    cwidth = TM.tmAveCharWidth;
    cheight = TM.tmHeight + TM.tmExternalLeading;
    SelectObject(hDC, holdfont);

    ReleaseDC(hWnd, hDC);

  // here we retrieve the pointer to our local window stucture.
    pMWnd = (PTTYWND)LOWORD(pCS->lpCreateParams);

  // set the extra data to be the pointer to the TWnd structure.
    SetWindowWord(hWnd,0,(WORD)pMWnd);

  // now create the text buffer and set some defaults.
    InitTTYWindow(pMWnd,0,0,width,height,cwidth,cheight,
				FALSE,FALSE,TRUE,(WORD)hfont,0x7f);
}

/* open a communications port */
static BOOL NEAR OpenAndSetCommPort(hInstance)
HANDLE hInstance;
{
    register int i;
    char modestr[40];
    char commstr[40];
    char errorstr[80];
    char appstr[10];
    int reply;

    for (i = 0; i < MAXCOMMPORTS; i++) {
        LoadString(hInstance, IDS_COM1 + i, (LPSTR)commstr, sizeof(commstr));
        if ((cid = OpenComm((LPSTR)commstr,INQUESIZE,OUTQUESIZE)) < 0) {
            LoadString(hInstance, IDS_CANNOTOPENFIRSTPORT, (LPSTR)errorstr,
				sizeof(errorstr));
	    reply = MessageBox(MWnd.hWnd, (LPSTR)errorstr, (LPSTR)commstr,
				MB_OKCANCEL | MB_ICONQUESTION);
	    if (reply == IDCANCEL)
	        return FALSE;
	}
	else
	    break;
    }

    if (i == MAXCOMMPORTS) {
        ShowMessage(MWnd.hWnd, IDS_CANNOTOPENANYPORT);
	return FALSE;
    }

  // we opened a port, so we will attach the port name to the title.
    GetWindowText(MWnd.hWnd, (LPSTR)modestr, sizeof(modestr));
    strcat(modestr, commstr);
    SetWindowText(MWnd.hWnd, (LPSTR)modestr);        

  // now read in the device control block.
    if (GetCommState(cid, (DCB FAR *)&CommData) >= 0) {
     // read win.ini to find out how to set the port.
	LoadString(hInstance, IDS_PORTSECTION, (LPSTR)appstr, sizeof(appstr));
	GetProfileString((LPSTR)appstr,(LPSTR)commstr,(LPSTR)"",
			(LPSTR)modestr,sizeof(modestr));
     // concatenate the comm data to the name of the port.
        strcat(commstr, modestr);
     // build the DCB according to commstr.
        if (BuildCommDCB((LPSTR)commstr, (DCB FAR *)&CommData) >= 0) {
	  // modify some additional DCB fields.
	    CommData.XonLim = INQUESIZE / 8;	// Low water mark
	    CommData.XoffLim = INQUESIZE / 8;	// High water mark
	    CommData.fNull = TRUE;		// ignore null data
	    CommData.XonChar = XON;		// use ^Q for xon
	    CommData.XoffChar = XOFF;		// use ^S for xoff
	    CommData.fOutX = TRUE;		// set outbound flow control
	    CommData.fInX = TRUE;		// set inbound flow control
	  // finally, set the values.
	    if (SetCommState ((DCB FAR *)&CommData) >= 0)
	        return TRUE;
	}
    }
  // everything failed, close the comm port and quit.
    ShowMessage(MWnd.hWnd, IDS_COMMSETERROR);
    CloseComm(cid);
    return FALSE;
}

/* Search the keystring buffer for 'key'. Keystrings are delimited by 0's */
static BOOL NEAR SearchKey(char *str, char *key, int len)
{

    register int i;
    register char *startptr;
    startptr = str;

    for (i = 0; i < len; i++) {
	if (*str++)
	    ;
	else {
	    if (strcmp(key, startptr) == 0)
		return(TRUE);
	    startptr = str;
	}
    }
    return (FALSE);

}
