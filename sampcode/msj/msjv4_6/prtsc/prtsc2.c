/*
 * WINDOWS SCREEN CAPTURE - DYNAMIC LIBRARY
 *
 * LANGUAGE : Microsoft C 5.0
 * SDK      : Windows 2.03 SDK
 * MODEL    : small
 * STATUS   : operational
 *
 * 05/06/88 1.00 - Kevin P. Welch - initial creation.
 *
 */
 
#include <windows.h>
#include <string.h>
#include "prtsc.h"

/* global data */
BOOL     bMono;                  /* convert to monochrome? */
WORD     wArea;                  /* current capture area */
WORD     wColors;                /* number of system colors */
HWND     hWndDOS;                /* handle to DOS session */
HANDLE   hInstance;              /* library instance handle */
FARPROC  lpfnDOSWnd;             /* DOS session function */
FARPROC  lpfnOldHook;            /* old hook function */

/**/

/*
 * PrtScInit( hLibInst, wDataSegment, wHeapSize, lpszCmdLine ) : BOOL
 *
 *    hLibInst       library instance handle
 *    wDataSegment   library data segment
 *    wHeapSize      default heap size
 *    lpszCmdLine    command line arguments
 *
 * This function performs all the initialization necessary to use
 * the screen capture dynamic library.  It is assumed that no local
 * heap is used, hence no call to LocalInit.  A non-zero value is
 * returned if the initialization was sucessful.
 *
 */
 
BOOL PASCAL PrtScInit( hLibInst, wDataSegment, wHeapSize, lpszCmdLine )
   HANDLE      hLibInst;
   WORD        wDataSegment;
   WORD        wHeapSize;
   LPSTR       lpszCmdLine;
{
   extern BOOL       bMono;
   extern WORD       wArea;
   extern WORD       wColors;
   extern HWND       hWndDOS;
   extern HANDLE     hInstance;
   extern FARPROC    lpfnDOSWnd;
   extern FARPROC    lpfnNewHook;

   HDC      hDC;                    /* handle to temporary DC */
   HWND     hWndFocus;              /* window who has focus */
   char     szClassName[32];        /* temporary class name */

   /* initialization - <Alt><PrtSc> active */
   bMono = FALSE;
   wArea = CAPTURE_WINDOW;
   hInstance = hLibInst;

   lpfnOldHook = SetWindowsHook( WH_KEYBOARD, (FARPROC)PrtScHook );
   
   hWndFocus = GetFocus();

   hDC = GetDC( hWndFocus );
   wColors = GetDeviceCaps( hDC, NUMCOLORS );
   ReleaseDC( hWndFocus, hDC );

   /* search for MS-DOS session */
   hWndDOS = GetWindow( hWndFocus, GW_HWNDFIRST );
   while ( hWndDOS ) {
      
      /* check class name */
      GetClassName( hWndDOS, szClassName, sizeof(szClassName) );
      if ( !lstrcmp(szClassName,"Session") ) {
      
         /* inform user that things are happening! */
         MSGBOX( hWndDOS, "<Alt><PrtSc> now available!" );

         /* add our menu item to MS-DOS Session */
         ChangeMenu( 
            GetSubMenu( GetMenu(hWndDOS), 2 ),
            NULL, 
            "Screen Ca&pture...",
            CMD_CAPTURE,
            MF_APPEND|MF_CHECKED
         );
         
         /* hook into the MS-DOS Session */
         lpfnDOSWnd = (FARPROC)SetWindowLong(
               hWndDOS,
               GWL_WNDPROC,
               (LONG)PrtScFilterFn
            );
      
         /* normal return */
         return( TRUE );
            
      }
      
      /* get next window handle */
      hWndDOS = GetWindow( hWndDOS, GW_HWNDNEXT );
   
   }
   
   /* abnormal return */
   return( FALSE );

}

/**/

/*
 *
 * PrtScFilterFn( hWnd, wMessage, wParam, lParam ) : LONG FAR PASCAL
 *
 *    hWnd        window handle
 *    wMessage    message number
 *    wParam      additional message information
 *    lParam      additional message information
 *
 * This window function processes all the messages received by the
 * MS-DOS Session window.  When the user selects the Screen Capture...
 * menu option this function displays the screen capture control
 * panel.  All other messages are passed on to the window without
 * modification!
 *
 */
 
LONG FAR PASCAL PrtScFilterFn( hWnd, wMessage, wParam, lParam )
   HWND        hWnd;
   WORD        wMessage;
   WORD        wParam;
   LONG        lParam;
{

   /* trap appropriate messages */
   switch( wMessage )
      {
   case WM_COMMAND : /* user command - check for our message */
      if ( wParam == CMD_CAPTURE ) {
         DialogBox( hInstance, "PrtSc", hWndDOS, PrtScDlgFn );
         return( 0L );
      }     
      break;
   case WM_DESTROY : /* window being destroyed - unhook everything */
      if ( wArea ) {
         wArea = CAPTURE_OFF;
         UnhookWindowsHook( WH_KEYBOARD, (FARPROC)PrtScHook );
      }
      SetWindowLong( hWndDOS, GWL_WNDPROC, (LONG)lpfnDOSWnd );
      break;
   default : /* something else - ignore it! */
      break;
   }
   
   /* pass message on to window */
   return(  CallWindowProc(lpfnDOSWnd,hWndDOS,wMessage,wParam,lParam) );

}

