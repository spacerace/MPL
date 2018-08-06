/*
 *   IsRectEmpty
 *
 *   This example application demonstrates the use of the IsRectEmpty
 *   function. IsRectEmpty returns a boolean specifying whether or not
 *   the given rectangle is empty. A rectangle is empty if either the
 *   height or width is 0. IsRectEmpty is called twice in WinMain in this
 *   sample application. In the first call, IsRectEmpty is passed a non-
 *   empty rectangle. In the second call, it is passed an empty rectangle.
 */

#include "windows.h"

long	FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

  pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
  pHelloClass->hIcon          = LoadIcon( hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName  = (LPSTR)"Sample Application";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
/* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return FALSE;

  LocalFree( (HANDLE)pHelloClass );
  return TRUE;        /* Initialization succeeded */
}


int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  HDC hDC;		   /** handle to display context	   **/
  RECT lpRect1;	   /** rectangle coordinates structure	   **/
  RECT lpRect2;	   /** rectangle coordinates structure	   **/
  BOOL EMPTY; 	   /** return value from IsRectEmpty calls **/

  HelloInit( hInstance );
  hWnd = CreateWindow((LPSTR)"Sample Application", (LPSTR)"Sample Application",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, 	NULL, hInstance, NULL);

/* Make window visible according to the way the app is activated */
  ShowWindow( hWnd, cmdShow );
  UpdateWindow( hWnd );

/** get handle to display context **/
  hDC = GetDC(hWnd);

/** fill lpRect1 with coordinate info for non-empty rectangle **/
  lpRect1.left = 10;
  lpRect1.top	 = 10;
  lpRect1.right = 150;
  lpRect1.bottom = 150;

/** fill lpRect2 with coordinate info for  empty rectangle **/
  lpRect2.left = 260;
  lpRect2.top	 = 10;
  lpRect2.right = 260;
  lpRect2.bottom = 200;

  MessageBox(GetFocus(), (LPSTR)"the non-empty rectangle",
      (LPSTR)"I am about to InvertRect...", MB_OK);

/** invert pixels in lpRect1  (the non-empty rectangle) **/
  InvertRect (hDC, (LPRECT) & lpRect1);

  MessageBox(GetFocus(), (LPSTR)"with this non-empty rectangle",
      (LPSTR)"I am about to call IsRectEmpty...", MB_OK);

/** see is lpRect1 is empty **/
  EMPTY = IsRectEmpty((LPRECT) & lpRect1);
  if (EMPTY)
    MessageBox(GetFocus(), (LPSTR)"meaning the rectangle IS empty",
        (LPSTR)"IsRectEmpty returned TRUE...", MB_OK);
  else
    MessageBox(GetFocus(), (LPSTR)"meaning the rectangle is NOT empty",
        (LPSTR)"IsRectEmpty returned FALSE...", MB_OK);

  InvalidateRect(hWnd, NULL, TRUE);

  MessageBox(GetFocus(), (LPSTR)"the EMPTY rectangle",
      (LPSTR)"I am about to InvertRect...", MB_OK);

/** invert pixels in lpRect2 (the empty rectangle) **/
  InvertRect (hDC, (LPRECT) & lpRect2);

  MessageBox(GetFocus(), (LPSTR)"with this empty rectangle",
      (LPSTR)"I am about to call IsRectEmpty...", MB_OK);

/** see is lpRect2 is empty **/
  EMPTY = IsRectEmpty((LPRECT) & lpRect2);
  if (EMPTY)
    MessageBox(GetFocus(), (LPSTR)"meaning the rectangle IS empty",
        (LPSTR)"IsRectEmpty returned TRUE...", MB_OK);
  else
    MessageBox(GetFocus(), (LPSTR)"meaning the rectangle is NOT empty",
        (LPSTR)"IsRectEmpty returned FALSE...", MB_OK);

/* Polling messages from event queue */
  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long	FAR PASCAL HelloWndProc( hWnd, message, wParam, lParam )
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


