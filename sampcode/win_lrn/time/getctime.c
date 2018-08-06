/*
 *
 *  GetCurrentTime
 *  
 *  This program demonstrates the use of the function GetCurrentTime.
 *  This function retrieves the current Window time.
 *
 *
 */

#include <windows.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  DWORD dwCurTime;
  char szbuff[80];

  MessageBox (NULL, (LPSTR)"Getting current time", (LPSTR)"GetCurrentTime",
     MB_OK);

  dwCurTime = GetCurrentTime ();

  sprintf ( szbuff, "The current time in milliseconds is %ld",
     dwCurTime );
  MessageBox (NULL, (LPSTR) szbuff, (LPSTR)"GetCurrentTime", MB_OK);

  return FALSE;
}
