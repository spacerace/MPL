/*
 *   InvertRgn
 *
 *   This program demonstrates the use of the InvertRgn function. InvertRgn
 *   inverts the pixels in the given region. InvertRgn is called from
 *   WinMain in this sample application.
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

  hWnd = CreateWindow ( (LPSTR)"Sample Application", (LPSTR)"InvertRgn Application",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  hDC = GetDC (hWnd);
  hRegion = CreateEllipticRgn (5, 5, 220, 100);

/*******    invert the region "hRegion"    *******/
  if (!InvertRgn (hDC, hRegion))
    MessageBox (GetFocus (), (LPSTR)"InvertRgn failed",
        (LPSTR)" ", MB_OK);
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
