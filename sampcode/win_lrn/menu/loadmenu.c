/*
 *   LoadMenu
 *
 *   This program demonstrates the use of the LoadMenu function.
 *   LoadMenu loads a menu resource into memory. To put the menu resource
 *   in a window, SetMenu must be called after LoadMenu.
 */

#include "windows.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

/**************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE     hInstance, hPrevInstance;
LPSTR      lpszCmdLine;
int        cmdShow;
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

  hWnd = CreateWindow ( (LPSTR)"Sample Application",
      (LPSTR)"LoadMenu Application",
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

  MessageBox (hWnd, (LPSTR)"call LoadMenu", (LPSTR)"I am about to...", MB_OK);

  hMenu = LoadMenu (hInstance, "loadmenu");       /* Load menu into memory */

  if (!hMenu)
    MessageBox (hWnd, (LPSTR)"LoadMenu failed", (LPSTR)NULL, MB_OK);
  else
    SetMenu (hWnd, hMenu); /* put the menu in the window */

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (int)msg.wParam;
  }

/**************************************************************************/
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
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
