/*
 *   IsZoomed
 *
 *   This program demonstrates the use of the IsZoomed function. The IsZoomed
 *   function returns a boolean telling whether or not the given window is
 *   zoomed (maximized). Choosing the "Zoomed?" option in the system menu
 *   enacts the call to IsZoomed in this sample application.
 */

#include "windows.h"
#define IDSABOUT 200

char	szAbout[10];

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon             = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName     = (LPSTR)"Sample Application";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree ( (HANDLE)pHelloClass);

  hWnd = CreateWindow ( (LPSTR)"Sample Application", (LPSTR)"Sample Application",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0,
                      NULL, NULL, hInstance, NULL);

/* Insert "Zoomed?" into system menu */
  hMenu = GetSystemMenu (hWnd, FALSE);
  ChangeMenu (hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
  ChangeMenu (hMenu, 0, (LPSTR)"Zoomed?", IDSABOUT, MF_APPEND | MF_STRING);

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
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
  {
  PAINTSTRUCT ps;
  BOOL Zoomed;

  switch (message)
    {
    case WM_SYSCOMMAND:
      switch (wParam)
        {
        case IDSABOUT:
/***********************************************************************/
          Zoomed = IsZoomed (hWnd);
          if (Zoomed)
            MessageBox (hWnd, (LPSTR)"This window is zoomed",
                       (LPSTR)"ZOOM INFO", MB_OK);
          else
            MessageBox (hWnd, (LPSTR)"This window is NOT zoomed",
                       (LPSTR)"ZOOM INFO", MB_OK);
/************************************************************************/
        break;

        default:
          return DefWindowProc (hWnd, message, wParam, lParam);
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
