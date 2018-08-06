/*
 *  This program demonstrates the use of the function BringWindowToTop.
 *  It brings a popup style or child style window to the top of a stack
 *  of overlapping windows. 
 */

#include "windows.h"

/* Registering the parent and child window classes */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
  WNDCLASS   wcParentClass, wcChildClass;

/* registering the parent window class */
  wcParentClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
  wcParentClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
  wcParentClass.lpszMenuName   = (LPSTR)NULL;
  wcParentClass.lpszClassName  = (LPSTR)"Parent";
  wcParentClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  wcParentClass.hInstance      = hInstance;
  wcParentClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcParentClass.lpfnWndProc    = DefWindowProc;
  wcParentClass.cbClsExtra     = 0;
  wcParentClass.cbWndExtra     = 0;

  RegisterClass( (LPWNDCLASS) & wcParentClass );

/* registering the child window class */
  wcChildClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
  wcChildClass.hIcon          = (HICON)NULL;
  wcChildClass.lpszMenuName   = (LPSTR)NULL;
  wcChildClass.lpszClassName  = (LPSTR)"Child";
  wcChildClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  wcChildClass.hInstance      = hInstance;
  wcChildClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcChildClass.lpfnWndProc    = DefWindowProc;
  wcChildClass.cbClsExtra     = 0;
  wcChildClass.cbWndExtra     = 0;

  RegisterClass( (LPWNDCLASS) & wcChildClass );
  return TRUE;        /* Initialization succeeded */
}


int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  HWND  hParent, hChild1, hChild2;     /* Handles to the windows */

  WinInit (hInstance);

/* creating the parent window */
  hParent = CreateWindow((LPSTR)"Parent",
      (LPSTR)"Parent Window",
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
    50,                /* x         */
50,                /* y         */
600,               /* width     */
250,               /* height    */
  (HWND)NULL,        /* no parent */
  (HMENU)NULL,       /* use class menu */
  (HANDLE)hInstance, /* handle to window instance */
  (LPSTR)NULL        /* no params to pass on */
  );

/* Make window visible according to the way the app is activated */
  ShowWindow( hParent, cmdShow );
  UpdateWindow( hParent );

/* creating the child windows */
  hChild1 = CreateWindow ((LPSTR) "Child",
      (LPSTR) "Child #1",
      WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION | WS_VISIBLE,
    50,                 /* x         */
50,                 /* y         */
100,                /* width     */
50,                 /* height    */
  (HWND) hParent,     /* parent of this window   */
  (HMENU) 1,          /* child window identifier */
  (HANDLE) hInstance, /* handle to window instance */
  (LPSTR) NULL);      /* no params to pass on    */

  hChild2 = CreateWindow ((LPSTR) "Child",
      (LPSTR) "Child #2",
      WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION | WS_VISIBLE,
    60,                 /* x          */
60,                 /* y          */
100,                /* width      */
50,                 /* height     */
  (HWND) hParent,     /* parent of this window   */
  (HMENU) 2,          /* child window identifier */
  (HANDLE) hInstance, /* handle to window instance */
  (LPSTR) NULL);      /* no params to pass on    */


/* Bring the bottom window to the top */
  MessageBox (hParent, (LPSTR)"Before bringing the bottom window up",
      (LPSTR)"Ready ?", MB_OK);

  BringWindowToTop (hChild2);

  MessageBox (hParent, (LPSTR)"Child #2 has been brought up!",
      (LPSTR)"Done", MB_OK);

  return 0;
}


