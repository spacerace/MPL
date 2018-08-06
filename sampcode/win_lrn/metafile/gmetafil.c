/*
 *
 *  GetMetaFile
 *  
 *  This program demonstrates the use of the function GetMetaFile.
 *  This function creates a handle for the metafile named by the parameter.
 *
 */

#include <windows.h>

static HANDLE hWnd;

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  static char szFileName[] = "gmetafil";
  static char szFuncName[] = "GetMetaFile";
  HDC hDC;
  HANDLE hMF;
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) szFileName;
     rClass.hInstance     = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
     rClass.hIcon         = LoadIcon ( hInstance, IDI_APPLICATION );
     rClass.lpszMenuName  = ( LPSTR ) "GetMenuFunc";
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
                      ( HWND ) NULL, ( HWND ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  hDC = GetDC ( hWnd );
  
  MessageBox (NULL, (LPSTR)"Getting handle to metafile", (LPSTR) szFuncName,
     MB_OK);

  hMF = GetMetaFile ( (LPSTR) "GMETAFIL.MET" );

  if ( hMF != NULL )
     MessageBox (NULL, (LPSTR)"Metafile handle gotten", (LPSTR) szFuncName,
        MB_OK);
  else
     MessageBox (NULL, (LPSTR)"Metafile handle not gotten", (LPSTR) szFuncName,
        MB_OK);

  PlayMetaFile ( hDC, hMF );
  MessageBox (NULL, (LPSTR)"This is the metafile", (LPSTR) szFuncName, MB_OK);
  ReleaseDC ( hWnd, hDC);
  return 0;
}
