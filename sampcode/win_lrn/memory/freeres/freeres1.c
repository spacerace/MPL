/*
 *
 *   FreeResource
 *   freeres1.c travisd, v1.00, 29-Dec-1987
 *
 *   This program demonstrates the use of the function FreeResource.
 *   FreeResource removes a loaded resource from memory by freeing the
 *   alllocated memory occupied by that resource.
 *
 *   Microsoft Product Support Services
 *   Windows Version 2.0 function demonstration application
 *   Copyright (c) Microsoft 1987
 *
 */

#include "windows.h"
#include "freeres1.h"
#include "string.h"

char szAppName[10];
static HANDLE hInst;

/* Forward-Reference All Functions */

long FAR PASCAL FreeRes1WndProc(HWND, unsigned, WORD, LONG);

/* --------------------------------------------------------------------- */

void FreeRes1Paint(hWnd, hDC )
HWND hWnd;
HDC  hDC;

{
    GLOBALHANDLE hBitmap1Data; /* Handle to global memory containing bitmap */
    HBITMAP hBitmap1Info;
    BOOL nResult;

/***************************************************************************/
/* This section demonstrates the use of FreeResource */

   /* Locate the bitmap in the resource file and identify it with a handle */
    hBitmap1Info = FindResource(hInst, "face1", RT_BITMAP);

            /* Identify the global memory block to receive the bitmap data */
    hBitmap1Data = LoadResource(hInst, hBitmap1Info);

    nResult = FreeResource(hBitmap1Info);         /* Free allocated memory */
    if (nResult = FALSE)
       {
       TextOut( hDC, 10, 10, (LPSTR)"FreeResource did not work",
                strlen("FreeResource did not work") );
       }
    else
       {
       TextOut( hDC, 10, 10, (LPSTR)"FreeResource worked",
                strlen("FreeResource worked") );
       }

/* End of the FreeResource demonstration section */
/***************************************************************************/
}
/* End FreeRes1Paint */
/* --------------------------------------------------------------------- */

/* Procedure called when the application is loaded for the first time */
BOOL FreeRes1Init( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pFreeRes1Class;

    /* Load strings from resource */
    LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );

    pFreeRes1Class = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pFreeRes1Class->hCursor       = LoadCursor( NULL, IDC_ARROW );
    pFreeRes1Class->hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE(FREERES1ICON) );
    pFreeRes1Class->lpszMenuName  = (LPSTR)NULL;
    pFreeRes1Class->lpszClassName = (LPSTR)szAppName;
    pFreeRes1Class->hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pFreeRes1Class->hInstance     = hInstance;
    pFreeRes1Class->style         = CS_HREDRAW | CS_VREDRAW;
    pFreeRes1Class->lpfnWndProc   = FreeRes1WndProc;

    if (!RegisterClass( (LPWNDCLASS)pFreeRes1Class ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pFreeRes1Class );
    return TRUE;        /* Initialization succeeded */
}
/* End FreeRes1Init */
/* --------------------------------------------------------------------- */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance) {
        /* Call initialization procedure if this is the first instance */
        if (!FreeRes1Init( hInstance ))
            return FALSE;
        }
    else {
        /* Copy data from previous instance */
        GetInstanceData( hPrevInstance, (PSTR)szAppName, 10 );
        }
    hWnd = CreateWindow((LPSTR)szAppName,
                        (LPSTR)"FreeResource",
                        WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
                        10,              /*  x - ignored for tiled windows */
                        10,              /*  y - ignored for tiled windows */
                        CW_USEDEFAULT,   /* cx - ignored for tiled windows */
                        CW_USEDEFAULT,   /* cy - ignored for tiled windows */
                        (HWND)NULL,      /* no parent */
                        (HMENU)NULL,     /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Save instance handle for DialogBox */
    hInst = hInstance;

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
/* End WinMain */
/* --------------------------------------------------------------------- */

/* Procedures which make up the window class. */
long FAR PASCAL FreeRes1WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        FreeRes1Paint(hWnd, ps.hdc );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}

/* End FreeRes1WndProc */
/* --------------------------------------------------------------------- */
