/*
 * MDI3.C - Menu Handling Routines
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * This module contains all the MDI code to handle the menus.  It
 * creates as well as places and removes the document system menu from
 * the MDI desktop menu bar.  It puts the title of a recently created
 * or unhidden document on the WINDOW submenu and removes it when
 * destroyed or hidden.  It switches the WINDOW submenu from one menu
 * to another when switching between documents.  Finally it handles
 * most of the keyboard interface to the menus.
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
#include <string.h>
#include <windows.h>

#include "mdi.h"

/* Static variables */
static FARPROC    lpMsgHook;        /* For the keyboard hooks */
static FARPROC    lpOldMsgHook;

/* Global variables */
int               iCurrentPopup = POP_NONE;  /* What menu is up */
int               iNextPopup = POP_NONE;     /* What menuis next */

/**/

/*
 * MdiZoomMenu( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Adjust the document menu to include the MDI system menu.  This
 * happens when a document is maximized.
 *
 */

void MdiZoomMenu( 
   HWND        hwndChild )
{
   HBITMAP     hBitmap;          /* Handle to system menu bitmap */
   HMENU       hmenuSystem;      /* Handle to system menu */
   HWND        hwndMain;         /* Handle to MDI desktop */

   /* Change the menu */
   hwndMain = GetParent( hwndChild );
   hmenuSystem = MdiGetChildSysMenu( );
   hBitmap = GetProp( hwndMain, PROP_SYSMENU );
   ChangeMenu( GetProp( hwndChild, PROP_CHILDMENU ),
      0,
      ( LPSTR ) ( DWORD ) hBitmap,
      hmenuSystem,
      MF_BITMAP | MF_BYPOSITION | MF_INSERT | MF_POPUP );
}

/**/

/*
 * MdiRestoreMenu( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Adjust the document menu by removing the MDI system menu.  This
 * happens when a document is restored.
 *
 */

void MdiRestoreMenu(
   HWND        hwndChild )
{
   /* Change the menu */
   ChangeMenu( GetProp( hwndChild, PROP_CHILDMENU ),
      0,
      ( LPSTR ) NULL,
      0,
      MF_BYPOSITION | MF_REMOVE );
}

/**/

/*
 * MdiAppendWindowMenu( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Add a newly created document's title to the WINDOW submenu.  The
 * index is calculated by counting the number of items on the submenu.
 *
 */

void MdiAppendWindowToMenu( 
   HWND        hwndChild )
{
   int         iIndex;           /* Numeric index for WINDOW menu */
   char        szCurrent[50];    /* Text of title */
   char        szText[50];       /* Text for menu */
   HMENU       hmenuChild;       /* This document's menu */
   HMENU       hmenuWindow;      /* WINDOW menu */
   HWND        hwndMain;         /* Handle to MDI desktop */

   /* Get important info */
   hwndMain = GetParent( hwndChild );
   hmenuChild = GetProp( hwndChild, PROP_CHILDMENU );
   hmenuWindow = GetProp( hwndMain, PROP_WINDOWMENU );

   /* Put in the separator */
   if ( GetMenuItemCount( hmenuWindow ) == WINDOW_POS - 1 )
   {
      ChangeMenu( hmenuWindow,
         0,
         NULL,
         0,
         MF_APPEND | MF_SEPARATOR );
   }

   /* Now add us to the WINDOW menu */
   iIndex = GetMenuItemCount( hmenuWindow ) - WINDOW_POS + 1;
   GetWindowText( hwndChild, szCurrent, sizeof( szCurrent ) );
   sprintf( szText, "&%d. %s", iIndex, szCurrent );
   ChangeMenu( hmenuWindow,
      0,
      szText,
      GetProp( hwndChild, PROP_MENUID ),
      MF_APPEND | MF_BYPOSITION | MF_CHECKED | MF_STRING );
}

/**/

/*
 * MdiReinsertWindowInMenu( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Restore a hidden document's title to the WINDOW menu.  The index is
 * calculated by counting the number of items on the submenu.
 *
 */

