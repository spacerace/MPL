/*
 *   InvalidateRgn
 *
 *   This program demonstrates the use of the InvalidateRgn function.
 *   InvalidateRgn marks the given region for painting during the next
 *   "paint session." The next paint session starts after the next WM_PAINT
 *   message is received by the window. InvalidateRgn is called from WinMain
 *   in this sample application. UpdateWindow is called directly after
 *   InvalidateRgn to send a WM_PAINT message.
 */

#include "windows.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;
  HRGN hRegion;
  HDC  hDC;
  RECT ClearRect;

  if (!hPrevInstance)
    {
    WNDCLASS   HelloClass;

    HelloClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    HelloClass.hIcon          = LoadIcon (hInstance, NULL);
    HelloClass.lpszMenuName   = (LPSTR)NULL;
    HelloClass.lpszClassName  = (LPSTR)"Sample Application";
    HelloClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    HelloClass.hInstance      = hInstance;
    HelloClass.style          = CS_HREDRAW | CS_VREDRAW;
    HelloClass.lpfnWndProc    = HelloWndProc;

    if (!RegisterClass ( (LPWNDCLASS)&HelloClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"Sample Application", (LPSTR)"Sample Application",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  hDC = GetDC (hWnd);
  hRegion = CreateEllipticRgn (5, 5, 220, 110);

  GetClientRect (hWnd, (LPRECT) & ClearRect);
  InvertRect (hDC, (LPRECT) & ClearRect);

  MessageBox (GetFocus (), (LPSTR)"Call InvalidateRgn",
      (LPSTR)"I am about to...", MB_OK);

  InvalidateRgn (hWnd, hRegion, TRUE);   /*  mark the region for painting    */
/*  TRUE parameter says erase first */
  UpdateWindow (hWnd);  /* force a paint */

  ReleaseDC (hWnd, hDC);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
