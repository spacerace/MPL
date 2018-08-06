/*
 *   This program demonstrates the use of the Chord() function.
 *   Chord() draws a chord. Chord() is called in response to a WM_PAINT
 *   message in HelloWndProc();
 */

#include "windows.h"

long    FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit(hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

  pHelloClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
  pHelloClass->hIcon             = LoadIcon(hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName	 = (LPSTR)"Chord";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass((LPWNDCLASS)pHelloClass))
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return FALSE;

  LocalFree((HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }


int     PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE   hInstance, hPrevInstance;
LPSTR    lpszCmdLine;
int      cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  HelloInit(hInstance);
  hWnd = CreateWindow((LPSTR)"Chord",
                      (LPSTR)"Chord()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL );     /* no params to pass on */

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
    }

  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL HelloWndProc(hWnd, message, wParam, lParam)
HWND       hWnd;
unsigned   message;
WORD       wParam;
LONG       lParam;
  {
  PAINTSTRUCT ps;
  BOOL bDrawn;       /* was the chord drawn? */

  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT) & ps);
      bDrawn = Chord(ps.hdc, 5, 5, 100, 100, 0, 22, 150, 22);
                  /***** draw the chord *****/
      if (!bDrawn)
        MessageBox(NULL,
                   (LPSTR)"Chord failed!",
                   (LPSTR)"Chord info:",
                   MB_OK);          /* Tell User it didn't work */
      EndPaint(hWnd, (LPPAINTSTRUCT) & ps);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }
  return(0L);
  }
