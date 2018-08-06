/*
 *  Function (s) demonstrated in this program: FillRect
 *
 *  Description:
 *   This function will fill in a specified area of type RECT with the
 *   designated brush.  The program below fills a red rectangle.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  HWND     hWnd;
  WNDCLASS wcClass;
  MSG      msg;

  if (!hPrevInstance)
    {
    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon (hInstance, NULL);
    wcClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    wcClass.lpszMenuName   = NULL;
    wcClass.lpszClassName  = "FILLRECT";

    if (!RegisterClass (&wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ("FILLRECT",
                      "FillRect",
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

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC    hDC;
  HMENU  hMenu;
  HBRUSH hBrush;
  RECT   ShowRect;

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Fill", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_INITMENU:
      InvalidateRect (hWnd, NULL, TRUE);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        hDC = GetDC (hWnd);
        hBrush = CreateSolidBrush (RGB (255, 0, 0));
        ShowRect.left   = 50;
        ShowRect.top    = 20;
        ShowRect.right  = 400;
        ShowRect.bottom = 100;
        FillRect (hDC, (LPRECT) & ShowRect, hBrush);
        DeleteObject (hBrush);
        ReleaseDC (hWnd, hDC);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
      break;
    }
  return (0L);
  }
