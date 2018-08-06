/*
 *  SetMapMode
 *
 *  This function demonstrates the use of the SetMapMode function.  It will
 *  create a window, and procede to draw a triangle in the window in
 *  the MM_ANISOTROPIC mapping mode.
 */

#include <windows.h>

BOOL FAR PASCAL InitSetMapMode (HANDLE, HANDLE, int);
long    FAR PASCAL SetMapModeWindowProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)

HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  MSG  msg;

  InitSetMapMode (hInstance, hPrevInstance, cmdShow);   /*  Init Routine  */

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  exit (msg.wParam);
  }

BOOL FAR PASCAL InitSetMapMode (hInstance, hPrevInstance, cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int	cmdShow;
  {
  WNDCLASS  wcSetMapModeClass;
  HWND	hWnd;

  wcSetMapModeClass.lpszClassName = (LPSTR) "SetMapMode";
  wcSetMapModeClass.hInstance     = hInstance;
  wcSetMapModeClass.lpfnWndProc   = SetMapModeWindowProc;
  wcSetMapModeClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcSetMapModeClass.hIcon         = NULL;
  wcSetMapModeClass.lpszMenuName  = (LPSTR) NULL;
  wcSetMapModeClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcSetMapModeClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcSetMapModeClass.cbClsExtra    = 0;
  wcSetMapModeClass.cbWndExtra    = 0;

  RegisterClass ( (LPWNDCLASS) & wcSetMapModeClass);

  hWnd = CreateWindow ( (LPSTR) "SetMapMode", (LPSTR) "SetMapMode",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,    0,
                      CW_USEDEFAULT,    0,
                      NULL,     NULL,   hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }

long    FAR PASCAL SetMapModeWindowProc (hWnd, message, wParam, lParam)

HWND	    hWnd;
unsigned    message;
WORD	    wParam;
LONG	    lParam;
  {
  switch (message)
    {
    case WM_PAINT:
      PaintSetMapModeWindow (hWnd);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
      break;
    }
  return (0L);
  }

PaintSetMapModeWindow (hWnd)
HWND	hWnd;
  {
  PAINTSTRUCT	ps;
  HDC		hDC;
  POINT 	lpTriangle[4];
  HANDLE        hOldBrush, hBrush;
  RECT		rRect;

  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
  hDC = ps.hdc;

  hBrush = GetStockObject (GRAY_BRUSH);
  hOldBrush = SelectObject (hDC, hBrush);

  lpTriangle[0].x = 150;
  lpTriangle[0].y = 100;
  lpTriangle[1].x = 100;
  lpTriangle[1].y = 200;
  lpTriangle[2].x = 200;
  lpTriangle[2].y = 200;

  SetMapMode (hDC, MM_ANISOTROPIC);     /*  Set the mapping mode             */

  SetWindowExt (hDC, 300, 300);        /*  Set the extent of the drawing
                                        *  area.  This is the area that
                                        *  holds graphics that you create
                                        *  with GDI functions.  Do not
                                        *  confuse this function with
                                        *  the actual window.  The
                                        *  SetViewportExt sets the
                                        *  extent of the area to be mapped
                                        *  to which is the actual window
                                        */
  GetClientRect (hWnd, (LPRECT) & rRect);
/*  Get the size of the client area
					  *  so that we can set the viewport
					  *  extent
					  */

  SetViewportExt (hDC, rRect.right, rRect.bottom);

  Polygon (hDC, lpTriangle, 3);

  ValidateRect (hWnd, (LPRECT) NULL);
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  SelectObject (hDC, hOldBrush);
  return TRUE;
  }
