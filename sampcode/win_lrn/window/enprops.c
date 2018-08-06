/*
 *  Function Name:   EnumProps
 *  Program Name:    enprops.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   This function will enumerate properties assigned to a window.
 *   The program below will assign two properties to the window
 *   and display them during the enumeration.
 */

#include "windows.h"
#include "string.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

HANDLE hInst;
int    nResult;

/*************************************************************************/

int FAR PASCAL EnumProc(hWnd, lpString, hData)
HWND   hWnd;
LPSTR  lpString;
HANDLE hData;
{
  HDC  hDC;
  static int nInc = 10;

  hDC = GetDC(hWnd);
  TextOut(hDC, 10, nInc+=20, lpString, strlen((PSTR) LOWORD(lpString)));
  ReleaseDC (hWnd, hDC);
  return (1);                    /* return value is user defined.   */
}

/***********************************************************************/

void CALL_EnumProps(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  FARPROC lpprocEnumProp;
  char szstring[80];

  SetProp (hWnd, (LPSTR)"prop1", (HANDLE) NULL);
  SetProp (hWnd, (LPSTR)"prop2", (HANDLE) NULL);

    lpprocEnumProp = MakeProcInstance ((FARPROC) EnumProc, hInst);
    EnumProps (hWnd, lpprocEnumProp);       /*  function demonstrated   */
    FreeProcInstance ((FARPROC) lpprocEnumProp);

  RemoveProp(hWnd, (LPSTR)"prop1");
  RemoveProp(hWnd, (LPSTR)"prop2");

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
    wcClass.lpszClassName  = (LPSTR)"EnumProps";

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

    hWnd = CreateWindow((LPSTR)"EnumProps",
                        (LPSTR)"EnumProps()",
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
        CALL_EnumProps(hWnd, ps.hdc);
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
