/*
 *  SetVoiceQueueSize
 *  setvcque
 *
 *  SetVoiceQueueSize(nVoice,nBytes) allocates nBytes bytes for the voice
 *  queue specified by nVoice.	If the queue size is not set, the default
 *  is 192 bytes, room for about 32 notes.  All voice queues are locked in
 *  memory.  The queues cannot be set while music is playing.
 *
 *  This program demonstrates the use of the function SetVoiceQueueSize.
 *  As shown below, OpenSound must be invoked before SetVoiceQueueSize can
 *  be called.	SetVoiceQueueSize returns one of three possible values -
 *  one success value and two error values.
 *
 */

#include "windows.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pClass;

    pClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    /* registering the parent window class */
    pClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pClass->lpszMenuName   = (LPSTR)NULL;
    pClass->lpszClassName  = (LPSTR)"Window";
    pClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pClass->hInstance      = hInstance;
    pClass->style          = CS_HREDRAW | CS_VREDRAW;
    pClass->lpfnWndProc    = DefWindowProc;

    if (!RegisterClass( (LPWNDCLASS)pClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE) pClass );
    return TRUE;        /* Initialization succeeded */
}

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND  hWnd;             /* Handle to the parent window            */
    short nResult;          /* Returned by SetVoiceQueueSize function */
    short nVoice;           /* Specifies the voice queue              */
    short nVoices;          /* Specifies the number of voices available */
    short nBytes;           /* Specifies the number of bytes to be
                             * allocated to the voice queue           */
   
    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Window",
                        (LPSTR)"SetVoiceQueueSize",
                        WS_TILEDWINDOW,
                        20,20,400,200,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the application is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );
    /*********************************************************************/

    nVoices = OpenSound ();   /* Must open access to the play device before
			       * SetVoiceQueueSize can be invoked */

    /* Allocate nBytes for the voice queue specified by nVoice */
    nVoice = 1;
    nBytes = 300;	 /* Default is 192 bytes, room for about 32 notes */
    nResult = SetVoiceQueueSize (nVoice, nBytes);
	   /* The return value is zero if the function is successful.  On error
	    * it is one of the following:
	    *	    S_SEROFM	Out of memory
	    *	    S_SERMACT	Music active	 */

    CloseSound(); /* CloseSound must be invoked to allow other applications
		   * access to the play device */

    /*********************************************************************/
    /* return code for SetVoiceQueueSize routine */
    if (nResult == 0)                           /* Function is successful */
      {  MessageBox (hWnd, (LPSTR)"300 bytes are allocated to voice queue number one",
                    (LPSTR)"Done", MB_OK);
      }
    else if (nResult == S_SEROFM)     /* If nResult == S_SERTOFM then out of memory */
      {  MessageBox (hWnd, (LPSTR)"Out of memory",
                    (LPSTR)NULL, MB_OK);
      }
    else
      {  MessageBox (hWnd, (LPSTR)"Music active", /* If nResult = S_SERMACT then music active */
                    (LPSTR)NULL, MB_OK);
      }
    return 0;
}
