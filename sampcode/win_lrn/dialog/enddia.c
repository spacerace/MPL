/*
 *  Function Name:   EndDialog
 *  Program Name:    enddia.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   This function frees resources and reactivates the parent window
 *   while destroying the dialog box.
 */

#include "windows.h"
#include "enddia.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

HANDLE   hInst;

/**************************************************************************/

/* Window procedure for the dialog box */
BOOL FAR PASCAL DialogProc (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
  if (message == WM_COMMAND)           /* check for WM_COMMAND            */
    EndDialog(hDlg, TRUE);            /* free resources                   */
  else
    return FALSE;
}

/**************************************************************************/

void CALL_EndDialog(hWnd)
HWND   hWnd;
{
  FARPROC lpprocDialogBox;
                    /*  Bind calback function with module instance  */
    lpprocDialogBox = MakeProcInstance ((FARPROC) DialogProc, hInst);
                    /*  Create a modal dialog box  */
    DialogBox (hInst, MAKEINTRESOURCE(IDD_10), hWnd, lpprocDialogBox);
    FreeProcInstance ((FARPROC) lpprocDialogBox);

  return;
}

/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra     =0;
    wcClass.cbWndExtra     =0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon( hInstance,NULL );
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.lpszMenuName   = (LPSTR)"MenuName";
    wcClass.lpszClassName  = (LPSTR)"EndDialog";

    if (!RegisterClass( (LPWNDCLASS)&wcClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance)
        {
        /* Call initialization procedure if this is the first instance */
        if (!WinInit( hInstance ))
            return FALSE;
        }

    hWnd = CreateWindow((LPSTR)"EndDialog",
                        (LPSTR)"EndDialog()",
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

     hInst = hInstance;

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
        {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }
    return (int)msg.wParam;
}

/* Procedures which make up the window class. */
long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
      case WM_COMMAND:
        switch (wParam)
        {
          case IDM_1:
            CALL_EndDialog(hWnd);
            break;

          default:
            return DefWindowProc( hWnd, message, wParam, lParam );
            break;
        }

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}























c
