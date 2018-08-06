/*
 *   GrayString
 *
 *   This program demonstrates the use of the GrayString function. The
 *   GrayString function outputs gray text to the specified window.
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
    HDC   hDC;			    /* Handle to display context */
    HBRUSH hMyBrush;		    /* Handle to a brush	 */
    DWORD gray=RGB(0x22,0x22,0x22); /* Red, Green, Blue, values for brush */

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
			(LPSTR)"Sample Application",
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

/******************************************************************/
  /* get handle to display context */
     hDC = GetDC(hWnd);
  /* create brush with the colors specified by "gray" */
    hMyBrush = CreateSolidBrush(gray);
  /* if brush not created succesfully */
    if (hMyBrush = NULL)
  /* output error */
	MessageBox(hWnd,(LPSTR)"Unable to Create Brush",(LPSTR)"Too Bad",
		   MB_OK);
    else
  /* else output gray text */

  GrayString(hDC,hMyBrush,NULL,(DWORD)(LPSTR)"This is gray text",0,5,5,0,0);

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
        switch (wParam)
        {
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
        }
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