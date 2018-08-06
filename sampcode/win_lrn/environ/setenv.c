/*
 *  SetEnvironment
 *  Setenv.c
 *  
 *  This program demonstrates the use of the function SetEnvironment.
 *  This function copies the contents of the buffer specified by the second
 *  parameter into the environment associated with the device attached to
 *  the system port specified by the first parameter.
 *
 */

#include <windows.h>
#include "setenv.h"

LPSTR far PASCAL lstrcpy( LPSTR, LPSTR );

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  short nCopied;
  DEVMODE devmode;

  lstrcpy ( (LPSTR) devmode.DeviceName, (LPSTR) "HP LaserJet Series II" );
  devmode.orient = PORTRAIT;
  devmode.cartridge = DEFAULT_CTG + CARTRIDGE;
  devmode.paper = LETTER;

  MessageBox ( NULL, (LPSTR) "Setting environment", (LPSTR) "SetEnvironment",
     MB_OK );

  nCopied = SetEnvironment ( (LPSTR) "LPT1:", (LPSTR) &devmode,
     sizeof (DEVMODE) );

  if ( nCopied == 0 )
     MessageBox (NULL, (LPSTR)"Environment not set!", (LPSTR)"SetEnvironment",
        MB_OK|MB_ICONEXCLAMATION);
  else
     MessageBox (NULL, (LPSTR)"Environment set", (LPSTR)"SetEnvironment",
        MB_OK);
 
  return 0;
}
