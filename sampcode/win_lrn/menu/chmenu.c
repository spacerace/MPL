/*
 *  Function Name:   ChangeMenu
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   The program below will add new items to a menu.
 */

#include "windows.h"

long    FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_ChangeMenu(hWnd)
HWND hWnd;
  {
  HMENU     hSubItem;
  HMENU     hCurrentWindowMenu;
  unsigned  wIDNewItem1, wIDNewItem2;    /* assign items numbers for ID */
  BOOL      bChanged1;

  hCurrentWindowMenu = CreateMenu();
  hSubItem           = CreateMenu();
    /* menu heading created. */
  bChanged1 = ChangeMenu (hCurrentWindowMenu, NULL, (LPSTR) "Heading",
      hSubItem, MF_APPEND | MF_BYPOSITION | MF_POPUP);

  if (bChanged1 == FALSE)
    MessageBox(hWnd, (LPSTR)"ChangeMenu failed", (LPSTR)"ERROR", MB_ICONHAND);

   /* 2 items added under Heading */
  ChangeMenu (hSubItem, NULL, (LPSTR) "Item1", wIDNewItem1,
              MF_APPEND | MF_BYCOMMAND | MF_STRING);
  ChangeMenu (hSubItem, NULL, (LPSTR) "Item2", wIDNewItem2,
              MF_APPEND | MF_BYCOMMAND | MF_STRING);
  SetMenu(hWnd, hCurrentWindowMenu);

  return;
  }

/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit(hInstance)
HANDLE hInstance;
  {
  WNDCLASS   wcClass;

  wcClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcClass.lpfnWndProc    = WndProc;
  wcClass.cbClsExtra     = 0;
  wcClass.cbWndExtra     = 0;
  wcClass.hInstance      = hInstance;
  wcClass.hIcon          = LoadIcon(hInstance, NULL);
  wcClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wcClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcClass.lpszMenuName   = (LPSTR)NULL;
  wcClass.lpszClassName  = (LPSTR)"ChangeMenu";

  if (!RegisterClass((LPWNDCLASS) & wcClass))
    return FALSE;

  return TRUE;
  }

int     PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR   lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  if (!hPrevInstance)
    {
    if (!WinInit(hInstance))
      return FALSE;
    }

  hWnd = CreateWindow((LPSTR)"ChangeMenu",
                     (LPSTR)"ChangeMenu()",
                     WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     (HWND)NULL,        /* no parent */
                     (HMENU)NULL,       /* use class menu */
                     (HANDLE)hInstance, /* handle to window instance */
                     (LPSTR)NULL );     /* no params to pass on */

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
    }
  return (int)msg.wParam;
  }

        /* Procedures which make up the window class. */
long    FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  switch (message)
    {
    case WM_CREATE:
      CALL_ChangeMenu (hWnd); /* Change the menu when the window is created */
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }
  return(0L);
  }
