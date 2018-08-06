/*
 *  Function Name:   EnableMenuItem
 *  Program Name:    enmenuit.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   The program below allows a menu item to be grayed, enabled, or disabled.
 *   The following program will gray a menu item.
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_EnableMenuItem(hWnd)
HWND hWnd;
{
  HMENU hCurrentWindowMenu, hSubItem;
  unsigned wIDNewItem1, wIDNewItem2;
  BOOL  bPrevState;

  hCurrentWindowMenu = CreateMenu();
  hSubItem           = CreateMenu();
                                              /* menu heading created. */
  ChangeMenu (hCurrentWindowMenu, NULL, (LPSTR) "Heading",
              hSubItem, MF_APPEND | MF_BYPOSITION | MF_POPUP);

                                             /* 2 items added under Heading */
  ChangeMenu (hSubItem, NULL, (LPSTR ) "Item1", wIDNewItem1,
               MF_APPEND | MF_BYCOMMAND | MF_STRING);
  ChangeMenu (hSubItem, NULL, (LPSTR) "Item2", wIDNewItem2,
               MF_APPEND | MF_BYCOMMAND | MF_STRING);

  SetMenu(hWnd, hCurrentWindowMenu);

  bPrevState = EnableMenuItem(hCurrentWindowMenu, wIDNewItem1, MF_GRAYED);

/*  if (bPrevState == 0 or 1 or 2)         0 = ENABLED or error (as in
                                              BYPOSITION invalid number)
                                          1 = GRAYED
                                          2 = DISABLED
  {
    MessageBox(NULL,(LPSTR)"EnableMenuItem failed",(LPSTR)"ERROR",MB_ICONHAND);
    return FALSE;
  }
*/

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
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"EnableMenuItem";

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

    hWnd = CreateWindow((LPSTR)"EnableMenuItem",
                        (LPSTR)"EnableMenuItem()",
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

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        CALL_EnableMenuItem(hWnd);
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
