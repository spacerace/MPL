/*
 *
 *  GetCaretBlinkTime
 *
 *  This program demonstrates the use of the function GetCaretBlinkTime.
 *  GetCaretBlinkTime returns the delay in milliseconds between each blink
 *  of the caret. So a big Blink Time means a slow blink speed. GetCaret-
 *  BlinkTime is called from WinMain in this application.
 *
 */

#include <stdio.h>
#include <windows.h>

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon	   = LoadIcon (hInstance, (LPSTR)NULL);
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Setcarbt";
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
    HWND	 hWnd;		 /*    Handle to the parent window	*/
    WORD	 wBlinkDelay;	 /* return value from GetCaretBlinkTime */
    char	 szBuff[30];	 /*	  buffer for message box	*/

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setcarbt",
			(LPSTR)"GetCaretBlinkTime",
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
   
    /* create a caret before showing it */
    CreateCaret (hWnd, NULL, 8, 12);
    SetCaretPos (50,50);    
    ShowCaret (hWnd);

/* get the delay in milliseconds of the caret blink speed */
    wBlinkDelay = (WORD)GetCaretBlinkTime();

    sprintf(szBuff,"%d milliseconds",wBlinkDelay);

    MessageBox(hWnd,(LPSTR)szBuff,
		    (LPSTR)"The blink delay is",
		    MB_OK);

    MessageBox(hWnd,(LPSTR)"speed the blink up",
		    (LPSTR)"I am going to...",
		    MB_OK);

    SetCaretBlinkTime (GetCaretBlinkTime()/5);	 /* cut the delay down */

/* get the delay in milliseconds of the caret blink speed */
    wBlinkDelay = (WORD)GetCaretBlinkTime();

    sprintf(szBuff,"%d milliseconds",wBlinkDelay);

    MessageBox(hWnd,(LPSTR)szBuff,
		    (LPSTR)"The blink delay is",
		    MB_OK);

    return 0;
}