void MdiReinsertWindowInMenu( 
   HWND        hwndChild )
{
   char        szCurrent[50];    /* Text of title */
   char        szText[50];       /* Text for menu */
   int         iIndex;           /* Numeric index for WINDOW menu */
   HMENU       hmenuWindow;      /* WINDOW menu */
   HWND        hwndMain;         /* Handle to MDI desktop */

   /* Get important info */
   hwndMain = GetParent( hwndChild );
   hmenuWindow = GetProp( hwndMain, PROP_WINDOWMENU );

   /* Put in the separator */
   if ( GetMenuItemCount( hmenuWindow ) == WINDOW_POS - 1 )
   {
      ChangeMenu( hmenuWindow,
         0,
         NULL,
         0,
         MF_APPEND | MF_SEPARATOR );
   }

   /* Prepare window string */
   iIndex = GetMenuItemCount( hmenuWindow ) - WINDOW_POS + 1;
   GetWindowText( hwndChild, szCurrent, sizeof( szCurrent ));
   szCurrent[sizeof( szCurrent ) - 1] = '\0';
   sprintf( szText, "&%d. %s", iIndex, szCurrent );

   /* Append us to the bottom */
   ChangeMenu( hmenuWindow,
      0,
      szText,
      GetProp( hwndChild, PROP_MENUID ),
      MF_APPEND | MF_CHECKED | MF_STRING );

   /* All done */
   DrawMenuBar( hwndMain );
}

/**/

/*
 * MdiRemoveWindowFromMenu( hwndChild, bWindowDying ) : void;
 *
 *    hwndChild      Handle to document
 *    bWindowDying   Is the window being destroyed
 *
 * Remove the title of a about-to-be-hidden/destroyed document from
 * the WINDOW submenu.  If the document's title wasn't the last item on
 * the submenu then it updates the index for the documents that follow.
 *
 */

void MdiRemoveWindowFromMenu( 
   HWND        hwndChild,
   BOOL        bWindowDying )
{
   char        szCurrent[50];    /* Text of title */
   char        szText[50];       /* Text for menu */
   int         iIndex;           /* Numeric index for WINDOW menu */
   HMENU       hmenuChild;       /* This document's menu */
   HMENU       hmenuWindow;      /* WINDOW menu */
   HWND        hwndMain;         /* Handle to MDI desktop */

   /* Get important info */
   hwndMain = GetParent( hwndChild );
   hmenuChild = GetProp( hwndChild, PROP_CHILDMENU );
   hmenuWindow = GetProp( hwndMain, PROP_WINDOWMENU );

   /* Calculate our index */
   for ( iIndex = WINDOW_POS;
      iIndex < GetMenuItemCount( hmenuWindow );
      iIndex++ )
   {
      if ( GetMenuItemID( hmenuWindow, iIndex )
         == GetProp( hwndChild, PROP_MENUID ) )
      {
         /* Found us */
         break;
      }
   }

   /* Delete us from everyone's menu */
   ChangeMenu( hmenuWindow,
      GetProp( hwndChild, PROP_MENUID ),
      NULL,
      NULL,
      MF_BYCOMMAND | MF_DELETE );

   /* Adjust the rest of the window */
   if ( GetMenuItemCount( hmenuWindow ) == WINDOW_POS )
   {
      /* Remove separator */
      ChangeMenu( hmenuWindow,
         WINDOW_POS - 1,
         NULL,
         0,
         MF_DELETE | MF_BYPOSITION );
   }
   else
   {
      /* Shuffle menus down */
      for ( ; iIndex < GetMenuItemCount( hmenuWindow ); iIndex++ )
      {
         GetMenuString( hmenuWindow,   
            iIndex,
            szCurrent,
            sizeof( szCurrent ),
            MF_BYPOSITION );
         sprintf( szText,
            "&%d. %s",
            iIndex - WINDOW_POS + 1,
            strchr( szCurrent, ' ' ) + 1 );
         ChangeMenu( hmenuWindow,
            iIndex,
            szText,
            GetMenuItemID( hmenuWindow, iIndex ),
            MF_BYPOSITION | MF_CHANGE | MF_STRING | MF_UNCHECKED );
      }
   }
}

/**/

/*
 * MdiWindowMenu( hwndMain, mmenuChild, bAttach ) : void
 *
 *    hwndMain       Handle to MDI desktop
 *    hmenuChild     Current document's menu
 *    bAttach        Attach or detach
 *
 * Put or remove the WINDOW menu from the current document's menu.
 *
 */

