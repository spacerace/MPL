/*
 *
 *  SetCommEventMask
 *  
 *  This program demonstrates the use of the function SetCommEventMask.
 *  This function enables and retrieves the event mask of the communication
 *  device specified bye the first parameter.  
 *  
 *  Microsoft Product Support Services
 *  Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  int nCid;
  WORD lpEvent;
  char szBuffer [70];

  nCid = OpenComm ( (LPSTR)"COM1", 50, 50);
  if ( nCid < 0 )
     {
     MessageBox (NULL, (LPSTR)"OpenComm Failed", (LPSTR)"OpenComm",
        MB_OK|MB_ICONEXCLAMATION);
     exit (1);
     }
  MessageBox (NULL, (LPSTR)"Setting EV_ERR mask", (LPSTR)"SetCommEventMask",
     MB_OK);

  SetCommEventMask ( nCid, EV_ERR );

  GetCommEventMask ( nCid, EV_ERR );
  WriteComm ( nCid, (LPSTR)"This is a test!!!", 25 );
  
  lpEvent = SetCommEventMask( nCid, EV_ERR );
  sprintf (szBuffer, "Pointer to the event mask is %x", lpEvent);

  MessageBox (NULL, (LPSTR)szBuffer, (LPSTR)"SetCommEventMask",
              MB_OK);

  CloseComm ( nCid );
  
  return 0;
}
