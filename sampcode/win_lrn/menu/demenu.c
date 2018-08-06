/*  Function Name:   DestroyMenu
 *
 *  Description:
 *   The program below will create a menu.  When the menuitem is selected,
 *   that menu will be destroyed and a new menu will be created.
 *
 *   Windows Version 2.0 function demonstration application
 *
 */

#include "windows.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

HMENU hMenu;

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
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
    wcClass.lpszClassName  = (LPSTR)"DestroyMenu";

    if (!RegisterClass ( (LPWNDCLASS) & wcClass))
      return FALSE;
    }

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, "Add MenuItem", 100, MF_APPEND);

  hWnd = CreateWindow ( (LPSTR)"DestroyMenu",
                      (LPSTR)"DestroyMenu ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)hMenu,      /* use class menu */
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
  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case 100:
          SetMenu (hWnd, NULL);  /*  No Menu for the Window while we play  */
          DestroyMenu (hMenu);   /*  Trash the Menu  */
          hMenu = CreateMenu (); /*  Create a new one  */
          ChangeMenu (hMenu, NULL, "Delete Extra MenuItem  ", 105, MF_APPEND);
          ChangeMenu (hMenu, NULL, "  Extra MenuItem", 110, MF_APPEND);
          SetMenu (hWnd, hMenu);  /*  Change the menu for the window  */
          break;

        case 105:
          SetMenu (hWnd, NULL);
          DestroyMenu (hMenu);
          hMenu = CreateMenu ();
          ChangeMenu (hMenu, NULL, "Add MenuItem", 100, MF_APPEND);
          SetMenu (hWnd, hMenu);
          break;

        case 110:
          MessageBox (GetFocus(), "Extra MenuItem Was Selected",
                      "Extra MenuItem", MB_OK | MB_ICONEXCLAMATION);
          break;

        default:
          return DefWindowProc (hWnd, message, wParam, lParam);
        }
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
