/*
 *   LoadCursor
 *
 *   This program demonstrates the use of the LoadCursor function. The
 *   LoadCursor function loads either a standard windows cursor or a user
 *   defined cursor created with the Icon Editor. LoadCursor is called in
 *   LoadCursInit in this sample application.
 */

#include "windows.h"

long    FAR PASCAL LoadCursWndProc (HWND, unsigned, WORD, LONG);
int     PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
BOOL FAR PASCAL About (HWND, unsigned, WORD, LONG);

/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  if (!hPrevInstance)
    {
    WNDCLASS   LoadCursClass;

  /*----------------------- Invoke LoadCursor -------------------------------*/
    LoadCursClass.hCursor        = LoadCursor (hInstance, "mycursor");
  /*-------------------------------------------------------------------------*/
    LoadCursClass.hIcon          = LoadIcon (hInstance, NULL);
    LoadCursClass.lpszMenuName   = (LPSTR)NULL;
    LoadCursClass.lpszClassName  = (LPSTR)"Sample Application";
    LoadCursClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    LoadCursClass.hInstance      = hInstance;
    LoadCursClass.style          = CS_HREDRAW | CS_VREDRAW;
    LoadCursClass.lpfnWndProc    = LoadCursWndProc;

    if (!RegisterClass ( (LPWNDCLASS)&LoadCursClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"Sample Application",
      (LPSTR)"LoadCursor Application",
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


/***************************************************************************/
long    FAR PASCAL LoadCursWndProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  switch (message)
    {
    case WM_DESTROY:
  /* Remove the cursor bitmap from memory or decrement the
           * reference count if it is greater than 1.
           */
      FreeResource (GetClassWord (hWnd, GCW_HCURSOR));
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
