/*
 *
 *  GetDeviceCaps
 *  
 *  This program demonstrates the use of the function GetDeviceCaps.
 *  This function retrieves device-specific information about a given 
 *  display device.  The last parameter specifies the type os information
 *  desired.
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
  short nValue;
  char szbuff[80];
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) "getdcaps";
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

  hWnd = CreateWindow ( ( LPSTR ) "getdcaps", ( LPSTR ) "GetDeviceCaps",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  hDC = GetDC ( hWnd );

  MessageBox (NULL, (LPSTR)"Getting driver version number", 
             (LPSTR)"GetDeviceCaps", MB_OK);

  nValue = GetDeviceCaps ( hDC, DRIVERVERSION );

  sprintf ( szbuff, "%s%d\0", "Driver version number is ",
     nValue );
  MessageBox (NULL, (LPSTR) szbuff, (LPSTR)"GetDeviceCaps", MB_OK);

  ReleaseDC ( hWnd, hDC );

  return 0;
}
