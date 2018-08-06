/*
 *
 *  GetBrushOrg
 *  gbruorg.c
 *  
 *  This program demonstrates the use of the function GetBrushOrg.
 *  This function retrieves the current brush origin for the given
 *  device context.
 *  
 */

#include "windows.h"

static HANDLE hWnd;

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  DWORD ptOrigin;
  HDC hDC;
  PAINTSTRUCT ps;
  char szbuff[80];

  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) "gbruorg";
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

  hWnd = CreateWindow ("gbruorg", "GetBrushOrg",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
		      NULL, NULL,
		      hInstance, NULL );
 
  ShowWindow(hWnd, cmdShow);
  BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
  hDC=ps.hdc;
    
  MessageBox(NULL, "Getting brush origin", "GetBrushOrg", MB_OK);
  ptOrigin=GetBrushOrg(hDC);
  sprintf(szbuff, "%s%d%s%d\0", "Brush origin is: x= ", HIWORD(ptOrigin),
	    " y= ", LOWORD(ptOrigin));

  MessageBox (NULL, szbuff, "GetBrushOrg", MB_OK);
  return 0;
}
