/*
 *  SetSoundNoise
 *  setsndns
 *
 *  SetSoundNoise(nSource, nDuration) sets the source and duration of a
 *  noise in the noise hardware of the play device.  nSource is a short
 *  integer specifying the noise source.  nDuration is a short integer value
 *  specifying the duration of the noise in clock tics.
 *
 *  This program demonstrates the use of the function SetSoundNoise.
 *  As shown below, OpenSound must be invoked before SetSoundNoise can
 *  be called.	SetSoundNoise(nSource, nDuration) returns zero if the
 *  function is successful.  If the source is invalid, it returns S_SERDSR.
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
    HWND  hWnd;      /* Handle to the parent window */
    short nResult;   /* Contains return value for SetSoundNoise function */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Window",
			(LPSTR)"SetSoundNoise",
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

    OpenSound ();   /* Must open access to the play device before
		     * SetSoundNoise can be invoked */

    /*********************************************************************/
    /* S_PERIOD512 is the noise source (high pitch hiss).  3000 is the
     * duration of the noise in clock tics. */

    nResult = SetSoundNoise(S_PERIOD512,3000); /* nResult = 0 if successful */

    /*********************************************************************/

    CloseSound();   /* Close access to play device */

    /* return code for SetSoundNoise routine */

    if (nResult==0)
	MessageBox (hWnd, (LPSTR)"SetSoundNoise worked",
		   (LPSTR)"Done", MB_OK);

    else if (nResult == S_SERDSR)
	MessageBox (hWnd, (LPSTR)"The noise source parameter is invalid",
		   (LPSTR)NULL, MB_OK);

    return 0;
}
