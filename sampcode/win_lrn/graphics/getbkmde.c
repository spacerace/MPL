/*
 *
 *   This program demonstrates the use of the GetBkMode function. GetBkMode
 *   retrieves the background mode of the given device. The background mode
 *   can be either OPAQUE or TRANSPARENT. The mode affects the background
 *   painting behind text, hatched brushes, and certain pens. GetBkMode is
 *   called from WinMain in this application.
 *
 */

#include "windows.h"

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon		= LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName	= (LPSTR)"Sample Application";
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc    = HelloWndProc;

    if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pHelloClass );
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
    HDC   hDC;		/*   display context		  */
    short nBkMode;	/*   return value from GetBkMode  */
    int   i;		/*   used in for loop		  */

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
			(LPSTR)"GetBkColor",
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

    hDC = GetDC(hWnd);

    for (i=0; i<=200; i+=5)
	{		      /* draw line so we can see the affect */
	 MoveTo(hDC,i,0);     /*      of the background mode	    */
	 LineTo(hDC,i,200);
	}

    TextOut(hDC,5,25,
	    (LPSTR)"This line written with an OPAQUE Backgound mode",
	    (short)47);

    nBkMode = GetBkMode(hDC);  /* Get the background mode of the DC */

    if (nBkMode == OPAQUE)
	MessageBox(hWnd,
		   (LPSTR)"is OPAQUE",
		   (LPSTR)"GetBkMode says the background mode...",
		   MB_OK);
    else if(nBkMode == TRANSPARENT)
	MessageBox(hWnd,
		   (LPSTR)"is TRANSPARENT",
		   (LPSTR)"GetBkMode says the background mode...",
		   MB_OK);
    else
	MessageBox(hWnd,
		   (LPSTR)"ERROR",
		   (LPSTR)"ERROR",
		   MB_OK);


    SetBkMode(hDC,TRANSPARENT);  /* make background mode TRANSPARENT */


    TextOut(hDC,5,55,
	    (LPSTR)"This line written with a TRANSPARENT Backgound mode",
	    (short)51);

    nBkMode = GetBkMode(hDC);  /* Get the background mode of the DC */

    if (nBkMode == OPAQUE)
	MessageBox(hWnd,
		   (LPSTR)"is OPAQUE",
		   (LPSTR)"GetBkMode says the background mode...",
		   MB_OK);
    else if(nBkMode == TRANSPARENT)
	MessageBox(hWnd,
		   (LPSTR)"is TRANSPARENT",
		   (LPSTR)"GetBkMode says the background mode...",
		   MB_OK);
    else
	MessageBox(hWnd,
		   (LPSTR)"ERROR",
		   (LPSTR)"ERROR",
		   MB_OK);

    ReleaseDC(hWnd,hDC);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long FAR PASCAL HelloWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_SYSCOMMAND:
	return DefWindowProc( hWnd, message, wParam, lParam );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
