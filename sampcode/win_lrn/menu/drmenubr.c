/*
 *  Function Name:   DrawMenuBar
 *
 *  Description:
 *   The program below will redraw a window's menu, usually with
 *   modifications by ChangeMenu.  DrawMenuBar gives the same result as
 *   SetMenu, but DrawMenuBar cannot initially create the menu bar like
 *   SetMenu can.  The program below uses SetMenu to set and display
 *   'Heading1'and uses DrawMenuBar to display the addition 'Heading2'
 *   by ChangeMenu.
 *
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG	     msg;
  HWND	     hWnd;
  WNDCLASS   wcClass;

  if (!hPrevInstance)
    {
    wcClass.style	   = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra	   = 0;
    wcClass.cbWndExtra	   = 0;
    wcClass.hInstance	   = hInstance;
    wcClass.hIcon          = LoadIcon (hInstance, NULL);
    wcClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    wcClass.lpszMenuName   = NULL;
    wcClass.lpszClassName  = "DrawMenuBar";

    if (!RegisterClass (&wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ("DrawMenuBar",
      "DrawMenuBar ()",
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
  HMENU      hCurrentWindowMenu;
  unsigned   wIDItem;

  switch (message)
    {
    case WM_CREATE:
      hCurrentWindowMenu = CreateMenu ();
      ChangeMenu (hCurrentWindowMenu, NULL, "Heading1", wIDItem,
                  MF_APPEND | MF_BYCOMMAND | MF_STRING);
      SetMenu (hWnd, hCurrentWindowMenu);
      ChangeMenu (hCurrentWindowMenu, NULL, "Heading2", wIDItem,
                  MF_APPEND | MF_BYCOMMAND | MF_STRING);
      DrawMenuBar (hWnd);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;
    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    }
  return (0L);
  }
