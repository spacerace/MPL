/*
 *  Function Name:   CreateRectRgn
 *  Program Name:    crrectrg.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   The program below will create a rectagular region and make it visble
 *   by inverting the region.
 */

#include "windows.h"

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_CreateRectRgn(hWnd, hDC)
HWND hWnd;
HDC hDC;
{
  HRGN hNewRgn;
  RECT ClientRect;

  GetClientRect( hWnd, (LPRECT) & ClientRect );   /* get client area   */

  hNewRgn = CreateRectRgn(10, 10,
      (short) ClientRect.right / 2,  /* half client area */
  (short) ClientRect.bottom);
  if (hNewRgn == NULL)                   /* Check for error */
  {
    MessageBox(hWnd, (LPSTR)"CreateRectRgn failed.", (LPSTR)"ERROR", MB_ICONHAND);
    return;
  }
  InvertRgn(hDC, hNewRgn);                       /* invert region  */
  DeleteObject( hNewRgn );

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
  wcClass.lpszClassName  = (LPSTR)"CreateRectRgn";

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

  hWnd = CreateWindow((LPSTR)"CreateRectRgn",
      (LPSTR)"CreateRectRgn()",
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
    CALL_CreateRectRgn(hWnd, ps.hdc);
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


