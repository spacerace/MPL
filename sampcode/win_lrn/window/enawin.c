/*
 *  Function Name:   EnableWindow
 *  Program Name:    enawin.c
 *
 *  Description:
 *   This function can enable or disable keyboard and mouse input to a
 *   particular window.  The program below will disable input to the
 *   window when it loses the focus. The problem is that then you
 *   can't get the focus back. Oh Well....
 *
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);


/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
    HANDLE hInstance;
    {
    WNDCLASS   wcClass;

    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra     =0;
    wcClass.cbWndExtra     =0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon( hInstance,NULL );
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"EnableWindow";

    if (!RegisterClass( (LPWNDCLASS)&wcClass ) )
        return FALSE;

    return TRUE;        /* Initialization succeeded */
    }


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
    HANDLE hInstance, hPrevInstance;
    LPSTR lpszCmdLine;
    int cmdShow;
    {
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance)
        {
        /* Call initialization procedure if this is the first instance */
        if (!WinInit( hInstance ))
            return FALSE;
        }

    hWnd = CreateWindow((LPSTR)"EnableWindow",
                        (LPSTR)"EnableWindow()",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    MessageBox(hWnd, "This window is disabled when it loses \
the input focus.", "EnableWindow", MB_OK);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
        {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
    }

/* Procedures which make up the window class. */
long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
    HWND hWnd;
    unsigned message;
    WORD wParam;
    LONG lParam;
    {
    switch (message)
	{
	/* Disable window if focus is moved to another window */
	case WM_KILLFOCUS:
	    EnableWindow(hWnd, FALSE);
	    break;

	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

	default:
	    return DefWindowProc( hWnd, message, wParam, lParam );
	    break;
	}
    return(0L);
    }
