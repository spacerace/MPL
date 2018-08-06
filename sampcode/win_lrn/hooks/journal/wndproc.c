#include "windows.h"
#include "APP.h"

extern HANDLE hInst;

extern APPPaint  ( HWND, WORD, LONG );

/* Function proto-types of Journal control functions in DLL */
void FAR PASCAL StartRecord();
void FAR PASCAL StopRecord();
void FAR PASCAL StartPlay();
void FAR PASCAL StopPlay();

long FAR PASCAL WndProc( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL About( HWND, unsigned, WORD, LONG );

long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpprocAbout;
    HMENU hMenu;

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
           case IDRECORDON:
                MessageBox( hWnd, (LPSTR)"About to Record!", (LPSTR)"WARNING!", MB_OK );
                StartRecord();
                break;
           case IDRECORDOFF:
                StopRecord();
                break;
           case IDPLAY:
                MessageBox( hWnd, (LPSTR)"About to Play!", (LPSTR)"WARNING!", MB_OK );
                StartPlay();
                break;

           default:
                return DefWindowProc( hWnd, message, wParam, lParam );
        }
        break;

    case WM_DESTROY:
        /* Just in case.  Make sure un-hooked before app dies */
        StopRecord();
        StopPlay();
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
