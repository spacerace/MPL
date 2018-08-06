/*
 *
 *  GetInstanceData
 *  
 *  This program demonstrates the use of the function GetInstanceData.
 *  This function copies data from a previous intance of an application
 *  into the data area of the current instance.
 *  
 *  Other references: numerous apps. with SDK
 *
 *  Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>

LPSTR far PASCAL lstrcpy( LPSTR, LPSTR );

static HANDLE hWnd;

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  int nBytes = 0;
  char szAppName[25];
  char szFuncName[25];
  char szBuffer[80];
  
  lstrcpy ( (LPSTR) szAppName, (LPSTR) "ginstdat" );

  lstrcpy ( (LPSTR) szFuncName, (LPSTR) "GetIntanceData" );

  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) szAppName;
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
  else
     {
     MessageBox (NULL, (LPSTR)"Getting instance data", 
       (LPSTR)szFuncName, MB_OK );

     nBytes = GetInstanceData( hPrevInstance, (PSTR)szAppName, 25 );

     sprintf ( szBuffer, "%s%d", "The number of bytes copied is ", nBytes );

     MessageBox ( NULL, (LPSTR)szBuffer, (LPSTR)szFuncName, MB_OK );
     }

  hWnd = CreateWindow ( ( LPSTR ) szAppName, ( LPSTR ) szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );

  if ( nBytes == 0 )
  MessageBox ( NULL, (LPSTR)"Start another instance before \npushing ok", 
      (LPSTR)szFuncName, MB_OK );
     
  return 0;
}