void MdiWindowMenu(
   HWND        hwndMain,
   HMENU       hmenuChild,
   BOOL        bAttach )
{
   HMENU       hmenuWindow;         /* WINDOW menu */

   hmenuWindow = GetProp( hwndMain, PROP_WINDOWMENU );
   switch( bAttach )
   {
   case TRUE:
      /* Attach */
      ChangeMenu( hmenuChild,
         0,
         "&Window",
         hmenuWindow,
         MF_APPEND | MF_BYPOSITION | MF_POPUP );
      break;

   case FALSE:
      /* Detatch */
      ChangeMenu( hmenuChild,
         GetMenuItemCount( hmenuChild ) - 1,
         ( LPSTR ) NULL,
         0,
         MF_BYPOSITION | MF_REMOVE );
      break;
   }
   return;
}

/**/

/*
 * MdiInitSystemMenu( hwndChild ) : void
 *
 *    hwndChild      Handle to document
 *
 * Ensure that the MDI child's system menu is accurate.  This is not
 * an issue when the child is maximized, as the submenu that appears
 * on the menu bar is our own creation; however, when the system menu
 * is in the upper left corner of the window, it is actually a windows
 * system menu and contains the accelerator text for the <alt> key
 * rather than the <ctrl> key.
 *
 */

void MdiInitSystemMenu( 
   HWND        hwndChild )
{
   HMENU       hmenuSystem;      /* Handle to system menu */

   /* Get a copy of the system menu */
   hmenuSystem = GetSystemMenu( hwndChild, FALSE );

   /* Initialize system menu */
   ChangeMenu( hmenuSystem,
      SC_RESTORE,
      "&Restore    Ctrl+F5",
      SC_RESTORE,
      GetProp( GetParent( hwndChild ), PROP_ZOOM )
         ? MF_BYCOMMAND | MF_CHANGE | MF_STRING
         : MF_BYCOMMAND | MF_CHANGE | MF_GRAYED | MF_STRING );
   ChangeMenu( hmenuSystem,
      SC_MOVE,
      "&Move       Ctrl+F7",
      SC_MOVE,
      MF_BYCOMMAND | MF_CHANGE | MF_STRING );
   ChangeMenu( hmenuSystem,
      SC_SIZE,
      "&Size       Ctrl+F8",
      SC_SIZE,
      MF_BYCOMMAND | MF_CHANGE | MF_STRING );
   ChangeMenu( hmenuSystem,
      SC_MINIMIZE,
      ( LPSTR ) NULL,
      SC_MINIMIZE,
      MF_BYCOMMAND | MF_DELETE );
   ChangeMenu( hmenuSystem,
      SC_MAXIMIZE,
      "Ma&ximize   Ctrl+F10",
      SC_MAXIMIZE,
      MF_BYCOMMAND | MF_CHANGE | MF_STRING );
   ChangeMenu( hmenuSystem,
      SC_CLOSE,
      "&Close      Ctrl+F4",
      SC_CLOSE,
      MF_BYCOMMAND | MF_CHANGE | MF_STRING );
}

/**/

/*
 * MdiGetChildSysMenu() : HMENU;
 *
 *
 * Create a document system menu.  This menu is only used when the
 * document is maximized.  Otherwise the default system menu is used
 * and the text modified at WM_INITMENU.
 *
 */

HMENU MdiGetChildSysMenu( void )
{
   HMENU       hmenuSystem;      /* Handle to system menu */

   /* Change the menu */
   hmenuSystem = CreateMenu( );
   ChangeMenu( hmenuSystem, 0,
      "&Restore    Ctrl+F5",
      IDM_RESTORE,
      MF_APPEND | MF_STRING );
   ChangeMenu( hmenuSystem, 0,
      "&Move       Ctrl+F7",
      IDM_MOVE,
      MF_APPEND | MF_GRAYED | MF_STRING );
   ChangeMenu( hmenuSystem, 0,
      "&Size       Ctrl+F8",
      IDM_SIZE,
      MF_APPEND | MF_GRAYED | MF_STRING );
   ChangeMenu( hmenuSystem, 0,
      "Ma&ximize   Ctrl+F10",
      IDM_MAXIMIZE,
      MF_APPEND | MF_STRING );
   ChangeMenu( hmenuSystem, 0,
      ( LPSTR ) NULL,
      0,
      MF_APPEND | MF_SEPARATOR );
   ChangeMenu( hmenuSystem, 0,
      "&Close      Ctrl+F4",
      IDM_CLOSE,
      MF_APPEND | MF_STRING );
   return hmenuSystem;
}

