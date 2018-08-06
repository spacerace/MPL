/*
 *  Function Name:   FillRgn
 *  Program Name:    fillrgn.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   The program below will create a region and fill it with a
 *   red background.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_FillRgn (hWnd, hDC)
HWND hWnd;
HDC hDC;
  {
  HBRUSH    hBrush;
  HRGN      hNewRgn;
  RECT      ClientRect;

  hBrush = CreateSolidBrush (RGB (255, 0, 0));   /* hBrush created (red). */
  GetClientRect (hWnd, (LPRECT) &ClientRect);
  hNewRgn = CreateRectRgn (10, 10, (ClientRect.right / 2),
                          (ClientRect.bottom / 2));
  FillRgn (hDC, hNewRgn, hBrush);                /* region filled */
  DeleteObject (hBrush);
  DeleteObject (hNewRgn);
  return;
  }

/**************************************************************************/
BOOL WinInit (hInstance)
HANDLE hInstance;
  {
  WNDCLASS   wcClass;

  wcClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcClass.lpfnWndProc    = WndProc;
  wcClass.cbClsExtra     = 0;
  wcClass.cbWndExtra     = 0;
  wcClass.hInstance      = hInstance;
  wcClass.hIcon          = LoadIcon (hInstance, NULL);
  wcClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
  wcClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  wcClass.lpszMenuName   = (LPSTR)NULL;
  wcClass.lpszClassName  = (LPSTR)"FillRgn";

  if (!RegisterClass ( (LPWNDCLASS) & wcClass))
    return FALSE;

  return TRUE;        /* Initialization succeeded */
  }


/**************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  if (!hPrevInstance)
    if (!WinInit (hInstance))
      return FALSE;

  hWnd = CreateWindow ( (LPSTR)"FillRgn",
                      (LPSTR)"FillRgn ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,          /* no parent */
                      (HMENU)NULL,         /* use class menu */
                      (HANDLE)hInstance,   /* handle to window instance */
                      (LPSTR)NULL);        /* no params to pass on */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }

  return (int)msg.wParam;
  }

/**************************************************************************/
long    FAR PASCAL WndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      CALL_FillRgn (hWnd, ps.hdc);
      ValidateRect (hWnd, (LPRECT) NULL);
      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
