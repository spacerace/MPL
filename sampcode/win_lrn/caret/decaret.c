/*
 *  Function Name:   DestroyCaret
 *  Program Name:    decaret.c
 *  Special Notes:
 *
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   The program below will display a caret and then destroy it.
 *
 *   Microsoft Product Support Services
 *   Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	   hInstance, hPrevInstance;
LPSTR	   lpszCmdLine;
int	cmdShow;
{
  MSG	     msg;
  HWND	     hWnd;
  HMENU      hMenu;
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
    wcClass.lpszClassName  = "DestroyCaret";

    if (!RegisterClass(&wcClass))
      return FALSE;
  }

  hWnd = CreateWindow("DestroyCaret",
      "DestroyCaret()",
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


long	FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned	message;
WORD	   wParam;
LONG	   lParam;
{
  PAINTSTRUCT ps;

  switch (message)
  {
  case WM_PAINT:
    {
      BeginPaint(hWnd, &ps);
      if (!IsIconic (hWnd))
      {
	CreateCaret (hWnd, NULL, 10, 20);
	SetCaretPos (40, 40);
	ShowCaret (hWnd);
	MessageBox(hWnd,
	    "Caret shown, press return to destroy",
	    "DestroyCaret", MB_OK);
	DestroyCaret();
      }
      ValidateRect(hWnd, NULL);
      EndPaint(hWnd, &ps);
      break;
    }
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


