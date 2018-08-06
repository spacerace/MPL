/*
 *
 *  GetMapMode
 *  
 *  This program demonstrates the use of the function GetMapMode.
 *  This function retrieves the cuurent mapping mode.
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
  static char szFileName[] = "getmapm";
  static char szFuncName[] = "GetMapMode";
  short nMapMode;
  
  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.lpszClassName = ( LPSTR ) szFileName;
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

  hWnd = CreateWindow ( ( LPSTR ) szFileName, ( LPSTR ) szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      ( HWND ) NULL, ( HMENU ) NULL,
                      ( HANDLE ) hInstance, ( LPSTR ) NULL );
 
  ShowWindow ( hWnd , cmdShow );
  hDC = GetDC ( hWnd );
  SetMapMode ( hDC, MM_TEXT);

  MessageBox (NULL, (LPSTR)"Getting mapping mode", (LPSTR)szFuncName, MB_OK);

  nMapMode = GetMapMode ( hDC );

  if ( nMapMode == MM_ANISOTROPIC )
     MessageBox (NULL, (LPSTR)"Mapping mode is anisotropic" ,
        (LPSTR)szFuncName, MB_OK);
  else
     if ( nMapMode == MM_HIENGLISH )
        MessageBox (NULL, (LPSTR)"Mapping mode is high english" ,
           (LPSTR)szFuncName, MB_OK);
     else
        if ( nMapMode == MM_HIMETRIC )
           MessageBox (NULL, (LPSTR)"Mapping mode is high metric" ,
              (LPSTR)szFuncName, MB_OK);
        else
           if ( nMapMode == MM_ISOTROPIC )
              MessageBox (NULL, (LPSTR)"Mapping mode is isotropic" ,
                 (LPSTR)szFuncName, MB_OK);
           else
              if ( nMapMode == MM_LOMETRIC )
                 MessageBox (NULL, (LPSTR)"Mapping mode is low metric" ,
                    (LPSTR)szFuncName, MB_OK);
              else
                 if ( nMapMode == MM_LOENGLISH )
                    MessageBox (NULL, (LPSTR)"Mapping mode is low english" ,
                       (LPSTR)szFuncName, MB_OK);
                 else
                    if ( nMapMode == MM_TEXT )
                       MessageBox (NULL, (LPSTR)"Mapping mode is text" ,
                          (LPSTR)szFuncName, MB_OK);
                    else
                       if ( nMapMode == MM_TWIPS )
                          MessageBox (NULL, (LPSTR)"Mapping mode is twips" ,
                             (LPSTR)szFuncName, MB_OK);
                       else
                          MessageBox (NULL, (LPSTR)"Error no mapping mode" ,
                             (LPSTR)szFuncName, MB_OK|MB_ICONEXCLAMATION);

  ReleaseDC ( hWnd, hDC );

  return 0;
}
