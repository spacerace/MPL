/*
 *  TransmitCommChar
 *  trncomch.c
 *
 *  This program demonstrates the use of the function TransmitCommChar.
 *  It puts the specified character at the header of the transmit queue
 *  of the communication device for immediate transmission.
 *
 *  Note: Depending on how com1 is set up, this may have unusual side
 *  effects.
 *
 */

#include <windows.h>
#include <stdio.h>

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  HWND  hWnd;
  short	nCid;            /* identifer for open com device */
  short	nFlag;           /* Result of the TransmitCommChar function */
  DCB   dcb;             /* The device control block */
  BYTE  nChar;           /* Byte value of the char transmitted */
  char	szBuffer[40];
  WNDCLASS   wcClass;

  wcClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
  wcClass.hIcon          = LoadIcon (hInstance, "WindowIcon");
  wcClass.lpszMenuName   = NULL;
  wcClass.lpszClassName  = "Trncomch";
  wcClass.hbrBackground  = GetStockObject (WHITE_BRUSH);
  wcClass.hInstance      = hInstance;
  wcClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcClass.lpfnWndProc    = DefWindowProc;
  wcClass.cbClsExtra     = 0;
  wcClass.cbWndExtra     = 0;

  RegisterClass ( (LPWNDCLASS) & wcClass);

  hWnd = CreateWindow ("Trncomch", "Transmitting Characters",
      WS_OVERLAPPEDWINDOW, 50, 50, 600, 250,
      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  nCid = OpenComm ("COM1", 20, 20);

  if (nCid < 0)
  {
    MessageBox (hWnd, "Com port not opened!!", "OpenComm", MB_OK);
    return 0;
  }

/* set the device control block of the communication device */
  if (GetCommState (nCid, (DCB FAR * ) & dcb) >= 0)
  {
    dcb.BaudRate = 9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;
    dcb.fRtsflow = FALSE;
    dcb.fDtrflow = FALSE;
    dcb.fOutX    = FALSE;
    dcb.fInX     = FALSE;
    dcb.fNull    = FALSE;
    dcb.XonLim   = 1;
    dcb.XoffLim  = 20;
  }

  SetCommState ( (DCB FAR * ) & dcb);

  MessageBox (hWnd, "Ready to transmit char", "Ready?", MB_OK);

/* Sending the alphabets over to a dumb terminal immediately. nFlag
   * is negative if the character cannot be transmitted. 
   */
  for (nChar = 65; nChar <= 90; nChar++)
  {
    nFlag = TransmitCommChar (nCid, nChar);
    if (!nFlag)
    {
      sprintf (szBuffer, "Character %c transmitted", nChar);
      MessageBox (hWnd, szBuffer, "OK", MB_OK);
    }
    else
    {
      sprintf (szBuffer, "Character %c not transmitted", nChar);
      MessageBox (hWnd, szBuffer, "FAIL", MB_OK);
    }
  }

/* must close the communication device before leaving the program */
  CloseComm (nCid);
  return 0;
}


