/*
 *   ShowOwnedPopups
 *   showpops.c
 *
 *   This program demonstrates the use of the function ShowOwnedPopups.
 *   A pop-up window is initially shown visible.   At the same time a
 *   message box appears.  When the message box is acknowledged then 
 *   the pop-up window becomes invisible.  Another message box appears.
 *   When the second message box is acknowledged then the pop-up window
 *   becomes visible again.
 *
 */

#include "windows.h"

/* Forward references */

BOOL FAR PASCAL SampleInit( HANDLE );

long FAR PASCAL SampleWndProc(HWND, unsigned, WORD, LONG);

/* ---------------------------------------------------------------------- */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, CmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int CmdShow;
{
    MSG   msg;
    HWND  hParent,hPopup;
    HMENU hMenu;

    SampleInit( hInstance );
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
/* Begging of ShowOwnedPopups section */

/* void ShowOwnedPopups( hWnd, fShow ) 
 *
 * The first parameter 'hWnd' is the handle to the parent window that
 * owns the pop-up.  The second parameter 'fShow' is nonzero if all
 * hidden pop-up windows should be shown; it is zero if all visible
 * pop-up windows should be hidden                                          */

    MessageBox (hParent, (LPSTR)"Hide the popup.",
               (LPSTR)"Done", MB_OK);

    ShowOwnedPopups(hParent,FALSE);                /* Hide the popup */  

    MessageBox (hParent, (LPSTR)"Show the popup.",
   		      (LPSTR)"Done", MB_OK);

    ShowOwnedPopups(hParent,TRUE);                /* Show the popup */

/* End of ShowOwnedPopups section */
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

BOOL FAR PASCAL SampleInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pSampleClass;

    pSampleClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pSampleClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pSampleClass->hIcon		      = LoadIcon( hInstance,NULL);
    pSampleClass->lpszMenuName   = (LPSTR)NULL;
    pSampleClass->lpszClassName  = (LPSTR)"ShowOwnedPopups";
    pSampleClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pSampleClass->hInstance      = hInstance;
    pSampleClass->style          = CS_HREDRAW | CS_VREDRAW;
    pSampleClass->lpfnWndProc    = SampleWndProc;

    if (!RegisterClass( (LPWNDCLASS)pSampleClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pSampleClass );
    return TRUE;        /* Initialization succeeded */
}


/* ---------------------------------------------------------------------- */
/* Every message for this window will be delevered right here.         */

long FAR PASCAL SampleWndProc( hWnd, message, wParam, lParam )
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