/**/

/*
 * PrtScDlgFn( hDlg, wMessage, wParam, lParam ) : BOOL;
 *
 *    hDlg        handle to dialog box
 *    wMessage    message ID
 *    wParam      short parameter
 *    lParam      long parameter
 *
 * This function processes all the messages which relate to the
 * PrtSc dialog box.  This function inserts or removes the keyboard
 * hook function depending on the user's selection.
 *
 */

BOOL FAR PASCAL PrtScDlgFn( hDlg, wMessage, wParam, lParam )
   HWND        hDlg;
   WORD        wMessage;
   WORD        wParam;
   LONG        lParam;
{
   
   /* process messages */
   switch( wMessage )
      {
   case WM_INITDIALOG : /* initialize dialog box */
      CheckDlgButton( hDlg, DLGSC_MONOCHROME, bMono );
      EnableWindow( GetDlgItem(hDlg,DLGSC_MONOCHROME), (wColors>2) );
      CheckRadioButton( hDlg, DLGSC_OFF, DLGSC_SCREEN, DLGSC_OFF+wArea );
      break;
   case WM_COMMAND : /* window command */
      
      /* process sub-message */
      switch( wParam )
         {
      case DLGSC_OFF : /* turn screen capture off */
         if ( wArea ) {
            wArea = CAPTURE_OFF;
            UnhookWindowsHook( WH_KEYBOARD, (FARPROC)PrtScHook );
            EnableWindow( GetDlgItem(hDlg,DLGSC_MONOCHROME), FALSE );
         }
         CheckMenuItem( GetMenu(hWndDOS), CMD_CAPTURE, MF_UNCHECKED );
         break;
      case DLGSC_CLIENT : /* capture client area of active window */
      case DLGSC_WINDOW : /* capture active window */
      case DLGSC_SCREEN : /* capture entire screen */
         if ( !wArea ) {
            lpfnOldHook = SetWindowsHook( WH_KEYBOARD, (FARPROC)PrtScHook );
            EnableWindow( GetDlgItem(hDlg,DLGSC_MONOCHROME), (wColors>2) );
         }
         wArea = wParam - DLGSC_OFF;
         CheckMenuItem( GetMenu(hWndDOS), CMD_CAPTURE, MF_CHECKED );
         break;
      case DLGSC_MONOCHROME : /* capture image in monochrome */
         bMono = !bMono;
         break;
      case IDOK :
         EndDialog( hDlg, TRUE );
         break;
      default : /* ignore everything else */
         break;
      }
      
      break;
   default : /* message not processed */
      return( FALSE );
      break;
   }
   
   /* normal return */
   return( TRUE );
   
}

/**/

/*
 * PrtScHook( nCode, wParam, lParam ) : WORD
 *
 *    nCode       process message?
 *    wParam      virtual key code
 *    lParam      misc key parameters
 *
 * This function is called whenever the user presses any key.
 * The <Alt><PrtSc> keychord is trapped, and a bitmap copy of the
 * desired portion of the screen is copied to the clipboard.  This
 * area can be the client area of the currently active window,
 * the entire active window, or the entire display.
 *
 * The return value is FALSE if the message should be processed by
 * Windows; the return value is TRUE if the message should
 * be discarded.
 *
 */

