/*
 *   HiliteMenuItem
 *
 *   This program demonstrates the use of the HiliteMenuItem function. The
 *   HiliteMenuItem function hilites or removes hiliting from an item in a
 *   menu. In this program, the 4th item in the system menu was hilited in
 *   the WinMain procedure.
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
  pHelloClass->hIcon             = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName     = (LPSTR)"Sample Application";
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
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  HelloInit (hInstance);
  hWnd = CreateWindow ( (LPSTR)"Sample Application",
      (LPSTR)"Sample Application",
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

/*** get the handle to the system menu *********************************/
  hMenu = GetSystemMenu (hWnd, FALSE);

/*** hilite the 4th item in the system menu *****************************/
  HiliteMenuItem (hWnd, hMenu, 3, MF_BYPOSITION | MF_HILITE);

/* Make window visible according to the way the app is activated */
  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  MessageBox (hWnd, (LPSTR)" ...the fourth menu-item will be hilited.",
      (LPSTR)"  Click on the System Menu and...  ", MB_OK);

/* Polling messages from event queue */
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
    }
  return (0L);
  }
