/*
 *   InvalidateRect
 *
 *   This program demonstrates the use of the InvalidateRect function. The
 *   InvalidateRect function adds the given rectangle to the update region,
 *   which marks that rectangle for painting. InvalidateRect is called
 *   twice in this program: once with a specified rectangle identified by
 *   the lpRect parameter, and once with NULL sent as the parameter
 *   representing the rectangle to invalidate, which causes the whole
 *   client area to be invalidated. Both times the "bErase" paramter is
 *   TRUE which causes the given rectangles to be erased. Also, both times
 *   InvalidateRect is called, UpdateWindow is called immediately after to
 *   ensure that the rectangle is immediately redrawn.
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

  DWORD rectcolor = RGB (0x22, 0x22, 0x22); /* color for the brush   */
  HDC hDC;		 /* handle to display context		    */
  HBRUSH hMyBrush;	 /* handle to brush			    */
  RECT lpRect;       /* structure to hold rectangle coordinates */

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

/* create brush to fill rectangle with */
  hMyBrush = CreateSolidBrush (rectcolor);

/* get handle to display context */
  hDC = GetDC (hWnd);

/* select brush into display context */
  SelectObject (hDC, hMyBrush);

/* draw a rectangle so we can see InvalidateRect's effects */
  Rectangle (hDC, 5, 5, 650, 250);

/* fill lpRect with coordinate information */
  lpRect.left = 10;
  lpRect.top  = 10;
  lpRect.right = 50;
  lpRect.bottom = 50;

/* notify user call to InvalidateRect about to occur */
  MessageBox (GetFocus (), (LPSTR)"a small rectangle",
      (LPSTR)"I am about to InvalidateRect...", MB_OK);

/* invalidate the rect.identified by lpRect (TRUE param. causes erase) */
  InvalidateRect (hWnd, (LPRECT) & lpRect, (BOOL)TRUE);

/* call UpdateWindow so that draw will take place immediately */
  UpdateWindow (hWnd);

/* notify user call to InvalidateRect about to occur */
  MessageBox (GetFocus (), (LPSTR)"the whole client area",
      (LPSTR)"I am about to InvalidateRect...", MB_OK);

/* invalidate the client area (TRUE param. causes erase) */
  InvalidateRect (hWnd, (LPRECT)NULL, (BOOL)TRUE);

/* call UpdateWindow so that draw will take place immediately */
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
