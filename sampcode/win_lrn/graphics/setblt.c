/*
 *
 *  SetStretchBltMode
 *  setblt
 *
 *  SetStretchBltMode sets the stretching mode for the StretchBlt function.
 *  The stretching mode defines which scan lines and/or columns
 *  StretchBlt eliminates when contracting a bitmap.
 *
 *  This program demonstrates the use of the function SetStretchBltMode.
 *  The stretch mode is set in one statement.  The SetStretchBltMode function
 *  is passed two parameters: a handle to a display context,(ie. hDC) and a
 *  new stretching mode (ie. WHITEONBLACK, BLACKONWHITE, or COLORONCOLOR).
 *  SetStretchBltMode returns the previous stretching mode.
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
    HWND  hWnd;             /* Handle to the parent window    */
    short nOldStretchMode;  /* Return value for the SetStretchBltMode Routine */
    HDC   hDC;              /* display context of client area */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Window",
                        (LPSTR)"SetStretchBltMode",
                        WS_TILEDWINDOW,
                        20,20,400,200,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow(
    hWnd );
    /**************************************************************************/
    hDC = GetDC (hWnd); 	       /* Get a handle to the Display Context */

    /* Set the stretching mode for the StretchBlt function and return the
     * previous stretching mode in a (short) variable. */
    nOldStretchMode = SetStretchBltMode ( hDC, WHITEONBLACK);

    ReleaseDC (hWnd, hDC);	      /* Release handle to Display Context */
    /*************************************************************************/
    
    /* return codes for SetStretchBltMode routine */
    if (nOldStretchMode == WHITEONBLACK)
      {  MessageBox (hWnd, (LPSTR)"The stretching mode is set. The previous mode was WHITONBLACK",
		     (LPSTR)"Done", MB_OK);
      }
    else if (nOldStretchMode == BLACKONWHITE)
	   {  MessageBox (hWnd, (LPSTR)"The stretching mode is changed. The previous mode was BLACKONWHITE",
			  (LPSTR)"Done", MB_OK);
	   }
    else if (nOldStretchMode == COLORONCOLOR)
	  {   MessageBox (hWnd, (LPSTR)"The stretching mode is changed. The previous mode was COLORONCOLOR",
			  (LPSTR)"Done", MB_OK);
	  }
    return 0;
}
