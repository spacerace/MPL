/*
 *  Function Name:   DispatchMessage
 *
 *  Description:
 *   This function passes all messages from the main procedure (WinMain) to
 *   the window procedure.  It passes the message in the MSG structure to the
 *   window function of the specified window.
 */

#include "windows.h"

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);



int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	  hInstance, hPrevInstance;
LPSTR	  lpszCmdLine;
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
    wcClass.hIcon	   = LoadIcon(hInstance, NULL);
    wcClass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcClass.lpszMenuName   = NULL;
    wcClass.lpszClassName  = "DispatchMessage";

    if (!RegisterClass(&wcClass))
      return FALSE;
  }

  hWnd = CreateWindow("DispatchMessage",
      "DispatchMessage()",
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
  MessageBox(hWnd, "DispatchMessage working in loop",
      "DispatchMessage", MB_ICONASTERISK);
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}


long	FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned	message;
WORD	   wParam;
LONG	   lParam;
{
  switch (message)
  {
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return(0L);
}


