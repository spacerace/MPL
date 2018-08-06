/*
 *  Function Name:   EnumTaskWindows
 *  Program Name:    entawin.c
 *
 *  Special Notes:   The enumeration does not work.  The callback function
 *                   EnumProc is never reached.
 *
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

HANDLE hInst;

/*************************************************************************/

BOOL FAR PASCAL EnumProc(hWnd, lParam)
HWND      hWnd;
LONG      lParam;
{
    MessageBox(hWnd,(LPSTR)"Made it to enumeration",(LPSTR)"",MB_ICONHAND);
    return (1);
}

/***********************************************************************/

void CALL_EnumTaskWindows(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  FARPROC lpprocEnumTaskWin;
  HANDLE  hTask;

  hTask = GetCurrentTask();
  if (hTask == NULL)
    MessageBox(hWnd,(LPSTR)"task = null",(LPSTR)"",MB_ICONHAND);

  lpprocEnumTaskWin = MakeProcInstance ((FARPROC)EnumProc, hInst);
  if (lpprocEnumTaskWin == NULL)
    MessageBox(hWnd,(LPSTR)"lpEnumFunc = null",(LPSTR)"",MB_ICONHAND);

  EnumTaskWindows (hTask, lpprocEnumTaskWin, (LONG) NULL);

  FreeProcInstance (lpprocEnumTaskWin);
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
    wcClass.lpszClassName  = (LPSTR)"EnumTaskWindows";

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

    hWnd = CreateWindow((LPSTR)"EnumTaskWindows",
                        (LPSTR)"EnumTaskWindows()",
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

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    MessageBox(hWnd, "The enumeration does not work.  \
The callback function is never reached.", "EnumTaskWindows", MB_OK);

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
        CALL_EnumTaskWindows(hWnd, ps.hdc);
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
