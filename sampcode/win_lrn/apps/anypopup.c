/*
 *   This program demonstrates the use of the AnyPopup () function.
 *   AnyPopup () determines if a popup window is present on the screen, even
 *   if hidden. AnyPop () is called in response to choosing the "AnyPopup?"
 *   menu option. To create a popup, choose the "Create Popup" menu option.
 *   This function can also be tested by bringing up another application
 *   which creates a popup window.
 */

#include "windows.h"
#include "anypopup.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);
long    FAR PASCAL PopupProc (HWND, unsigned, WORD, LONG);

HWND hPopup = NULL;   /* handle to the popup window */
HANDLE hInst;

BOOL HelloInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName      = (LPSTR)"anypopup";
  pHelloClass->lpszClassName     = (LPSTR)"AnyPopup";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName  = (LPSTR)"CHILD A";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc	 = PopupProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree ( (HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  HelloInit (hInstance);

  hInst = hInstance;

  hWnd = CreateWindow ( (LPSTR)"AnyPopup", "AnyPopup ()",
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
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
        {
        case IDM_ANYPOPUP:
                /* are there any  popup windows present in the system ? */
          if (AnyPopup ())  /* if yes, say so */
            MessageBox (NULL, (LPSTR)"there ARE popup windows present",
                       (LPSTR)"AnyPopup () says...", MB_OK);
          else
            MessageBox (NULL, (LPSTR)"there are NOT popup windows present",
                       (LPSTR)"AnyPopup () says...", MB_OK);
          break;

        case IDM_CREATEPOPUP:     /*  Create a popup Window  */
          hPopup = CreateWindow ( (LPSTR)"CHILD A",
                                  (LPSTR)"Child A (WS_POPUP)", WS_POPUP |
                                  WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS |
                                  WS_BORDER | WS_SYSMENU, 45, 60, 250, 200,
                                  hWnd, NULL, hInst, NULL);
          break;
        }
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

long    FAR PASCAL PopupProc (hChildAWnd, message, wParam, lParam)
HWND      hChildAWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  switch (message)
    {
    default:
      return DefWindowProc (hChildAWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
