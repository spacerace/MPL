/*
 *   IsWindow
 *
 *   This program demonstrates the use of the IsWindow function. The IsWindow
 *   function checks to see if the given handle is indeed a handle to a valid
 *   window.
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
  pHelloClass->hIcon		 = LoadIcon( hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName	 = (LPSTR)"Sample Application";
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
  HWND  hWnd;       /** VALID HANDLE TO VALID WINDOW	**/
  HWND invalWnd;     /** INVALID HANDLE TO PHONY WINDOW **/
  HMENU hMenu;
  BOOL isvalid;     /** BOOLEAN TO HOLD RETURN VALUE FROM IsWindow **/

/** INVALID ASSIGNMENT TO A HANDLE TO A WINDOW **/
  invalWnd = 44;

  HelloInit( hInstance );

/** VALID ASSIGNMENT TO A HANDLE TO A WINDOW   **/
  hWnd = CreateWindow((LPSTR)"Sample Application", (LPSTR)"Sample Application",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, 	NULL, hInstance, NULL);

/* Make window visible according to the way the app is activated */
  ShowWindow( hWnd, cmdShow );
  UpdateWindow( hWnd );
  MessageBox(GetFocus(), (LPSTR)"Checking if this window valid", (LPSTR)" ",
      MB_OK);

/** is hWnd a valid handle to a window? **/
  isvalid = IsWindow(hWnd);

/** if yes  **/
  if (isvalid)
/** say so **/
    MessageBox(GetFocus(), (LPSTR)"this window is valid", (LPSTR)"WINDOW INFO",
        MB_OK);
  else /** if not valid, say so **/
    MessageBox(GetFocus(), (LPSTR)"this window is NOT valid", (LPSTR)"WINDOW INFO",
        MB_OK);

  MessageBox(GetFocus(), (LPSTR)"Checking if phoney window valid", (LPSTR)" ",
      MB_OK);

/** is invalWnd a valid handle to a window? **/
  isvalid = IsWindow(invalWnd);
/** if yes, say so **/
  if (isvalid)
    MessageBox(GetFocus(), (LPSTR)"the phony window is valid", (LPSTR)"WINDOW INFO",
        MB_OK);
  else /** if not valid, say so **/
    MessageBox(GetFocus(), (LPSTR)"the phony window is NOT valid", (LPSTR)"WINDOW INFO",
        MB_OK);

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
  PAINTSTRUCT ps;

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


