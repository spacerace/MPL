/*
 *  Polygon
 *
 *  This function demonstrates the use of the Polygon function.  It will
 *  create a window, and procede to draw a triangle in the window.
 *
 */

#include <windows.h>

BOOL FAR PASCAL InitPolygon (HANDLE, HANDLE, int);
long	FAR PASCAL PolygonWindowProc (HANDLE, unsigned, WORD, LONG);

int	PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)

HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  MSG  msg;

  InitPolygon (hInstance, hPrevInstance, cmdShow);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  exit(msg.wParam);
}


BOOL FAR PASCAL InitPolygon (hInstance, hPrevInstance, cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int	cmdShow;

{
  WNDCLASS  wcPolygonClass;
  HWND hWnd;

  wcPolygonClass.lpszClassName = (LPSTR) "Polygon";
  wcPolygonClass.hInstance     = hInstance;
  wcPolygonClass.lpfnWndProc   = PolygonWindowProc;
  wcPolygonClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcPolygonClass.hIcon        = NULL;
  wcPolygonClass.lpszMenuName  = (LPSTR) NULL;
  wcPolygonClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcPolygonClass.style        = CS_HREDRAW | CS_VREDRAW;
  wcPolygonClass.cbClsExtra    = 0;
  wcPolygonClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) & wcPolygonClass);

  hWnd = CreateWindow((LPSTR)"Polygon", (LPSTR) "Polygon",
      WS_OVERLAPPEDWINDOW,  
      CW_USEDEFAULT,  0,
      CW_USEDEFAULT,  0,
      NULL,  NULL,  hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
}


long	FAR PASCAL PolygonWindowProc (hWnd, message, wParam, lParam)

HWND     hWnd;
unsigned	message;
WORD     wParam;
LONG     lParam;
{
  switch (message)
  {
  case WM_PAINT:
    PaintPolygonWindow (hWnd);
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


PaintPolygonWindow (hWnd)

HWND hWnd;
{
  PAINTSTRUCT ps;
  HDC  hDC;
  POINT  lpTriangle[4];
  HANDLE hOldBrush, hBrush;


  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
  hDC = ps.hdc;

  hBrush = GetStockObject (GRAY_BRUSH);
  hOldBrush = SelectObject (hDC, hBrush);

  lpTriangle[0].x = 150;    /* The values of the points  */
  lpTriangle[0].y = 100;
  lpTriangle[1].x = 100;
  lpTriangle[1].y = 200;
  lpTriangle[2].x = 200;
  lpTriangle[2].y = 200;

  Polygon (hDC, lpTriangle, 3);  /*  Draw the triangle */

  ValidateRect (hWnd, (LPRECT) NULL);
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  SelectObject(hDC, hOldBrush);
  return TRUE;
}


