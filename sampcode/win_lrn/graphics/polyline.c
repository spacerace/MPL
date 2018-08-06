/*
 *  Polyline
 *
 *  This function demonstrates the use of the Polyline function.  It will
 *  create a window, and procede to draw a small 3-D Box in the window.
 *
 */

#include <windows.h>

BOOL FAR PASCAL InitPolyline (HANDLE, HANDLE, int);
long	FAR PASCAL PolylineWindowProc (HANDLE, unsigned, WORD, LONG);

int	PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)

HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  MSG  msg;

  InitPolyline (hInstance, hPrevInstance, cmdShow);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  exit(msg.wParam);
}


BOOL FAR PASCAL InitPolyline (hInstance, hPrevInstance, cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int	cmdShow;

{
  WNDCLASS  wcPolylineClass;
  HWND hWnd;

  wcPolylineClass.lpszClassName = (LPSTR) "Polyline";
  wcPolylineClass.hInstance     = hInstance;
  wcPolylineClass.lpfnWndProc   = PolylineWindowProc;
  wcPolylineClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcPolylineClass.hIcon         = NULL;
  wcPolylineClass.lpszMenuName  = (LPSTR) NULL;
  wcPolylineClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcPolylineClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcPolylineClass.cbClsExtra    = 0;
  wcPolylineClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) & wcPolylineClass);

  hWnd = CreateWindow((LPSTR) "Polyline", (LPSTR) "Polyline",
      WS_OVERLAPPEDWINDOW, 
      CW_USEDEFAULT,  0,
      CW_USEDEFAULT,  0,
      NULL,  NULL,  hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
}


long	FAR PASCAL PolylineWindowProc (hWnd, message, wParam, lParam)

HWND     hWnd;
unsigned	message;
WORD     wParam;
LONG     lParam;
{
  switch (message)
  {
  case WM_PAINT:
    PaintPolylineWindow (hWnd);
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


PaintPolylineWindow (hWnd)

HWND hWnd;
{
  PAINTSTRUCT ps;
  HDC  hDC;
  POINT       lpSide1[4];
  POINT       lpSide2[4];
  POINT       lpSide3[4];
  POINT       lpSide4[4];

  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
  hDC = ps.hdc;

  lpSide1[0].x = 140;  /*  The values of the box  */
  lpSide1[0].y = 130;
  lpSide1[1].x = 100;
  lpSide1[1].y = 100;
  lpSide1[2].x = 170;
  lpSide1[2].y = 100;
  lpSide1[3].x = 200;
  lpSide1[3].y = 130;
  lpSide2[0].x = 100;
  lpSide2[0].y = 100;
  lpSide2[1].x = 100;
  lpSide2[1].y = 170;
  lpSide2[2].x = 170;
  lpSide2[2].y = 170;
  lpSide2[3].x = 170;
  lpSide2[3].y = 100;
  lpSide3[0].x = 100;
  lpSide3[0].y = 170;
  lpSide3[1].x = 140;
  lpSide3[1].y = 200;
  lpSide3[2].x = 200;
  lpSide3[2].y = 200;
  lpSide3[3].x = 170;
  lpSide3[3].y = 170;
  lpSide4[0].x = 140;
  lpSide4[0].y = 200;
  lpSide4[1].x = 140;
  lpSide4[1].y = 130;
  lpSide4[2].x = 200;
  lpSide4[2].y = 130;
  lpSide4[3].x = 200;
  lpSide4[3].y = 200;

  Polyline(hDC, lpSide1, 4);  /*  Draw the sides of the box  */
  Polyline(hDC, lpSide2, 4);
  Polyline(hDC, lpSide3, 4);
  Polyline(hDC, lpSide4, 4);

  ValidateRect (hWnd, (LPRECT) NULL);
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  return TRUE;
}


