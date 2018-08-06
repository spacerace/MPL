#include "windows.h"
#include "APP.h"
#include "stdlib.h"
#include "string.h"

/* Static definitions */
FARPROC lpprocHook;
FARPROC lplpfnNextHook;

/* Keep track of the application's window handle for hook processing */
HWND hApp;

/* Function proto-types */
void FAR PASCAL InitHook( HWND );
BOOL FAR PASCAL KillHook();
DWORD FAR PASCAL Hook( int, WORD, LPMSG );

extern HANDLE hInst;

extern APPCommand( HWND, WORD, LONG );
extern APPPaint  ( HWND, WORD, LONG );

long FAR PASCAL WndProc( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL About( HWND, unsigned, WORD, LONG );


long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpprocAbout;

    switch (message)
    {
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case IDSABOUT:
            /* Bind callback function with module instance */
            lpprocAbout = MakeProcInstance( (FARPROC)About, hInst);
            DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
            FreeProcInstance( (FARPROC)About );
            break;
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
        }
        break;

    case WM_COMMAND:
        switch (wParam) {
           case IDAPPMENU:
                APPCommand( hWnd, wParam, lParam );
                break;
           default:
                return DefWindowProc( hWnd, message, wParam, lParam );
        }
        break;

    case WM_CREATE:
        InitHook( hWnd );
        break;

    case WM_DESTROY:
        KillHook();
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
        APPPaint( hWnd, wParam, lParam );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
/*
*/


BOOL FAR PASCAL About( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    if (message == WM_COMMAND) {
        EndDialog( hDlg, TRUE );
        return TRUE;
        }
    else if (message == WM_INITDIALOG)
        return TRUE;
    else return FALSE;
}
/*
*/


/* Set the HOOK */
void FAR PASCAL InitHook( hWnd )
HWND hWnd;
{
        hApp = hWnd;

        lpprocHook = MakeProcInstance( (FARPROC)Hook, hInst);
        lplpfnNextHook = SetWindowsHook( WH_MSGFILTER, lpprocHook );

        return;
}
/*
*/

/* Remove the HOOK */
BOOL FAR PASCAL KillHook()
{
        BOOL fSuccess;

        fSuccess = UnhookWindowsHook( WH_MSGFILTER, lpprocHook );
        FreeProcInstance( lpprocHook );

        return fSuccess;
}
/*
*/

/* Hook function */
DWORD FAR PASCAL Hook( nCode, wParam, lParam )
int nCode;
WORD wParam;
LPMSG lParam;
{
        HDC hDC;
        TEXTMETRIC tmFontInfo;
        RECT rRect;
        int TextHeight;
        WORD y;

        if ((nCode != MSGF_MENU) || (lParam->message != WM_MENUSELECT) ) {
            DefHookProc( nCode, wParam, lParam, (FARPROC FAR *)&lplpfnNextHook );
            return 0;
        }

        hDC = GetDC( hApp );
        GetTextMetrics( hDC, (LPTEXTMETRIC)&tmFontInfo );
        TextHeight = tmFontInfo.tmExternalLeading + tmFontInfo.tmHeight;
        GetClientRect( hApp, (LPRECT)&rRect );
        y = rRect.bottom - TextHeight;
        switch (lParam->wParam) {
           case SC_CLOSE:
              TextOut( hDC, 0, y, (LPSTR)"Close the application ", 22 );
              break;
           case SC_MAXIMIZE:
              TextOut( hDC, 0, y, (LPSTR)"Enlarge to full size  ", 22 );
              break;
           case SC_MINIMIZE:
              TextOut( hDC, 0, y, (LPSTR)"Reduce window to icon ", 22 );
              break;
           case SC_MOVE:
              TextOut( hDC, 0, y, (LPSTR)"Change window position", 22 );
              break;
           case SC_RESTORE:
              TextOut( hDC, 0, y, (LPSTR)"Restore to normal size", 22 );
              break;
           case SC_SIZE:
              TextOut( hDC, 0, y, (LPSTR)"Change window size    ", 22 );
              break;
           
           case IDAPPMENU:
              TextOut( hDC, 0, y, (LPSTR)"Item 1 highlighted    ", 22 );
              break;
           case IDAPPMENU+1:
              TextOut( hDC, 0, y, (LPSTR)"Item 2 highlighted    ", 22 );
              break;
           case IDAPPMENU+2:
              TextOut( hDC, 0, y, (LPSTR)"Item 3 highlighted    ", 22 );
              break;
           case IDHELP:
              TextOut( hDC, 0, y, (LPSTR)"HELP the poor user    ", 22 );
              break;
           case IDSABOUT:
              TextOut( hDC, 0, y, (LPSTR)"Program information   ", 22 );
              break;

           default:
              TextOut( hDC, 0, y, (LPSTR)"                      ", 22 );
              break;
        }
        ReleaseDC( hApp, hDC );
        DefHookProc( nCode, wParam, lParam, (FARPROC FAR *)&lplpfnNextHook );
        return 0;
}
