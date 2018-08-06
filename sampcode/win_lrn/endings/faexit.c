/*
 *  Function demonstrated in this program: FatalExit
 *  Compiler version: C 5.1
 *
 *  Description:
 *   This function is typically used for debugging and cannot be
 *   demonstrated in this program.  Only a message box will display
 *   in this program.
 */

#include "windows.h"


/* PROTOTYPES */

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
void CALL_FatalExit(HWND hWnd, HDC hDC);
BOOL WinInit(HANDLE hInstance);

/***********************************************************************/

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;
  HDC   hDC;

  if (!hPrevInstance)
  {
/* Call initialization procedure if this is the first instance */
    if (!WinInit( hInstance ))
      return FALSE;
  }

  hWnd = CreateWindow((LPSTR)"FatalExit",
      (LPSTR)"FatalExit()",
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
  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  GetDC(hWnd);
  CALL_FatalExit(hWnd, hDC);
  ReleaseDC(hWnd, hDC);

/* Polling messages from event queue */
  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  return (int)msg.wParam;
}


/***********************************************************************/

/* Procedures which make up the window class. */
long	FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  switch (message)
  {
  case WM_DESTROY:
    PostQuitMessage( 0 );
    break;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
    break;
  }
  return(0L);
}


/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit(hInstance)
HANDLE hInstance;
{
  WNDCLASS   wcClass;

  wcClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcClass.lpfnWndProc    = WndProc;
  wcClass.cbClsExtra     = 0;
  wcClass.cbWndExtra     = 0;
  wcClass.hInstance      = hInstance;
  wcClass.hIcon          = LoadIcon(hInstance, NULL);
  wcClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wcClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcClass.lpszMenuName   = (LPSTR)NULL;
  wcClass.lpszClassName  = (LPSTR)"FatalExit";

  if (!RegisterClass((LPWNDCLASS) & wcClass))
    return FALSE;

  return TRUE;        /* Initialization succeeded */
}


/***********************************************************************/

void CALL_FatalExit(hWnd, hDC)
HWND hWnd;
HDC hDC;
{

/*  FatalExit(0);  */        /*      FatalExit function  */

  MessageBox(hWnd, (LPSTR)"FatalExit cannot be demonstrated \
- debugging Windows not installed.",
      (LPSTR)"FatalExit", MB_OK);
  return;
}


