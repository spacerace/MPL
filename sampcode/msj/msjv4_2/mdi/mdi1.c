/*
 * MDI1.C - Application Interface
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * This module contains all the MDI code to handle the programmatic
 * interface.  This includes window creation, default message handling,
 * and message loop support code.  The MDI desktop window and the
 * document windows have separate creation and message handling
 * routines, while the message loop support is window independant.
 *
 * Developed by:
 *   Geoffrey Nicholls
 *   Kevin P. Welch
 *
 * (C) Copyright 1988
 * Eikon Systems, Inc.
 * 989 E. Hillsdale Blvd, Suite 260
 * Foster City  CA  94404
 *
 */

#include <stdio.h>
#include <windows.h>

#include "mdi.h"

/* External variables */
extern int        iCurrentPopup;          /* What menu is up */
extern int        iNextPopup;             /* What menu is next */

/**/

/*
 * MdiMainCreateWindow( ... ) : HWND;
 *
 *    szClassName    Class name of MDI desktop
 *    szTitleName    Caption for MDI desktop
 *    lStyle         Style of MDI desktop
 *    wLeft          Left position of window
 *    wTop           Top position of window
 *    wWidth         Width of window
 *    wHeight        Height of window
 *    hwndParent     Parent of MDI desktop
 *    hmenuMain      Handle to menu
 *    hInst          Current instance handle
 *    lpCreateParam  Pointer to creation parameters
 *
 * Create the MDI desktop window.  The parameters exactly match the
 * Windows call CreateWindow().  The MDI code keeps track of desktop
 * information in property lists, such as handles to menus,
 * accelerators, and bitmaps as well as counts of document windows.
 *
 */

HWND MdiMainCreateWindow(
   LPSTR       szClassName,
   LPSTR       szTitleName,
   DWORD       lStyle,
   int         wLeft,
   int         wTop,
   int         wWidth,
   int         wHeight,
   HWND        hwndParent,
   HMENU       hmenuMain,
   HANDLE      hInst,
   LPSTR       lpCreateParam )
{
   HWND        hwndMain;         /* Handle of MDI desktop */

   hwndMain = CreateWindow( szClassName,
      szTitleName,
      lStyle,
      wLeft, wTop, wWidth, wHeight,
      hwndParent,
      hmenuMain,
      hInst,
      lpCreateParam );

   /* Success? */
   if ( hwndMain )
   {
      /* Message hook for tracking keyboard interface to menu */
      MdiSetMenuKeyHook( hInst );

      /* Make sure variables are initialized */
      SetProp( hwndMain, PROP_ACTIVE, NULL );
      SetProp( hwndMain, PROP_COUNT, 0 );
      SetProp( hwndMain, PROP_HIDDEN, 0 );
      SetProp( hwndMain, PROP_ZOOM, FALSE );
      SetProp( hwndMain, PROP_MAINMENU, GetMenu( hwndMain ) );
      SetProp( hwndMain, PROP_WINDOWMENU, LoadMenu(hInst, "MdiMenu") );
      SetProp( hwndMain,
         PROP_CTRLACCEL,
         LoadAccelerators( hInst, "MdiChildAccel" ) );
      SetProp( hwndMain,
         PROP_SYSMENU,
         MdiCreateChildSysBitmap( hwndMain ) );
      SetProp( hwndMain, PROP_TITLE, AddAtom( szTitleName ) );

      /* Attach window menu to main menu */
      MdiWindowMenu( hwndMain, GetMenu( hwndMain ), TRUE);
   }
   return hwndMain;
}

/**/

/*
 * MdiMainDefWindowProc( hwndMain, message, wParam, lParam ) : long;
 *
 *    hwndMain       Handle to MDI desktop
 *    message        Current message
 *    wParam         Word parameter to message
 *    lParam         Long parameter to message
 *
 * Handle the MDI desktop messages.  Messages reach here only after
 * the application MDI desktop window procedure has processed those
 * that it wants.  Major occurances that are recognized in this
 * function include:  parent window activation & deactivation, WINDOW
 * menu messages (esp for switching between children), record keeping
 * of which menu is currently poped up (if any), and sizing of
 * document windows which are maximized.
 *
 */

