/*
 *  SetCommState
 *  setcomms.c
 *
 *  This program demonstrates the use of the function SetCommState.
 *  It sets a communication device to the state specified by the 
 *  device control block (DCB) . This function call is necessary
 *  to set the communication to the desired configurations before 
 *  any communication should take place. 
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
    wcClass.lpszClassName  = (LPSTR)"Setcomms";
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
    HWND  hWnd;            /* Handle to the parent window               */
    short nCid;            /* Short integer identifying the opened
                            * communication device                      */
    short nFlag;           /* Result of the SetCommState function call  */
    DCB   dcb;             /* The device control block                  */
       
    WinInit( hInstance );

    hWnd = CreateWindow((LPSTR)"Setcomms",
                        (LPSTR)"Setting Communication Device",
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
        MessageBox (hWnd, (LPSTR)"Com port not opened!!",(LPSTR)NULL,MB_OK); 
        return 0;
      }


    /* Get the current setting of the communication device and then
     * made modifications to each field of the device control block. 
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

    nFlag = SetCommState ((DCB FAR *) &dcb);

    /* Check the return value of SetCommState to see if the function 
     * was successful
     */
    if (nFlag == 0)
      MessageBox (hWnd, (LPSTR)"Communication device set successfully",
                  (LPSTR)"OK", MB_OK);
   
    if (nFlag < 0) 
      MessageBox (hWnd, (LPSTR)"Communication device not set",
                  (LPSTR)NULL, MB_OK); 
     

    /* must close the communication device before leaving the program */
    CloseComm (nCid);

    return 0; 
}
