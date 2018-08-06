/*
 *   GetWindowWord
 *
 *   This program demonstrates the use of the GetWindowWord function.
 *   GetWindowWord returns information about the window identified by
 *   the first parameter. The information it returns is specified by the
 *   second parameter. In this sample application, GetWindowWord was
 *   called in WinMain and passed "hWnd" and "GWW_HINSTANCE," telling it
 *   to return the instance handle of the module owning "hWnd."
 *
 *   Windows Version 2.0 function demonstration application
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
    HDC hDC;		  /* handle to display context */
    HANDLE ourhInstance;  /* return value from GetWindowWord */

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
			(LPSTR)"GetWindowWord",
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

    hDC = GetDC(hWnd);	  /** get a display context for hWnd **/

    TextOut(hDC,
	    20,
	    5,
  (LPSTR)"I will now call GetWindowWord and pass it 'hWnd' and 'GWW_HINSTANCE,'",
  (short)68);

    TextOut(hDC,
	    20,
	    15,
(LPSTR)"telling it to retrieve the instance handle of the module owning the",
  (short)67);

    TextOut(hDC,
	    20,
	    25,
  (LPSTR)"window associated with hWnd. GetWindowWord should return 'hInstance.'",
  (short)69);

  MessageBox(hWnd,(LPSTR)"  OKAY",(LPSTR)" ",MB_OK);

/**   call GetWindowWord, tell it to retrieve the instance handle  **/
/**   of the module owning the window associated with hWnd	   **/
  ourhInstance = GetWindowWord(hWnd,GWW_HINSTANCE);

/**   clear screen **/
  InvalidateRect(hWnd,(LPRECT)NULL,(BOOL)TRUE);
  UpdateWindow(hWnd);

/**  if GetWindowWord correctly returned the instance handle **/
/**  of the  module owning the window associated with hWnd.. **/
  if (ourhInstance = hInstance)
      MessageBox(hWnd,
		(LPSTR)"instance handle of the module owning hWnd.",
		(LPSTR)"GetWindowWord correctly retrieved the...",
		MB_OK);
    else
      MessageBox(hWnd,
		(LPSTR)"instance handle of the module owning hWnd.",
		(LPSTR)"GetWindowWord DID NOT correctly retrieve the...",
		MB_OK);

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
