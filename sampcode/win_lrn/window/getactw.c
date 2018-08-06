/*
 * GetActiveWindow
 *
 * This function obtains a handle to the active window.
 *
 */

#include <windows.h>
#include "getactw.h"

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
  HWND     hWnd;     /* Window handle.          */
  WNDCLASS wndclass; /* Window class structure. */
  MSG      msg;      /* Message structure.      */

  if (!hPrevInstance) 
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = ActiveWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = (LPSTR)"getactw";
    wndclass.lpszClassName = (LPSTR)"getactw";

    if (!RegisterClass(&wndclass))
      return FALSE;
    }

  hWnd = CreateWindow ((LPSTR)"getactw",
                       (LPSTR)"GetActiveWindow",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, 0,
                       CW_USEDEFAULT, 0,
                       NULL, NULL,
                       hInstance, NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    } 
  return (msg.wParam);
}

long FAR PASCAL ActiveWndProc(hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
{
  HWND hWndActive; /* Handle to the active window. */

  switch(iMessage)
    {
     case WM_COMMAND:
       switch (wParam)
         {
         int i; /* Loop Counter. */
         case IDM_GETACTIVEW:
           /* Get the active window and flash that handle. */
           hWndActive = GetActiveWindow();
           for (i = 0; i < 5; i++)
             {
             FlashWindow(hWndActive, FALSE);
             MessageBeep(NULL);
             }
           break;

         default:
           return DefWindowProc(hWnd, iMessage, wParam, lParam);
           break;
         }
      break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
    break;
  }
  return (0L); 
}

