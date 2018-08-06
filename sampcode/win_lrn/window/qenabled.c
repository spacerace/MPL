/*
 *
 *   qenabled.c
 *
 *   This program demonstrates the use of the IsWindowEnabled function.
 *   IsWindowEnabled checks to see if the given window is enabled to receive
 *   input. Each window in this application, if enabled, should display a
 *   message box in response to a left button click. IsWindowEnabled is called
 *   from WinMain in this sample application.
 */

#include "windows.h"

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
long	FAR PASCAL ChildAProc(HWND, unsigned, WORD, LONG);
long	FAR PASCAL ChildBProc(HWND, unsigned, WORD, LONG);

HWND hChAWnd = NULL;
HWND hChBWnd = NULL;


int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	 hInstance, hPrevInstance;
LPSTR	 lpszCmdLine;
int	cmdShow;
{
  MSG	    msg;
  HWND	    hWnd;
  HMENU     hMenu;
  BOOL	    bEnabled;
  WNDCLASS  wndclass;

  if (!hPrevInstance)
  {
    wndclass.style		 = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc	 = WndProc;
    wndclass.cbClsExtra	 = 0;
    wndclass.cbWndExtra	 = 0;
    wndclass.hInstance	 = hInstance;
    wndclass.hIcon		 = NULL;
    wndclass.hCursor	 = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground	 = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName	 = NULL;
    wndclass.lpszClassName	 = "Sample Application";

    if (!RegisterClass(&wndclass))
      return FALSE;

    wndclass.style		 = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc	 = ChildAProc;
    wndclass.cbClsExtra	 = 0;
    wndclass.cbWndExtra	 = 0;
    wndclass.hInstance	 = hInstance;
    wndclass.hIcon		 = NULL;
    wndclass.hCursor	 = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground	 = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName	 = NULL;
    wndclass.lpszClassName	 = "CHILD A";

    if (!RegisterClass(&wndclass))
      return FALSE;

    wndclass.style		 = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc	 = ChildBProc;
    wndclass.cbClsExtra	 = 0;
    wndclass.cbWndExtra	 = 0;
    wndclass.hInstance	 = hInstance;
    wndclass.hIcon		 = NULL;
    wndclass.hCursor	 = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground	 = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName	 = NULL;
    wndclass.lpszClassName	 = "CHILD B";

    if (!RegisterClass(&wndclass))
      return FALSE;
  }

  hWnd = CreateWindow("Sample Application",
      "IsWindowEnabled",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance,
      NULL);
  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  hChAWnd = CreateWindow("CHILD A",
      "Child A",
      WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
      5,
      5,
      150,
      150,
      hWnd,
      (HMENU)1,
      hInstance,
      NULL);

  hChBWnd = CreateWindow("CHILD B",
      "Child B",
      WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
      270,
      5,
      150,
      150,
      hWnd,
      (HMENU)2,
      hInstance,
      NULL);

  MessageBox(hWnd,
      "if Child A is enabled",
      "I am going to see...",
      MB_OK);
  if (IsWindowEnabled(hChAWnd))
    MessageBox(hWnd,
        "Child A IS enabled",
        "'IsWindowEnabled' says...",
        MB_OK);
  else
    MessageBox(hWnd,
        "Child A is NOT enabled",
        "'IsWindowEnabled' says...",
        MB_OK);
  MessageBox(hWnd,
      "disable Child B",
      "I am going to...",
      MB_OK);
  EnableWindow(hChBWnd, FALSE);
  MessageBox(hWnd,
      "if Child B is enabled",
      "I am going to see...",
      MB_OK);
  if (IsWindowEnabled(hChBWnd))
    MessageBox(hWnd,
        "Child B IS enabled",
        "'IsWindowEnabled' says...",
        MB_OK);
  else
    MessageBox(hWnd,
        "Child B is NOT enabled",
        "'IsWindowEnabled' says...",
        MB_OK);

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return msg.wParam;
}


long	FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	  hWnd;
unsigned	message;
WORD	  wParam;
LONG	  lParam;
{
  switch (message)
  {
  case WM_LBUTTONDOWN:
    {
      MessageBox(hWnd,
          "Left Button Click",
          "PARENT WINDOW",
          MB_OK);
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return(0L);
}


long	FAR PASCAL ChildAProc(hChAWnd, message, wParam, lParam)
HWND hChAWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  PAINTSTRUCT ps;

  switch (message)
  {

  case WM_LBUTTONDOWN:
    MessageBox(hChAWnd, (LPSTR)"Left Button Click",
        (LPSTR)"CHILD A",
        MB_OK);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_PAINT:
    BeginPaint(hChAWnd, (LPPAINTSTRUCT) & ps);
    EndPaint(hChAWnd, (LPPAINTSTRUCT) & ps);
    break;

  default:
    return DefWindowProc(hChAWnd, message, wParam, lParam);
    break;
  }
  return(0L);
}


long	FAR PASCAL ChildBProc(hChBWnd, message, wParam, lParam)
HWND hChBWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  PAINTSTRUCT ps;

  switch (message)
  {

  case WM_LBUTTONDOWN:
    MessageBox(hChBWnd, (LPSTR)"Left Button Click",
        (LPSTR)"CHILD B",
        MB_OK);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_PAINT:
    BeginPaint(hChBWnd, (LPPAINTSTRUCT) & ps);
    EndPaint(hChBWnd, (LPPAINTSTRUCT) & ps);
    break;

  default:
    return DefWindowProc(hChBWnd, message, wParam, lParam);
    break;
  }
  return(0L);
}


