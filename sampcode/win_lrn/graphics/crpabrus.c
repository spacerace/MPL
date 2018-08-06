/*
 *  Function Name:   CreatePatternBrush
 *  Program Name:    crpabrus.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   This program takes a valid bitmap pattern and assigns it to be used as a
 *   brush.  The program below draws a rectangle and fills it with a
 *   crosshatch pattern brush created from a bitmap.
 */

#include "windows.h"

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_CreatePatternBrush(hWnd, hDC)
HWND hWnd;
HDC  hDC;
{
  HBRUSH    hBrush, hOldBrush;
  HBITMAP   hBitLoad;
/* set up 8x8 crosshatch pattern */
  static short	BitmapArray[] = 
  {
    0xFF, 0xF7, 0xEB, 0xDD, 0xBE, 0x7F, 0xFF, 0xFF    };

  hBitLoad = CreateBitmap(8, 8, 1, 1, (LPSTR)BitmapArray); /* for monochrome */

  hBrush = CreatePatternBrush(hBitLoad);      /* load pattern brush    */

  if (hBrush == NULL)                /* checks for successful brush creation */
  {
    MessageBox(hWnd, (LPSTR)"CreatePatternBrush failed",
        (LPSTR)"ERROR", MB_ICONHAND);
    return;
  }

  hOldBrush = (HBRUSH) SelectObject (hDC, (HANDLE) hBrush);
  Rectangle (hDC, 10, 10, 300, 100);
  hBrush = SelectObject (hDC, hOldBrush);
  DeleteObject(hBrush);
  DeleteObject(hBitLoad);

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
  wcClass.hIcon          = LoadIcon( hInstance, NULL );
  wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
  wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  wcClass.lpszMenuName   = (LPSTR)NULL;
  wcClass.lpszClassName  = (LPSTR)"CreatePatternBrush";

  if (!RegisterClass( (LPWNDCLASS) & wcClass ) )
/* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return FALSE;

  return TRUE;        /* Initialization succeeded */
}


int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  if (!hPrevInstance)
  {
/* Call initialization procedure if this is the first instance */
    if (!WinInit( hInstance ))
      return FALSE;
  }

  hWnd = CreateWindow((LPSTR)"CreatePatternBrush",
      (LPSTR)"CreatePatternBrush()",
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
  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long	FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  PAINTSTRUCT ps;

  switch (message)
  {

  case WM_PAINT:
    BeginPaint( hWnd, (LPPAINTSTRUCT) & ps );
    CALL_CreatePatternBrush(hWnd, ps.hdc);
    ValidateRect(hWnd, (LPRECT) NULL);
    EndPaint( hWnd, (LPPAINTSTRUCT) & ps );
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


