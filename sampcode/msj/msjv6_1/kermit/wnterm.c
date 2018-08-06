/* 
 * WNTERM Main Module
 *
 * Copyright (c) 1990 by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
*/

#define NOKANJI
#define NOATOM
#define NOSOUND
#define NOMINMAX
#include <windows.h>
#include <limits.h>
#include <string.h>
#include <ascii.h>
#include <stdlib.h>
#include "ttycls.h"

/* Turn declarations in header file into defintions */
#define EXTERN
#include "wnterm.h"

/* Kermit information */
#if defined(KERMIT)
#define KERMITEXTERN
#include "wnkerm.h"
#endif

/* program entry point */
int FAR PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, 
		       LPSTR lpszCmdLine, int cmdShow)
{

    MSG msg;
    
  /* initialize program */
    if (!InitProgram(hInstance,hPrevInstance, lpszCmdLine, cmdShow))
	return FALSE;

  /* stay in this loop until window is destroyed */
    while (TRUE) {
        if (PeekMessage((LPMSG)&msg,NULL,0,0,PM_REMOVE)) {
	    if (msg.message == WM_QUIT)
		break;
	    TranslateMessage((LPMSG)&msg);
	    DispatchMessage((LPMSG)&msg);
	}
	else {		/* Poll the communications loop */
	    if (LineState == IDM_ONLINE) {
	        if (ProcessComm()
#if defined(KERMIT)
		    || Kermit.InTransfer
#endif
		   )
		PostMessage(MWnd.hWnd, WM_USER, Buflen, 0L);
	    }
	}
    }
    return (int)msg.wParam;	// terminate program.
}

