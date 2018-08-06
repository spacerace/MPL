/*
 *  Function Name:   EnumChildWindows
 *  Program Name:    enchwin.c
 *
 *  Description:
 *   This program will enumerate the child window(s) for the parent
 *   window.  The Class name of the child window will be displayed
 *   in a message box.  This program has only one child window.
 *
 */

#include "windows.h"
#include "string.h"
#include "enchwin.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ChildProc(HWND, unsigned, WORD, LONG);

char   szAppName [] = "EnChWin" ;
HWND   hWndChild1, hWndChild2;		   /*	child window handle   */
HANDLE hInst;

/************************************************************************/

BOOL FAR PASCAL EnumProc(hChildWindow, lParam)
HWND   hChildWindow;
LONG   lParam;
{
   char szstr[31];

   GetWindowText (hChildWindow, szstr, 30);
   MessageBox(hChildWindow, (LPSTR)szstr,
	     (LPSTR)"Name of child window", MB_OK);
   return TRUE;
}

/***********************************************************************/

void CALL_EnumChildWindows(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  FARPROC lpprocEnumChWin;
  char    szstring[80];
  LONG    lParam=0;

    lpprocEnumChWin = MakeProcInstance ((FARPROC) EnumProc, hInst);
    EnumChildWindows (hWnd, lpprocEnumChWin, lParam);
    FreeProcInstance ((FARPROC) lpprocEnumChWin);

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
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon( hInstance,NULL );
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.lpszMenuName   = (LPSTR)szAppName;
    wcClass.lpszClassName  = (LPSTR)"EnumChildWindows";

    if (!RegisterClass( (LPWNDCLASS)&wcClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = ChildProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon( hInstance,NULL );
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Child";

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

    hWnd = CreateWindow((LPSTR)"EnumChildWindows",
                        (LPSTR)"EnumChildWindows()",
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    hWndChild1 = CreateWindow((LPSTR)"Child",
			(LPSTR)"Child Window 1",
                        WS_CHILD | WS_VISIBLE | WS_CAPTION,
                        50,
			20,
                        120,
			50,
                        (HWND)hWnd,        /* specify hWnd as parent */
                        (HMENU)NULL,       /* use class menu */
			(HANDLE)hInst, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

     hInst = hInstance;    /*  save Instance			 */

     hWndChild2 = CreateWindow((LPSTR)"Child",
			(LPSTR)"Child Window 2",
                        WS_CHILD | WS_VISIBLE | WS_CAPTION,
                        50,
			70,
			120,
			50,
                        (HWND)hWnd,        /* specify hWnd as parent */
                        (HMENU)NULL,       /* use class menu */
			(HANDLE)hInst, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

     hInst = hInstance;    /*  save Instance			 */


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
    case WM_COMMAND:
	switch (wParam)
	{
	case IDM_ENUM:
	     ps.hdc = GetDC (hWnd);
	     CALL_EnumChildWindows(hWnd, ps.hdc);
	     ReleaseDC (hWnd, ps.hdc);
	     break ;
	default:
	     break ;
	}
    break ;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}


long FAR PASCAL ChildProc( hChildWnd, message, wParam, lParam )
HWND hChildWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;
    switch (message)
    {

    default:
        return DefWindowProc( hChildWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
