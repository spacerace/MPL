/*
 *  PtVisible
 *
 *  This function demonstrates the use of the PtVisible function.  It will
 *  create a window using the CreateWindow function, and will display a
 *  message box telling the user if the point (200,200) is inside the client
 *  area of the window.
 *
 */

#include <windows.h>

BOOL FAR PASCAL InitPtVisible (HANDLE, HANDLE, int);
long	FAR PASCAL PtVisibleWindowProc (HANDLE, unsigned, WORD, LONG);
void FAR PASCAL CheckPoint (HWND);

int	PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)

HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	nCmdShow;
{
  MSG  msg;

  InitPtVisible (hInstance, hPrevInstance, nCmdShow);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  exit(msg.wParam);
}


BOOL FAR PASCAL InitPtVisible (hInstance, hPrevInstance, nCmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int	nCmdShow;

{
  WNDCLASS  wcPtVisibleClass;
  HWND hWnd;

  wcPtVisibleClass.lpszClassName = (LPSTR) "PtVisible";
  wcPtVisibleClass.hInstance  = hInstance;
  wcPtVisibleClass.lpfnWndProc  = PtVisibleWindowProc;
  wcPtVisibleClass.hCursor  = LoadCursor (NULL, IDC_ARROW);
  wcPtVisibleClass.hIcon  = NULL;
  wcPtVisibleClass.lpszMenuName  = (LPSTR) "PtVisibleMenu";
  wcPtVisibleClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcPtVisibleClass.style  = CS_HREDRAW | CS_VREDRAW;
  wcPtVisibleClass.cbClsExtra  = 0;
  wcPtVisibleClass.cbWndExtra  = 0;

  RegisterClass ((LPWNDCLASS) & wcPtVisibleClass);

  hWnd = CreateWindow((LPSTR) "PtVisible", (LPSTR) "PtVisible",
      WS_OVERLAPPEDWINDOW,  
      CW_USEDEFAULT,  0,
      CW_USEDEFAULT,  0,
      NULL,  NULL,  hInstance, NULL);

  LoadMenu(hInstance, (LPSTR) "PtVisibleMenu");

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  return TRUE;
}


long	FAR PASCAL PtVisibleWindowProc (hWnd, message, wParam, lParam)

HWND     hWnd;
unsigned	message;
WORD     wParam;
LONG     lParam;
{
  switch (message)
  {
  case WM_COMMAND:
    if (wParam == 1)
      CheckPoint(hWnd);
    break;

  case WM_PAINT:
    PaintPtVisibleWindow (hWnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return(DefWindowProc(hWnd, message, wParam, lParam));
    break;
  }
  return(0L);
}


PaintPtVisibleWindow (hWnd)

HWND hWnd;
{
  PAINTSTRUCT ps;
  HDC  hDC;

  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
  hDC = ps.hdc;

  SetPixel (hDC, 200, 200, RGB(0, 0, 0));
/*  Draw a black pixel  */

  ValidateRect (hWnd, (LPRECT) NULL);
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  return TRUE;
}


/*  CheckPoint(hWnd) -  Check to see if (200, 200) is visible  */

void FAR PASCAL CheckPoint (hWnd)

HWND     hWnd;
{
  HDC  hDC;

  hDC = GetDC (hWnd);
  if (PtVisible (hDC, 200, 200))
/*  Find out if the point 200, 200 is in the current display
     *  context
     */

    MessageBox(hWnd, (LPSTR) "(200, 200) is Visible",
        (LPSTR)"OK", MB_OK);
  else
    MessageBox(hWnd, (LPSTR) "(200, 200) is not Visible",
        (LPSTR)"OK", MB_OK);

  ReleaseDC (hWnd, hDC);
}


