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

    BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
    TextOut( ps.hdc,
             (short)10,
             (short)10,
             (LPSTR)szMessage,
             (short)MessageLength );
    EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
}
/*
*/
