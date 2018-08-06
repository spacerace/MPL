/*
 *  Function Name:   CreatePolygonRgn
 *
 *  Description:
 *   The program below will create a triangular region and then display it
 *   by inverting the region.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/
void CALL_CreatePolygonRgn (hWnd, hDC)
HWND hWnd;
HDC hDC;
  {
  HRGN hNewRgn;
  POINT Points[3];

  Points[0].x = 10;
  Points[0].y = 10;
  Points[1].x = 10;
  Points[1].y = 190;                      /* clip area     */
  Points[2].x = 200;
  Points[2].y = 10;
     /*  polygon region created  */
  hNewRgn = CreatePolygonRgn ( (LPPOINT)Points, 3, WINDING);

  if (hNewRgn == NULL)                   /* Check for error */
    {
    MessageBeep (0);
    return;
    }
  InvertRgn (hDC, hNewRgn);
  DeleteObject (hNewRgn);
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
    wcClass.lpszClassName  = (LPSTR)"CreatePolygonRgn";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"CreatePolygonRgn",
                      (LPSTR)"CreatePolygonRgn ()",
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
      CALL_CreatePolygonRgn (hWnd, ps.hdc);
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
