/*
 *   This program demonstrates the use of the CheckMenuItem() function.
 *   CheckMenuItem() checks or unchecks the given menu item. CheckMenuItem()
 *   is called in response to a WM_COMMAND message in HelloWndProc() in
 *   this sample application.
 */

#include "windows.h"
#include "ckmnuitm.h"

WORD   mOldChoice = NULL;   /* the last chosen menu item */
HMENU  hMenu;		    /* handle to menu of window  */

long    FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit(hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

  pHelloClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon(hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)"menucnt";
  pHelloClass->lpszClassName  = (LPSTR)"CheckMenuItem";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass((LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree((HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }


int     PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE   hInstance, hPrevInstance;
LPSTR    lpszCmdLine;
int      cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  HelloInit(hInstance);
  hWnd = CreateWindow((LPSTR)"CheckMenuItem",
                      (LPSTR)"CheckMenuItem()",
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

  hMenu = GetMenu(hWnd);     /* get the handle to the menu */

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
    }
  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL HelloWndProc(hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_COMMAND:
      switch( wParam )
        {
        case IDM_CHOICE1:
        case IDM_CHOICE2:
        case IDM_CHOICE3:
          if (mOldChoice)  /* if another menu item previously chosen */
            CheckMenuItem(hMenu, mOldChoice, MF_UNCHECKED);   /* uncheck it */
          mOldChoice = wParam;  /* keep track of this choice for later */
          CheckMenuItem(hMenu, mOldChoice, MF_CHECKED);   /* check the new choice */
          break;
        }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }
  return(0L);
  }
