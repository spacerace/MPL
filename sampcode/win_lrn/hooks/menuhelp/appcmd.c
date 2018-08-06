#include "windows.h"
#include "APP.h"

void APPCommand( HWND, WORD, LONG );


void APPCommand( hWnd, wParam, lParam )
HWND hWnd;
WORD wParam;
LONG lParam;
{
        MessageBox ( hWnd, (LPSTR)"Item Selected!",
                           (LPSTR)"Menu", MB_OK );
        return;
}