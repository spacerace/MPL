/*
 *   This program demonstrates the use of the CreateBrushIndirect () function.
 *   CreateBrushIndirect () returns a handle to a brush with characteristics
 *   described by a LOGBRUSH structure. CreateBrushIndirect () is called in
 *   response to a WM_CREATE message in HelloWndProc (). The brush is used
 *   to fill a rectangle in response to a WM_PAINT message.
 *
 */

#include    <windows.h>
HBRUSH      hMyBrush;
LOGBRUSH    lbBrushStyle;
RECT        MyRect;

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon             = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName     = (LPSTR)"CreateBrushIndirect";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

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
  hWnd = CreateWindow ( (LPSTR)"CreateBrushIndirect",
                      (LPSTR)"CreateBrushIndirect ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL );       /* no params to pass on */

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

  switch (message)
    {
    case WM_CREATE:
          /*  initialize the LOGBRUSH structure "lbBrushStyle  */
      lbBrushStyle.lbStyle = BS_HATCHED;
      lbBrushStyle.lbColor = RGB (0x00, 0x00, 0x00);
      lbBrushStyle.lbHatch = HS_BDIAGONAL;
          /* initialize the rectangle structure "MyRect" */
      MyRect.left   =  17;
      MyRect.top    =  17;
      MyRect.right  = 300;
      MyRect.bottom = 100;
          /*** create a hatched brush described by lbBrushStyle ***/
      hMyBrush = CreateBrushIndirect ( (LPLOGBRUSH) & lbBrushStyle);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      TextOut (ps.hdc, 5, 5,
              (LPSTR)"This rectangle filled with hatched brush created via CreateBrushIndirect ()",
              (long)74);
      FillRect (ps.hdc, (LPRECT) & MyRect, hMyBrush);
      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