WORD FAR PASCAL PrtScHook( nCode, wParam, lParam )
   int         nCode;
   WORD        wParam;
   LONG        lParam;
{
   extern BOOL       bMono;            /* convert to monochrome? */
   extern WORD       wArea;            /* area to capture */
   extern FARPROC    lpfnOldHook;      /* old keyboard hook */
   
   WORD        uWidth;                 /* width of bitmap */
   WORD        uHeight;                /* height of bitmap */
   WORD        wDiscard;               /* return value */
   POINT       ptClient;               /* client point */
   RECT        rcWindow;               /* window rectangle */
   HDC         hScreenDC;              /* handle to screen DC */
   HDC         hMemoryDC;              /* handle to memory DC */
   HWND        hActiveWnd;             /* handle to active window */
   HBITMAP     hOldBitmap;             /* handle to old bitmap */
   HBITMAP     hMemoryBitmap;          /* handle to memory bitmap */
   
   if (nCode == HC_ACTION) {
   
      /*
       * This check traps the <Alt><PrtSc> keychord using bit 29
       * for the <Alt> key and bit 31 for the key being released.
       *
       * Note that on old AT keyboards the PrtSc key was on the
       * multiply key above the numeric keypad.  Although newer
       * keyboard layouts have the PrtSc key elsewhere, the PrtSc
       * key is still identified by the VK_MULTIPLY ID code.
       */
      
      if ((wParam==VK_MULTIPLY) && ((lParam&0xA0000000)==0xA0000000)) {
         
         /* set return value so Windows will not process message */
         wDiscard = TRUE;
         
         /* retrieve top-level active window */
         hActiveWnd = GetFocus();
         while ( (GetWindowLong(hActiveWnd,GWL_STYLE)&WS_CHILD) == WS_CHILD )
            hActiveWnd = GetParent( hActiveWnd );
         
         /* continue if window visible */
         if ( IsWindowVisible(hActiveWnd) ) {
            
            switch( wArea )
               {
            case CAPTURE_CLIENT :
               
               /* retrieve client area dimensions */
               GetClientRect( hActiveWnd, &rcWindow );
               
               /* convert client coordinates to screen */
               ptClient.x = rcWindow.left;
               ptClient.y = rcWindow.top;
               ClientToScreen( hActiveWnd, &ptClient );
               
               rcWindow.left = ptClient.x;
               rcWindow.top = ptClient.y;
               ptClient.x = rcWindow.right;
               ptClient.y = rcWindow.bottom;
               ClientToScreen( hActiveWnd, &ptClient );
               
               rcWindow.right = ptClient.x;
               rcWindow.bottom = ptClient.y;
               
               break;
            case CAPTURE_WINDOW : 
               
               /* retrieve dimensions of active window */
               GetWindowRect( hActiveWnd, &rcWindow );
               
               break;
            case CAPTURE_SCREEN :
               
               /* retrieve dimensions of entire screen */
               rcWindow.top = 0;
               rcWindow.left = 0;
               rcWindow.right = SCREEN_WIDTH;
               rcWindow.bottom = SCREEN_HEIGHT;
               
               break;
            }
            
            /*
             * Note that the window boundaries need to be
             * adjusted as part of the window may not be
             * visible on the screen.  This eliminates the
             * problem of a partially defined bitmap.
             *
             */
            
            /* adjust boundaries to screen clipping region */
            if (rcWindow.right > SCREEN_WIDTH)
               rcWindow.right = SCREEN_WIDTH;
            
            if (rcWindow.bottom > SCREEN_HEIGHT)
               rcWindow.bottom = SCREEN_HEIGHT;
            
            if (rcWindow.left < 0)
               rcWindow.left = 0;
            
            if (rcWindow.top < 0)
               rcWindow.top = 0;
            
            /* compute display size of window */
            uWidth = rcWindow.right - rcWindow.left;
            uHeight = rcWindow.bottom - rcWindow.top;
            
            /* open clipboard */
            if ( OpenClipboard(hActiveWnd) ) {
               
               /* empty clipboard */
               EmptyClipboard();
               
               /* create screen DC & compatible memory DC's */
               hScreenDC = CreateDC( "DISPLAY", NULL, NULL, NULL );
               hMemoryDC = hScreenDC ?
                           CreateCompatibleDC(hScreenDC) : NULL;
               
               /* create color or monochrome bitmap */
               if (hMemoryDC && hScreenDC) {             
                  hMemoryBitmap = CreateCompatibleBitmap(
                     bMono ? hMemoryDC : hScreenDC,
                     uWidth,
                     uHeight
                  );
               } else
                  hMemoryBitmap = NULL;
               
               if ( hMemoryBitmap ) {
                  
                  /* select bitmap & copy bits */
                  hOldBitmap = SelectObject( hMemoryDC, hMemoryBitmap );
                  BitBlt(
                     hMemoryDC,              /* dest DC */
                     0,                      /* X upper left */
                     0,                      /* Y upper left */
                     uWidth,                 /* dest width */
                     uHeight,                /* dest height */
                     hScreenDC,              /* source DC */
                     rcWindow.left,          /* source X upper left */
                     rcWindow.top,           /* source Y upper left */
                     SRCCOPY                 /* ROP code */
                  );
                  SelectObject( hMemoryDC, hOldBitmap );
                  SetClipboardData( CF_BITMAP, hMemoryBitmap );
                  
               } else
                  MSGBOX( hWndDOS, "Insufficient memory!" );
               
               /* close clipboard */
               CloseClipboard();
               
               /* delete DCs */
               if ( hMemoryDC )
                  DeleteDC( hMemoryDC );
               if ( hScreenDC )
                  DeleteDC( hScreenDC );
                  
            } else 
               MSGBOX( hWndDOS, "Unable to open clipboard!" );
    
         } else 
            MSGBOX( hWndDOS, "Active window is iconic!" );
            
      } else      
         wDiscard = FALSE;
         
   } else 
      wDiscard = (WORD)DefHookProc(
         nCode,
         wParam,
         lParam,
         (FARPROC FAR *)&lpfnOldHook
      );
   
   /* return value */
   return( wDiscard );
   
}
