/*
 * WNTERM function support module
 *
 * Copyright (c) by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
*/

#define NOKANJI
#define NOATOM
#define NOSOUND
#define NOMINMAX
#include <windows.h>
#include <ascii.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ttycls.h"
#include "wnterm.h"
#include "wntdlg.h"

/* paint the main window */
void NEAR MainWndPaint(HWND hWnd, LPPAINTSTRUCT lpps)
{

    HDC hDC = lpps->hdc;

  // if the window is iconic, draw in the icon area
    if (IsIconic(hWnd)) {
	RECT rIcon;
	GetClientRect(hWnd, (LPRECT)&rIcon);
	Rectangle(hDC, 0,0,rIcon.right, rIcon.bottom);
        TextOut(hDC,2,rIcon.bottom/3,(LPSTR)szIconTitle,strlen(szIconTitle));
    }
  // otherwise update the text in the window
    else 
	TTYWndPaint(&MWnd, lpps->hdc, lpps->rcPaint.top, lpps->rcPaint.bottom);
}

/* come here to handle menu items */
void NEAR WndCommand(HWND hWnd, WORD menuitem, LONG param)
{

    HMENU hMenu;
    FARPROC fp;

    switch (menuitem) {
	case IDM_ABOUT:		// create the about box
	    fp = MakeProcInstance((FARPROC)AboutBoxProc, hInst);
	    DialogBox(hInst, MAKEINTRESOURCE(DT_ABOUT),hWnd,fp);
	    FreeProcInstance(fp);
	    break;

	case IDM_OFFLINE:
	  // if currently offline, then change the window proc
	  // over to the subclass window procedure.
	    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWndSubProc);
	  // rewrite the menu bar.
	    hMenu = GetMenu(hWnd);
	    ChangeMenu(hMenu,IDM_OFFLINE,(LPSTR)szOnLine,IDM_ONLINE,
			MF_BYCOMMAND | MF_CHANGE);
	    DrawMenuBar(hWnd);
	  // indicate that we are online.
	    LineState = IDM_ONLINE;
	    break;

	case IDM_ONLINE:
	  // if currently online, reset the window proc.
	    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWndProc);
	  // redraw the menu.
	    hMenu = GetMenu(hWnd);
	    ChangeMenu(hMenu,IDM_ONLINE,(LPSTR)szOffLine,IDM_OFFLINE,
			MF_BYCOMMAND | MF_CHANGE);
	    DrawMenuBar(hWnd);
	  // show new state.
	    LineState = IDM_OFFLINE;
	    break;

	case IDM_COMM:	// make communications settings.
	    fp = MakeProcInstance((FARPROC)SetCommParams, hInst);
	    DialogBox(hInst, MAKEINTRESOURCE(DT_COMM),hWnd, fp);
	    FreeProcInstance(fp);
	    break;

	case IDM_LOCAL:	// set local echo.
	    hMenu = GetMenu(hWnd);
	    if (MWnd.LocalEcho) {
		MWnd.LocalEcho = FALSE;
		CheckMenuItem(hMenu, menuitem, MF_UNCHECKED);
	    }
	    else {	    
		MWnd.LocalEcho = TRUE;
		CheckMenuItem(hMenu, menuitem, MF_CHECKED);
	    }	    
	    break;

	case IDM_CLEAR:	// clear the screen.
	    HideCaret(hWnd);
	    TTYClear(&MWnd);
	    SetCaretPos(MWnd.Pos.x, MWnd.Pos.y);
	    ShowCaret(hWnd);
	    break;
    }
}

/* 
 * This function replaces printf.
 * To use this function you must load the device driver OX.SYS
 * Do NOT run Windows with redirection to AUX.
 * Use dbf just like printf.
 * Example dbf("function foo %d %s, myint, mystring);
 */
void FAR _cdecl dbs(const char *fmt, ...)
{
    char buf[255];

    va_list arg_ptr;

    va_start(arg_ptr, fmt);
    vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    OutputDebugString(buf);

}