long MdiMainDefWindowProc(
   HWND        hwndMain,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   FARPROC     lpProc;           /* Procedure instance for dialogs */
   HANDLE      hInst;            /* Current instance handle */
   HWND        hwndActive;       /* Currently active document window */
   HWND        hwndChosen;       /* Newly chosen document window */
   HWND        hwndIter;         /* Iterating document windows */
   LONG        lStyle;           /* Window style */
   RECT        rcClient;         /* Client area of MDI desktop */

   hwndActive = GetProp( hwndMain, PROP_ACTIVE );
   switch ( message )
   {
   case WM_ACTIVATE:
      /* MDI child reflects the activation status of the parent */
      if ( hwndActive )
      {
         switch( wParam )
         {
         case 0:
            /* Deactivating */
            MdiDeactivateChild( hwndActive );
            break;

         case 1:
         case 2:
            /* Activating */
            MdiActivateChild( hwndActive, FALSE );
            break;
         }
      }
      break;

   case WM_COMMAND:
      hInst = GetWindowWord( hwndMain, GWW_HINSTANCE );
      switch ( wParam )
      {
      case IDM_UNHIDE:
         /* Unhide dialog box */
         lpProc = MakeProcInstance( MdiDlgUnhide, hInst );
         hwndChosen = DialogBox( hInst, "MdiUnhide", hwndMain, lpProc);
         if ( hwndChosen )
         {
            /* MDI child being unhidden */
            MdiUnhideChild( hwndChosen );
         }
         FreeProcInstance( lpProc );
         break;

      default:
         /* Is this a WINDOW menu message to select an MDI child */
         if ( wParam > IDM_FIRSTWIN
            && wParam <= IDM_FIRSTWIN + GetProp( hwndMain, PROP_COUNT))
         {
            /* Iterate all the children of the mdi parent */
            for ( hwndIter = GetTopWindow( hwndMain );
               hwndIter != NULL;
               hwndIter = GetWindow( hwndIter, GW_HWNDNEXT ) )
            {
               /* Ignore self & non-mdi children */
               if ( hwndIter == hwndActive
                  || !GetProp( hwndIter, PROP_ISMDI ) )
               {
                  continue;
               }

               /* Is this the window? */
               if ( wParam == GetProp( hwndIter, PROP_MENUID ) )
               {
                  /* Make a new MDI child the current one */
                  if ( GetProp( hwndMain, PROP_ZOOM ) )
                  {
                     MdiSwitchZoom( hwndIter, hwndActive );
                  }
                  MdiActivateChild( hwndIter, FALSE );
                  break;
               }
            }
         }
         else
         {
            if ( hwndActive )
            {
               /* MDI child gets all other WM_COMMAND messages */
               return SendMessage(hwndActive, message, wParam, lParam);
            }
         }
         break;
      }
      break;

   case WM_DESTROY:
      /* Let MDI clean up */
      DeleteObject( GetProp( hwndMain, PROP_SYSMENU ) );
      DeleteAtom( GetProp( hwndMain, PROP_TITLE ) );
      MdiFreeMenuKeyHook( );
      break;

   case WM_INITMENU:
      /* Anything to hide? */
      EnableMenuItem( ( HMENU ) wParam,
         IDM_HIDE,
         hwndActive ? MF_ENABLED : MF_GRAYED );

      /* Any hidden windows?  Gray UNHIDE appropriately */
      EnableMenuItem( ( HMENU ) wParam,
         IDM_UNHIDE,
         GetProp( hwndMain, PROP_HIDDEN ) ? MF_ENABLED : MF_GRAYED );

      /* Maybe the MDI child wants to do something too */
      if ( hwndActive )
      {
         SendMessage( hwndActive, message, wParam, lParam );
      }
      break;

   case WM_INITMENUPOPUP:
      /* Keep track of which menu is popped up so that if we hit a left
      ** or right arrow, we know which the next menu to popup is.
      */
      if ( HIWORD( lParam ) )
      {
         iCurrentPopup = POP_MAINSYS;
      }
      else
      {
         iCurrentPopup = LOWORD( lParam );
         if ( GetProp( hwndMain, PROP_ZOOM ) )
            iCurrentPopup--;
      }
      if ( hwndActive )
      {
         /* Let the MDI child do something with this too */
         SendMessage( hwndActive, message, wParam, lParam );
      }
      break;

   case WM_MENUCHAR:
      if ( wParam == '-' )
      {
         /* Bring up active MDI child's system menu */
         if ( ( LOWORD( lParam ) & MF_POPUP ) == 0 && hwndActive )
         {
            /* Is the MDI child zoomed? */
            if ( GetProp( hwndMain, PROP_ZOOM ) )
            {
               /* MDI child system menu is on the main menu bar */
               return MAKELONG( 0, MC_SELECT );
            }

            iNextPopup = POP_CHILDSYS;
            /* This result will cause no menu to hilite, but because
            ** iNextPopup is set, the call right after the message
            ** processing loop will hilite the child system menu
            */
            return MAKELONG( 0, MC_ABORT );
         }
         break;
      }
      break;

   case WM_SETFOCUS:
      /* Make the MDI child reflect the focus status of the MDI main */
      if ( hwndActive )
      {
         SetFocus( hwndActive );
      }
      break;

   case WM_SIZE:
      if ( wParam != SIZEICONIC )
      {
         /* Resize MDI child if zoomed */
         if ( GetProp( hwndMain, PROP_ZOOM ) )
         {
            GetClientRect( hwndMain, &rcClient );
            lStyle = GetWindowLong( hwndActive, GWL_STYLE );
            AdjustWindowRect( &rcClient, lStyle, FALSE );
            MoveWindow( hwndActive,
               rcClient.left,
               rcClient.top,
               rcClient.right - rcClient.left,
               rcClient.bottom - rcClient.top,
               TRUE );
         }
      }
      break;
   }
   return DefWindowProc( hwndMain, message, wParam, lParam );
}

