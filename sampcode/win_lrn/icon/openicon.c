/*
 * Function (s) demonstrated in this program: OpenIcon
 *
 * Description:
 * This sample program demonstrates how to use the OpenIcon Windows
 * function.  Two windows are created.  The first window is initially
 * displayed as an open window.  The second window is initially
 * displayed as an iconized window.  After both windows are created and
 * displayed then a message box prompts the user to press return.  Upon
 * receiving <RETURN>, The message box closes.  This is followed by a
 * call to the function OpenIcon which opens the iconized window.
 *
 */

#include <windows.h>
#include <string.h>
#include "openicon.h"

long    FAR PASCAL WndProc1 (HWND, unsigned, WORD, LONG);
long    FAR PASCAL WndProc2 (HWND, unsigned, WORD, LONG);

static char     szAppName1 [] = "OpenIcon, window number 1";
static char     szAppName2 [] = "OpenIcon, window number 2";
static char	szResName [] = "ResMenu";
static HWND hWnd1, hWnd2;

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int	nCmdShow;
  {
  WNDCLASS wndclass;
  MSG      msg;
  HDC      hDC;
  HMENU    hMenu;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc1;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName1;

    if (!RegisterClass (&wndclass))
      return FALSE;

    wndclass.lpfnWndProc   = WndProc2;
    wndclass.lpszClassName = szAppName2;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hMenu = LoadMenu (hInstance, "ResMenu");

  hWnd1 = CreateWindow (szAppName1, szAppName1,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, 0,
                       CW_USEDEFAULT, 0,
                       NULL, hMenu, hInstance, NULL);

  ShowWindow (hWnd1, nCmdShow);
  UpdateWindow (hWnd1);

  hWnd2 = CreateWindow (szAppName2, szAppName2,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, 0,
                       CW_USEDEFAULT, 0,
                       NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd2, SW_MINIMIZE);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL WndProc1 (hWnd1, iMessage, wParam, lParam)
HWND     hWnd1;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == IDM_EXECUTE)
        OpenIcon (hWnd2);                /* Open the iconized window */
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd1, iMessage, wParam, lParam);
    }
  return (0L);
  }

long    FAR PASCAL WndProc2 (hWnd2, iMessage, wParam, lParam)
HWND     hWnd2;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC hDC;
  PAINTSTRUCT ps;

  switch (iMessage)
    {
    case WM_PAINT:
      hDC = BeginPaint (hWnd2, &ps);
      TextOut (hDC, 30, 30,
          (LPSTR)"This window was opened by the OpenIcon function", 47);
      EndPaint (hWnd2, &ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd2, iMessage, wParam, lParam);
    }
  return (0L);
  }
