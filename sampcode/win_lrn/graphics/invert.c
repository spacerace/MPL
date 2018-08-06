/*
 *   InvertRect
 *
 *   This program demonstrates the use of the InvertRect function. The
 *   InvertRect function inverts the contents of the given rectangle.
 *   In this application, two calls are made to InvertRect. The first
 *   inverts the pixels in a rectangle specified by lpRect. The second
 *   call inverts the whole client area.
 */

#include "windows.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

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

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, "Invert!", 100, MF_APPEND);

  hWnd = CreateWindow ( (LPSTR)"Sample Application", (LPSTR)"InvertRect",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0,
                      NULL, hMenu, hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

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
  HDC hDC;
  RECT rRect;

  switch (message)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        hDC = GetDC (hWnd);

        rRect.left = 10; /* fill lpRect with coordinate info for rectangle */
        rRect.top  = 10;
        rRect.right = 150;
        rRect.bottom = 20;

        InvertRect (hDC, (LPRECT) &rRect);    /** invert pixels in lpRect **/
        ReleaseDC (hWnd, hDC);
        }
      else
        return DefWindowProc (hWnd, message, wParam, lParam);
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
