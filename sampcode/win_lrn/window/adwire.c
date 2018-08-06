/*
 *
 *   Function (s) demonstrated in this program:
 *        AdjustWindowRect ()
 *
 *   Compiler version:
 *        Microsoft C 5.1
 *
 *   Description:
 *        This function demonstrates the use of the AdjustWindowRect function.
 *        It will compute the required size of the window rectangle based on
 *        the desired client-rectangle size.  The window rectangle will then be
 *        passed to the CreateWindow function to create a Window whose client
 *        area is the desired size.
 *
 *
 */

#include <windows.h>

BOOL FAR PASCAL InitCreateWindow (HANDLE, HANDLE, int);
long	FAR PASCAL CreateWindowProc (HANDLE, unsigned, WORD, LONG);

int	PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int	nCmdShow;
{
  MSG msg;

  InitCreateWindow (hInstance, hPrevInstance, nCmdShow);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
  }
  exit (msg.wParam);
}


BOOL FAR PASCAL InitCreateWindow (hInstance, hPrevInstance, cmdShow)
HANDLE   hInstance;
HANDLE   hPrevInstance;
int	cmdShow;
{
  WNDCLASS  wcCreateWindowClass;
  HWND  hWnd;

  wcCreateWindowClass.lpszClassName = (LPSTR) "CreateWindow";
  wcCreateWindowClass.hInstance     = hInstance;
  wcCreateWindowClass.lpfnWndProc   = CreateWindowProc;
  wcCreateWindowClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcCreateWindowClass.hIcon         = NULL;
  wcCreateWindowClass.lpszMenuName  = (LPSTR) NULL;
  wcCreateWindowClass.hbrBackground = GetStockObject (BLACK_BRUSH);
  wcCreateWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcCreateWindowClass.cbClsExtra    = 0;
  wcCreateWindowClass.cbWndExtra    = 0;

  RegisterClass ( (LPWNDCLASS) & wcCreateWindowClass);

  hWnd = CreateWindow ( (LPSTR) "CreateWindow", (LPSTR) "AdjustWindowRect",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, cmdShow);         /* Display this window on the screen  */
  UpdateWindow (hWnd);                              /* Cause a paint message */

  return TRUE;
}


long	FAR PASCAL CreateWindowProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned	message;
WORD        wParam;
LONG        lParam;
{
  switch (message)
  {
  case WM_PAINT:
    PaintCreate (hWnd);
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


PaintCreate (hWnd)  /*  The Paint Procedure  */
HWND    hWnd;
{
  PAINTSTRUCT  ps;
  HDC          hDC;
  BOOL         bMenu;
  long	lStyle;
  HANDLE       hOldBrush, hBrush;
  RECT         lpRect;

  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);

  hDC = ps.hdc;                                  /* Get the Display Context  */
  lStyle = WS_TILED;
  bMenu = 0;
  hBrush         = GetStockObject (BLACK_BRUSH);  /* Get a Black brush     */
  hOldBrush      = SelectObject (hDC, hBrush);   /* Select the new brush  */

  SetMapMode (hDC, MM_ANISOTROPIC);              /* Set the mapping mode  */
  SetWindowExt (hDC, 200, 200);           /* Set Extent of viewing area. */
  GetClientRect (hWnd, (LPRECT) & lpRect);     /* Get size of client area. */
  SetViewportExt (hDC, lpRect.right, lpRect.bottom); /* Set extent of viewport */
  AdjustWindowRect (&lpRect, lStyle, bMenu);
  TextOut (hDC, 10, 10,
      (LPSTR)"Smallest rectangle that will encompass the entire Window.",
      strlen ("Smallest rectangle that will encompass the entire Window."));

  ValidateRect (hWnd, (LPRECT) NULL);   /* Disable any more paint messages  */
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  SelectObject (hDC, hOldBrush);                /*  Replace the old brush  */

  return TRUE;
}


