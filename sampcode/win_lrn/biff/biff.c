/*  biff.c
    biff Application
    Windows Toolkit Version 2.10
    Copyright (c) Microsoft 1989 */

/*  This IS the old HELLO application with
    just one piece of code added to bring 
    up a dialog box when a key is pressed.
    See bigg.c for more information.
 */

#include "windows.h"
#include "biff.h"

char szAppName[10];
char szAbout[10];
char szMessage[20];
int MessageLength;

HWND    hWnd;
HANDLE  hInst;
FARPROC lpprocAbout;

extern  void FAR GetBiff(HWND, HANDLE);

long FAR PASCAL BiffWndProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL BiffDlgProc(HWND, WORD, WORD, LONG);

BOOL FAR PASCAL About( HWND hDlg, WORD wMessage, WORD wParam, LONG lParam )
{
    switch (wMessage) {
        case WM_INITDIALOG:
            break;
        case WM_COMMAND:
            EndDialog(hDlg, TRUE);
            break;
        default:
            return FALSE;
            break;
        }
    return(TRUE);
}


void BiffPaint( hDC )
HDC hDC;
{
    TextOut( hDC, 10, 10, 
             (LPSTR)"Press any key to bring up dialog.", 33 );
}


/* Procedure called when the application is loaded for the first time */
BOOL BiffInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pbiffClass;

    /* Load strings from resource */
    LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );
    LoadString( hInstance, IDSABOUT, (LPSTR)szAbout, 10 );
    MessageLength = LoadString( hInstance, IDSTITLE, (LPSTR)szMessage, 15 );

    pbiffClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pbiffClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pbiffClass->hIcon          = LoadIcon( hInstance, MAKEINTRESOURCE(BIFFICON) );
    pbiffClass->lpszMenuName   = (LPSTR)NULL;
    pbiffClass->lpszClassName  = (LPSTR)szAppName;
    pbiffClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pbiffClass->hInstance      = hInstance;
    pbiffClass->style          = CS_HREDRAW | CS_VREDRAW;
    pbiffClass->lpfnWndProc    = BiffWndProc;

    if (!RegisterClass( (LPWNDCLASS)pbiffClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pbiffClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HMENU hMenu;

    if (!hPrevInstance) {
        /* Call initialization procedure if this is the first instance */
        if (!BiffInit( hInstance ))
            return FALSE;
        }
    else {
        /* Copy data from previous instance */
        GetInstanceData( hPrevInstance, (PSTR)szAppName, 10 );
        GetInstanceData( hPrevInstance, (PSTR)szAbout, 10 );
        GetInstanceData( hPrevInstance, (PSTR)szMessage, 15 );
        GetInstanceData( hPrevInstance, (PSTR)&MessageLength, sizeof(int) );
        }

    hWnd = CreateWindow((LPSTR)szAppName,
                        (LPSTR)szMessage,
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Save instance handle for DialogBox */
    hInst = hInstance;

    /* Insert "About..." into system menu */
    hMenu = GetSystemMenu(hWnd, FALSE);
    ChangeMenu(hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu, 0, (LPSTR)szAbout, IDSABOUT, MF_APPEND | MF_STRING);

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long FAR PASCAL BiffWndProc( HWND hWnd, WORD wMessage, WORD wParam, LONG lParam )
{
    PAINTSTRUCT ps;

    switch (wMessage)
    {
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case IDSABOUT:
            lpprocAbout = MakeProcInstance( (FARPROC)About, hInst );
            DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
            FreeProcInstance(lpprocAbout);
            break;
        default:
            return DefWindowProc( hWnd, wMessage, wParam, lParam );
        }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_RBUTTONDOWN:
    case WM_CHAR:

        GetBiff(hWnd, hInst);
        break;

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        BiffPaint( ps.hdc );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, wMessage, wParam, lParam );
        break;
    }
    return(0L);
}
