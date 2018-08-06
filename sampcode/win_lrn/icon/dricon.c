/*
 *  Function Name:   DrawIcon
 *
 *  Description:
 *   The program below will display a stock icon.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR	lpszCmdLine;
int	cmdShow;
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
    wcClass.lpszClassName  = "DrawIcon";

    if (!RegisterClass (&wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ("DrawIcon",
                      "DrawIcon ()",
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
  HANDLE      hIcon;
  BOOL	      bDrawn;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      hIcon = LoadIcon ( (HANDLE) NULL, IDI_EXCLAMATION);
      if (!DrawIcon (ps.hdc, 50, 50, hIcon))
        {
        MessageBeep (0);
	return FALSE;
        }
      ValidateRect (hWnd, (LPRECT) NULL);
      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    }
  return (0L);
  }
