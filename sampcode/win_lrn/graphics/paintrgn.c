/*

A handle to a display context is generated using the BeginPaint function.
This handle is used to select a new brush into the current display context.
Once this new brush is created, it is used to paint a grey backround within
a rectangle.

*/

#include <windows.h>
#include <string.h>

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance ;
LPSTR       lpszCmdLine ;
int	nCmdShow ;
{
  static char	szAppName [] = "PaintRgn" ;
  HWND        hWnd ;
  WNDCLASS    wndclass ;
  MSG msg;

  if (!hPrevInstance)
  {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;

    if (!RegisterClass (&wndclass))
      return FALSE ;
  }

  hWnd = CreateWindow (szAppName, (LPSTR)"PaintRgn", 
      WS_OVERLAPPEDWINDOW, 
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, nCmdShow) ;
  UpdateWindow (hWnd) ;

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (msg.wParam) ;
}


long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned	iMessage ;
WORD     wParam ;
LONG     lParam ;
{
  HDC      hDC;
  HRGN     hRgn;
  short	xX,                                      /* X coordinate */
  yY;                                      /* Y coordinate */

  PAINTSTRUCT ps;
  switch (iMessage)
  {
  case WM_PAINT:
    {
      hDC = BeginPaint(hWnd, (LPPAINTSTRUCT) & ps);

      TextOut (hDC, 10, 10,
          (LPSTR)"The Windows function PaintRgn is used to fill the backround of the rectangle",
          strlen("The Windows function PaintRgn is used to fill the backround of the rectangle")
          );

      TextOut (hDC, 10, 22,
          (LPSTR)"with the selected grey brush.",
          strlen("with the selected grey brush.")
          );


      SelectObject (hDC, GetStockObject (GRAY_BRUSH));

      xX = GetSystemMetrics (SM_CXSCREEN);
      yY = GetSystemMetrics (SM_CYSCREEN);

      hRgn = CreateRectRgn (xX / 4, yY / 4, xX / 2, yY / 2);
/* PaintRgn fills the region specified by hRgn with the brush selected *
    * into the display context.
    */
      PaintRgn (hDC, hRgn);
      EndPaint(hWnd, (LPPAINTSTRUCT) & ps);
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    {
      return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
    }
  }
  return (0L);
}


