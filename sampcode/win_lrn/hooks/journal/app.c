/*  APP.c
    Skeleton Application
    Windows Toolkit Version 2.00
    Copyright (c) Microsoft 1987
*/

#include "windows.h"
#include "APP.h"

char szAppName[10];
char szAbout[10];
char szMessage[15];
int MessageLength;
HANDLE hInst;
FARPROC lpprocAbout;

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About( HWND, unsigned, WORD, LONG );


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;

    if (!hPrevInstance) {
        /* Call initialization procedure if this is the first instance */
        if (!Init( hInstance ))
            return FALSE;
        }
    else {
        /* ONLY ONE INSTANCE */
        return FALSE;
        }

    hWnd = CreateWindow((LPSTR)szAppName,
                        (LPSTR)szMessage,
                        WS_OVERLAPPEDWINDOW,
                        400,               /*  x - ignored for tiled windows */
                        0,                 /*  y - ignored for tiled windows */
                        CW_USEDEFAULT,     /* cx - ignored for tiled windows */
                        0,                 /* cy - ignored for tiled windows */
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
/*
*/


/* Procedure called when the application is loaded for the first time */
BOOL Init( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pAPPClass;

    /* Load strings from resource */
    LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );
    LoadString( hInstance, IDSABOUT, (LPSTR)szAbout, 10 );
    MessageLength = LoadString( hInstance, IDSTITLE, (LPSTR)szMessage, 15 );

    pAPPClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pAPPClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pAPPClass->hIcon          = LoadIcon( hInstance, MAKEINTRESOURCE(APPICON) );
    pAPPClass->lpszMenuName   = (LPSTR)szAppName;
    pAPPClass->lpszClassName  = (LPSTR)szAppName;
    pAPPClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pAPPClass->hInstance      = hInstance;
    pAPPClass->style          = CS_HREDRAW | CS_VREDRAW;
    pAPPClass->lpfnWndProc    = WndProc;

    if (!RegisterClass( (LPWNDCLASS)pAPPClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pAPPClass );
    return TRUE;        /* Initialization succeeded */
}
/*
*/