/**/

/*
 * MdiChildCreateWindow( ... ) : HWND;
 *
 *    szClassName    Class name of document
 *    szTitleName    Caption for document
 *    lStyle         Style of document
 *    wLeft          Left position of window
 *    wTop           Top position of window
 *    wWidth         Width of window
 *    wHeight        Height of window
 *    hwndMain       MDI desktop
 *    hmenuChild     Handle to document menu
 *    hInst          Current instance handle
 *    lpCreateParam  Pointer to creation parameters
 *
 * Create an MDI document window.  The parameters exactly match the
 * Windows call CreateWindow() except that the hmenuChild message
 * must be a valid menu handle, whereas the CreateWindow() call expects
 * a child id.  The MDI code keeps track of document information in
 * property lists, such as handles to menus and accelerators, as well
 * as whether this child of the MDI desktop is an MDI document.
 *
 */

HWND MdiChildCreateWindow(
   LPSTR       szClassName,
   LPSTR       szTitleName,
   DWORD       lStyle,
   int         wLeft,
   int         wTop,
   int         wWidth,
   int         wHeight,
   HWND        hwndMain,
   HMENU       hmenuChild,
   HANDLE      hInst,
   LPSTR       lpCreateParam )
{
   char        szTitle[128];     /* Title for document */
   char        szCompose[128];   /* Used to compose title */
   int         iCount;           /* Child window ID */
   int         wLeftAct = wLeft;       /* Left position of window */
   int         wTopAct = wTop;         /* Top position of window */
   int         wWidthAct = wWidth;     /* Width of window */
   int         wHeightAct = wHeight;   /* Height of window */
   HWND        hwndActive;       /* Currently active document window */
   HWND        hwndChild;        /* Handle of document window */
   RECT        rcClient;         /* Client area of MDI desktop */

   /* Calculate size & position */
   iCount = GetProp( hwndMain, PROP_COUNT );
   if ( wLeftAct == CW_USEDEFAULT )
      wLeftAct = 10 + 15 * iCount;
   if ( wTopAct == CW_USEDEFAULT )
      wTopAct = 10 + 15 * iCount;
   if ( wWidthAct == CW_USEDEFAULT )
      wWidthAct = 200;
   if ( wHeightAct == CW_USEDEFAULT )
      wHeightAct = 75;

   /* Ensure that it falls within client area */
   GetClientRect( hwndMain, &rcClient );
   if ( wLeft == CW_USEDEFAULT )
      while ( wLeftAct > rcClient.right )
         wLeftAct -= rcClient.right;
   if ( wTop == CW_USEDEFAULT )
      while ( wTopAct > rcClient.bottom )
         wTopAct -= rcClient.bottom;

   /* Create it */
   hwndChild = CreateWindow( szClassName,
      szTitleName,
      lStyle,
      wLeftAct, wTopAct, wWidthAct, wHeightAct,
      hwndMain,
      ( HMENU ) iCount,
      hInst,
      lpCreateParam );

   /* Success? */
   if ( hwndChild )
   {
      /* New child */
      SetProp( hwndMain, PROP_COUNT, ++iCount );

      /* Keep important info */
      SetProp( hwndChild, PROP_CHILDMENU, hmenuChild );
      SetProp( hwndChild, PROP_ACCEL, 0 );
      SetProp( hwndChild, PROP_MENUID, IDM_FIRSTWIN + iCount );
      SetProp( hwndChild, PROP_ISMDI, TRUE );

      /* Reflect change in WINDOW menu */
      MdiAppendWindowToMenu( hwndChild );

      /* Let's activate this MDI child */
      hwndActive = GetProp( hwndMain, PROP_ACTIVE );
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         MdiSwitchZoom( hwndChild, hwndActive );
      }
      MdiActivateChild( hwndChild, FALSE );

      /* Show menu bar changes now */
      DrawMenuBar( hwndMain );
   }
   return hwndChild;
}

