/*
 *
 *  GetKeyState
 *  
 *  This program demonstrates the use of the function GetKeyState
 *  This function retrieves the state of the virtual key specified by the
 *  parameter.
 *  
 */

#include <windows.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  static char szFuncName[] = "GetKeyState";
  int nState;
  
  MessageBox (NULL, (LPSTR)"Getting the key state of F1", (LPSTR)szFuncName,
     MB_OK);

  nState = GetKeyState ( VK_F1 );

  if ( ( nState & 0x8000 ) != 0x8000 )
     MessageBox (NULL, (LPSTR)"The key F1 is up", (LPSTR)szFuncName, MB_OK);
  else
     MessageBox (NULL, (LPSTR)"The key F1 is down", (LPSTR)szFuncName, MB_OK);

  return 0;
}
