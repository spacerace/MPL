/*
 *
 *  Function (s) demonstrated in this program: FrameRgn
 *  Compiler version: C 5.1
 *
 *  Description:
 *     This program demonstrates the use of the function FrameRgn.
 *     This function draws a border around the region specified by a
 *     handle to a region, using the brush specified by a hanle to a
 *     brush. The fourth parameter specifies the width of the border
 *     on vertical brush strokes, and the fifth parameter specifies
 *     the height on horizontal strokes.
 *  
 */

#include <windows.h>

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  HWND     hWnd;
  WNDCLASS rClass;
  MSG      msg;

  if (!hPrevInstance)
    {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = "framergn";

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hWnd = CreateWindow ("framergn",            /* window class name       */
                      "FrameRgn",                 /* window caption          */
                      WS_OVERLAPPEDWINDOW,        /* window style            */
                      CW_USEDEFAULT,              /* initial x position      */
                      0,                          /* initial y position      */
                      CW_USEDEFAULT,              /* initial x size          */
                      0,                          /* initial y size          */
                      NULL,                       /* parent window handle    */
                      NULL,                       /* window menu handle      */
                      hInstance,                  /* program instance handle */
                      NULL);                      /* create parameters       */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL WndProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned    message;
WORD        wParam;
LONG        lParam;
  {
  HDC    hDC;
  HMENU  hMenu;
  HRGN   hRgn;
  HBRUSH hBrush;

  switch (message)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Frame", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_INITMENU:
      InvalidateRect (hWnd, NULL, TRUE);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        hRgn = CreateRectRgn (1, 1, 200, 100);
        hBrush = CreateSolidBrush ( (DWORD) 0x0000FF);
        hDC = GetDC (hWnd);
        FrameRgn (hDC, hRgn, hBrush, 10, 10);
        DeleteObject (hRgn);
        DeleteObject (hBrush);
        ReleaseDC (hWnd, hDC);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
    }
  return (0L);
  }