/**/

/*
 * MdiChildDefWindowProc( hwndChild, message, wParam, lParam ) : long;
 *
 *    hwndChild      Handle to document
 *    message        Current message
 *    wParam         Word parameter to message
 *    lParam         Long parameter to message
 *
 * Handle MDI document messages.  Messages reach here only after the
 * application document window proedure has processed those that it
 * wants.  Major occurances that are recognized in this function
 * include:  child system menu choices, window creation, window
 * activation, window destruction, and translating <alt>- into
 * selecting the child system menu.
 *
 */

long MdiChildDefWindowProc(
   HWND        hwndChild,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   HWND        hwndMain;         /* Handle to MDI desktop */
   PAINTSTRUCT Paint;            /* Paint structure */

   hwndMain = GetParent( hwndChild );
   switch ( message )
   {
   case WM_CLOSE:
      /* We're going away */
      MdiDestroyChildWindow( hwndChild );
      break;

   case WM_COMMAND:
      switch( wParam )
      {
      case IDM_HIDE:
         /* MDI child being hidden */
         MdiHideChild( hwndChild );
         break;

      case IDM_RESTORE:
         SendMessage( hwndChild, WM_SYSCOMMAND, SC_RESTORE, lParam );
         break;

      case IDM_MOVE:
         SendMessage( hwndChild, WM_SYSCOMMAND, SC_MOVE, lParam );
         break;

      case IDM_SIZE:
         SendMessage( hwndChild, WM_SYSCOMMAND, SC_SIZE, lParam );
         break;

      case IDM_MAXIMIZE:
         SendMessage( hwndChild, WM_SYSCOMMAND, SC_MAXIMIZE, lParam );
         break;

      case IDM_PREVWINDOW:
         /* This function is achieved only through the keyboard */
         SendMessage( hwndChild, WM_SYSCOMMAND, SC_PREVWINDOW, lParam);
         break;

      case IDM_NEXTWINDOW:
         /* This function is achieved only through the keyboard */
         SendMessage( hwndChild, WM_SYSCOMMAND, SC_NEXTWINDOW, lParam);
         break;
      }
      break;

   case WM_INITMENU:
      MdiInitSystemMenu( hwndChild );
      break;

   case WM_MENUCHAR:
      /* Was the <alt>- key hit when no popup currently popped up? */
      if ( ( LOWORD( lParam ) & MF_POPUP ) == 0 )
      {
         if ( wParam == '-' )
         {
            /* Let Alt-Minus mean activate our system menu */
            return MAKELONG( 0, MC_SELECT );
         }
         else
         {
            /* Another <alt> key stroke, let the parent handle it */
            SendMessage( hwndMain,
               WM_SYSCOMMAND,
               SC_KEYMENU,
               ( DWORD ) wParam );
            return MAKELONG( 0, MC_ABORT );
         }
      }
      break;

   case WM_MENUSELECT:
      iCurrentPopup = POP_CHILDSYS;
      break;

   case WM_MOUSEACTIVATE:
      MdiActivateChild( hwndChild, FALSE );
      break;

   case WM_SYSCOMMAND:
      switch( wParam & 0xFFF0 )
      {
      case SC_KEYMENU:
         if ( GetProp( hwndMain, PROP_ZOOM ) || LOWORD(lParam) != '-' )
         {
            /* Pass any other Alt-Key messages to the parent */
            return SendMessage(hwndMain,WM_SYSCOMMAND, wParam, lParam);
         }
         break;

      case SC_MAXIMIZE:
         if ( GetProp( hwndMain, PROP_ZOOM ) )
         {
            SetProp( hwndMain, PROP_ZOOM, FALSE );
            MdiRestoreChild( hwndChild, TRUE );
         }
         else
         {
            SetProp( hwndMain, PROP_ZOOM, TRUE );
            MdiZoomChild( hwndChild );
         }
         return 0L;

      case SC_RESTORE:
         if ( GetProp( hwndMain, PROP_ZOOM ) )
         {
            SetProp( hwndMain, PROP_ZOOM, FALSE );
            MdiRestoreChild( hwndChild, TRUE );
         }
         return 0L;

      case SC_NEXTWINDOW:
         /* This function is achieved only through the keyboard */
         MdiActivateNextChild( hwndChild );
         return 0L;

      case SC_PREVWINDOW:
         /* This function is achieved only through the keyboard */
         MdiActivatePrevChild( hwndChild );
         return 0L;
      }
      break;
   }
   return DefWindowProc( hwndChild, message, wParam, lParam );
}

