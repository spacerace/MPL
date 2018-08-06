/*
 *  WriteComm
 *
 *  This program demonstrates the use of the function WriteComm.
 *  It attempts to write a specified number of characters to the 
 *  communication device that has already been opened. Before 
 *  attempting to do any write, the communication device must have 
 *  its device control block set up properly to a correct configuration.
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
    wcClass.lpszClassName  = (LPSTR)"Wrtcomm";
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
    HWND  hWnd;            /* Handle to the parent window             */
    short nCid;            /* Short integer identifying the opened
                            * communication device                    */
    short nBytes;          /* The number of characters written        */
    DCB   dcb;             /* The device control block                */
       
    WinInit( hInstance );

    hWnd = CreateWindow((LPSTR)"Wrtcomm",
                        (LPSTR)"Writing to Communication Devices",
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
    nCid = OpenComm ((LPSTR)"COM1", 25, 25);
    if (nCid < 0) 
      {
        MessageBox (hWnd, (LPSTR)"Com port not opened!!",(LPSTR)NULL,MB_OK); 
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

    nBytes = WriteComm (nCid, (LPSTR)"Writing to COM port #1", 22);
    if (nBytes > 0)
      MessageBox (hWnd, (LPSTR)"Written successfully", (LPSTR)"OK", MB_OK);
    else
      /* Error should be checked by calling GetCommError at this point 
       * so that other communiation function will work properly
       */
      MessageBox (hWnd, (LPSTR)"Error Occurred", (LPSTR)"ERROR", MB_OK);

    /* must close the communication device before leaving the program */
    CloseComm (nCid);

    return 0; 
}


