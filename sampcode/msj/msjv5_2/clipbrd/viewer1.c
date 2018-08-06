/*
 * WINDOWS CLIPBOARD VIEWER - SOURCE CODE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * 1.01-Kevin P. Welch- add param to GetClipboardFmtName.
 *
 */

#define  NOCOMM

#include <windows.h>
#include <process.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#include "viewer.h"

/* viewer window child id numbers */
#define  ID_STATUS     1
#define  ID_CLIENT     2
#define  ID_SIZEBOX    3

/* viewer window properties */
#define  CBCHAIN       MAKEINTRESOURCE(1)
#define  CBFORMAT      MAKEINTRESOURCE(2)
#define  CBGETDATA     MAKEINTRESOURCE(3)
#define  CBLIBRARY     MAKEINTRESOURCE(4)
#define  HWNDSTATUS    MAKEINTRESOURCE(5)
#define  HWNDCLIENT    MAKEINTRESOURCE(6)
#define  HWNDSIZEBOX   MAKEINTRESOURCE(7)



/*
 * WinMain( hInstance, hPrevInstance, lpszCmd, wCmdShow ) : VOID
 *
 *    hCrntInst      current instance handle
 *    hPrevInst     previous instance handle
 *    lpszCmdLine    current command line
 *    wInitShowCmd   initial show window command
 *
 * This function is responsible for registering and creating
 * the clipboard viewer window.  Once the window has been created,
 * it is also responsible for retrieving and dispatching all the
 * messages related to the window.
 *
 */

VOID PASCAL WinMain(
   HANDLE      hCrntInst,
   HANDLE      hPrevInst,
   LPSTR       lpszCmdLine,
   WORD        wInitShowCmd )
{
   /* local variables */
   MSG         msgViewer;
 WORD      wWndWidth;
 WORD      wWndHeight;
 HWND      hWndViewer;
 WNDCLASS    classViewer;

 /* warning level 3 compatibility */
 lpszCmdLine;

 /* define viewer class */
   memset( &classViewer, 0, sizeof(WNDCLASS) );
   classViewer.lpszClassName =(LPSTR)"Viewer";
   classViewer.hCursor =      LoadCursor( NULL, IDC_ARROW );
   classViewer.lpszMenuName = (LPSTR)"ViewerMenu";
   classViewer.style =        CS_HREDRAW | CS_VREDRAW;
   classViewer.lpfnWndProc =  ViewerWndFn;
   classViewer.hInstance =    hCrntInst;
   classViewer.hIcon =        LoadIcon( hCrntInst, "ViewerIcon" );

 /* register class if no previous instance */
 if ( (hPrevInst) || (RegisterClass(&classViewer)) ) {

   /* calculate initial width & height */
   wWndWidth = ( GetSystemMetrics(SM_CXSCREEN) >
                 40 * GetSystemMetrics(SM_CXVSCROLL) ) ?
                 40 * GetSystemMetrics(SM_CXVSCROLL )  :
                  2 * GetSystemMetrics(SM_CXSCREEN) / 3;

   wWndHeight = ( GetSystemMetrics(SM_CYSCREEN) >
                 30 * GetSystemMetrics(SM_CYHSCROLL) ) ?
                 30 * GetSystemMetrics(SM_CYHSCROLL )  :
                  2 * GetSystemMetrics(SM_CYSCREEN) / 3;

   /* create viewer window */
      hWndViewer = CreateWindow(
            "Viewer",
            "Clipboard Viewer",
            WS_TILEDWINDOW|WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
       wWndWidth,
       wWndHeight,
            (HWND)NULL,
            (HMENU)NULL,
            hCrntInst,
            (LPSTR)(DWORD)hPrevInst
         );

      /* continue if successful */
      if ( hWndViewer ) {

     /* display window */
         ShowWindow( hWndViewer, wInitShowCmd );

         /* process all related messages */
         while ( GetMessage( &msgViewer, NULL, 0, 0 ) ) {
            TranslateMessage( &msgViewer );
            DispatchMessage( &msgViewer );
         }

         /* normal exit */
         exit( msgViewer.wParam );

      } else
         WARNING( NULL, "Unable to create Clipboard Viewer!" );

   } else
      WARNING( NULL, "Unable to register Clipboard Viewer!" );

 /* abnormal exit */
 exit( TRUE );

}



