/*
 * Function demonstrated in this program: KillTimer
 * Compiler version: C 5.1
 *
 * Description:  
 *   This program demonstrates the use of the KillTimer function. KillTimer
 *   kills the timer event associated with "hWnd" and having the same
 *   event identifier.
 */

#include "windows.h"

long   FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;
  WNDCLASS   HelloClass;

  if (!hPrevInstance)
    {
    HelloClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    HelloClass.hIcon          = LoadIcon (hInstance, NULL);
    HelloClass.lpszMenuName   = (LPSTR)NULL;
    HelloClass.lpszClassName  = (LPSTR)"Hello";
    HelloClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    HelloClass.hInstance      = hInstance;
    HelloClass.style          = CS_HREDRAW | CS_VREDRAW;
    HelloClass.lpfnWndProc    = HelloWndProc;

    if (!RegisterClass ( (LPWNDCLASS)&HelloClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"Hello",
                     (LPSTR)"KillTimer",
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

  SetTimer (hWnd, 100, 500, (FARPROC)NULL);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  KillTimer (hWnd, 100);      /* Kill The Timer */
  return (int)msg.wParam;
  }

/*************************************************************************/
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  switch (message)
    {
    case WM_TIMER:
      MessageBeep(0);
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
