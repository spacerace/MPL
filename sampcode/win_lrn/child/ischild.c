/*
 *
 *   IsChild
 *
 *   This program demonstrates the use of the IsChild function. IsChild
 *   determines if one window is a child of another. IsChild is called from
 *   WinMain in this sample application.
 */

#include "windows.h"

long	FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);
/* parent's window procedure */

long	FAR PASCAL ChildAProc(HWND, unsigned, WORD, LONG);
/* child A's window procedure */

long	FAR PASCAL ChildBProc(HWND, unsigned, WORD, LONG);
/* child B's window procedure */

HWND hChAWnd = NULL;  /* handle to Child A's window */
HWND hChBWnd = NULL;  /* handle to Child B's window */

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
  pHelloClass->lpfnWndProc	 = HelloWndProc;

  if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
/* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return FALSE;

  pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
  pHelloClass->hIcon		 = LoadIcon( hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName	 = (LPSTR)"CHILD A";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc	 = ChildAProc;

  if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
/* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return FALSE;

  pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
  pHelloClass->hIcon		 = LoadIcon( hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName	 = (LPSTR)"CHILD B";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc	 = ChildBProc;

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
  BOOL  bChild;	 /* return value from IsChild */

  HelloInit( hInstance );

  hWnd = CreateWindow((LPSTR)"Sample Application", (LPSTR)"IsChild (PARENT)",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

/* Make window visible according to the way the app is activated */
  ShowWindow( hWnd, cmdShow );
  UpdateWindow( hWnd );

  hChAWnd = CreateWindow((LPSTR)"CHILD A", (LPSTR)"Child A",
      WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
      5, 5,
      150, 150,
      hWnd, 1, 	hInstance, NULL);

  hChBWnd = CreateWindow((LPSTR)"CHILD B", 	(LPSTR)"Child B",
      WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
      270, 5,
      150, 150,
      hWnd, 	2, 	hInstance, NULL);

/***************************************************************************/

  MessageBox(GetFocus(), (LPSTR)"Child A is a Child of the Parent Window",
      (LPSTR)"Checking to see if...",
      MB_OK);

  bChild = IsChild(hWnd, hChAWnd);

  if (bChild)
    MessageBox(GetFocus(), (LPSTR)"IS a child of the Parent Window",
        (LPSTR)"'IsChild' says that Child A...",
        MB_OK);
  else
    MessageBox(GetFocus(), (LPSTR)"is NOT a child of the Parent Window",
        (LPSTR)"'IsChild' says that Child A...",
        MB_OK);


  MessageBox(GetFocus(), (LPSTR)"Child A is a child of Child B",
      (LPSTR)"Checking to see if...",
      MB_OK);

  bChild = IsChild(hChBWnd, hChAWnd); /* is Child A a child of Child B? */

  if (bChild)
    MessageBox(GetFocus(), (LPSTR)"IS a child of Child B",
        (LPSTR)"'IsChild' says that Child A...",
        MB_OK);
  else
    MessageBox(GetFocus(), (LPSTR)"is NOT a child of Child B",
        (LPSTR)"'IsChild' says that Child A...",
        MB_OK);

/***************************************************************************/

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
  case WM_LBUTTONDOWN:
    MessageBox(GetFocus(), (LPSTR)"Left Button Click", (LPSTR)"PARENT WINDOW",
        MB_OK);
    break;

  case WM_DESTROY:
    PostQuitMessage( 0 );
    break;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
    break;
  }
  return(0L);
}


long	FAR PASCAL ChildAProc( hChAWnd, message, wParam, lParam )
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
    PostQuitMessage( 0 );
    break;

  default:
    return DefWindowProc( hChAWnd, message, wParam, lParam );
    break;
  }
  return(0L);
}


long	FAR PASCAL ChildBProc( hChBWnd, message, wParam, lParam )
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
    PostQuitMessage( 0 );
    break;

  default:
    return DefWindowProc( hChBWnd, message, wParam, lParam );
    break;
  }
  return(0L);
}


