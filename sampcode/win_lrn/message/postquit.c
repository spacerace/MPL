/* PostQuitMessage
 *
 *  When Execute is selected, a PostQuitMessage is generated and
 *  closes down this application.
 *
 */

#include <windows.h>
#include "postquit.h"

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

static char	szResName [] = "ResMenu";

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance ;
LPSTR       lpszCmdLine ;
int	nCmdShow ;
{
  static char	szAppName [] = "PostQuitMessage";
  HWND        hWnd ;
  MSG msg;
  WNDCLASS  wcPostQuitMessageClass;

  wcPostQuitMessageClass.lpszClassName = szAppName;
  wcPostQuitMessageClass.hInstance     = hInstance;
  wcPostQuitMessageClass.lpfnWndProc   = WndProc;
  wcPostQuitMessageClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcPostQuitMessageClass.hIcon         = NULL;
  wcPostQuitMessageClass.lpszMenuName  = (LPSTR) NULL;
  wcPostQuitMessageClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcPostQuitMessageClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcPostQuitMessageClass.cbClsExtra    = 0;
  wcPostQuitMessageClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) & wcPostQuitMessageClass);

  hWnd = CreateWindow(szAppName, (LPSTR) "PostQuitMessage",
      WS_OVERLAPPEDWINDOW,  
      CW_USEDEFAULT,  0,    
      CW_USEDEFAULT,  0,    
      NULL, NULL,  hInstance, NULL);

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (msg.wParam) ;
}


long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned	iMessage ;
WORD     wParam ;
LONG     lParam ;
{
  static HANDLE hInstance;
  HMENU hMenu;
  switch (iMessage)
  {
  case WM_CREATE:
    {
      hInstance = GetWindowWord(hWnd, GWW_HINSTANCE);
      hMenu = LoadMenu(hInstance, "ResMenu");
      SetMenu(hWnd, hMenu);
      DrawMenuBar(hWnd);
      break;
    }
  case WM_COMMAND:
    {
      switch (wParam)
      {
      case IDM_EXECUTE:
	{
	  PostQuitMessage(0);
	}
      }
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    {
      return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
    }
  }
  return (0L);
}


