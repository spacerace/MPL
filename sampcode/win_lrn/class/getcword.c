/*
 *
 *  GetClassWord
 *  
 *  This program demonstrates the use of the function GetClassWord.
 *  This function retrieves the word that is specified by the last para-
 *  meter form the WNDCLASS stucture of the window specified by the 
 *  first parameter.
 *
 *  Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>

static HANDLE hWnd;

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  WORD wClassWord;
  char szbuff[80];
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) "getcword";
     rClass.hInstance     = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
     rClass.hIcon         = LoadIcon ( hInstance, IDI_APPLICATION );
     rClass.lpszMenuName  = ( LPSTR ) NULL;
     rClass.hbrBackground = GetStockObject ( WHITE_BRUSH );
     rClass.style         = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra    = 0;
     rClass.cbWndExtra    = 0;
   
     RegisterClass ( ( LPWNDCLASS ) &rClass );
     }

  hWnd = CreateWindow ( ( LPSTR ) "getcword", ( LPSTR ) "GetClassWord",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );

  MessageBox (NULL, (LPSTR)"Getting windows style bits",
     (LPSTR)"GetClassWord", MB_OK);

  wClassWord = GetClassWord ( hWnd, GCW_STYLE );

  sprintf ( szbuff, "%s%d\0", "Style bits are ", wClassWord );

  MessageBox (NULL, (LPSTR) szbuff, (LPSTR)"GetClassWord", MB_OK);

  return 0;
}
