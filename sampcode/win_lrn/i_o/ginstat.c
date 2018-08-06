/*
 *
 *  GetInputState
 *  
 *  This program demonstrates the use of the function GetInputState.
 *  This function determines whether there are mouse, keyboard, or timer
 *  events in the system queue that require processing.
 *  
 *  Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  BOOL bState;
  
  MessageBox (NULL, (LPSTR)"Getting input state", (LPSTR)"GetInputState",
     MB_OK);

  bState = GetInputState ();

  if ( bState != 0 )
     MessageBox (NULL, (LPSTR)"No Important Messages on the Queue", (LPSTR)"GetInputState",
        MB_OK);
  else
     MessageBox (NULL, (LPSTR)"Important Messages are Waiting",
        (LPSTR)"GetInputState", MB_OK|MB_ICONEXCLAMATION);

  return 0;
}
