/*
 *  Function Name:   DrawText
 *
 *  Description:
 *   This program will put text into the user rectangle with some control
 *   over the method of formatting the text.
 */

#include "windows.h"
#include "stdio.h"

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
    wcClass.lpszClassName  = "DrawText";

    if (!RegisterClass (&wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ("DrawText",
      "DrawText ()",
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
HWND	   hWnd;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
  {
  PAINTSTRUCT ps;
  HDC	      hDC;
  RECT	      RectDraw;
  char	szString[80];
  int	nLength;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      hDC = ps.hdc;
      RectDraw.left	 = 10;
      RectDraw.top	 = 10;
      RectDraw.right	 = 150;
      RectDraw.bottom	 = 70;
      nLength = sprintf (szString, "Wow!  Some Text!");
      DrawText (hDC, szString, nLength, &RectDraw, DT_LEFT | DT_WORDBREAK);
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
