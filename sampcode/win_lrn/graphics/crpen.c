/*
 *  Function Name:   CreatePen
 *
 *  Description:
 *   It creates a logical pen that can replace the current pen for the device.
 *   The module below draws an arc.
 *   CreatePen selects the style (0=solid), a width of 20, and the color
 *   red. A width of 0 is one pixel width on raster devices.  The mode used
 *   is MM_TEXT (default).
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/
void CALL_CreatePen (hWnd, hDC)
HWND hWnd;
HDC hDC;
  {
  HPEN hPen, hOldPen;

  hPen = CreatePen (0, 20, RGB (255, 0, 0));   /* hPen created.   */

  if (hPen == NULL)                /* checks for successful pen creation */
    {
    MessageBeep(0);
    return;
    }

  hOldPen = (HPEN) SelectObject (hDC, (HANDLE) hPen);
  Arc (hDC, 0, 0, 300, 200, 200, 180, 100, 30);
  DeleteObject (hPen);

  return;
  }

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  if (!hPrevInstance)
    {
    WNDCLASS   wcClass;

    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon (hInstance, NULL);
    wcClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"CreatePen";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"CreatePen",
                      (LPSTR)"CreatePen ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL);      /* no params to pass on */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (int)msg.wParam;
  }

/* Procedures which make up the window class. */
long    FAR PASCAL WndProc (hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {

  case WM_PAINT:
    BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
    CALL_CreatePen (hWnd, ps.hdc);
    ValidateRect (hWnd, (LPRECT) NULL);
    EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
    break;

  case WM_DESTROY:
    PostQuitMessage (0);
    break;

  default:
    return DefWindowProc (hWnd, message, wParam, lParam);
    break;
    }
  return (0L);
  }
