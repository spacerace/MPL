/*
 *  SetBKColor
 *  setbkcol.c,
 *
 *  This program demonstrates the use of the function SetBkColor.  The
 *  change in the backround color is demonstrated using the TextOut function.
 *  The color of the text is also changed so that it can be seen against
 *  against the new backround color.
 *
 */

#include "windows.h"
#define ERR 0x80000000 /* Error value returned by SetBkColor
			* if an error occurs */

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
    HWND  hWnd; 		       /* Handle to the parent window  */
    HDC   hDC;		      /* Handle to the display context of client area */
    DWORD rgbcolor;	      /* Return value from SetTextColor routine */
    DWORD black = RGB(0x00,0x00,0x00); /* Color of text backround      */
    DWORD white = RGB(0xff,0xff,0xff); /* Color of the output text     */
    long  lpReturn;		       /* Return value for SetBkColor  */


    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Window",
			(LPSTR)"SetBkColor",
                        WS_TILEDWINDOW,
                        20,20,400,200,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    hDC = GetDC (hWnd);

		   /* The following text will be black so it can be seen
		    * on the white backround				 */
    TextOut(hDC,
	    0,
	    0,
	    (LPSTR) "The backround color is white before invoking SetBkColor.",
	    strlen ("The backround color is white before invoking SetBkColor.")
	    );

/**************************************************************************/
/* SetBkColor sets the backround color of future text output to black.
 * The parameters needed are a handle to a display context (hDC) and
 * and an RGB color value - black in this case				*/

    lpReturn  = SetBkColor(hDC,black); /* lpReturn contains 80000000H if
					* an error occurs */

/**************************************************************************/

    SetTextColor(hDC,white); /* Set the text color to white to
			      * so the text can be seen on the
			      * black backround */

    TextOut(hDC,
	    0,
	    15,
	    (LPSTR) "The backround color is black after invoking SetBkcolor.",
	    strlen ("The backround color is black after invoking SetBkcolor.")
	   );
    ReleaseDC (hWnd, hDC);

    if (lpReturn = ERR)
      MessageBox (hWnd, (LPSTR)"The backround color is changed. No errors",
		  (LPSTR)"Done", MB_OK);

    return 0;
}
