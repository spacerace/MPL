/*
 *
 *  GetEnvironment
 *  getenv.c
 *  
 *  This program demonstrates the use of the function GetEnvironment.
 *  This function retrieves the currewnt environment that is associated with
 *  the device attached to the system port specified by the first parameter,
 *  and copies it into the buffer specified by the second parameter.
 *
 */

#include <windows.h>
#include "getenv.h"

LPSTR far PASCAL lstrcpy( LPSTR, LPSTR );

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  short nCopied;
  short nSCopied;
  DEVMODE devmode;

  lstrcpy(devmode.DeviceName, "HP LaserJet Series II");
  devmode.orient = PORTRAIT;
  devmode.cartridge = DEFAULT_CTG + CARTRIDGE;
  devmode.paper = LETTER;

  nSCopied = SetEnvironment("LPT1:", (LPSTR)&devmode, sizeof(DEVMODE));
  if (!nSCopied)
     {
     MessageBox (NULL, "Environment not set!", "SetEnvironment",
        MB_OK|MB_ICONEXCLAMATION);
     return (1);
     }

  /* Clear out name string to show that we can retrieve it */
  lstrcpy(devmode.DeviceName, "xxxxxxxxxxxxxx");

  MessageBox(NULL, "Getting environment", "GetEnvironment", MB_OK);
  nCopied=GetEnvironment("LPT1:", (LPSTR)&devmode, sizeof(DEVMODE));

  if (nCopied)
     MessageBox(NULL, devmode.DeviceName, "DeviceName", MB_OK);
  else
     MessageBox(NULL, "Environment not found", "GetEnvironment", MB_OK);
 
  return 0;
}