/*
 * ViewerWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *    hWnd        window handle
 *    wMsg      message number
 *    wParam      additional message information
 *    lParam      additional message information
 *
 * This window function processes all the messages related to
 * the clipboard viewer window.  When created this window
 * registers and creates the associated status and client windows.
 *
 */

LONG FAR PASCAL ViewerWndFn( hWnd, wMsg, wParam, lParam )
   HWND        hWnd;
   WORD        wMsg;
   WORD        wParam;
   LONG        lParam;
{
   LONG        lResult;

   /* initialization */
   lResult = FALSE;

   /* process each message */
   switch( wMsg )
      {
 case WM_CREATE : /* create window */
   {
     char *    pKey;
     HANDLE    hFmtLib;
     HANDLE    hLibrary;
     HANDLE    hCrntInst;
     HANDLE    hPrevInst;
     LPLIBRARY lpLibrary;
     HWND      hWndStatus;
     HWND      hWndClient;
     HWND      hWndSizebox;
     WNDCLASS    classStatus;
     WNDCLASS    classClient;
     char      szLib[128];
     char      szList[128];

     /* define instance handles */
     hCrntInst = INSTANCE(hWnd);
     hPrevInst = (HANDLE)((LPCREATESTRUCT)lParam)->lpCreateParams;

     /* define initial property lists */
     SetProp( hWnd, CBCHAIN, NULL );
     SetProp( hWnd, CBFORMAT, NULL );
     SetProp( hWnd, CBGETDATA, NULL );
     SetProp( hWnd, HWNDSTATUS, NULL );
     SetProp( hWnd, HWNDCLIENT, NULL );
     SetProp( hWnd, HWNDSIZEBOX, NULL );

/* define library data structure */
hLibrary = GlobalAlloc( GHND, (DWORD)sizeof(LIBRARY) );
if ( hLibrary ) {
  lpLibrary = (LPLIBRARY)GlobalLock( hLibrary );
  if ( lpLibrary ) {

    /* initialization */
    lpLibrary->wModules = 0;

    /* retrieve list of supported formats */
    if ( GetProfileString("Clipboard",NULL,"",
                          szList,sizeof(szList)) ) {
      pKey = &szList[0];
      while ( *pKey ) {
        GetProfileString("Clipboard",pKey,"", szLib, sizeof(szLib));
        if ( szLib[0] > ' ' ) {
          hFmtLib = LoadLibrary( szLib );
          if ( hFmtLib >= 32 ) {
            lpLibrary->Module[lpLibrary->wModules].hModule = hFmtLib;
            lpLibrary->Module[lpLibrary->wModules++].wFormat =
              GetClipboardFmtNumber(pKey);
          }
        }
        pKey += strlen(pKey) + 1;
      }
    }

         /* unlock library & save handle */
         GlobalUnlock( hLibrary );
         SetProp( hWnd, CBLIBRARY, hLibrary );

         /* define status class */
           memset( &classStatus, 0, sizeof(WNDCLASS) );
           classStatus.lpszClassName =(LPSTR)"ViewerStatus";
           classStatus.hCursor =      LoadCursor( NULL, IDC_ARROW );
           classStatus.style =        CS_HREDRAW | CS_VREDRAW;
           classStatus.lpfnWndProc =  StatusWndFn;
           classStatus.hInstance =    hCrntInst;
           classStatus.hbrBackground =(HBRUSH)(COLOR_WINDOW + 1);

           /* define client class */
           memset( &classClient, 0, sizeof(WNDCLASS) );
           classClient.lpszClassName =(LPSTR)"ViewerClient";
           classClient.hCursor =      LoadCursor( NULL, IDC_ARROW );
           classClient.style =        CS_HREDRAW | CS_VREDRAW;
           classClient.lpfnWndProc =  ClientWndFn;
           classClient.hInstance =    hCrntInst;
           classClient.hbrBackground =(HBRUSH)(COLOR_APPWORKSPACE+1);

         /* register classes if no previous instance */
         if ( (hPrevInst) ||
              (RegisterClass(&classStatus) &&
               RegisterClass(&classClient))
            ) {

           /* create child windows */
             hWndSizebox = CreateWindow(
                                         "Scrollbar",
                                         "",
                                         WS_CHILD|SBS_SIZEBOX,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         hWnd,
                                         (HMENU)ID_SIZEBOX,
                                         hCrntInst,
                                         (LPSTR)(DWORD)hPrevInst
                                        );

             hWndClient = CreateWindow(
                                        "ViewerClient",
                                        "",
                                        WS_CHILD|WS_BORDER|
                                        WS_HSCROLL|WS_VSCROLL|
                                        WS_CLIPSIBLINGS,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        hWnd,
                                        (HMENU)ID_CLIENT,
                                        hCrntInst,
                                        (LPSTR)(DWORD)hPrevInst
                                      );

           hWndStatus = CreateWindow(
                                     "ViewerStatus",
                                     "",
                                     WS_CHILD|WS_BORDER|
                                     WS_CLIPSIBLINGS,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     hWnd,
                                     (HMENU)ID_STATUS,
                                     hCrntInst,
                                     (LPSTR)(DWORD)hPrevInst
                                    );

           /* continue if successful */
           if ( hWndStatus && hWndClient && hWndSizebox ) {

             /* update property lists */
             SetProp( hWnd, HWNDSTATUS, hWndStatus );
             SetProp( hWnd, HWNDCLIENT, hWndClient );
             SetProp( hWnd, HWNDSIZEBOX, hWndSizebox );

             SetProp( hWnd, CBCHAIN, SetClipboardViewer(hWnd) );

             /* make child windows visible */
               ShowWindow( hWndStatus, SW_SHOWNA );
               ShowWindow( hWndClient, SW_SHOWNA );
               ShowWindow( hWndSizebox, SW_SHOWNA );

           } else {
                 WARNING( hWnd, "Unable to create child windows!" );
             PostQuitMessage( 4 );
           }

         } else {
               WARNING( hWnd, "Unable to register child windows!" );
           PostQuitMessage( 3 );
         }

       } else {
         WARNING( hWnd, "Unable to access instance data!" );
         PostQuitMessage( 2 );
       }
     } else {
       WARNING( hWnd, "Insufficient memory!" );
       PostQuitMessage( 1 );
     }

   }
   break;
 case WM_GETMINMAXINFO : /* retrieve window size constraints */

   /* set minimum tracking size */
   ((LPPOINT)lParam)[3].x = 22 * GetSystemMetrics(SM_CXVSCROLL);
   ((LPPOINT)lParam)[3].y = 10 * GetSystemMetrics(SM_CYCAPTION);

   break;
 case WM_INITMENU : /* initialize menu */
   {
     LPLIBRARY     lpLibrary;

     /* check status window visibility */
     CheckMenuItem(
       wParam,
       IDM_STATUS,
       IsWindowVisible(GetProp(hWnd,HWNDSTATUS)) ?
                       MF_CHECKED : MF_UNCHECKED
     );

     /* enable-disable format library menu options */
     lpLibrary = (LPLIBRARY)GlobalLock( GetProp(hWnd,CBLIBRARY) );
     if ( lpLibrary ) {
       EnableMenuItem(wParam,IDM_ADDFMT,(lpLibrary->wModules <
                      MAX_MODULE) ? MF_ENABLED : MF_GRAYED );
       EnableMenuItem(wParam,IDM_REMFMT,(lpLibrary->wModules) ?
                      MF_ENABLED : MF_GRAYED );
       GlobalUnlock( GetProp(hWnd,CBLIBRARY) );
     } else
       WARNING( hWnd, "Unable to access instance data!" );

     /* enable-disable erase menu option */
     EnableMenuItem( wParam, IDM_ERASE, (GetProp(hWnd,CBFORMAT) >
                     IDM_FORMATS) ? MF_ENABLED : MF_GRAYED );

   }
   break;
 case WM_COMMAND : /* menu command */

   /* process sub-message */
   switch( wParam )
     {
   case IDM_STATUS : /* toggle status bar visibility */
     {
       RECT    rcClient;

       /* change status bar visibility */
       ShowWindow(
         GetProp(hWnd,HWNDSTATUS),
         IsWindowVisible(
            GetProp(hWnd,HWNDSTATUS)) ? SW_HIDE : SW_SHOWNA
       );

       /* force resizing of child windows */
       GetClientRect( hWnd, &rcClient );
       SendMessage( hWnd, WM_SIZE, 0,
                    MAKELONG(rcClient.right,rcClient.bottom) );

     }
     break;
   case IDM_EXIT : /* exit application */
       PostQuitMessage( 0 );
     break;
   case IDM_ABOUT : /* about viewer */
     Dialog( hWnd, "ViewerAbout", AboutDlgFn );
     break;
   case IDM_ADDFMT : /* add a new clipboard format */
     Dialog( hWnd, "ViewerAdd", AddFormatDlgFn );
     break;
   case IDM_REMFMT : /* remove a clipboard format */
     Dialog( hWnd, "ViewerRemove", RemFormatDlgFn );
     break;
   case IDM_ERASE : /* erase clipboard contents */

     /* verify option */
     if ( MessageBox(hWnd,"Erase clipboard contents?",
                     "Clipboard Viewer",
                     MB_ICONQUESTION|MB_YESNO) == IDYES ) {
       if ( OpenClipboard(hWnd) ) {
         EmptyClipboard();
         CloseClipboard();
       } else
         WARNING( hWnd, "Unable to open clipboard!" );
     }

     break;
   default : /* one of the selected formats */

     /* open clipboard */
     if ( OpenClipboard(hWnd) ) {

       WORD      wCrntFmt;
       HANDLE    hCrntData;
       HANDLE    hCrntModule;

       /* attempt to retrieve clipboard data - semaphore call */
       wCrntFmt = wParam - IDM_FORMATS;
       if ( wCrntFmt ) {
         SetProp( hWnd, CBGETDATA, TRUE );
         hCrntData = GetClipboardData( wCrntFmt );
         hCrntModule = GetClipboardModule( wCrntFmt,
                                           GetProp(hWnd,CBLIBRARY) );
         SetProp( hWnd, CBGETDATA, NULL );
         if ( hCrntData == NULL )
           WARNING( hWnd, "Unable to retrieve clipboard data!" );
       } else {
         hCrntData = NULL;
         hCrntModule = NULL;
       }

       /* close clipboard */
       CloseClipboard();

       /* update checked menu item - even if data inaccessible */
       CheckMenuItem( GetMenu(hWnd), GetProp(hWnd,CBFORMAT),
                      MF_UNCHECKED );
       SetProp( hWnd, CBFORMAT, wParam );
       CheckMenuItem( GetMenu(hWnd), wParam, MF_CHECKED );

       /* notify child windows - note that clipboard is now closed */
       SendMessage( GetProp(hWnd,HWNDSTATUS), WM_UPDATE, wCrntFmt,
                    MAKELONG(hCrntData,hCrntModule) );
       SendMessage( GetProp(hWnd,HWNDCLIENT), WM_UPDATE, wCrntFmt,
                    MAKELONG(hCrntData,hCrntModule) );

     } else
       WARNING( hWnd, "Unable to open clipboard!" );

     break;
   }

   break;
 case WM_ADDFMT : /* add a new clipboard library */
   {
     WORD        wEntry;
     HANDLE      hFmtLib;
     HANDLE      hOldLib;
     LPLIBRARY   lpLibrary;
     char        szFmt[64];
     char        szLib[64];
     char        szMsg[80];

     /* copy name of library */
     lstrcpy( szLib, (LPSTR)lParam );

     /* lock down instance data */
     lpLibrary = (LPLIBRARY)GlobalLock( GetProp(hWnd,CBLIBRARY) );
     if ( lpLibrary ) {

       /* define clipboard format name */
       GetClipboardFmtName( wParam, szFmt, sizeof(szFmt), FALSE );

       /* check to see if format already listed */
       for (wEntry=0;
            (wEntry<lpLibrary->wModules)&&
            (wParam!=lpLibrary->Module[wEntry].wFormat); wEntry++ );
       if ( wEntry < lpLibrary->wModules ) {

         /* entry already present - ask if replace? */
         GetModuleFileName( lpLibrary->Module[wEntry].hModule,
                            szLib, sizeof(szLib) );
         sprintf( szMsg, "Replace %s?", szLib );
         if ( MessageBox(hWnd,szMsg,"Clipboard Viewer",
                         MB_ICONQUESTION|MB_YESNO) == IDYES ) {
           hFmtLib = LoadLibrary( (LPSTR)lParam );
           if ( hFmtLib >= 32 ) {
             lResult = TRUE;
             hOldLib = lpLibrary->Module[wEntry].hModule;
             lpLibrary->Module[wEntry].hModule = hFmtLib;
             SendMessage( hWnd, WM_COMMAND, GetProp(hWnd,CBFORMAT),
                          0L );
             WriteProfileString( "Clipboard", szFmt, szLib );
             FreeLibrary( hOldLib );
           }
         }

       } else {

         /* check if space available internally */
         if ( lpLibrary->wModules < MAX_MODULE ) {
           hFmtLib = LoadLibrary( (LPSTR)lParam );
           if ( hFmtLib >= 32 ) {
             lResult = TRUE;
             lpLibrary->Module[lpLibrary->wModules].hModule=hFmtLib;
             lpLibrary->Module[lpLibrary->wModules++].wFormat =
                wParam;
             SendMessage( hWnd, WM_COMMAND, GetProp(hWnd,CBFORMAT),
                          0L );
             WriteProfileString("Clipboard", szFmt, (LPSTR)lParam );
           }
         } else
           WARNING( hWnd, "Insufficient memory to add library!" );

       }

       /* unlock library data */
       GlobalUnlock( GetProp(hWnd,CBLIBRARY) );

     } else
       WARNING( hWnd, "Unable to access instance data!" );
   }
   break;
 case WM_GETFMT : /* retrieve clipboard format list */
   {
     LPSTR       lpStr;
     WORD        wEntry;
     LPLIBRARY   lpLibrary;
     char        szFmt[64];
     char        szLib[80];
     char        szEntry[128];

     /* lock down instance data */
     lpLibrary = (LPLIBRARY)GlobalLock( GetProp(hWnd,CBLIBRARY) );
     if ( lpLibrary ) {
       lpStr = (LPSTR)lParam;
       lResult = lpLibrary->wModules;
       for (wEntry=0; wEntry<lpLibrary->wModules; wEntry++) {
         GetClipboardFmtName( lpLibrary->Module[wEntry].wFormat,
                              szFmt, sizeof(szFmt), FALSE );
         GetModuleFileName( lpLibrary->Module[wEntry].hModule,
                            szLib, sizeof(szLib) );
         sprintf( szEntry, "%s - %s", szFmt, szLib );
         lstrcpy( lpStr, szEntry );
         lpStr += strlen(szEntry) + 1;
       }
       lstrcpy( lpStr, "" );
       GlobalUnlock( GetProp(hWnd,CBLIBRARY) );
     } else
       WARNING( hWnd, "Unable to access instance data!" );

   }
   break;
 case WM_REMFMT : /* remove a listed clipboard library */
   {
     HANDLE      hOldLib;
     LPLIBRARY   lpLibrary;
     char        szFmt[64];

     /* lock down instance data & remove library module */
     lpLibrary = (LPLIBRARY)GlobalLock( GetProp(hWnd,CBLIBRARY) );
     if ( lpLibrary ) {
       if ( wParam < lpLibrary->wModules ) {
         lResult = TRUE;
         hOldLib = lpLibrary->Module[wParam].hModule;
         GetClipboardFmtName( lpLibrary->Module[wParam].wFormat,
                              szFmt, sizeof(szFmt), FALSE );
         while ( ++wParam < lpLibrary->wModules ) {
           lpLibrary->Module[wParam-1].wFormat =
                              lpLibrary->Module[wParam].wFormat;
           lpLibrary->Module[wParam-1].hModule =
                              lpLibrary->Module[wParam].hModule;
         }
         lpLibrary->wModules--;
         SendMessage( hWnd, WM_COMMAND, GetProp(hWnd,CBFORMAT), 0L );
         WriteProfileString( "Clipboard", szFmt, "" );
         FreeLibrary( hOldLib );
       } else
         WARNING( hWnd, "Attempt to remove invalid library!" );
       GlobalUnlock( GetProp(hWnd,CBLIBRARY) );
     } else
       WARNING( hWnd, "Unable to access instance data!" );

   }
   break;
 case WM_SIZE : /* window being resized */

   /* reposition status window (even if invisible) */
   MoveWindow(
     GetProp(hWnd,HWNDSTATUS),
     -1,
     -1,
     LOWORD(lParam)+2,
     GetSystemMetrics(SM_CYMENU)+2,
     TRUE
   );

   /* reposition client window (always visible) */
   MoveWindow(
              GetProp(hWnd,HWNDCLIENT),
              -1,
              (IsWindowVisible(GetProp(hWnd,HWNDSTATUS))) ?
                GetSystemMetrics(SM_CYMENU) : -1,
              LOWORD(lParam)+2,
              (IsWindowVisible(GetProp(hWnd,HWNDSTATUS))) ?
              HIWORD(lParam)-GetSystemMetrics(SM_CYMENU)+1 :
                HIWORD(lParam)+2,
              TRUE
   );

   /* reposition sizebox window (always visible) */
   MoveWindow(
              GetProp(hWnd,HWNDSIZEBOX),
              LOWORD(lParam)-GetSystemMetrics(SM_CXVSCROLL)+1,
              HIWORD(lParam)-GetSystemMetrics(SM_CYHSCROLL)+1,
              GetSystemMetrics(SM_CXVSCROLL),
              GetSystemMetrics(SM_CYHSCROLL),
              TRUE
   );

   break;
 case WM_DRAWCLIPBOARD : /* clipboard contents changing */
   {
     HMENU       hSubMenu;
     WORD        wCrntFmt;
     WORD        wCrntEntry;
     HANDLE      hCrntData;
     HANDLE      hCrntModule;
     char        szFmtName[48];
     char        szMenuName[48];

     /* filter out spurious WM_DRAWCLIPBOARD messages */
     if ( GetProp(hWnd,CBGETDATA) == NULL ) {

       /* pass the message down the chain first */
       if ( GetProp(hWnd,CBCHAIN) )
         SendMessage( GetProp(hWnd,CBCHAIN), wMsg, wParam, lParam );

       /* retrieve handle to view sub-menu */
       hSubMenu = GetSubMenu( GetMenu(hWnd), 1 );

       /* remove old clipboard formats */
       SetProp( hWnd, CBFORMAT, IDM_FORMATS );
       for ( wCrntEntry = GetMenuItemCount(hSubMenu)-1; wCrntEntry>1;
             wCrntEntry-- )
         ChangeMenu(
             hSubMenu,
             wCrntEntry,
             (wCrntEntry>2) ? NULL : "&1. (Empty)",
             (wCrntEntry>2) ? NULL : IDM_FORMATS,
             (wCrntEntry>2) ? MF_DELETE|MF_BYPOSITION :
                              MF_CHANGE|MF_GRAYED|MF_BYPOSITION
           );

       /* open clipboard */
       if ( OpenClipboard(hWnd) ) {

         /* enumerate available clipboard formats */
         wCrntEntry = 0;
         hCrntModule = NULL;
         wCrntFmt = EnumClipboardFormats( NULL );

         while ( wCrntFmt ) {

           /* define new menu entry */
           GetClipboardFmtName( wCrntFmt, szFmtName,
                                sizeof(szFmtName), TRUE );
           sprintf( szMenuName, "&%u. %s", wCrntEntry+1,
                    (szFmtName[0]) ? szFmtName : "Undefined" );

           /* update view menu */
           ChangeMenu(
             hSubMenu,
             (wCrntEntry) ? NULL : IDM_FORMATS,
             szMenuName,
             IDM_FORMATS + wCrntFmt,
             (wCrntEntry) ? MF_APPEND : MF_CHANGE
           );

           /* define selected format */
           if ( hCrntModule == NULL ) {
             SetProp( hWnd, CBFORMAT, IDM_FORMATS+wCrntFmt );
             hCrntModule=GetClipboardModule(wCrntFmt,
                                            GetProp(hWnd,CBLIBRARY));
           }

           /* retrieve next available format */
           wCrntEntry++;
           wCrntFmt = EnumClipboardFormats( wCrntFmt );

         }

         /* attempt to retrieve data handle - semaphore call */
         wCrntFmt = GetProp(hWnd,CBFORMAT) - IDM_FORMATS;
         if ( wCrntFmt ) {
           SetProp( hWnd, CBGETDATA, TRUE );
           hCrntData = GetClipboardData( wCrntFmt );
           SetProp( hWnd, CBGETDATA, NULL );
         } else {
           hCrntData = NULL;
           hCrntModule = NULL;
         }

         /* close clipboard */
         CloseClipboard();

         /* mark selected format */
         CheckMenuItem(hSubMenu, GetProp(hWnd,CBFORMAT), MF_CHECKED);

         /* notify child windows - clipboard now closed */
         SendMessage( GetProp(hWnd,HWNDSTATUS), WM_UPDATE, wCrntFmt,
                      MAKELONG(hCrntData,hCrntModule) );
         SendMessage( GetProp(hWnd,HWNDCLIENT), WM_UPDATE, wCrntFmt,
                      MAKELONG(hCrntData,hCrntModule) );

       } else
         WARNING( hWnd, "Unable to open clipboard!" );

     }

   }
   break;
 case WM_CHANGECBCHAIN : /* clipboard viewer chain being changed */

   /* re-link viewer chain */
   if ( wParam == GetProp(hWnd,CBCHAIN) )
     SetProp( hWnd, CBCHAIN, LOWORD(lParam) );
   else
     if ( GetProp(hWnd,CBCHAIN) )
       SendMessage( GetProp(hWnd,CBCHAIN), wMsg, wParam, lParam );

   break;
   case WM_DESTROY : /* destroy window */
   {
     WORD        wEntry;
     LPLIBRARY   lpLibrary;

     /* retrieve & lock module library */
     lpLibrary = (LPLIBRARY)GlobalLock( GetProp(hWnd,CBLIBRARY) );
     if ( lpLibrary ) {

       /* free each listed library */
       for ( wEntry=0; wEntry<lpLibrary->wModules; wEntry++ )
         FreeLibrary( lpLibrary->Module[wEntry].hModule );

       /* unlock data structure */
       GlobalUnlock( GetProp(hWnd,CBLIBRARY) );

     }

     /* free allocated memory & unlink from clipboard chain */
     GlobalFree( RemoveProp(hWnd,CBLIBRARY) );
     ChangeClipboardChain( hWnd, RemoveProp(hWnd,CBCHAIN) );

     /* remove remaining properties */
     RemoveProp( hWnd, CBFORMAT );
     RemoveProp( hWnd, CBGETDATA );
     RemoveProp( hWnd, HWNDSTATUS );
     RemoveProp( hWnd, HWNDCLIENT );
     RemoveProp( hWnd, HWNDSIZEBOX );

     /* end it all */
     PostQuitMessage( 0 );

   }
   break;
   default : /* send to default */
      lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
      break;
   }

   /* return normal result */
   return( lResult );

}

