/*
 *  Function Name:   EnumWindows
 *  Program Name:    enwin.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   This function will enumerate all the windows on the screen.  The
 *   name of each window enumerated will be displayed.
 */

#include "windows.h"
#include "string.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

HANDLE hInst;
HWND   hWndGlobal;      /*  for TextOut during enumeration  */

/*************************************************************************/

BOOL FAR PASCAL EnumProc(hWnd, lParam)
HWND   hWnd;
LONG   lParam;
{
  HDC  hDC;
  char szstr[80];
  static int inc = 10;

  GetClassName(hWnd, (LPSTR)szstr, 80);
  hDC = GetDC(hWndGlobal);
  TextOut(hDC, 10, inc+=15, (LPSTR)szstr, strlen(szstr));
  ReleaseDC(hWndGlobal, hDC);
  return (1);                    /* returning 0 will stop the enumeration */
}

/***********************************************************************/

void CALL_EnumWindows(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  FARPROC lpprocEnumWindow;
  LONG lParam;

    lpprocEnumWindow = MakeProcInstance ((FARPROC) EnumProc, hInst);
    EnumWindows (lpprocEnumWindow, lParam);   /*  function demonstrated   */
    FreeProcInstance ((FARPROC) lpprocEnumWindow);

  return;
}

/**************************************************************************/

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
    wcClass.lpszClassName  = (LPSTR)"EnumWindows";

    if (!RegisterClass( (LPWNDCLASS)&wcClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
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

    hWnd = CreateWindow((LPSTR)"EnumWindows",
                        (LPSTR)"EnumWindows()",
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
    hInst = hInstance;
    hWndGlobal = hWnd;

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

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
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        CALL_EnumWindows(hWnd, ps.hdc);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
