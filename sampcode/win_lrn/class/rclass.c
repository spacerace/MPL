/*
 *  RegisterClass
 *  This function demonstrates the use of the RegisterClass function.  It will
 *  register a window Class, and then display that window.
 *
 */

#include <windows.h>


long	FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE	  hInstance, hPrevInstance;
LPSTR	  lpszCmdLine;
int	nCmdShow;
{
  WNDCLASS  wcClass;
  MSG	    msg;
  HWND	    hWnd;

  if (!hPrevInstance)
  {
    wcClass.lpszClassName = "WndClass";
    wcClass.hInstance	  = hInstance;
    wcClass.lpfnWndProc   = WndProc;
    wcClass.hCursor	  = LoadCursor (NULL, IDC_ARROW);
    wcClass.hIcon	  = NULL;
    wcClass.lpszMenuName  = NULL;
    wcClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wcClass.style	  = CS_HREDRAW | CS_VREDRAW;
    wcClass.cbClsExtra	  = 0;
    wcClass.cbWndExtra	  = 0;

    if (!RegisterClass (&wcClass))
      return FALSE;
  }

  hWnd = CreateWindow("WndClass",
      "RegisterClass",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance,
      NULL);

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}


long	FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	    hWnd;
unsigned	message;
WORD	    wParam;
LONG	    lParam;
{
  PAINTSTRUCT   ps;
  HDC           hDC;
  HWND		hButton;
  HANDLE	hInst;
  short	xChar,
  yChar;
  TEXTMETRIC	tm;
  WNDCLASS	wndclass;
  BOOL		bRegClass;

  hInst = GetWindowWord(hWnd, GWW_HINSTANCE);
  switch (message)
  {
  case WM_CREATE:
    {
      hDC = GetDC(hWnd);
      GetTextMetrics(hDC, &tm);
      xChar = tm.tmAveCharWidth;
      yChar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC(hWnd, hDC);
      hButton = CreateWindow("button",
          "Register Window Class",
          WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
          xChar * 20,
          yChar * 7,
          xChar * 23,
          yChar * 3,
          hWnd,
          0,
          hInst,
          NULL);
      break;
    }
  case WM_COMMAND:
    {
      if (wParam == 0)
      {
	MessageBox(hWnd,
	    "Registering window\nclass FooClass:",
	    "RegisterClass()",
	    MB_OK);
	wndclass.lpszClassName = "FooClass";
	wndclass.hInstance     = hInst;
	wndclass.lpfnWndProc   = DefWindowProc;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hIcon	       = NULL;
	wndclass.lpszMenuName  = NULL;
	wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
	wndclass.style	       = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;

	bRegClass = RegisterClass(&wndclass);
	if (bRegClass)
	  MessageBox(hWnd,
	      "Window class FooClass\nhas been registered.\nAll re-registration\nattempts should fail.",
	      "RegisterClass()",
	      MB_ICONASTERISK);
	else
	  MessageBox(hWnd,
	      "Registration failed.\nThe class is already\nregistered.",
	      "RegisterClass()",
	      MB_ICONEXCLAMATION);
      }
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    return(DefWindowProc(hWnd, message, wParam, lParam));
  }
  return(0L);
}


