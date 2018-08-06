/*
 *   UpdateWindow
 *   updtwin.c
 *
 *   This program demonstrates the use of the function UpdateWindow.
 *   UpdateWindow sends a WM_PAINT message directly to the paint procedure
 *   of the given window if the update region for the window is not empty.
 *   The paint procedure invokes the TextOut function to let the operator
 *   know that the WM_PAINT message is being processed.  A message box is
 *   then shown to let the operator know that the call to UpdateWindow is
 *   complete.
 *
 */

#include "windows.h"
#include "string.h"

BOOL PASCAL SampleInit(HANDLE , HANDLE, int);
BOOL PASCAL SamplePaint(HANDLE);
long	FAR PASCAL SampleWndProc(HWND, unsigned, WORD, LONG);

static HANDLE  hInst;  /* Can be used througout the program but not normally
                        * passed to other intances */

int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, nCmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	nCmdShow;
{
  HWND  hWnd;
  MSG   msg;
  HWND hWindow;    /* For use in the UpdateWindow demonstration section */

  if (!hPrevInstance)
    SampleInit(hInstance, hPrevInstance, nCmdShow);

  hInst = hInstance;                      /* So window procedures can use */
  hWnd = CreateWindow((LPSTR)"UpdateWindow",
      (LPSTR)"UpdateWindow",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,        /* no parent */
  (HMENU)NULL,       /* use class menu */
  (HANDLE)hInstance, /* handle to window instance */
  (LPSTR)NULL        /* no params to pass on */
  );

/* Make window visible according to the way the app is activated */
  ShowWindow(hWnd, nCmdShow);
  hWindow = GetActiveWindow();
  UpdateWindow( hWindow ); /* Send a WM_PAINT message directly to the
                              * window procedure, bypassing the application
                                * queue.   */
  MessageBox(hWindow,
      (LPSTR)"UpdateWindow and processed a WM_PAINT message.",
      (LPSTR)"I called ...",
      MB_OK | MB_SYSTEMMODAL );

/* Polling messages from event queue */
  while (GetMessage((LPMSG) & msg, NULL, 0, 0)) 
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }
  return (int)msg.wParam;
}


/* Procedure called when the application is loaded for the first time */
BOOL PASCAL SampleInit( hInstance, hPrevInstance, nCmdShow )
HANDLE hInstance, hPrevInstance;
int	nCmdShow;
{
  PWNDCLASS   pSampleClass;

  pSampleClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

  pSampleClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
  pSampleClass->hIcon        = LoadIcon( hInstance, NULL);
  pSampleClass->lpszMenuName   = (LPSTR)NULL;
  pSampleClass->lpszClassName  = (LPSTR)"UpdateWindow";
  pSampleClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  pSampleClass->hInstance      = hInstance;
  pSampleClass->style          = CS_HREDRAW | CS_VREDRAW;
  pSampleClass->lpfnWndProc    = SampleWndProc;

  if (!RegisterClass((LPWNDCLASS)pSampleClass))
/* Initialization failed.
       * Windows will automatically deallocate all allocated memory.
       */
    return FALSE;

  LocalFree((HANDLE)pSampleClass);
  return TRUE;
}


/* Every message for this window will be delevered right here.         */
long	FAR PASCAL SampleWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  switch (message)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_PAINT:
    SamplePaint(hWnd);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
  }
  return(0L);
}


BOOL PASCAL SamplePaint(hWnd)
HWND hWnd;
{
  PAINTSTRUCT ps;
  HDC         hDC;

  BeginPaint( hWnd, (LPPAINTSTRUCT) & ps );
  hDC = ps.hdc;

  TextOut( hDC, 10, 10,
      (LPSTR)"The TextOut function is called in the paint procedure",
      strlen("The TextOut function is called in the paint procedure"));

  EndPaint( hWnd, (LPPAINTSTRUCT) & ps );

  return TRUE;
}


