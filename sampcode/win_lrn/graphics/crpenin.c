/*
 *  Function Name:   CreatePenIndirect
 *
 *  Description:
 *   This program creates a logical pen that replaces the current pen for the
 *   device.  It uses the data structure LOGPEN to set up the style, width,
 *   and color fields.  With lopnWidth, the data structure is type POINT.
 *   The x value is the width, and the y value is ignored.  Unlike CreatePen,
 *   the structure for LOGOPEN must first be allocated before values can be
 *   assigned.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

HPEN hPen, hOldPen;   /*  Global Handles to Pens  */

/***********************************************************************/
void CALL_CreatePenIndirect (hWnd)
HWND hWnd;
  {
  LOGPEN PenIndirect;
  POINT Point1;

  Point1.x = 10;                      /* assign width 10 to x.          */
  Point1.y = 0;                       /* y is ignored.                  */
  PenIndirect.lopnStyle = 0;         /* style is 0                    */
  PenIndirect.lopnWidth = Point1;      /* width is Point1.         */
  PenIndirect.lopnColor = RGB (255, 0, 0);       /* color is red.   */

  hPen = CreatePenIndirect ( (LPLOGPEN) & PenIndirect);  /*  pen created  */

  if (hPen == NULL)                /* checks for successful pen creation */
    {
    MessageBox (hWnd, (LPSTR)"CreatePenIndirect failed",
                (LPSTR)"ERROR", MB_ICONHAND | MB_OK);
    DestroyWindow (hWnd);  /*  The pen wasn't created, so trash the program  */
    return;
    }
  return;
  }


int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

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
    wcClass.lpszClassName  = (LPSTR)"CreatePenIndirect";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"CreatePenIndirect",
                      (LPSTR)"CreatePenIndirect ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL);        /* no params to pass on */

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
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_CREATE:
      CALL_CreatePenIndirect (hWnd);
      break;

    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      hOldPen = SelectObject (ps.hdc, hPen);
      Arc (ps.hdc, 0, 0, 300, 200, 200, 140, 100, 30);
      SelectObject (ps.hdc, hOldPen);
      ValidateRect (hWnd, (LPRECT) NULL);
      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    case WM_DESTROY:
      DeleteObject (hPen);
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
