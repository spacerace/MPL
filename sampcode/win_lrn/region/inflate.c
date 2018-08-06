/*
 *   InflateRect
 *
 *   This program demonstrates the use of the InflateRect function. The
 *   InflateRect function increase or decreases the values in the given
 *   LPRECT structure according to the x and y parameters in the function
 *   call. To demonstrate the increase in size of the rectangle described
 *   by lpRect in this application, after InflateRect was called, the
 *   rectangle was inverted using InvertRect.
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
  HDC hDC;		    /** handle to display context	  **/
  RECT lpRect;	  /** rectangle coordinates structure	**/

  HelloInit( hInstance );
  hWnd = CreateWindow((LPSTR)"Sample", (LPSTR)"Sample Application",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

/* Make window visible according to the way the app is activated */
  ShowWindow( hWnd, cmdShow );
  UpdateWindow( hWnd );

/** get handle to display context **/
  hDC = GetDC(hWnd);

/** fill lpRect with coordinate info for rectangle **/
  lpRect.left = 50;
  lpRect.top  = 50;
  lpRect.right = 150;
  lpRect.bottom = 150;

  MessageBox(GetFocus(), (LPSTR)"a small rectangle",
      (LPSTR)"I am about to InvertRect...", MB_OK);

/** invert pixels in lpRect **/
  InvertRect (hDC, (LPRECT) & lpRect);

  MessageBox(GetFocus(), (LPSTR)"the rectangle",
      (LPSTR)"I am about to InflateRect...", MB_OK);

/** inflate rect by 10 in x direction and 10 in y direction **/
  InflateRect((LPRECT) & lpRect, 10, 10);

  MessageBox(GetFocus(), (LPSTR)"the inflated rectangle",
      (LPSTR)"I am about to InvertRect...", MB_OK);

/** invert the inflated rect to show it is now bigger **/
  InvertRect (hDC, (LPRECT) & lpRect);

/* Polling messages from event queue */
  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  return msg.wParam;
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


