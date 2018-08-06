/*
 *  Function Name:   CreatePolyRgn
 *  Program Name:    crpolyrg.c
 *
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   The program below will set the map mode to MM_LOENGLISH, then
 *   assign a POINT data structure the logical coordinates for making
 *   a poloygon (triangle).  The POINT structure will then be converted
 *   to device coordinates and displayed in MM_TEXT (device
 *   coordinates) map mode.
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_LPtoDP(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  POINT Points[4];
  BOOL    bConverted;


  SetMapMode(hDC, MM_LOENGLISH);

  Points[0].x = 100;                        /* logical points */
  Points[0].y = -300;                     /* in MM_LOENGLISH mode */
  Points[1].x = 100;
  Points[1].y = -200;
  Points[2].x = 500;
  Points[2].y = -300;
  Points[3].x = 100;
  Points[3].y = -300;
                                          /* conversion to device points */
  bConverted = LPtoDP(hDC, (LPPOINT)Points, 4);

  if (bConverted == FALSE)                   /* Check for error */
    MessageBox(hWnd, (LPSTR)"LPtoDP failed.", (LPSTR)"ERROR", MB_ICONHAND);

  SetMapMode(hDC, MM_TEXT);    /* back to MM_TEXT - Points now in device  */
  Polyline (hDC, (LPPOINT)Points, 4);               /* coordinates.       */
  TextOut(hDC, 10, 10, (LPSTR)"Logical points converted in program", 35);

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
    wcClass.lpszClassName  = (LPSTR)"LPtoDP";

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

    hWnd = CreateWindow((LPSTR)"LPtoDP",
                        (LPSTR)"LPtoDP()",
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
        CALL_LPtoDP(hWnd, ps.hdc);
        ValidateRect(hWnd, (LPRECT) NULL);
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

