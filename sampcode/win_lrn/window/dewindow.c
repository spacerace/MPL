/*
 *  Function Name:   DestroyWindow
 *  Program Name:    dewindow.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   This program will destroy the window, leaving just a message box.
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	 hInstance, hPrevInstance;
LPSTR	 lpszCmdLine;
int	 cmdShow;
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
    wcClass.hIcon	   = LoadIcon(hInstance, NULL);
    wcClass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcClass.lpszMenuName   = NULL;
    wcClass.lpszClassName  = "DestroyWindow";

    if (!RegisterClass(&wcClass))
      return FALSE;
    }

  hWnd = CreateWindow("DestroyWindow",
		      "DestroyWindow()",
		      WS_OVERLAPPEDWINDOW,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      NULL,
		      NULL,
		      hInstance,
		      NULL);

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);
  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }
  return msg.wParam;
  }

long FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
  {
  PAINTSTRUCT ps;
  BOOL        bDestroyed;
  HDC	      hDC;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint(hWnd, &ps);
      hDC=ps.hdc;
      TextOut(hDC, 10, 10, "End program to demonstrate", 26);
      ValidateRect(hWnd, NULL);
      EndPaint(hWnd, &ps);
      break;
    case WM_CLOSE:
      bDestroyed = DestroyWindow(hWnd);
      MessageBox(hWnd, "Window was just destroyed", "DestroyWindow", MB_OK);
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  return(0L);
  }
