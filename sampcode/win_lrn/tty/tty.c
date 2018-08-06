/*  TTY.c
    a very simple sample terminal application. It demonstrates
    the basics in the use of the Windows Comm functions. It also
    shows the basic structure for a terminal program.
    Currently, there are a number of things that are hard coded,
    which could be made settable in a dialog box. These can be
    found in the Terminal data section below. They are things like
    the background and text colors, the background brush, and the
    font to be used. Also, the baud rate and other communication
    settings are hard coded. You can experiment with different
    settings, or change them just to match your available serial
    connection.

    Microsoft makes no claims to the correctness or operation of
    this code. It is provided for example and learning purposes
    only.

    Copyright (c) Microsoft 1985,1986,1987,1988,1989 */

#include "windows.h"
#include "TTY.h"

char szAppName[10];
char szAbout[10];
char szMessage[15];
int MessageLength;

/* Terminal data */
typedef char cpt[2];	    // point array of char sized values

#define cpX 0
#define cpY 1

#define PortSetting  "com1"
#define CommSettings "com1:96,n,8,1"
#define BufMax 160
HWND hTTYWnd;
char MsgBuff[BufMax + 1];   // Buffer to hold incoming characters
char sScrBuff[25][81];	    // Array of characters on TTY
char cLastLine; 	    // Index of last line on TTY in the array
char cCurrPos;		    // Current TTY output position
BOOL bConnected;	    // Flag to indicate if connected
int TTYCharWidth, TTYCharHeight;    // width and height of TTY font chars
DCB CommDCB;		    // DCB for comm port
COMSTAT CommStat;	    // COMSTAT info buffer for comm port

DWORD TTYbkRGB = RGB(0,0,0);		// background color
DWORD TTYtextRGB = RGB(255,255,255);	// text color
#define TTY_BRUSH BLACK_BRUSH		// background brush
#define TTY_FONT DEVICEDEFAULT_FONT	// font used for display

char sTemp[256];
HFONT hOldFont;

static HANDLE hInst;
FARPROC lpprocAbout;

