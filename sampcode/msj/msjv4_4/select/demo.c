#include <windows.h>
#include "select.h"
#include <stdio.h>
#include <string.h>
#include "demo.h"

HWND hWndDemo;
HANDLE hInstDemo;

static char szClassName[] = "demo";
static int nTrueFalse = 0;
static int nString = 0;

int PASCAL WinMain ( HANDLE hInstance, HANDLE hPrevInstance,
    LPSTR lpszCmdLine, int cmdShow );
long FAR PASCAL DemoWndProc ( HWND hWnd, unsigned message,
    WORD wParam, LONG lParam );
void DemoCommand ( HWND hWnd, WORD wCmd );
BOOL FAR PASCAL DemoDlg ( HWND hWndDlg, unsigned message, 
    WORD wParam, LONG lParam );


int PASCAL WinMain ( HANDLE hInstance, HANDLE hPrevInstance,
    LPSTR lpszCmdLine, int cmdShow )
{
    MSG msg;
    WNDCLASS Class;

    if (hPrevInstance) return TRUE;
    Class.hCursor       = LoadCursor(NULL,IDC_ARROW);
    Class.hIcon         = NULL;
    Class.cbClsExtra    = 0;
    Class.cbWndExtra    = 0;
    Class.lpszMenuName  = szClassName;
    Class.lpszClassName = szClassName;
    Class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);;
    Class.hInstance     = hInstance;
    Class.style         = CS_VREDRAW | CS_HREDRAW;
    Class.lpfnWndProc   = DemoWndProc;
   if (!RegisterClass(&Class)) return FALSE;

    hWndDemo = CreateWindow(
        szClassName, szClassName, WS_OVERLAPPEDWINDOW,
        30, 50, 200, 150,
        NULL, NULL, hInstDemo=hInstance, NULL);

    ShowWindow ( hWndDemo, cmdShow );
    UpdateWindow ( hWndDemo );
    
    while (GetMessage( &msg, NULL, 0, 0 ))    {
         TranslateMessage((LPMSG)&msg);
         DispatchMessage((LPMSG)&msg);
    }
        
    return msg.wParam;
}

long FAR PASCAL DemoWndProc ( HWND hWnd, unsigned message,
    WORD wParam, LONG lParam )
{
    switch (message) {

    case WM_COMMAND:
        DemoCommand ( hWnd, wParam );
        break;
    case WM_DESTROY:
        PostQuitMessage (0);
        break;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam) ;
    }
    return (0L) ;
}

void DemoCommand ( HWND hWnd, WORD wCmd )
{
    int i;
    HMENU hMenu;
    FARPROC lpfnDlg;

    switch (wCmd) {

    case IDM_TEST:
        lpfnDlg = MakeProcInstance( DemoDlg, hInstDemo );
        DialogBox( hInstDemo, "TEST", hWnd, lpfnDlg );
        FreeProcInstance ( lpfnDlg );
        break;

    default:
        break;
    }
}


BOOL FAR PASCAL DemoDlg ( HWND hWndDlg, unsigned message, 
    WORD wParam, LONG lParam )
{
    char szBuf[40];
    int i;
    switch ( message ) {
    case WM_INITDIALOG:
        for ( i = IDS_COLOR1 ; i <= IDS_COLOR8 ; i++ ) {
            LoadString( hInstDemo, i, szBuf, 40 );
            SendDlgItemMessage( hWndDlg, IDD_COLORS, SL_ADDSTRING,
                0, (LONG)(LPSTR)szBuf );
        }    
        for ( i = IDS_STRING1 ; i <= IDS_STRING5 ; i++ ) {
            LoadString( hInstDemo, i, szBuf, 40 );
            SendDlgItemMessage( hWndDlg, IDD_STRINGS , SL_ADDSTRING,
                0, (LONG)(LPSTR)szBuf);
        }    
        SendDlgItemMessage( hWndDlg, IDD_COLORS, SL_SETCURSEL,
            nTrueFalse, 0L );
        SendDlgItemMessage( hWndDlg, IDD_STRINGS, SL_SETCURSEL,
            nString, 0L );
        return TRUE;

    case WM_COMMAND:
        if (wParam == IDD_OK) {
            nTrueFalse = (int)SendDlgItemMessage( hWndDlg, IDD_COLORS,
                SL_GETCURSEL, 0, 0L );
            nString = (int)SendDlgItemMessage( hWndDlg, IDD_STRINGS,
                SL_GETCURSEL, 0, 0L );
            EndDialog( hWndDlg, TRUE );
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}
