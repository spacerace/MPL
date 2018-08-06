/*
 *  Function Name:   CreateEllipticRgn
 *
 *  Description:
 *   The program below will create an elliptical region which will be
 *   inverted so that it can be seen.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_CreateEllipticRgn (hWnd, hDC)
HWND hWnd;
HDC hDC;
  {
  HRGN hNewRgn;
  RECT ClientRect;

  GetClientRect (hWnd, (LPRECT) & ClientRect);       /* get client area */

  hNewRgn = CreateEllipticRgn (10, 10, (short) (ClientRect.right / 2),
                               (short) (ClientRect.bottom / 2));
  if (hNewRgn == NULL)
    {
    MessageBeep(0);    /*  Beep if we have an error  */
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
    wcClass.lpszClassName  = (LPSTR)"CreateEllipticRgn";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"CreateEllipticRgn",
                      (LPSTR)"CreateEllipticRgn ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,
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
      CALL_CreateEllipticRgn (hWnd, ps.hdc);
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
