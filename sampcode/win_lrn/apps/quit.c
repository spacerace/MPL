/*
 * 
 *  QUIT - Demonstrates PostAppMessage and GetWindowTask
 *
 *  This program will get the Task handle to the current Window, and send
 *  a WM_QUIT message to the window which will cause it to go away.
 */

#include <windows.h>
#define IDM_QUIT    100

HANDLE hInst;

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
long FAR PASCAL QuitWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE	 hInstance;
HANDLE	 hPrevInstance;
LPSTR	 lpCmdLine;
int	 nCmdShow;
  {
  HWND	    hWnd;
  MSG	    msg;
  HANDLE    hMenu;
  WNDCLASS  wndclass;
  if (!hPrevInstance)
    {
    wndclass.style	    = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = QuitWndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance	    = hInstance;
    wndclass.hIcon	    = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor	    = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName   = "QuitMenu";
    wndclass.lpszClassName  = "Quit";

    if (!RegisterClass(&wndclass))
      return FALSE;
    }
  hInst = hInstance;               /* Saves the current instance        */

  hMenu = LoadMenu(hInst, "QuitMenu");

  hWnd = CreateWindow("Quit",
		      "Quit Sample Application",
		      WS_OVERLAPPEDWINDOW,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      NULL,
		      hMenu,
		      hInstance,
		      NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }
  return msg.wParam;
  }


long FAR PASCAL QuitWndProc(hWnd, message, wParam, lParam)
HWND	  hWnd;
unsigned  message;
WORD	  wParam;
LONG	  lParam;
  {
  HANDLE  hTask;

  switch (message) 
    {
    case WM_COMMAND:
      if (wParam == IDM_QUIT)
	{
	hTask = GetWindowTask(hWnd);
	PostAppMessage(hTask, WM_QUIT, 0, 0L);
	}
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return (DefWindowProc(hWnd, message, wParam, lParam));
    }
  return (NULL);
  }
