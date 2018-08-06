/*
 *  Function Name:   EqualRgn
 *  Program Name:    eqrgn.c
 *
 *  Description:
 *   This function will compare the coordinates of two regions.  If
 *   both regions are the same size and in the same place, a nonzero
 *   value will return.  Otherwise a zero is returned.
 *
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_EqualRgn(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  HRGN hRgn1, hRgn2;
  BOOL bEqual;

  MessageBox(hWnd, "Creating two rectangular regions at \
(30,30)-(90,90)...", "EqualRgn", MB_OK);

  hRgn1 = CreateRectRgn(30, 30, 90, 90);
  hRgn2 = CreateRectRgn(30, 30, 90, 90);
  InvertRgn(hDC, hRgn1);                        /* make region visible  */

  bEqual = EqualRgn(hRgn1, hRgn2);        /* compare regions       */

  if (bEqual)
    TextOut (hDC, 10, 10, (LPSTR)"Regions are equal", 17);
  else
    TextOut (hDC, 10, 10, (LPSTR)"Regions are not equal", 21);

  DeleteObject( hRgn1 );
  DeleteObject( hRgn2 );

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
    wcClass.lpszClassName  = (LPSTR)"EqualRgn";

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

    hWnd = CreateWindow((LPSTR)"EqualRgn",
                        (LPSTR)"EqualRgn()",
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
	CALL_EqualRgn(hWnd, ps.hdc);
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
