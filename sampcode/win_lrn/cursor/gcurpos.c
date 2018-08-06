/*
 *  GetCursorPos
 *
 *  This program demonstrates the use of the function GetCursorPos.
 *  GetCursorPos retrieves the cursor position in screen coordinates.
 *  This means that even if the cursor is not in the current window,
 *  it's position will be retrieved. GetCursorPos is called from
 *  WinMain () in this application.
 *
 */

#include "windows.h"
#include <stdio.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
extern int      FAR PASCAL lstrlen (LPSTR);

static char	szAppName [] = "GCURPOS";
static char	szFuncName [] = "GetCursorPos";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  HWND      hWnd;
  WNDCLASS  wndclass;
  MSG       msg;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hWnd = CreateWindow (szAppName,
                      szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      0,
                      CW_USEDEFAULT,
                      0,
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
  return (msg.wParam);
  }


long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC   hDC;
  HMENU hMenu;
  POINT CursorPos;
  char	szBuffer[30];

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Curse Tracking", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_INITMENU:
      InvalidateRect (hWnd, NULL, TRUE);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        hDC = GetDC (hWnd);
        GetCursorPos ( (LPPOINT) & CursorPos);
        sprintf (szBuffer, "Cursor Position: x = %d y = %d", CursorPos.x,
            CursorPos.y);
        TextOut (hDC, 10, 10,  (LPSTR)szBuffer, lstrlen ( (LPSTR)szBuffer));
        ReleaseDC (hWnd, hDC);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
