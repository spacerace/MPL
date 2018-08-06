/*
 *   SwapMouseButton
 *   swapbtn
 *
 *   This program demonstrates the use of the function SwapMouseButton.
 *   First, a message box opens and only the left mouse button will close
 *   it.  The functionality of the mouse buttons are switched using
 *   SetMouseButton and another message box is displayed.  This time only
 *   the right mouse button will close the message box.  The mouse buttons
 *   are finally reset before closing.
 *
 */

#include "windows.h"

/* Forward References */

BOOL FAR PASCAL SampleInit(HANDLE);

long FAR PASCAL SampleWndProc(HWND, unsigned, WORD, LONG);

/***************************************************************************/

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;

    SampleInit( hInstance );
    hWnd = CreateWindow((LPSTR)"SwapMouseButton",
			(LPSTR)"SwapMouseButton",
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
/************************************************************************/
/* Beginning of SwapMouseButton section */

MessageBox (hWnd, (LPSTR)"The functionality of the mouse buttons will be switched using the fuction SetMouseButton.",
            (LPSTR)"Done", MB_OK);

SwapMouseButton(FALSE);                  /* Initialize the mouse buttons */

MessageBox (hWnd, (LPSTR)"Only the left mouse button will close this message box.",
            (LPSTR)"Done", MB_OK);

SwapMouseButton(TRUE);                   /* Switch the mouse buttons */

MessageBox (hWnd, (LPSTR)"Only the right mouse button will close this message box.",
            (LPSTR)"Done", MB_OK);

SwapMouseButton(FALSE);                  /* Reset the mouse buttons */

MessageBox (hWnd, (LPSTR)"The mouse buttons are now reset.",
            (LPSTR)"Done", MB_OK);

/************************************************************************/

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/***************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL FAR PASCAL SampleInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pSampleClass;

    pSampleClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pSampleClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pSampleClass->hIcon		      = LoadIcon( hInstance,NULL);
    pSampleClass->lpszMenuName   = (LPSTR)NULL;
    pSampleClass->lpszClassName  = (LPSTR)"SwapMouseButton";
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


/**************************************************************************/
/* Every message for this window will be delevered right here.         */

long FAR PASCAL SampleWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
