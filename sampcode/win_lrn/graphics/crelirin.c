/*
 *  Function Name:   CreateEllipticRgnIndirect
 *
 *  Description:
 *   The program below creates an elliptical region that will be displayed
 *   by inverting the region.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_CreateEllipticRgnIndirect (hWnd, hDC)
HWND hWnd;
HDC hDC;
  {
  RECT ClientRect;
  HRGN hNewRgn;

  GetClientRect (hWnd, (LPRECT) & ClientRect);   /* get client area  */

  ClientRect.right /= 2;
/* create new region   */
  hNewRgn = CreateEllipticRgnIndirect ( (LPRECT) & ClientRect);

  if (hNewRgn == NULL)                   /* Check for error */
    {
    MessageBeep (0);
    return;
    }
  InvertRgn (hDC, hNewRgn);
  DeleteObject (hNewRgn);
  return;
  }

/**************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
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
    wcClass.lpszClassName  = (LPSTR)"CreateEllipticRgnIndirect";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"CreateEllipticRgnIndirect",
                      (LPSTR)"CreateEllipticRgnIndirect ()",
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
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      CALL_CreateEllipticRgnIndirect (hWnd, ps.hdc);
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