/**/

/*
 * MdiGetMessage( hwndMain, lpMsg, hWnd, wMin, wMax ) : BOOL;
 *
 *    hwndMain       Handle to MDI desktop
 *    lpMsg          Message structure to receive message
 *    hWnd           Are messages for a specific window only?
 *    wMin           Is there a minimum message number?
 *    wMax           Is there a maximum message number?
 *
 * Get a normal windows message only after checking for keyboard
 * access to the menus.
 *
 */

BOOL MdiGetMessage(
   HWND        hwndMain,
   LPMSG       lpMsg,
   HWND        hWnd,
   WORD        wMin,
   WORD        wMax )
{
   /* Process keyboard interface to menu */
   MdiMenuMessageLoopUpdate( hwndMain );

   /* Now go get the next message */
   return GetMessage( lpMsg, hWnd, wMin, wMax );
}

/**/

/*
 * MdiTranslateAccelerators( hwndMain, lpMsg ) : int
 *
 *    hwndMain       Handle to MDI desktop
 *    lpMsg          Message structure containing message
 *
 * Translate this message via one of two accelerator tables:
 *  1) The document's system menu (i.e. <ctrl><F4>)
 *  2) The document's own accelerator table.  This table
 *     is set with the MdiSetAccelerator() call.
 *
 */

int MdiTranslateAccelerators(
   HWND        hwndMain,
   LPMSG       lpMsg )
{
   int         iResult = 0;      /* Result of last translation */
   HANDLE      hAccel;           /* Each of the two accel tables */
   HWND        hwndChild;        /* Handle to document */

   /* Is there a document on the MDI desktop? */
   hwndChild = GetProp( hwndMain, PROP_ACTIVE );
   if ( hwndChild )
   {
      /* Check for system accelerators */
      hAccel = GetProp( hwndMain, PROP_CTRLACCEL );
      iResult = TranslateAccelerator( hwndMain, hAccel, lpMsg );
      if ( !iResult )
      {
         /* Check for document accelerators */
         hAccel = GetProp( hwndChild, PROP_ACCEL );
         if ( hAccel )
         {
            iResult = TranslateAccelerator( hwndMain, hAccel, lpMsg );
         }
      }
   }
   return iResult;
}
