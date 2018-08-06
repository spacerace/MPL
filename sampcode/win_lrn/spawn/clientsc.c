/*
 *   This program demonstrates the use of the ClientToScreen () function.
 *   ClientToScreen () converts client area coordinates to screen coordinates
 *   (upper left of the screen being the origin). ClientToScreen () is called
 *   from ParentWndProc () in response to a WM_SIZE or WM_MOVE message.
 *
 */

#include <windows.h>

int     PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
long    FAR PASCAL ParentWndProc (HWND, unsigned, WORD, LONG);
long    FAR PASCAL PopupWndProc (HWND, unsigned, WORD, LONG);
int     sprintf (char *, const char *, ...);

HWND    hChild;
char    szBuff[80] = " ";   /*          used for output           */
short   nTextLength = 0;    /*   length of string for TextOut ()  */


int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG      msg;
  HWND     hParent;
  WNDCLASS wndClass;

  if (!hPrevInstance)
    {
    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = ParentWndProc;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hInstance      = hInstance;
    wndClass.hIcon          = LoadIcon (NULL, NULL);
    wndClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wndClass.hbrBackground  = GetStockObject (WHITE_BRUSH);
    wndClass.lpszMenuName   = NULL;
    wndClass.lpszClassName  = "ClientToScreenParent";

    if (!RegisterClass (&wndClass))
      return FALSE;

    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = PopupWndProc;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hInstance      = hInstance;
    wndClass.hIcon          = NULL;
    wndClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wndClass.hbrBackground  = GetStockObject (WHITE_BRUSH);
    wndClass.lpszMenuName   = NULL;
    wndClass.lpszClassName  = "ClientToScreenPopup";

    if (!RegisterClass (&wndClass))
      return FALSE;
    }

  hParent = CreateWindow ( (LPSTR)"ClientToScreenParent",
                         (LPSTR)"Parent Window for ClientToScreen () Demo",
                         WS_OVERLAPPEDWINDOW,
                         32,
                         200,
                         500,
                         48,
                         NULL,      /* no parent */
                         NULL,      /* use class menu */
                         hInstance, /* handle to window instance */
                         NULL);     /* no params to pass on */

  ShowWindow (hParent, cmdShow);
  UpdateWindow (hParent);

  hChild = CreateWindow ( (LPSTR)"ClientToScreenPopup",
                        (LPSTR)"Popup Window for ClientToScreen () Demo",
                        WS_POPUP | WS_CAPTION | WS_VISIBLE,
                        32,
                        256,
                        500,
                        48,
                        hParent,
                        NULL,      /* use class menu */
                        hInstance, /* handle to window instance */
                        NULL);     /* no params to pass on */

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }

  return (int)msg.wParam;
  }

long    FAR PASCAL ParentWndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  POINT myPoint;       /* point structure used in calls to ClientToScreen () */

  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    case WM_MOVE:
    case WM_SIZE:
      DefWindowProc (hWnd, message, wParam, lParam);
      myPoint.x = 0;
      myPoint.y = 0;
      ClientToScreen (hWnd, (LPPOINT) & myPoint);
      nTextLength = sprintf (szBuff,
          "Screen Coordinates of Upper Left-hand Corner: (%d, %d)",
          myPoint.x,
          myPoint.y);
      InvalidateRect (hChild, NULL, TRUE);
      UpdateWindow (hChild);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    break;
    }
  return (0L);
  }

long    FAR PASCAL PopupWndProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, &ps);
      TextOut (ps.hdc, 5, 5, szBuff, nTextLength);
      EndPaint (hWnd, &ps);
      break;
    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
