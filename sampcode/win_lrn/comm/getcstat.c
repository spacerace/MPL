/*
 *
 *  GetCommState
 *  
 *  This program demonstrates the use of the function GetCommState.
 *  This function fills the buffer pointed to by the last parameter with the
 *  device control block of the communication device specified by the first
 *  parameter.
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
  int nCid;
  WORD lpResult;
  DCB CommDCB;

  nCid = OpenComm ( (LPSTR)"COM1", 50, 50);
  if ( nCid < 0 )
     {
     MessageBox (NULL, (LPSTR)"OpenComm Failed", (LPSTR)"OpenComm",
        MB_OK|MB_ICONEXCLAMATION);
     exit (1);
     }

  CommDCB.BaudRate = 9600;
  CommDCB.ByteSize = 8;
  CommDCB.StopBits = ONESTOPBIT;
  CommDCB.TxDelay = 10;
  CommDCB.Parity = NOPARITY;
  CommDCB.PeChar = 0;
  CommDCB.RlsTimeout = 1000;
  CommDCB.CtsTimeout = 1000;
  CommDCB.DsrTimeout = 1000;
  CommDCB.XonChar = 2;
  CommDCB.XoffChar = 3;
  CommDCB.XonLim = 1000;
  CommDCB.XoffLim = 1000;
  CommDCB.EofChar = 4;
  CommDCB.EvtChar = 5;

  SetCommState( (DCB FAR *)&CommDCB );

  MessageBox (NULL, (LPSTR)"Requesting communication port state",
     (LPSTR)"GetCommState", MB_OK);

  lpResult = GetCommState ( nCid, (DCB FAR *)&CommDCB );

  if ( lpResult == 0 )
     MessageBox (NULL, (LPSTR)"Communication port state received", 
                (LPSTR)"GetCommState", MB_OK);
  else
     MessageBox (NULL, (LPSTR)"Error when getting state",
        (LPSTR)"GetCommState", MB_OK);

  CloseComm ( nCid );
  
  return 0;
}
