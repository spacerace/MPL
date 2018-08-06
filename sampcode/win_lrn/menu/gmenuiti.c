/*
 *  GetMenuItemID
 *  
 *  This program demonstrates the use of the function GetMenuItemID.
 *  This function 
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
  static char szFileName[] = "gmenuiti";
  static char szFuncName[] = "GetMenuItemID";
  HMENU hMenu;
  WORD wID;
  char szBuff[80];
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) szFileName;
     rClass.hInstance     = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
     rClass.hIcon         = LoadIcon ( hInstance, IDI_APPLICATION );
     rClass.lpszMenuName  = ( LPSTR ) "GetMenuItemCountFunc";
     rClass.hbrBackground = GetStockObject ( WHITE_BRUSH );
     rClass.style         = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra    = 0;
     rClass.cbWndExtra    = 0;
   
     RegisterClass ( ( LPWNDCLASS ) &rClass );
     }

  hWnd = CreateWindow ( ( LPSTR ) szFileName, ( LPSTR ) szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  hMenu = GetMenu ( hWnd );
  
  MessageBox (NULL, (LPSTR)"Getting ID of menu item Before", (LPSTR) szFuncName,
     MB_OK);

  wID = GetMenuItemID ( hMenu, 0 );

  if ( wID != -1 )
     {
     sprintf ( szBuff, "%s%d", "ID of menu item Before is ", wID );
     MessageBox (NULL, (LPSTR) szBuff, (LPSTR) szFuncName, MB_OK);
     }
  else
     MessageBox (NULL, (LPSTR)"Error", (LPSTR) szFuncName,
        MB_OK|MB_ICONEXCLAMATION);

  return 0;
}