long FAR PASCAL TTYWndProc(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL About( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    if (message == WM_COMMAND
	|| message == WM_LBUTTONDOWN) {
        EndDialog( hDlg, TRUE );
        return TRUE;
        }
    else if (message == WM_INITDIALOG)
        return TRUE;
    else return FALSE;
}

SetupTTYDC(HWND hWnd, HDC hDC)
{
    RECT rClRect;

    hOldFont = SelectObject(hDC,GetStockObject(TTY_FONT));
    GetClientRect(hWnd,&rClRect);
    SetViewportOrg(hDC,0,rClRect.bottom-TTYCharHeight*25);
    SetTextColor(hDC,TTYtextRGB);
    SetBkColor(hDC,TTYbkRGB);
    SetBkMode(hDC,OPAQUE);

}

ResetTTYDC(HWND hWnd, HDC hDC)
{
    SelectObject(hDC,hOldFont);
}

char *GetTTYLine(char ndx)		// get ptr to text for give line
{
    char pos;

    pos = cLastLine + ndx + 1;
    pos = pos > 24 ? (pos % 24)-1 : pos;
    return(sScrBuff[pos]);
}

POINT TTYScrPos(char X, char Y) 	// get window pos from TTY pos
{
    POINT ScrPos;

    ScrPos.x = X * TTYCharWidth;
    ScrPos.y = Y * TTYCharHeight;
    return(ScrPos);
}

// process incoming text to tty window - simulate tty actions
void putTTY(HWND hWnd, HDC hDC, WORD wParam, LPSTR lParam)
{
    short i,j;
    POINT ptTTYPos;
    char *sBuffer;
    char len = 0;
    RECT rClRect;
    char *psLine;
    short iLinesOut = 0;

    sBuffer = &sScrBuff[cLastLine][cCurrPos];

    for(i=0;i<wParam;i++){
	switch(lParam[i]) {

	case '\r': // return
	    // move to the start of the line
	    cCurrPos = 0;
	    len = 0;

	    break;

	case '\b': // backspace

	    // (ok, so most of this should be a function since it is
	    //	  repeated later)
	    // Note: it is easier to go ahead and display the text
	    //	up to this point for things such as backspace. This
	    //	will apply to other things, like arrow keys, tabs, etc.

	    // scroll screen by number of lines received
	    if(iLinesOut > 0) {
		// scroll iLinesOut lines
		ScrollWindow(hWnd,0,-(TTYCharHeight * iLinesOut),NULL,NULL);
		rClRect.top = TTYCharHeight * (24 - iLinesOut);
		rClRect.bottom = TTYCharHeight*25;
		rClRect.left = 0;
		rClRect.right = TTYCharWidth * 80;
		FillRect(hDC,&rClRect,GetStockObject(TTY_BRUSH));

	    }

	    // display lines in scrolled area previous to current line
	    for(j = 1;j<=iLinesOut;j++) {
		// TextOut line cLastLine - j;
		psLine = GetTTYLine(24 - j);
		ptTTYPos = TTYScrPos(0,24 - j);
		TextOut(hDC,
			ptTTYPos.x,
			ptTTYPos.y,
			psLine,
			strlen(psLine));

	    }
	    iLinesOut = 0;

	    if(len > 0){
		// display current line
		ptTTYPos = TTYScrPos(cCurrPos,24);
		TextOut(hDC,
		    ptTTYPos.x,
		    ptTTYPos.y,
		    sBuffer,
		    len);
		cCurrPos += len;
	    }
	    len = 0;

	    // move back one space
	    if(cCurrPos > 0) {
		--cCurrPos;
		ptTTYPos = TTYScrPos(cCurrPos,24);
		TextOut(hDC,
		    ptTTYPos.x,
		    ptTTYPos.y,
		    " ",
		    1);
	    }
	    break;

	case '\n': // new line

	    // "scroll" the window
	    ++iLinesOut; // increment lines to scroll
	    ++cLastLine;
	    if(cLastLine > 24) cLastLine = 0;

	    // clear the new line
	    sBuffer = &sScrBuff[cLastLine][0];
	    for(j=0;j<80;j++) sBuffer[j] = '\0';
	    len = 0;

	    break;

	default:
	    //add char to buffer
	    if(cCurrPos < 80){
		sBuffer[len] = lParam[i];
		++len;
	    }
	    break;
	}
    }

    // scroll screen by number of lines received
    if(iLinesOut > 0) {
	// scroll iLinesOut lines
	ScrollWindow(hWnd,0,-(TTYCharHeight * iLinesOut),NULL,NULL);
	rClRect.top = TTYCharHeight * (24 - iLinesOut);
	rClRect.bottom = TTYCharHeight*25;
	rClRect.left = 0;
	rClRect.right = TTYCharWidth * 80;
	FillRect(hDC,&rClRect,GetStockObject(TTY_BRUSH));

    }

    // display lines in scrolled area previous to current line
    for(j = 1;j<=iLinesOut;j++) {
	// TextOut line cLastLine - j;
	psLine = GetTTYLine(24 - j);
	ptTTYPos = TTYScrPos(0,24 - j);
	TextOut(hDC,
		ptTTYPos.x,
		ptTTYPos.y,
		psLine,
		strlen(psLine));

    }

    if(len > 0){
	// display current line
	ptTTYPos = TTYScrPos(cCurrPos,24);
	TextOut(hDC,
	    ptTTYPos.x,
	    ptTTYPos.y,
	    sBuffer,
	    len);
	cCurrPos += len;
    }
}


void TTYPaint(HDC hDC )    // repaint the text in the tty
{
    char *psLine;
    POINT ptTTYPos;
    register int i;

    for(i=0;i<25;i++){
	psLine = GetTTYLine(i);
	ptTTYPos = TTYScrPos(0,i);
	TextOut(hDC,
		ptTTYPos.x,
		ptTTYPos.y,
		psLine,
		strlen(psLine));
    }
}


/* Procedure called when the application is loaded for the first time */
BOOL TTYInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS	pTTYClass;

    /* Load strings from resource */
    LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );
    LoadString( hInstance, IDSABOUT, (LPSTR)szAbout, 10 );
    MessageLength = LoadString( hInstance, IDSTITLE, (LPSTR)szMessage, 15 );

    pTTYClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pTTYClass->hCursor	      = LoadCursor( NULL, IDC_ARROW );
    pTTYClass->hIcon	      = LoadIcon( hInstance, MAKEINTRESOURCE(TTYICON) );
    pTTYClass->lpszMenuName   = (LPSTR)"TTYMENU";
    pTTYClass->lpszClassName  = (LPSTR)szAppName;
    pTTYClass->hbrBackground  = (HBRUSH)GetStockObject( TTY_BRUSH );
    pTTYClass->hInstance      = hInstance;
    pTTYClass->style	      = CS_HREDRAW | CS_VREDRAW | CS_OWNDC
				| CS_BYTEALIGNCLIENT;
    pTTYClass->lpfnWndProc    = TTYWndProc;

    if (!RegisterClass( (LPWNDCLASS)pTTYClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pTTYClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;
    RECT rClRect, rWndRect;
    short iXExtra,iYExtra;
    short iNumRead,iError;

    if (!hPrevInstance) {
        /* Call initialization procedure if this is the first instance */
	if (!TTYInit( hInstance ))
            return FALSE;
        }
    else {
        /* Copy data from previous instance */
        GetInstanceData( hPrevInstance, (PSTR)szAppName, 10 );
        GetInstanceData( hPrevInstance, (PSTR)szAbout, 10 );
        GetInstanceData( hPrevInstance, (PSTR)szMessage, 15 );
        GetInstanceData( hPrevInstance, (PSTR)&MessageLength, sizeof(int) );
        }

    hWnd = CreateWindow((LPSTR)szAppName,
                        (LPSTR)szMessage,
                        WS_TILEDWINDOW,
			CW_USEDEFAULT,	  /*  x - ignored for tiled windows */
			CW_USEDEFAULT,	  /*  y - ignored for tiled windows */
			CW_USEDEFAULT,	  /* cx - ignored for tiled windows */
			CW_USEDEFAULT,	  /* cy - ignored for tiled windows */
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Save instance handle for DialogBox */
    hInst = hInstance;

    /* Bind callback function with module instance */
    lpprocAbout = MakeProcInstance( (FARPROC)About, hInstance );

    /* Insert "About..." into system menu */
    hMenu = GetSystemMenu(hWnd, FALSE);
    ChangeMenu(hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu, 0, (LPSTR)szAbout, IDSABOUT, MF_APPEND | MF_STRING);

    /* Make window visible according to the way the app is activated */
    GetWindowRect(hWnd,&rWndRect);
    GetClientRect(hWnd,&rClRect);
    iXExtra = (rWndRect.right-rWndRect.left)-(rClRect.right-rClRect.left);
    iYExtra = (rWndRect.bottom-rWndRect.top)-(rClRect.bottom-rClRect.top);
    SetWindowPos(hWnd,NULL,0,0,
		TTYCharWidth*80 + iXExtra,
		TTYCharHeight*25 + iYExtra,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Polling messages from event queue */
    while(TRUE){
	// if messages pending, process them
	if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
	    if(msg.message == WM_QUIT) break;
	    TranslateMessage((LPMSG)&msg);
	    DispatchMessage((LPMSG)&msg);
	} else {
	    // otherwise check the comm port and process any available
	    // characters. you could also use a timer instead, and have
	    // the timer case of the wndproc check the port.
	    if(bConnected){
		// get the CommStat record and clear any error condition
		GetCommError(CommDCB.Id,&CommStat);
		// get the number of characters available
		iNumRead = CommStat.cbInQue;
		if(iNumRead > 0) {
		    // get the number of characters rounded to the buffer size
		    if(iNumRead > BufMax) iNumRead = BufMax;
		    iNumRead = ReadComm(CommDCB.Id,MsgBuff,
					iNumRead);
		    // check for errors
		    if(iNumRead < 0) {
			iNumRead = abs(iNumRead);
			iError = GetCommError(CommDCB.Id,&CommStat);
			// clear the event mask
			GetCommEventMask(CommDCB.Id,0xFFFF);
			itoa(iError,sTemp,10);
			// display what the error was
			switch(iError){
			case CE_BREAK:
			    MessageBox(GetFocus(), sTemp, "CE_BREAK!",MB_OK);
			    break;
			case CE_CTSTO:
			    MessageBox(GetFocus(), sTemp, "CE_CTSTO!",MB_OK);
			    break;
			case CE_DNS:
			    MessageBox(GetFocus(), sTemp, "CE_DNS!",MB_OK);
			    break;
			case CE_DSRTO:
			    MessageBox(GetFocus(), sTemp, "CE_DSTRO!",MB_OK);
			    break;
			case CE_FRAME:
			    MessageBox(GetFocus(), sTemp, "CE_FRAME!",MB_OK);
			    break;
			case CE_IOE:
			    MessageBox(GetFocus(), sTemp, "CE_IOE!",MB_OK);
			    break;
			case CE_MODE:
			    MessageBox(GetFocus(), sTemp, "CE_MODE!",MB_OK);
			    break;
			case CE_OOP:
			    MessageBox(GetFocus(), sTemp, "CE_OOP!",MB_OK);
			    break;
			case CE_OVERRUN:
			    MessageBox(GetFocus(), sTemp, "CE_OVERRUN!",MB_OK);
			    break;
			case CE_PTO:
			    MessageBox(GetFocus(), sTemp, "CE_PTO!",MB_OK);
			    break;
			case CE_RLSDTO:
			    MessageBox(GetFocus(), sTemp, "CE_RLSDTO!",MB_OK);
			    break;
			case CE_RXOVER:
			    MessageBox(GetFocus(), sTemp, "CE_RXOVER!",MB_OK);
			    break;
			case CE_RXPARITY:
			    MessageBox(GetFocus(), sTemp, "CE_RXPARITY!",MB_OK);
			    break;
			case CE_TXFULL:
			    MessageBox(GetFocus(), sTemp, "CE_TXFULL!",MB_OK);
			    break;
			}
		    }
		    // send the characters to the tty window for processing
		    SendMessage(hTTYWnd,COMM_CHARS,
				iNumRead,(LONG)(LPSTR)MsgBuff);
		}


	    }
	}
    }

    return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long FAR PASCAL TTYWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;
    register int i,j;
    TEXTMETRIC Metrics;
    HDC hDC;

    switch (message)
    {
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case IDSABOUT:
            DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
            break;
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
        }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:  // repaint the tty window
	BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
	hDC = ps.hdc;
	SetupTTYDC(hWnd,hDC);

	TTYPaint( hDC );

	ResetTTYDC(hWnd,hDC);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_CREATE:  // initialize the comm and tty parameters
	/* initialize screen buffer to nulls */
	for(i=0;i<25;i++)
	    for(j=0;j<81;j++)
		sScrBuff[i][j] = '\0';
	/* initialize line and position to zero */
	cLastLine = 0;
	hDC = GetDC(hWnd);
	SetupTTYDC(hWnd,hDC);
	GetTextMetrics(hDC,&Metrics);
	ResetTTYDC(hWnd,hDC);
	ReleaseDC(hWnd,hDC);
	cCurrPos = 0;
	TTYCharWidth = Metrics.tmMaxCharWidth;
	TTYCharHeight = Metrics.tmHeight + Metrics.tmExternalLeading;
	bConnected = FALSE;
	hTTYWnd = hWnd;

	break;

    case WM_CLOSE:
	// disconnect if still connected
	if(bConnected)SendMessage(hWnd,WM_COMMAND,TTYCONNECT,0L);
	// go ahead and close down
	DefWindowProc( hWnd, message, wParam, lParam );
	break;

    case WM_COMMAND:
	switch(wParam){

        case IDSABOUT:
            DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
            break;

	case TTYEXIT:
	    PostMessage(hWnd,WM_CLOSE,0,0L);
	    break;

	case TTYCONNECT:
	    // connect to port if not already connected
	    if(!bConnected){
		if(OpenComm(PortSetting,1024,128)) break;
		if(BuildCommDCB(CommSettings,&CommDCB)) break;

		FlushComm(CommDCB.Id,0);
		FlushComm(CommDCB.Id,1);

		CommDCB.fInX = TRUE;
		CommDCB.fOutX = FALSE;
		CommDCB.XonChar = 0x11;
		CommDCB.XoffChar = 0x13;
		CommDCB.XonLim = 50;
		CommDCB.XoffLim = 500;

		if(SetCommState(&CommDCB))break;
		bConnected = TRUE;
		MessageBox(hWnd,"Connection was successful.","",MB_OK);
	    }else{
		// otherwise disconnect
		FlushComm(CommDCB.Id,0);
		FlushComm(CommDCB.Id,1);
		CloseComm(CommDCB.Id);
		MessageBox(hWnd,"Connection closed.","",MB_OK);
		bConnected = FALSE;
	    }
	    break;
	case TTYSETTINGS:
	    // settings dialog
	    i=1;	// this does nothing right now
	    break;
	}
	break;

    case WM_CHAR:
	if(!bConnected) break;
	i = GetCommError(CommDCB.Id,&CommStat);
		    if(i != 0) {
			itoa(i,sTemp,10);
			MessageBox(GetFocus(), sTemp, "Comm Read Error!",MB_OK);
		    }

	WriteComm(CommDCB.Id,&wParam,1);
	break;

    case COMM_CHARS:
	if(wParam > 0) {
	    hDC = GetDC(hWnd);
	    SetupTTYDC(hWnd,hDC);

	    putTTY(hWnd,hDC,wParam,(LPSTR)lParam);

	    ResetTTYDC(hWnd,hDC);
	    ReleaseDC(hWnd,hDC);
	}
	break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
