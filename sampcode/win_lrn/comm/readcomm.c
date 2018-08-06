/*
 *  ReadComm
 *  This program demonstrates the use of the function ReadComm.
 *  It attempts to read a specified number of characters from the 
 *  communication device that has already been opened and copy the 
 *  characters into a buffer. Before attempting to do any read, the 
 *  communication device must have its device control block set up 
 *  properly to a correct configuration.
 *
 *  This program will only run properly if there is a dumb terminal connected
 *  to COM1: port.
 *
 */

#include "windows.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Readcomm";
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.hInstance      = hInstance;
    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = DefWindowProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;

    RegisterClass( (LPWNDCLASS)&wcClass );
    return TRUE;        /* Initialization succeeded */
}

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND    hWnd;          /* Handle to the parent window             */
    short   nCid;          /* Short integer identifying the opened
                            * communication device                    */
    short   nBytes;        /* The number of characters read           */
    char    szBuffer[20];  /* The buffer that is recieving the    
                            * characters to be read                   */
    HDC     hDC;           /* Handle to the DC of the client area     */
    DCB     dcb;           /* The device control block                */
    COMSTAT cstat;         /* The current status of the communication 
                            * device                                  */
       
    WinInit( hInstance );

    hWnd = CreateWindow((LPSTR)"Readcomm",
                        (LPSTR)"Read from Communication Devices",
                        WS_OVERLAPPEDWINDOW,
                        50,                /* x         */
                        50,                /* y         */
                        600,               /* width     */
                        250,               /* height    */
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* attempt to open the com1 port */
    nCid = OpenComm ((LPSTR)"COM1", 20, 20);
    if (nCid < 0) 
      {
         MessageBox (hWnd, (LPSTR)"Com port not opened!!",(LPSTR)"FAIL!",MB_OK);
         return 0;
      }

    /* the communication device control block must be set to the appropriate
     * settings of the other dumb terminal in order to read and write
     * properly
     */
    if (GetCommState (nCid, (DCB FAR *) &dcb) >= 0)
      { dcb.BaudRate = 9600;
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

    SetCommState ((DCB FAR *) &dcb);

    /* Put something into the transmit queue of the dumb terminal */
    MessageBox (hWnd, (LPSTR)"Type in something at the other terminal", 
                (LPSTR)"Quick", MB_OK);

    /* attempt to read from the recieve queue of the communication device */
    nBytes = ReadComm (nCid, (LPSTR)szBuffer, 20);

    /* in case of an overflow of the recieve queue, GetCommError is 
     * called to clear the error condition,; otherwise no other communication
     * function will work properly
     */
    if (GetCommError (nCid, (COMSTAT FAR *) &cstat) == CE_RXOVER)
      {  MessageBox (hWnd, (LPSTR)"Receive Queue overflow", (LPSTR)"ERROR!!", MB_OK);

         /* The error code must be cleared first before reading the receive 
          * queue again
          */
         nBytes = ReadComm (nCid, (LPSTR)szBuffer, 20);
      }

    /* Check to see if the read is successfully */
    if (nBytes == 0)
      MessageBox (hWnd, (LPSTR)"No character in the recieve queue",
                    (LPSTR)"None Read", MB_OK);
   
    if (nBytes > 0)
      { hDC = GetDC (hWnd);
        TextOut (hDC, 10, 10, (LPSTR)szBuffer, nBytes);
        ReleaseDC (hWnd, hDC);
        MessageBox (hWnd, 
                    (LPSTR)"Some characters read from the recieve queue",
                    (LPSTR)"Read", MB_OK);
      }
  
    if (nBytes < 0) 
      MessageBox (hWnd, (LPSTR)"Nothing read", (LPSTR)NULL, MB_OK); 
     

    /* must close the communication device before leaving the program */
    CloseComm (nCid);

    return 0; 
}
