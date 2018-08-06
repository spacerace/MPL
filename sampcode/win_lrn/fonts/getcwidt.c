/*
 *
 *  GetCharWidth
 *  
 *  This program demonstrates the use of the function GetCharWidth.
 *  This function retrieves the widths of individual characters in a
 *  consecutive group of characters from the current font.  The funtion
 *  stores the values in the buffer pointed to by the last parameter.
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
  HDC hDC;
  PAINTSTRUCT ps;
  BOOL bCharWidth;
  WORD wFirstChar = 97;
  WORD wLastChar = 122;
  char szbuff[80];
  int  lpBuffer[26];
  int  CharWidthSum, CharWidth, Index;
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) "getcwidt";
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

  hWnd = CreateWindow ( ( LPSTR ) "getcwidt", ( LPSTR ) "GetCharWidth",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  BeginPaint ( hWnd, ( LPPAINTSTRUCT ) &ps );
  hDC = ps.hdc;

  MessageBox (NULL, (LPSTR)"Getting charater string width",
     (LPSTR)"GetCharWidth", MB_OK);

  bCharWidth = GetCharWidth ( hDC, wFirstChar, wLastChar,(LPINT) lpBuffer );
  for (Index = 0, CharWidthSum = 0; Index < 26; Index++) {
    CharWidthSum += lpBuffer [Index];
  }
  CharWidth = CharWidthSum / 26;
  sprintf (szbuff, "The average character width is %i", CharWidth);
  MessageBox (hWnd, szbuff, "GetCharWidth", MB_OK);

  return 0;
}
