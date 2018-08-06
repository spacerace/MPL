/*
 *  SetMapperFlags
 *  smapflag.c
 *  
 *  This program demonstrates the use of the function SetMapperFlags.
 *  This function alters the algorithm that the font mapper uses when it
 *  maps logical fonts to physical fonts.  When the first bit in the first
 *  parameter is set to one, the mapper will only select fonts whose aspect
 *  ratios match those of the specified device.  If no fonts exit with a
 *  matching aspect ratio, GDI chooses an aspect ratio and select fonts
 *  with aspect ratios that match the one by GDI.
 *
 */

#include "windows.h"

static HANDLE hWnd;

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  HDC hDC;
  PAINTSTRUCT ps;
  DWORD ptAspectRatio;
  DWORD ptMapperFlags;
  char szbuff[160];
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) "smapflag";
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

  hWnd = CreateWindow ( ( LPSTR ) "smapflag", ( LPSTR ) "SetMapperFlags",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  BeginPaint ( hWnd, ( LPPAINTSTRUCT ) &ps );
  hDC = ps.hdc;

  MessageBox (NULL, (LPSTR)"Setting font-mapper flag",
     (LPSTR)"SetMapperFlags", MB_OK);

  ptMapperFlags = SetMapperFlags( hDC, (long) 1 );

  ptAspectRatio = GetAspectRatioFilter ( hDC );

  sprintf ( szbuff, "%s%d%s%d\n%s%d%s%d\0", "Old aspect ratio is: x= ",
           HIWORD(ptMapperFlags), " y= ", LOWORD(ptMapperFlags),
           "New aspect ratio is: x= ", HIWORD(ptAspectRatio), " y= ",
           LOWORD(ptAspectRatio));

  MessageBox (NULL, (LPSTR) szbuff, (LPSTR)"SetMapperFlags", MB_OK);

  return 0;
}
