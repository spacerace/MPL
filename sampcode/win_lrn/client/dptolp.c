/*
 *  Function Name:   DPtoLP
 *
 *  Description:
 *   This program will take the client area in MM_TEXT mode (device
 *   coordinates), store the client area size in a POINT data structure,
 *   change the map mode to MM_LOENGLISH, then convert the points to
 *   MM_LOENGLISH (logical coordinates).  The size of the client area
 *   is then displayed for both map modes.
 */

#include "windows.h"
#include "stdio.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int         cmdShow;
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
    wcClass.lpszClassName  = "DPtoLP";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ("DPtoLP",
                      "DPtoLP ()",
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
  PAINTSTRUCT ps;
  RECT	      ClientRect;
  POINT       Points[1];
  char        szString1[80], szString2[80];
  int         nLength[2];

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, &ps);
      GetClientRect (hWnd, &ClientRect);
      Points[0].x = ClientRect.right;
      Points[0].y = ClientRect.bottom;
      SetMapMode (ps.hdc, MM_LOENGLISH);
      if (!DPtoLP (ps.hdc, Points, 1))
        {
        MessageBeep (0);
	break;
        }
      SetMapMode (ps.hdc, MM_TEXT);
      nLength[0] = sprintf (szString1,
          "Client area in MM_LOENGLISH mode: x=%d   y=%d",
          Points[0].x,
          Points[0].y);
      nLength[1] = sprintf (szString2,
          "Client area in MM_TEXT mode (pixels): x=%d   y=%d",
          ClientRect.right,
          ClientRect.bottom);
      TextOut (ps.hdc, 10, 10, szString1, nLength[0]);
      TextOut (ps.hdc, 30, 30, szString2, nLength[1]);
      ValidateRect (hWnd, NULL);
      EndPaint (hWnd, &ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    }
  return (0L);
  }
