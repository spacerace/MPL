/*
 * Function demonstrated in this program: LineTo
 *
 * Description:
 *   This program demonstrates the use of the LineTo () function.
 *   LineTo () draws a line from the current position up to, but not including
 *   the point specified by X, Y. It also updates the current position to X, Y.
 *   In this program, LineTo () is called in response to a WM_PAINT message
 *   in HelloWndProc ().
 *
 */

#include "windows.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG       msg;
  HWND      hWnd;
  HMENU     hMenu;
  WNDCLASS  HelloClass;

  if (!hPrevInstance)
    {
    HelloClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    HelloClass.hIcon          = LoadIcon (hInstance, NULL);
    HelloClass.lpszMenuName   = (LPSTR)NULL;
    HelloClass.lpszClassName  = (LPSTR)"LineTo";
    HelloClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    HelloClass.hInstance      = hInstance;
    HelloClass.style          = CS_HREDRAW | CS_VREDRAW;
    HelloClass.lpfnWndProc    = HelloWndProc;

    if (!RegisterClass ( (LPWNDCLASS)&HelloClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"LineTo",
                      (LPSTR)"LineTo ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      (HWND)NULL,           /* no parent */
                      (HMENU)NULL,          /* use class menu */
                      (HANDLE)hInstance,    /* handle to window instance */
                      (LPSTR)NULL);         /* no params to pass on */

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
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);

      MoveTo (ps.hdc, 100, 10);    /*  move current position to 100, 10 */
      LineTo (ps.hdc, 100, 200);   /*  draw from 100, 10 to 10, 200      */
      MoveTo (ps.hdc, 200, 10);    /*  move current position to 200, 10 */
      LineTo (ps.hdc, 200, 200);   /*  draw from 200, 10 to 200, 200     */
      LineTo (ps.hdc, 300, 10);    /* draw to 300, 10                      */

      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
