/*
 *  Pie
 *
 *  This function demonstrates the use of the Pie function.  It will create
 *  a window, and procede to draw a pie inside of that window
 *
 */

#include <windows.h>

BOOL FAR PASCAL InitPie (HANDLE, HANDLE, int);
long	FAR PASCAL PieWindowProc (HANDLE, unsigned, WORD, LONG);

int	PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)

HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  MSG  msg;
  InitPie (hInstance, hPrevInstance, cmdShow);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  exit(msg.wParam);
}


BOOL FAR PASCAL InitPie (hInstance, hPrevInstance, cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int	cmdShow;

{
  WNDCLASS  wcPieClass;
  HWND hWnd;

  wcPieClass.lpszClassName = (LPSTR) "Pie";
  wcPieClass.hInstance    = hInstance;
  wcPieClass.lpfnWndProc   = PieWindowProc;
  wcPieClass.hCursor    = LoadCursor (NULL, IDC_ARROW);
  wcPieClass.hIcon    = NULL;
  wcPieClass.lpszMenuName  = (LPSTR) NULL;
  wcPieClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcPieClass.style    = CS_HREDRAW | CS_VREDRAW;
  wcPieClass.cbClsExtra    = 0;
  wcPieClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) & wcPieClass);

  hWnd = CreateWindow((LPSTR) "Pie", (LPSTR) "Pie", 
      WS_OVERLAPPEDWINDOW, 
      CW_USEDEFAULT,  0,
      CW_USEDEFAULT,  0,
      NULL,  NULL,  hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
}


long	FAR PASCAL PieWindowProc (hWnd, message, wParam, lParam)

HWND     hWnd;
unsigned	message;
WORD     wParam;
LONG     lParam;
{
  switch (message)
  {
  case WM_PAINT:
    PaintPieWindow (hWnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return(DefWindowProc(hWnd, message, wParam, lParam));
    break;
  }
  return(0L);
}


PaintPieWindow (hWnd)

HWND hWnd;
{
  PAINTSTRUCT ps;
  HDC  hDC;

  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
  hDC = ps.hdc;

  Pie (hDC, 100, 100, 300, 300, 300, 200, 200, 100);
/*  Draw the Pie
  *  Upper Left of box holding pie slice  = 100, 100
  *  Lower Right of box holding pie slice = 300, 300
  *  Start the arc at 300, 300
  *  End the arc at 200, 100
  *  The arc will be drawn in a counter clockwise direction
  */

  ValidateRect (hWnd, (LPRECT) NULL);
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  return TRUE;
}


