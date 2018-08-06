/*
 *  OpenComm
 *
 *  This program demonstrates the use of the function OpenComm.
 *  It opens a communication device and assign a handle to it. The 
 *  communication device is initialized to a default configuration.
 *  Spaces are allocated for the "recieve" and "transmit" queues.
 *  The communication port must be close before it can be open again with
 *  OpenComm.
 *
 */

#include "windows.h"

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  short	nCid;            /* short integer identifying the opened
                          * communication device  */

  nCid = OpenComm ( (LPSTR)"COM1", 20, 20);

  if (nCid >= 0)
    MessageBox (GetFocus (), (LPSTR)"Com port opened!!", (LPSTR)"OK", MB_OK);
  else
    {
    MessageBox (GetFocus (), (LPSTR)"Com port not opened!!",
               (LPSTR)"FAIL!", MB_OK);
    return 0;
    }

  CloseComm (nCid);

  return 0;
  }
