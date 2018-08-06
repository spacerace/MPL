/*
 *   LineDDA
 *
 *   This program demonstrates the use of the LineDDA function. LineDDA
 *   performs a function for every point on the line from X1,X2 to Y1,Y2.
 *   LineDDA is called from Winmain in this sample application.
 */

#include "windows.h"

HDC hDC;

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

void FAR PASCAL DrawLine(int,int,LPSTR);


/****************** procedure that LineDDA "calls" *********************/

void FAR PASCAL DrawLine(X,Y,lpData)
int X;
int Y;
LPSTR lpData;

{
switch (X % 3)
    {
    case 0:
       MoveTo(hDC,X+30,Y-1);
       LineTo(hDC,X-1,Y+11);
       break;

    default:
       break;
    }
return;
}

/***********************************************************************/


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

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
			(LPSTR)"LineDDA",
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

/************************    call LineDDA    **************************/

    LineDDA(0,0,200,200,(FARPROC)DrawLine,(LPSTR)NULL);

/**********************************************************************/

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
