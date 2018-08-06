/*
 *
 *  LoadResource
 *  
 *  This program demonstrates the use of the function LoadResource.
 *  This function removes a loaded resource from memory by freeing
 *  the allocated memory occupied by that resource.
 */

#include "windows.h"

int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  HANDLE hResInfo;
  HANDLE hResData;
  HANDLE hLib;
  BOOL bFreed;
  LPSTR lpResInfo;

  hLib = LoadLibrary( (LPSTR)"lib.exe" );
  hResInfo = FindResource( hLib, MAKEINTRESOURCE(63), RT_STRING );

  MessageBox (NULL, (LPSTR)"Loading resource", (LPSTR)"ok", MB_OK);

  hResData = LoadResource ( hLib, hResInfo );

  if ( hResData != NULL )
    MessageBox (NULL, (LPSTR)"Resource loaded", (LPSTR)"ok", MB_OK);
  else
    MessageBox (NULL, (LPSTR)"Resource not loaded", (LPSTR)"ok", MB_OK);

  bFreed = FreeResource ( hResData );
  return 0;
}