/**/

/*
 * MdiCreateChildSysBitmap( hwndMain ) : HBITMAP;
 *
 *    hwndMain       Handle to MDI desktop
 *
 * Create a bitmap for the document system menu (the bitmap that looks
 * like a '-'.  The system already has one, so we use it; however there
 * are two parts to the system bitmap:  one half for parent system menu
 * bitmaps and the other half for the child system menu bitmap.  This
 * routine makes a memory DC for the two part bitmap, another memory DC
 * to receive the second (and smaller) half, creates a half sized
 * bitmap, and bitblt()s the smaller bitmap into the half sized bitmap
 * of ours.
 *
 */

HBITMAP MdiCreateChildSysBitmap( 
   HWND        hwndMain )
{
   BOOL        bResult;          /* Do we have a good bitmap made? */
   HDC         hDC;              /* DC for MDI desktop */
   HDC         hMemFullDC;       /* Mem DC for both system bitmaps */
   HDC         hMemHalfDC;       /* Mem DC for small system bitmap */
   BITMAP      Bitmap;           /* Bitmap information */
   HBITMAP     hFullBitmap;      /* Handle to both system bitmaps */
   HBITMAP     hHalfBitmap;      /* Handle to copy of small bitmap */
   int         iBitmapWidth;     /* Width of system bitmaps */
   int         iBitmapHeight;    /* Height of system bitmaps */

   bResult = FALSE;
   hFullBitmap = LoadBitmap( NULL, MAKEINTRESOURCE( OBM_CLOSE ) );
   if ( hFullBitmap )
   {
      GetObject( hFullBitmap, sizeof( Bitmap ), ( LPSTR ) &Bitmap );
      iBitmapWidth = Bitmap.bmWidth / 2;
      iBitmapHeight = GetSystemMetrics( SM_CYMENU );
      hDC = GetDC( hwndMain );
      if ( hDC )
      {
         hMemFullDC = CreateCompatibleDC( hDC );
         if ( hMemFullDC )
         {
            SelectObject( hMemFullDC, hFullBitmap );
            hMemHalfDC = CreateCompatibleDC( hDC );
            if ( hMemHalfDC )
            {
               hHalfBitmap = CreateCompatibleBitmap( hMemHalfDC,
                  iBitmapWidth, iBitmapHeight );
               if ( hHalfBitmap )
               {
                  SelectObject( hMemHalfDC, hHalfBitmap );
                  PatBlt( hMemHalfDC,
                     0, 0,
                     iBitmapWidth, iBitmapHeight,
                     WHITENESS );
                  bResult = BitBlt( hMemHalfDC,
                     0, 0,
                     iBitmapWidth, iBitmapHeight,
                     hMemFullDC,
                     iBitmapWidth, 0,
                     SRCCOPY );
               }
               DeleteDC( hMemHalfDC );
            }
            DeleteDC( hMemFullDC );
         }
         ReleaseDC( hwndMain, hDC );
      }
      DeleteObject( hFullBitmap );
   }
   return bResult ? hHalfBitmap : NULL;
}

/**/

/*
 * MdiSetMenuKeyhook( hInst ) : void;
 *
 *    hInst          Current instance handle
 *
 * Install our keyboard hook.  This allows document system menus
 * to be part of the keyboard interface to menus.
 *
 */

void MdiSetMenuKeyHook(
   HANDLE      hInst )
{
   lpMsgHook = MakeProcInstance( ( FARPROC ) MdiMsgHook, hInst );
   lpOldMsgHook = SetWindowsHook( WH_MSGFILTER, lpMsgHook );
}

/**/

