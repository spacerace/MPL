/*
 *
 *   qvisible.c
 *
 *   This program demonstrates the use of the IsWindowVisible function.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
long    FAR PASCAL ChildProc (HWND, unsigned, WORD, LONG);

HWND hChildWnd = NULL;

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE     hInstance, hPrevInstance;
LPSTR      lpszCmdLine;
int        cmdShow;
  {
  MSG	    msg;
  HWND	    hWnd;
  HMENU     hMenu;
  WNDCLASS  wndclass;

  if (!hPrevInstance)
    {
    wndclass.style	    = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance	    = hInstance;
    wndclass.hIcon	    = NULL;
    wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground  = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = "Sample Application";

    if (!RegisterClass (&wndclass))
      return FALSE;

    wndclass.style	    = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = ChildProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance	    = hInstance;
    wndclass.hIcon	    = NULL;
    wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground  = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = "CHILD";

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hWnd = CreateWindow ("Sample Application",
      "IsWindowVisible",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance,
      NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  hChildWnd = CreateWindow ("CHILD",
      "Child",
      WS_CHILD | WS_SIZEBOX | WS_CAPTION | WS_CLIPSIBLINGS,
      5,
      5,
      150,
      150,
      hWnd,
      (HMENU)1,
      hInstance,
      NULL);

  MessageBox (hWnd, "the child window is visible", "I am going to see if...",
             MB_OK);
  if (IsWindowVisible (hChildWnd))
    MessageBox (hWnd, "the child window IS visible",
        "'IsWindowVisible' says...", MB_OK);
  else
    MessageBox (hWnd, "the child window is NOT visible",
        "'IsWindowVisible' says...", MB_OK);

  ShowWindow (hChildWnd, SW_SHOWNORMAL);

  MessageBox (hWnd, "the child window is visible",
             "I am going to see if...", MB_OK);

  if (IsWindowVisible (hChildWnd))
    MessageBox (hWnd, "the child window IS visible",
        "'IsWindowVisible' says...", MB_OK);
  else
    MessageBox (hWnd, "the child window is NOT visible",
                "'IsWindowVisible' says...", MB_OK);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return msg.wParam;
  }


long    FAR PASCAL WndProc (hWnd, message, wParam, lParam)
HWND	  hWnd;
unsigned  message;
WORD	  wParam;
LONG	  lParam;
  {
  switch (message)
    {
    case WM_LBUTTONDOWN:
      MessageBox (hWnd, "Left Button Click", "PARENT WINDOW", MB_OK);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    }
  return (0L);
  }

long    FAR PASCAL ChildProc (hChildWnd, message, wParam, lParam)
HWND	  hChildWnd;
unsigned  message;
WORD	  wParam;
LONG	  lParam;
  {
  switch (message)
    {
    case WM_LBUTTONDOWN:
      MessageBox (hChildWnd, (LPSTR)"Left Button Click", (LPSTR)"CHILD A",
                  MB_OK);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hChildWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
