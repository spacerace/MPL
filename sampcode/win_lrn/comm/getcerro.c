/*
 *
 *  GetCommError
 *  
 *  This program demonstrates the use of the function GetCommError.
 *  This function fills the status buffer pointed to by the last parameter
 *  with the current status of the communication device specified by the
 *  first parameter.  It also returns the error code that have occurred
 *  since the last GetCommError call.
 *  
 *  Windows Version 2.0 function demonstration application
 *
 */

#include "windows.h"

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  int nCid;
  short nError;

  nCid = OpenComm ( (LPSTR)"COM1", 50, 50);
  if ( nCid < 0 )
     {
     MessageBox (NULL, (LPSTR)"OpenComm Failed", (LPSTR)"OpenComm",
        MB_OK|MB_ICONEXCLAMATION);
     exit (1);
     }
  WriteComm ( nCid, (LPSTR)"This is a test!!!", 25 );
  
  MessageBox (NULL, (LPSTR)"Clearing communication port", (LPSTR)"GetCommError",
     MB_OK);

  nError = GetCommError ( nCid, NULL );

  if ( nError == CE_BREAK )
     MessageBox (NULL, (LPSTR)"Hardware detected a brake condition",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_CTSTO )
     MessageBox (NULL, (LPSTR)"Clear-to-send timeout",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_DNS )
     MessageBox (NULL, (LPSTR)"Parallel device is not selected",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_DSRTO )
     MessageBox (NULL, (LPSTR)"Data-set-ready timeout",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_FRAME )
     MessageBox (NULL, (LPSTR)"Hardware detects a framing error",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_IOE )
     MessageBox (NULL, (LPSTR)"I/O error while communicate with parallel device",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_MODE )
     MessageBox (NULL, (LPSTR)"Requested mode is not supported",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_OOP )
     MessageBox (NULL, (LPSTR)"Parallel device signals its out of paper",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_OVERRUN )
     MessageBox (NULL, (LPSTR)"A character has been overrun and lost",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_PTO )
     MessageBox (NULL, (LPSTR)"Timeout while communicating with parallel device",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_RLSDTO )
     MessageBox (NULL, (LPSTR)"Recieve-line-signal-detect timeout",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_RXOVER )
     MessageBox (NULL, (LPSTR)"Receive queue overflow",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_RXPARITY )
     MessageBox (NULL, (LPSTR)"Hardware detected a parity error",
        (LPSTR)"GetCommError", MB_OK);
  else
  if ( nError == CE_TXFULL )
     MessageBox (NULL, (LPSTR)"Transmit gueue is full while trying \n to queue a character",
        (LPSTR)"GetCommError", MB_OK);
  else
     MessageBox (NULL, (LPSTR)"No ERROR",
        (LPSTR)"GetCommError", MB_OK|MB_ICONEXCLAMATION);

  CloseComm ( nCid );
  return 0;
}
