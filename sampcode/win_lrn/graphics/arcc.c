/*
 *  Function Name:   Arc
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   The program below will draw an arc.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_Arc (hWnd, hDC)
HWND hWnd;
HDC hDC;
  {
  BOOL      bDrawn;
  int   xScreen, yScreen;

  xScreen = GetSystemMetrics (SM_CXSCREEN);
  yScreen = GetSystemMetrics (SM_CYSCREEN);

  bDrawn = Arc (hDC, xScreen / 3, yScreen / 3, (2 * xScreen) / 3, (2 * yScreen) / 3,
      (2 * xScreen) / 5, (2 * yScreen) / 5, xScreen / 2, yScreen / 2);  /* function demonstrated  */

  if (bDrawn != TRUE)
    MessageBeep (MB_OK);  /*  Couldn't Draw the ARC, so BEEP!!!  */

  return;
  }


/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit (hInstance)
HANDLE hInstance;
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
  wcClass.lpszClassName  = (LPSTR)"Arc";

  if (!RegisterClass ( (LPWNDCLASS) & wcClass))
    return FALSE;

  return TRUE;
  }


int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  if (!hPrevInstance)
    if (!WinInit (hInstance))
      return FALSE;

  hWnd = CreateWindow ( (LPSTR)"Arc", (LPSTR)"Arc ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0,
                      NULL, NULL, hInstance, NULL);

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
      CALL_Arc (hWnd, ps.hdc);
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
