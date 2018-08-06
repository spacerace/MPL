/*
 *  Function(s) demonstrated in this program: WindowFromPoint
 *  Compiler version: 5.1
 *  Description:
 *     This program will get the handle to the window that contains the
 *  point ( 100, 100 ) in it.
 */

#include <windows.h>

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance ;
LPSTR       lpszCmdLine ;
int         nCmdShow ;
  {
  HWND        hWnd;          /*  Handle to the window to iconize  */
  POINT       pPoint;        /*  Will hold the point  */

  pPoint.x = 100;            /*  Set the point to 100, 100  */
  pPoint.y = 100;

  hWnd = WindowFromPoint( pPoint );  /*  Get the window handle  */

  MessageBox( GetFocus(), (LPSTR)"We have the handle to the window",
              (LPSTR)"", MB_OK );
  return( TRUE );
  }
