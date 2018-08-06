/*
Date: 7/19/88
Function(s) demonstrated in this program: SetResourceHandler
Compiler version: 5.1
Description: This example is taken from Application Note 12 (Windows "Icon"
             utility). The SetResourceHandler should only be used with
             user defined resources. In this example a generic bitmap is
             read through ICONLIB.LIB and ICONLIB.EXE (both of this files are
             need) and displayed in ICONTEST as an icon. The function
             SetResourceHandler is used to have the user defined function
             ResourceHandler setup the bitmap as an icon when the bitmap
             is loaded.
*/

#include <windows.h>
#include <stdio.h>
#include "icontest.h"
 
typedef struct {
    short   csHotX;
    short   csHotY;
    short   csWidth;
    short   csHeight;
    short   csWidthBytes;
    short   csColor;
    char    csBits[ 1 ];
    } CURSORSHAPE;
 
typedef struct {
    short magic;
    CURSORSHAPE info;
    } CURSORFILEHEADER;
 
typedef CURSORFILEHEADER FAR *LPCURSOR;
 
static char szAppName [] = "ICONTEST" ;
static char szFuncName [] = "SetResourceHandler" ;
WORD hInst;
 
int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
    MSG        msg;
    WNDCLASS   wndclass;
    HWND       hWnd;
    HMENU      hMenu;
 
    if ( !hPrevInstance )
       {
       wndclass.style          = CS_HREDRAW | CS_VREDRAW;
       wndclass.lpfnWndProc    = WndProc;
       wndclass.cbWndExtra     = 0;
       wndclass.cbClsExtra     = 0;
       wndclass.hInstance      = hInstance;
       wndclass.hCursor        = LoadCursor( NULL, IDC_ARROW );
       wndclass.hIcon          = LoadIcon (NULL, IDI_APPLICATION);
       wndclass.hbrBackground  = GetStockObject( WHITE_BRUSH );
       wndclass.lpszMenuName   = "icontest";
       wndclass.lpszClassName  = szAppName;
    
      if ( !RegisterClass( &wndclass ) )
           return FALSE;
       }
 
    hMenu = LoadMenu( hInstance, (LPSTR)"icontest" );
    hInst = hInstance;
 
    hWnd = CreateWindow( szAppName,     /* window class name       */
            szFuncName,                 /* window caption          */
            WS_OVERLAPPEDWINDOW,        /* window style            */
            CW_USEDEFAULT,              /* initial x position      */
            0,                          /* initial y position      */
            CW_USEDEFAULT,              /* initial x size          */
            0,                          /* initial y size          */
            NULL,                       /* parent window handle    */
            hMenu,                      /* window menu handle      */
            hInstance,                  /* program instance handle */
            NULL );                     /* create parameters       */
                            
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );
 
    while ( GetMessage( ( LPMSG )&msg, NULL, 0, 0 ) )
       {
       TranslateMessage( ( LPMSG )&msg );
       DispatchMessage( ( LPMSG )&msg );
       }
    return( msg.wParam );
}
 
long FAR PASCAL WndProc( hWnd, iMessage, wParam, lParam )
HWND       hWnd;
unsigned   iMessage;
WORD       wParam;
LONG       lParam;
{
    HDC          hDC;
 
    switch ( iMessage )
    {
    case WM_INITMENU:
       InvalidateRect ( hWnd, NULL, TRUE );
       break;
      
    case WM_CLOSE:
       DestroyWindow ( hWnd );
       break;
 
    case WM_DESTROY:
       PostQuitMessage( 0 );
       break;
 
    case WM_COMMAND:
       IconTestCommand ( hWnd, wParam );
       break;
 
    default:
        return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
    }
    return( 0L );
}
 
/*
**  ResourceHandler - routine to service "calculated" resources.
**  In this case, Icons.
**
**  A resource handler can be installed for each resource TYPE.
*/
 
HANDLE FAR PASCAL ResourceHandler( hRes, hResFile, hResIndex )
HANDLE hRes;
HANDLE hResFile;
HANDLE hResIndex;
{
    WORD       wSize;
    BYTE FAR   *lpIcon;
    int        nIndex;
 
    /* Process machine dependent ICON and CURSOR resources. */
    wSize = ( WORD )SizeofResource( hResFile, hResIndex );
    nIndex = 0;
 
    if ( !hRes )
       {
       if ( !( hRes = AllocResource( hResFile, hResIndex, 0L ) ) )
           return NULL;
       nIndex = -1;
       }
 
    while ( !( lpIcon = ( BYTE FAR * )GlobalLock( hRes ) ) )
    if ( !GlobalReAlloc( hRes, ( DWORD )wSize, 0 ) )
        return NULL;
    else
        nIndex = -1;
 
    if ( nIndex )
    if ( ( nIndex = AccessResource( hResFile, hResIndex ) ) != -1 &&
        _lread( nIndex, lpIcon, wSize ) != -1 )
        _lclose( nIndex );
    else
        {
        if ( nIndex != -1 )
        _lclose( nIndex );
        GlobalUnlock( hRes );
        return NULL;
        }
 
    /* Remove magic word. */
    wSize -= 2;
    while ( wSize-- )
       {
       *lpIcon = *( lpIcon+2 );
       lpIcon++;
       }
 
    GlobalUnlock( hRes );
    GlobalReAlloc( hRes, ( DWORD )wSize, 0 );

    return ( hRes );
}
 
void IconTestCommand ( hWnd, wCommand )
HWND hWnd;
WORD wCommand;
{
    FARPROC lpResourceHandler;
    FARPROC lpOld;
    HDC     hDC;
    HICON   hIcon;
    HANDLE  hAppDLL;
    HANDLE  hModule;
    int     nDrawn;
   
 
    switch ( wCommand )
       {
       case CMD_GETICON:
          DummyEntry (  );
          hDC = GetDC ( hWnd );
          TextOut ( hDC, 10, 10, ( LPSTR ) "LibIcon", 7 );
   
          hAppDLL = LoadLibrary ( (LPSTR)"ICONLIB.LIB" );
   
          if ( hAppDLL < 32 )
             MessageBox ( hWnd, (LPSTR)"Loading DLL failed!",
                (LPSTR)szFuncName, MB_OK );
          else
             {
             hModule = GetModuleHandle ( ( LPSTR )"ICONLIB" );
             if ( hModule != NULL )
                {
                lpResourceHandler = MakeProcInstance (
                   (FARPROC)ResourceHandler, hInst );
                                                    /* Install own handler. */
                lpOld = SetResourceHandler( hModule, RT_ICON,
                   lpResourceHandler );
   
                hIcon = LoadIcon ( hModule, ( LPSTR ) "LibIcon" );
                nDrawn = DrawIcon ( hDC, 10, 25, hIcon );
   
                FreeProcInstance( SetResourceHandler( hModule, RT_ICON,
                   ( FARPROC )lpOld ) );
                }
             else 
                MessageBox ( hWnd, (LPSTR)"Resource Handlers not initialized",
                   (LPSTR)szFuncName, MB_OK );
   
             FreeLibrary ( hAppDLL );
             }
          ReleaseDC ( hWnd, hDC );
          break;
       
       default:
          return;
       }
}
 