/*
 * MdiMenuMessageLoopUpdate( hwndMain ) : void;
 *
 *    hwndMain       Handle to MDI desktop
 *
 * Activate the correct popup menu if the right or left arrow key
 * was hit and the document's system menu is involved.  Notice that
 * instead of doing this when the actual event occured, we wait until
 * DispatchMessage() is done.  This way the currently popped up menu
 * closes down in an orderly fashion before the next popup menu is
 * popped up.
 *
 */

void MdiMenuMessageLoopUpdate( 
   HWND        hwndMain )
{
   int         iOldNext;         /* Preserve current state info */
   HWND        hwndActive;       /* Current document window */

   /* With no messages, we're assured that no menu is popped up */
   iCurrentPopup = POP_NONE;

   if ( iNextPopup != POP_NONE )
   {
      /*
       * This sequence is used because the SendMessage() call
       * could generate reentrancy problems.
       */
      iOldNext = iNextPopup;
      iNextPopup = POP_NONE;

      /* Hilite the proper popup */
      hwndActive = GetProp( hwndMain, PROP_ACTIVE );
      switch( iOldNext )
      {
      case POP_MAINSYS:
         SendMessage( hwndMain,
            WM_SYSCOMMAND,
            SC_KEYMENU,
            (DWORD) ' ');
         break;

      case POP_MAIN1ST:
         SendMessage( hwndMain,
            WM_SYSCOMMAND,
            SC_KEYMENU,
            (DWORD) 'F');
         break;

      case POP_CHILDSYS:
         if ( hwndActive )
         {
            SendMessage(
               hwndActive,
               WM_SYSCOMMAND,
               SC_KEYMENU,
               (DWORD) '-');
         }
         break;
      }
   }
}

/**/

/*
 * MdiMsgHook( iContext, wCode, lParam ) : LONG;
 *
 *    iContext       What is the key message's destination
 *    wCode          Will this key result in an action
 *    lParm          Pointer to the key message
 *
 * Watch for left and right arrows when a menu is hilited.  This allows
 * these keys to move to the document's system menu between the MDI
 * desktop's system menu and the FILE menu.  Notice that we only set
 * variables here, we don't perform the actions.
 *
 */

LONG FAR PASCAL MdiMsgHook( 
int         iContext,
WORD        wCode,
LONG        lParam )
{
   BOOL        bCancelMenu = FALSE;
   BOOL        bActive;
   BOOL        bZoomed;
   HWND        hwndMain;
   WORD        wKey;
   MSG FAR     *lpMsg;

   lpMsg = ( MSG FAR * ) lParam;
   hwndMain = lpMsg->hwnd;
   if ( GetProp( hwndMain, PROP_ISMDI ) )
   {
      hwndMain = GetParent( hwndMain );
   }
   wKey = lpMsg->wParam;
   if ( iContext == MSGF_MENU
      && wCode == HC_ACTION
      && GetProp( hwndMain, PROP_ACTIVE )
      && !GetProp( hwndMain, PROP_ZOOM )
      && lpMsg->message == WM_KEYDOWN
      && wKey == VK_LEFT || wKey == VK_RIGHT )
   {
      /* we have a left or right arrow key in the menus */
      switch( iCurrentPopup )
      {
      case POP_CHILDSYS:
         iNextPopup = ( wKey == VK_LEFT ? POP_MAINSYS : POP_MAIN1ST );
         bCancelMenu = TRUE;
         break;

      case POP_MAINSYS:
         if ( wKey == VK_RIGHT )
         {
            iNextPopup = POP_CHILDSYS;
            bCancelMenu = TRUE;
         }
         break;

      case POP_MAIN1ST:
         if ( wKey == VK_LEFT )
         {
            iNextPopup = POP_CHILDSYS;
            bCancelMenu = TRUE;
         }
         break;
      }

      /* Do we want this to go through? */
      if ( bCancelMenu )
      {
         lpMsg->wParam = VK_CANCEL;
      }
   }
   /* Pass it on */
   return DefHookProc( iContext,
      wCode,
      lParam,
      ( FARPROC FAR * ) &lpOldMsgHook );
}

/**/

/*
 * MdiFreeMenuKeyhook( void ) : void
 *
 * Free the procedure instance required for the keyboard hook.
 *
 */

void MdiFreeMenuKeyHook( void )
{
   FreeProcInstance( lpMsgHook );
}
