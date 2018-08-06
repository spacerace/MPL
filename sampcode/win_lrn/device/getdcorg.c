/*
 *  GetDCOrg
 *  
 *  This program demonstrates the use of the function GetDCOrg.
 *  This function obtians the final translation origin for the device
 *  context.  
 *
 *  Microsoft Product Support Services
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
  LONG ptOrigin;
  char szbuff[80];
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) "getdcorg";
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

  hWnd = CreateWindow ( ( LPSTR ) "getdcorg", ( LPSTR ) "GetDCOrg",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  hDC = GetDC ( hWnd );

  MessageBox (NULL, (LPSTR)"Getting origin", (LPSTR)"GetDCOrg", MB_OK);

  ptOrigin = GetDCOrg ( hDC );

  sprintf ( szbuff, "The origin of this window is at (%d,%d)",
     LOWORD(ptOrigin), HIWORD(ptOrigin) );
  MessageBox (NULL, (LPSTR) szbuff, (LPSTR)"GetDCOrg", MB_OK);

  ReleaseDC ( hWnd, hDC );

  return 0;
}
