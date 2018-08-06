/*
 *   LoadAccelerators
 *
 *   This program demonstrates the use of the LoadAccelerators function.
 *   The LoadAccelerators function loads an accelerator table. In this
 *   program, LoadAccelerators in called in LoadAccWindProc. The two
 *   accelerators that are loaded are ^A and ^B, which are accelerators
 *   for the menu choices "CHOICE1" and "CHOICE2."
 */

#include <windows.h>
#include "loadacc.h"

long    FAR PASCAL LoadAccWndProc (HWND, unsigned, WORD, LONG);

static HANDLE hAccelTable;		    /*	handle to accelerator table  */

/**************************************************************************/

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
    WNDCLASS   LoadAccClass;

    LoadAccClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    LoadAccClass.hIcon          = LoadIcon (hInstance, NULL);
  /*---------------------- important for adding menu ------------------------*/
    LoadAccClass.lpszMenuName   = (LPSTR)"example";
  /*-------------------------------------------------------------------------*/
    LoadAccClass.lpszClassName  = (LPSTR)"Sample Application";
    LoadAccClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    LoadAccClass.hInstance      = hInstance;
    LoadAccClass.style          = CS_HREDRAW | CS_VREDRAW;
    LoadAccClass.lpfnWndProc    = LoadAccWndProc;

    if (!RegisterClass ( (LPWNDCLASS)&LoadAccClass))
      return FALSE;
    }

  hWnd = CreateWindow ( (LPSTR)"Sample Application",
      (LPSTR)"Sample Application",
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
/* if translation of accelerator succesful */
    if (TranslateAccelerator (msg.hwnd, hAccelTable, (LPMSG) & msg) == 0)
      {
      TranslateMessage ( (LPMSG) & msg);
      DispatchMessage ( (LPMSG) & msg);
      }
    }
  return (int)msg.wParam;
  }

/* Procedures which make up the window class. */
long    FAR PASCAL LoadAccWndProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  PAINTSTRUCT    ps;
  TEXTMETRIC     tm;
  static short   Xchar, Ychar;
  HDC            hDC;
  HANDLE         hInstance;

  switch (message)
    {
    case WM_CREATE:
      hDC = GetDC (hWnd);
      GetTextMetrics (hDC, &tm);
      Xchar = tm.tmAveCharWidth;
      Ychar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC (hWnd, hDC);
      hInstance = GetWindowWord (hWnd, GWW_HINSTANCE);
  /**** Load the accelerator table ***/
      hAccelTable = LoadAccelerators (hInstance, (LPSTR)"example");
      if (!hAccelTable)      /* if accelerator table not properly loaded */
        return FALSE;
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case CHOICE1:   /** can be chosen with accelerator ^A **/
          MessageBox (hWnd, (LPSTR)"CHOICE1 has been chosen",
              (LPSTR)"MENU INFO", MB_OK);
          break;

        case CHOICE2:   /** can be chosen with accelerator ^B **/
          MessageBox (hWnd, (LPSTR)"CHOICE2 has been chosen",
              (LPSTR)"MENU INFO", MB_OK);
          break;

        default:
          break;
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      TextOut (ps.hdc, Xchar, Ychar,
          (LPSTR)"To test accelerators press ^A or ^B", 35);
      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
