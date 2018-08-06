#include "windows.h"
#include "APP.h"

extern char szMessage[];
extern int MessageLength;

void APPPaint( HWND, WORD, LONG );


void APPPaint(  hWnd, wParam, lParam )
HWND hWnd;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;
    RECT rRect;

    BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
/*
    GetClientRect( hWnd, (LPRECT)&rRect );
    ScrollWindow( hWnd, 0, -rRect.bottom, (LPRECT)NULL, (LPRECT)NULL);
*/
    EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
}
/*
*/
