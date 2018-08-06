/*
 *  Function Name:   CreateSolidBrush
 *
 *  Description:
 *   This function creates a logical brush that can replace the current brush
 *   for the device.  The program below draws a rectangle and fills it in
 *   with a red brush that was selected.
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG     msg;
  HWND    hWnd;
  HBRUSH  hBrush;

  hBrush = CreateSolidBrush (RGB (255, 0, 0));   /* hBrush created (red). */

  if (hBrush == NULL)                /* checks for successful brush creation */
    {
    MessageBox (hWnd, (LPSTR)"CreateSolidBrush failed", (LPSTR)"ERROR", MB_ICONHAND);
    return (0L);
    }

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
    wcClass.hbrBackground  = hBrush;
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"CreateSolidBrush";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"CreateSolidBrush",
                      (LPSTR)"CreateSolidBrush ()",
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
  DeleteObject (hBrush);
  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL WndProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
