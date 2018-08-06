/*
 *  Function Name:   EnumFonts
 *  Program Name:    enfonts.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   This program will enumerate one font for each available typeface.
 *   The alphabet of each font will be displayed.
 */

#include "windows.h"
#include "stdio.h"
#include "string.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

HWND   hWndMain;            /*  need global handle to window   */
HANDLE hInst;

/************************************************************************/

short FAR PASCAL EnumProc(lpLogFont, lpTextMetric, FontType, lpData)
LPLOGFONT lpLogFont;
LPTEXTMETRIC lpTextMetric;
short FontType;
LPSTR lpData;
{
     HFONT hnewFont, holdFont;
     HDC   theDC;
     char  messageStr[256];
     static int row=10;

  theDC = GetDC(hWndMain);
  hnewFont = CreateFontIndirect(lpLogFont);
  holdFont = SelectObject(theDC,hnewFont);       /* load new font  */
  GetTextFace(theDC, 100, (LPSTR)messageStr);
  sprintf (messageStr + strlen(messageStr), " Height: %d Width %d",
           lpLogFont->lfHeight, lpLogFont->lfWidth);
  TextOut(theDC,10,row,(LPSTR) messageStr, strlen(messageStr));
  row += lpLogFont->lfHeight;             /*  advance line   */
  DeleteObject(holdFont);
  ReleaseDC(hWndMain, theDC);

  return (1);                    /* return value is user defined  */
}

/***********************************************************************/

void CALL_EnumFonts(hDC)
HDC hDC;
{
  FARPROC lpprocEnumFonts;

    lpprocEnumFonts = MakeProcInstance ((FARPROC) EnumProc, hInst);
    EnumFonts (hDC, (LPSTR)NULL, lpprocEnumFonts, 0L);
    FreeProcInstance ((FARPROC) lpprocEnumFonts);

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
    wcClass.lpszClassName  = (LPSTR)"EnumFonts";

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

    hWnd = CreateWindow((LPSTR)"EnumFonts",
                        (LPSTR)"EnumFonts()",
                        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    hWndMain = hWnd;
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

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        CALL_EnumFonts(ps.hdc);
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