/* Main window procedure */
long FAR PASCAL MainWndProc(HWND hWnd,unsigned message,WORD wParam,LONG lParam)
{

    // get our data structure for the window.
    PTTYWND pMWnd = (PTTYWND)GetWindowWord(hWnd,0); 
    PAINTSTRUCT ps;
    register int i;

    switch(message) {

#if defined(KERMIT)
	case WM_QUERYDRAGICON:
	    return MAKELONG(krmIcon, 0);
#endif

      // control scrolling with this key.
	case WM_KEYDOWN:
	    if (wParam == VK_SCROLL)
	        ScrollLock = GetKeyState(VK_SCROLL) & 1;
	    break;

      // process menu command.
	case WM_COMMAND:
#if defined(KERMIT)
	    if (krmWndCommand(hWnd, wParam))
		break;
#endif
	    WndCommand(hWnd, wParam, lParam);
	    break;

    /* 
       When a character is received, and send it to tty display procedure.
       This is the action when terminal is off-line
    */
	case WM_CHAR:
	    HideCaret(hWnd);
	  // the loop is a crude way to handle repeat characters
	    for (i = 0; i < (int)LOWORD(lParam); i++)
	        TTYDisplay(pMWnd, (short)1, (BYTE *)&wParam);
	    SetCaretPos(pMWnd->Pos.x, pMWnd->Pos.y);
	    ShowCaret(hWnd);
	    break;

      // recreate caret and reset its position
    	case WM_SIZE:
	    pMWnd->Width = LOWORD(lParam);
	    pMWnd->Height = HIWORD(lParam);
	    pMWnd->Pos.y = pMWnd->Height - pMWnd->CHeight - 1; 
	    if (hWnd == GetFocus()) {
	        CreateCaret(hWnd, (HBITMAP)NULL,2,pMWnd->CHeight);
	        SetCaretPos(pMWnd->Pos.x, pMWnd->Pos.y);
	        ShowCaret(hWnd);
	    }
	    break;

      // window has focus, so window can create a caret.
	case WM_SETFOCUS:
	    CreateCaret(hWnd, (HBITMAP)NULL,2,pMWnd->CHeight);
	    SetCaretPos(pMWnd->Pos.x, pMWnd->Pos.y);
	    if (!IsIconic(hWnd))
	        ShowCaret(hWnd);
	    break;

      // window has lost focus, so window must kill the caret.
	case WM_KILLFOCUS:
	    HideCaret(hWnd);
	    DestroyCaret();
	    break;

      // main window now exists so do some initialization.
	case WM_CREATE:
	    WndCreate(hWnd,(LPCREATESTRUCT)lParam);
#if defined(KERMIT)
	    if (!krmInit(hWnd, Buffer, &Buflen, &cid))
		PostMessage(MWnd.hWnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
#endif
	    break;

      // have to close comm port if open before killing window.
	case WM_CLOSE:
#if defined(KERMIT)
	    if (krmShutdown() != IDYES)
	        break;
#endif
	    if (cid >= 0) {
		CloseComm(cid);
		cid = INT_MIN;
	    }
	    if (smallfont)
		DeleteObject(hfont);
	    DestroyWindow(hWnd);
	    break;

	case WM_QUERYENDSESSION:
#if defined(KERMIT)
	    if (krmShutdown() != IDYES)
	        break;
#endif
	    return ((LONG)TRUE);

      // This application is being destroyed by Windows exec.
	case WM_ENDSESSION:
	    if (wParam)		// if true, close the comm port.
		if (cid >= 0) {
		    CloseComm(cid);
		    cid = INT_MIN;
		}
	    if (smallfont)
		DeleteObject(hfont);
	    break;

      // tell program to exit.
	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

      // if iconic, paint the icon window, otherwise redraw text buffer.
	case WM_PAINT:
	    BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
#if defined(KERMIT)
	    if (IsIconic(hWnd)) {
		if (Kermit.InTransfer)
		    krmPaint(hWnd, ps.hdc);
		else {
		    RECT rect;
		    GetClientRect(hWnd, &rect);
		    DrawIcon(ps.hdc, 
			     (rect.right - GetSystemMetrics(SM_CXICON)) / 2,
			     (rect.bottom - GetSystemMetrics(SM_CYICON)) / 2,
			     krmIcon);
		}
	    }
	    else
#endif	    
 	        MainWndPaint(hWnd, (LPPAINTSTRUCT)&ps);
	    EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
	    break;

      // all other messages go through here.
	default:
	    return ((long)DefWindowProc(hWnd,message,wParam,lParam));
    }
    return(0L);	/* this means we processed the message */
}

/* 
 * Subclass window procedure.  When we are on-line, all window 
 *  messages come here first
 */
LONG FAR PASCAL MainWndSubProc(HWND hWnd, unsigned message,
			       WORD wParam, LONG lParam)
{

    PTTYWND pMWnd = (PTTYWND)GetWindowWord(hWnd,0);
    register int i;

  // we are only interested in these messages.
    switch(message) {

      // buffer from comm port has been posted to us.
	case WM_USER:
#if defined(KERMIT)
	    if (Kermit.InTransfer) {
		wart();
	        break;
	    }
#endif
	    if ((Buflen) && !IsIconic(MWnd.hWnd)) {
	        HideCaret(hWnd);
	        Buflen = TTYDisplay(pMWnd, (short)wParam, Buffer);
	        SetCaretPos(pMWnd->Pos.x, pMWnd->Pos.y);
	        ShowCaret(hWnd);
	    }
	    break;

      // transmit key from keyboard.  Loop is crude way to handle repeat chars.
	case WM_CHAR:
	    for (i = 0; i < (int)LOWORD(lParam); i++)
	        WriteComm(cid, (LPSTR)&wParam,1);
	    if (!MWnd.LocalEcho)	// if no local echo, then done
		break;

    // all other messages must go thru normal channels.
	default:
	    return(CallWindowProc(fpMainWndProc,hWnd,message,wParam,lParam));
    }
    return(0L);
}

/* come here to read the communications buffer if no messages to process */
int NEAR ProcessComm(void)
{

    COMSTAT ComStatus;
    static int OldBuflen;
    register int result = 0;
    register WORD room = BUFSIZE - Buflen;	// check for space

  // if anything left over, move it to front.
    if ((Buflen > 0) && (Buflen < OldBuflen))
	memmove(Buffer, Buffer + OldBuflen - Buflen, Buflen);

    if (room > 0) {
	if (GetCommError(cid, (COMSTAT FAR *)&ComStatus) == 0) {
            if (ComStatus.cbInQue) {
	        result = ReadComm(cid,Buffer+Buflen,min(ComStatus.cbInQue,room));
		OldBuflen = Buflen += abs(result);
	    }
	}
    }
    return Buflen;
}
