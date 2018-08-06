/*
 *   SetWindowPos
 *   setwnpos.c
 *
 *   This function changes the size, position, and ordering of child,
 *   popup, and top-level windows.  Child, pop-up, and top-level windows
 *   are ordered according to their appearance on the screen; the topmost
 *   window receives the highest rank, and it is the first window in the
 *   list.  This ordering is recorded in a window list.
 *
 */

#include "windows.h"

/* Forward references */

BOOL FAR PASCAL SetWnPosInit( HANDLE );

long FAR PASCAL SetWnPosWndProc(HWND, unsigned, WORD, LONG);

/* ---------------------------------------------------------------------- */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, CmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int CmdShow;
{
    MSG   msg;
    HWND  hParent, hPopup;
    HMENU hMenu;

    SetWnPosInit( hInstance );
    hParent = CreateWindow((LPSTR)"ShowOwnedPopups",
			(LPSTR)"ShowOwnedPopups",
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
    ShowWindow( hParent, CmdShow );
    UpdateWindow( hParent );

/* Create the popup window */

    hPopup = CreateWindow((LPSTR)"ShowOwnedPopups",
                  		 (LPSTR)"Popup",
	                      WS_POPUP|WS_CAPTION|WS_VISIBLE,
			                50,
			                50,
			                200,
			                100,
                         (HWND)hParent,     /* parent */
                         (HMENU)NULL,       /* use class menu */
                         (HANDLE)hInstance, /* handle to window instance */
                         (LPSTR)NULL        /* no params to pass on */
                        );
/****************************************************************************/
/* Begin SetWindowPos */
/* I am about to move and resize the parent window */

    MessageBox (hParent, (LPSTR)"Move and resize the SetWindowPos window",
               (LPSTR)"I'm about to ...", MB_OK);
    SetWindowPos(hParent, hPopup, 100, 100, 200, 200, SWP_NOACTIVATE);

/* End of SetWindowPos section */
/****************************************************************************/

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/* ---------------------------------------------------------------------- */
/* Procedure called when the application is loaded for the first time */

BOOL FAR PASCAL SetWnPosInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pSetWnPosClass;

    pSetWnPosClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pSetWnPosClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pSetWnPosClass->hIcon		      = LoadIcon( hInstance,NULL);
    pSetWnPosClass->lpszMenuName   = (LPSTR)NULL;
    pSetWnPosClass->lpszClassName  = (LPSTR)"ShowOwnedPopups";
    pSetWnPosClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pSetWnPosClass->hInstance      = hInstance;
    pSetWnPosClass->style          = CS_HREDRAW | CS_VREDRAW;
    pSetWnPosClass->lpfnWndProc    = SetWnPosWndProc;

    if (!RegisterClass( (LPWNDCLASS)pSetWnPosClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pSetWnPosClass );
    return TRUE;        /* Initialization succeeded */
}


/* ---------------------------------------------------------------------- */
/* Every message for this window will be delevered right here.         */

long FAR PASCAL SetWnPosWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

/*    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break; */

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
