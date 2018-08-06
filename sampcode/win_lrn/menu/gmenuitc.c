/*
 *
 *  GetMenuItemCount
 *  
 *  This program demonstrates the use of the function GetMenuItemCount.
 *  This function determines the number of items in the menu identifed by
 *  the parameter.  This may be either a pop-up of top-level menu.
 *  
 */

#include <windows.h>

static HANDLE hWnd;

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  static char szFileName[] = "gmenuitc";
  static char szFuncName[] = "GetMenuItemCount";
  HMENU hMenu;
  WORD wCount;
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
  
  MessageBox (NULL, (LPSTR)"Getting number of menu items", (LPSTR) szFuncName,
     MB_OK);

  wCount = GetMenuItemCount ( hMenu );

  if ( wCount != -1 )
     {
     sprintf ( szBuff, "%s%d", "Number of menu items is ", wCount );
     MessageBox (NULL, (LPSTR) szBuff, (LPSTR) szFuncName, MB_OK);
     }
  else
     MessageBox (NULL, (LPSTR)"Error", (LPSTR) szFuncName,
        MB_OK|MB_ICONEXCLAMATION);

  return 0;
}
