/*
 *  Function Name:   DlgDirSelect
 *  Program Name:    ddirsel.c
 *  Author:          bertm
 *  Date Completed:  2-Feb-88
 *  Special Notes:
 *
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   This program allows the user to select filenames or directories from
 *   the list box. The program below will display the current directory
 *   and allow the user to select the filenames.  The program will not
 *   select directories (demonstrating the return value).
 *
 *   Microsoft Product Support Services
 *   Windows Version 2.0 function demonstration application
 *   Copyright (c) Microsoft 1988
 *
 */

#include "windows.h"
#include "ddirsel.h"

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
  char  str[128];

  switch (message)
  {
    case WM_INITDIALOG:
      DlgDirList(hDlg, (LPSTR) "*.*", ID_LISTBOX, NULL, 0x4010);
      SetDlgItemText(hDlg, ID_EDIT, (LPSTR) "");
      return TRUE;
      break;

    case WM_COMMAND:
      switch (wParam)
      {
        case ID_LISTBOX:
          switch (HIWORD(lParam))
          {
            case LBN_SELCHANGE:
                                         /* filenames only chosen   */
             if (!DlgDirSelect(hDlg, (LPSTR)str, ID_LISTBOX))
               SetDlgItemText(hDlg, ID_EDIT, (LPSTR)str);
              return TRUE;
              break;

            default:
              return FALSE;
          }
        case IDCANCEL:
          EndDialog(hDlg, TRUE);
          break;

        default:
          return FALSE;
      }
    default:
      return FALSE;
  }
  return TRUE;              /*  return TRUE if message is processed by us  */
}

/**************************************************************************/

void CALL_DlgDirSelect(hWnd)
HWND   hWnd;
{
  FARPROC lpprocDialogBox;
                    /*  Bind callback function with module instance  */
    lpprocDialogBox = MakeProcInstance ((FARPROC) DialogProc, hInst);
                    /*  Create a modal dialog box  */
    DialogBox (hInst, MAKEINTRESOURCE(DIALOGID), hWnd, lpprocDialogBox);
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
    wcClass.lpszClassName  = (LPSTR)"DlgDirSelect";

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
    HMENU hMenu;

    if (!hPrevInstance)
        {
        /* Call initialization procedure if this is the first instance */
        if (!WinInit( hInstance ))
            return FALSE;
        }

    hWnd = CreateWindow((LPSTR)"DlgDirSelect",
                        (LPSTR)"DlgDirSelect()",
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
            CALL_DlgDirSelect(hWnd);
            break;

          default:
            return DefWindowProc( hWnd, message, wParam, lParam );
            break;
        }

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        ValidateRect(hWnd, (LPRECT) NULL);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}


