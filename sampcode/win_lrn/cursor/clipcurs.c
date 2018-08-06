/*
 *   This program demonstrates use of the ClipCursor () function.
 *   ClipCursor () confines movement of the cursor to the rectangle specified
 *   by the "lpRect" parameter . In WinMain (), GetClientRect () is
 *   called, then ClientToScreen () is called to convert the client area
 *   coordinates to screen coordinates (because ClipCursor () is a screen-
 *   coordinate relative function).  ClipCursor () is then called to confine
 *   the cursor to the left half of the client area.
 */

#include "windows.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName  = (LPSTR)"ClipCursor";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree ( (HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }


int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR   lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  RECT  rClientRect;
  POINT UpperLeft;
  POINT LowerRight;

  HelloInit (hInstance);
  hWnd = CreateWindow ( (LPSTR)"ClipCursor",
                      (LPSTR)"ClipCursor ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL);      /* no params to pass on */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  GetClientRect (hWnd, (LPRECT) & rClientRect);
  UpperLeft.x  = rClientRect.left;
  UpperLeft.y  = rClientRect.top;
  ClientToScreen (hWnd, (LPPOINT) & UpperLeft);

  LowerRight.x = rClientRect.bottom;
  LowerRight.y = rClientRect.right / 2;
  ClientToScreen (hWnd, (LPPOINT) & LowerRight);

  rClientRect.left   = UpperLeft.x;
  rClientRect.top    = UpperLeft.y;
  rClientRect.bottom = LowerRight.x;
  rClientRect.right  = LowerRight.y;

  ClipCursor ( (LPRECT) & rClientRect);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
  {
  switch (message)
    {
  case WM_DESTROY:
/*** release the cursor (a shared resource) to the rest of the system ***/
    ClipCursor ( (LPRECT)NULL);
    PostQuitMessage (0);
    break;

  default:
    return DefWindowProc (hWnd, message, wParam, lParam);
    break;
    }
  return (0L);
  }
