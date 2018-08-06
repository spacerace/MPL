/*
 * MDI2.C - Child Window Routines
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * This module contains all the MDI code to handle the activation
 * sequence for and switching activation between the MDI document
 * windows.  It also contains the maximizing and restoring of document
 * windows and especially the activation and switching between them.
 * It also contains the code for the unhide dialog box.
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

/**/

/*
 * MdiDestroyChildWindow( hwndChild ) : void;
 *
 *    hwndChild      Handle of document window
 *
 * This routine is called when a document window is closing.  It picks
 * a new active document window and activates it.  If no other document
 * window is available (visible) then it restores the original menu
 * on the MDI desktop and sets the focus to the desktop.
 *
 */

void MdiDestroyChildWindow( 
   HWND        hwndChild )
{
   HMENU       hmenuMain;        /* MDI desktop menu */
   HMENU       hmenuOld;         /* Document menu */
   HWND        hwndNext;         /* Next active document window */
   HWND        hwndMain;         /* Handle to MDI desktop */

   /* Remove ourselves from the menus */
   MdiRemoveWindowFromMenu( hwndChild, TRUE );

   /* Let's choose a new MDI child to be active */
   hwndMain = GetParent( hwndChild );
   hwndNext = MdiChooseNewActiveChild( hwndChild );
   if ( hwndNext )
   {
      /* Make this MDI child active */
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         MdiSwitchZoom( hwndNext, hwndChild );
      }
      MdiActivateChild( hwndNext, FALSE );

   }
   else
   {
      /* No other MDI child visible */
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         SetProp( hwndMain, PROP_ZOOM, FALSE );
         MdiRestoreChild( hwndChild, FALSE );
      }

      /* Back to bare-bones menu */
      hmenuOld = GetMenu( hwndMain );
      hmenuMain = GetProp( hwndMain, PROP_MAINMENU );
      MdiWindowMenu( hwndMain, hmenuMain, TRUE );
      SetMenu( hwndMain, hmenuMain );
      MdiWindowMenu( hwndMain, hmenuOld, FALSE );
      SetProp( hwndMain, PROP_ACTIVE, NULL );

      /* Focus back to parent */
      SetFocus( hwndMain );
   }

   /* Ensure menu updated */
   DrawMenuBar( hwndMain );
}

/**/

/*
 * MdiActivateChild( hwndChild, bHidden ) : void;
 *
 *    hwndChild      Handle to document window
 *    bHidden        Was document previous hidden?
 *
 * Bring a specific document window to the fore.  Give it the correct
 * title bar, bring it to the top, put the matching menu on the MDI
 * desktop, etc.
 *
 */

void MdiActivateChild( 
   HWND        hwndChild,
   BOOL        bHidden )
{
   HMENU       hmenuOld;         /* Previous document's menu */
   HMENU       hmenuNew;         /* This document's menu */
   HMENU       hmenuWindow;      /* WINDOW submenu */
   HWND        hwndActive;       /* Handle to previous document */
   HWND        hwndMain;         /* Handle to MDI desktop */
   LONG        lStyle;           /* Style of document window */

   /* Check if this MDI child already activated */
   hwndMain = GetParent( hwndChild );
   lStyle = GetWindowLong( hwndChild, GWL_STYLE );
   if ( ( lStyle & WS_MAXIMIZEBOX ) == 0 )
   {
      /* Change the title bar color */
      SendMessage( hwndChild, WM_NCACTIVATE, TRUE, 0L );

      /* Add system menu & maximize arrow to title bar */
      SetWindowLong( hwndChild,
         GWL_STYLE,
         lStyle | WS_MAXIMIZEBOX | WS_SYSMENU );

      /* Make changes visible */
      if ( bHidden )
      {
         BringWindowToTop( hwndChild );
         ShowWindow( hwndChild, SW_SHOW );
      }
      else
      {
         /* Bring to top & draw nonclient area */
         SetWindowPos( hwndChild,
            NULL,
            0, 0, 0, 0,
            SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE );
      }

      /* Update this child */
      hwndActive = GetProp( hwndMain, PROP_ACTIVE );
      if ( hwndActive != hwndChild )
      {
         /* Remove traces of activeness from previous MDI child */
         if ( hwndActive && hwndActive != hwndChild )
         {
            MdiDeactivateChild( hwndActive );
         }

         /* Install correct menu for this MDI child */
         hmenuNew = GetProp( hwndChild, PROP_CHILDMENU );
         hmenuOld = GetMenu( hwndMain );
         MdiWindowMenu( hwndMain, hmenuNew, TRUE );
         SetMenu( hwndMain, hmenuNew );
         MdiWindowMenu( hwndMain, hmenuOld, FALSE );
         DrawMenuBar( hwndMain );

         /* This child now active */
         SetProp( hwndMain, PROP_ACTIVE, hwndChild );
      }

      /* Check our menu on the WINDOW menu */
      hmenuWindow = GetProp( hwndMain, PROP_WINDOWMENU );
      CheckMenuItem( hmenuWindow,
         GetProp( hwndChild, PROP_MENUID ),
         MF_CHECKED | MF_BYCOMMAND );

      /* Should have focus */
      if ( GetFocus( ) != hwndChild )
      {
         SetFocus( hwndChild );
      }
   }
}

/**/

/*
 * MdiActivateNextChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to current document
 *
 * Activate the next document in the internal windows queue.
 * This function reached via <ctrl><F6> only.
 *
 */

void MdiActivateNextChild( 
   HWND        hwndChild )
{
   HWND        hwndActive;       /* Handle to previous document */
   HWND        hwndIter;         /* Iterating document windows */
   HWND        hwndMain;         /* Handle to MDI desktop */

   for ( hwndIter = GetWindow( hwndChild, GW_HWNDNEXT );
      hwndIter != NULL;
      hwndIter = GetWindow( hwndIter, GW_HWNDNEXT ) )
   {
      /*
      ** Get next visible, MDI child in internal windows chain
      */
      if ( GetProp(hwndIter, PROP_ISMDI) && IsWindowVisible(hwndIter) )
      {
         break;
      }
   }

   /* Did we find another MDI child? */
   if ( hwndIter )
   {
      hwndMain = GetParent( hwndIter );
      hwndActive = GetProp( hwndMain, PROP_ACTIVE );
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         /* Activate the new one */
         MdiSwitchZoom( hwndIter, hwndActive );
         MdiActivateChild( hwndIter, FALSE );

         /* Place window at end of internal windows queue */
         SetWindowPos( hwndChild,
            ( HWND ) 1,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
      }
      else
      {
         /* Place window at end of internal windows queue */
         SetWindowPos( hwndChild,
            ( HWND ) 1,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

         /* Activate the new one */
         MdiActivateChild( hwndIter, FALSE );
      }
   }
}

/**/

/*
 * MdiActivatePrevChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to current document
 *
 * Activate the previous document in the internal windows queue.
 * This function reached via <ctrl><shift><F6> only.
 *
 */

void MdiActivatePrevChild( 
   HWND        hwndChild )
{
   HWND        hwndActive;       /* Handle to previous document */
   HWND        hwndIter;         /* Iterate document windows */
   HWND        hwndMain;         /* Handle to MDI desktop */

   for ( hwndIter = GetWindow( hwndChild, GW_HWNDLAST );
      hwndIter != NULL;
      hwndIter = GetWindow( hwndIter, GW_HWNDPREV ) )
   {
      /*
      ** Get prev visible, MDI window in internal windows chain
      */
      if ( GetProp(hwndIter, PROP_ISMDI) && IsWindowVisible(hwndIter) )
      {
         break;
      }
   }

   /* Did we find another MDI child? */
   if ( hwndIter )
   {
      /* Activate the new one */
      hwndMain = GetParent( hwndChild );
      hwndActive = GetProp( hwndMain, PROP_ACTIVE );
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         MdiSwitchZoom( hwndIter, hwndActive );
      }
      MdiActivateChild( hwndIter, FALSE );
   }
}

/**/

/*
 * MdiDectivateChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to document window
 *
 * Put a specific document window to the back.  Either another
 * document is being activated or the application is losing focus.
 *
 */

void MdiDeactivateChild( 
   HWND        hwndChild )
{
   LONG        lStyle;           /* Style of document window */

   /* Check if this MDI child already deactivated */
   lStyle = GetWindowLong( hwndChild, GWL_STYLE );
   if ( lStyle & WS_MAXIMIZEBOX )
   {
      /* Change the title bar color */
      SendMessage( hwndChild, WM_NCACTIVATE, FALSE, 0L );

      /* Add system menu & maximize arrow to title bar */
      SetWindowLong( hwndChild,
         GWL_STYLE,
         lStyle & ~( WS_MAXIMIZEBOX | WS_SYSMENU ) );

      SetWindowPos( hwndChild,
         NULL,
         0, 0, 0, 0,
         SWP_DRAWFRAME  | SWP_NOACTIVATE  | SWP_NOMOVE
                        | SWP_NOSIZE      | SWP_NOZORDER );
   }

   /* Remove check mark for our window */
   CheckMenuItem( GetProp( GetParent( hwndChild ), PROP_WINDOWMENU ),
      GetProp( hwndChild, PROP_MENUID ),
      MF_UNCHECKED | MF_BYCOMMAND );
}

/**/

/*
 * MdiZoomChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Bring the specific document to full client area display.  A default
 * maximize puts the border and title bar in the client area.  This
 * routine matches the document client area to the MDI desktop client
 * area.  It also appends the document title to the MDI desktop
 * title.
 *
 */

void MdiZoomChild( 
   HWND        hwndChild )
{
   char        szTitle[128];     /* Text for title bar */
   char        szMain[128];      /* Text on non-zoomed title bar */
   char        szChild[128];     /* Text for document title bar */
   HWND        hwndMain;         /* Handle to MDI desktop */
   LONG        lStyle;           /* Style of document window */
   RECT        rcClient;         /* Client area of MDI desktop */

   /* Change the menu */
   hwndMain = GetParent( hwndChild );
   MdiZoomMenu( hwndChild );
   DrawMenuBar( hwndMain );

   /* Change the title bar */
   GetAtomName( GetProp( hwndMain, PROP_TITLE ),
      szMain,
      sizeof(szMain) );
   GetWindowText( hwndChild, szChild, sizeof( szChild ) );
   sprintf( szTitle, "%s - %s", szMain, szChild );
   SetWindowText( hwndMain, szTitle );

   /* Remember where this window was in the client area */
   GetWindowRect( hwndChild, &rcClient );
   ScreenToClient( hwndMain, ( LPPOINT ) &rcClient+0 );
   ScreenToClient( hwndMain, ( LPPOINT ) &rcClient+1 );

   SetProp( hwndChild, PROP_LEFT, rcClient.left );
   SetProp( hwndChild, PROP_TOP, rcClient.top );
   SetProp( hwndChild, PROP_WIDTH, rcClient.right - rcClient.left );
   SetProp( hwndChild, PROP_HEIGHT, rcClient.bottom - rcClient.top );

   /* Bring the window to use the whole client area */
   GetClientRect( hwndMain, &rcClient );
   lStyle = GetWindowLong( hwndChild, GWL_STYLE );
   AdjustWindowRect( &rcClient, lStyle, FALSE );
   SetWindowPos( hwndChild,
      ( HWND ) NULL,
      rcClient.left, rcClient.top,
      rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
      0 );

   /* Mark maximized in the window style */
   SetWindowLong( hwndChild, GWL_STYLE, lStyle | WS_MAXIMIZE );
}

/**/

/*
 * MdiRestoreChild( hwndChild, bHidden ) : void;
 *
 *    hwndChild      Handle to document
 *    bShowMove      Should we show this occurance or hide it
 *
 * Bring the specific document back from full client area display.
 * This routine places the document window where it was before the
 * maximize by remembering the position in property lists.  It also
 * restores the original title to the MDI desktop titlebar.
 *
 */

void MdiRestoreChild( 
   HWND        hwndChild,
   BOOL        bShowMove )
{
   char        szMain[128];      /* Text on non-zoomed title bar */
   HWND        hwndMain;         /* Handle to MDI desktop */
   LONG        lStyle;           /* Style of document window */

   /* Change the menu */
   hwndMain = GetParent( hwndChild );
   MdiRestoreMenu( hwndChild );
   DrawMenuBar( hwndMain );

   /* Reflect in the title bar of the main app */
   GetAtomName( GetProp( hwndMain, PROP_TITLE ),
      szMain,
      sizeof( szMain ) );
   SetWindowText( hwndMain, szMain );

   /* Mark normal size in the window style */
   lStyle = GetWindowLong( hwndChild, GWL_STYLE );
   SetWindowLong( hwndChild, GWL_STYLE, lStyle & ~WS_MAXIMIZE );

   /* Are we HIDING the last visible MDI child */
   if ( !bShowMove )
   {
      ShowWindow( hwndChild, SW_HIDE );
   }

   /* Move the window back to where it was before it zoomed */
   /* Also place in beginning of internal windows queue */
   SetWindowPos( hwndChild,
      ( HWND ) NULL,
      GetProp( hwndChild, PROP_LEFT ),
      GetProp( hwndChild, PROP_TOP ),
      GetProp( hwndChild, PROP_WIDTH ),
      GetProp( hwndChild, PROP_HEIGHT ),
      0 );
}

/**/

/*
 * MdiSwitchZoom( hwndNew, hwndCur ) : void;
 *
 *    hwndNew        Handle to new document
 *    hwndCur        Handle to previous document
 *
 * This routine switches activation between two document windows
 * when the original document window is maximized.  Every time document
 * windows switch, the sequence of activation is important.  When the
 * documents are maximized, it is crucial to not follow the ordinary
 * sequence, otherwise it is possible to see the other, non-maximized
 * document windows briefly before the new document window covers
 * the client area.
 *
 */

void MdiSwitchZoom( 
   HWND        hwndNew,
   HWND        hwndCur )
{
   char        szTitle[128];     /* Text for title bar */
   char        szMain[128];      /* Text on non-zoomed title bar */
   char        szChild[128];     /* Text for document title bar */
   HWND        hwndMain;         /* Handle to MDI desktop */
   LONG        lStyle;           /* Style of document window */
   RECT        rcClient;         /* Client area of MDI desktop */

   /* Change the title bar */
   hwndMain = GetParent( hwndCur );
   GetAtomName( GetProp( hwndMain, PROP_TITLE ),
      szMain,
      sizeof(szMain) );
   GetWindowText( hwndNew, szChild, sizeof( szChild ) );
   sprintf( szTitle, "%s - %s", szMain, szChild );
   SetWindowText( hwndMain, szTitle );

   /* NEW WINDOW */

   /* Remember where this window was in the client area */
   GetWindowRect( hwndNew, &rcClient );
   ScreenToClient( hwndMain, ( LPPOINT ) &rcClient+0 );
   ScreenToClient( hwndMain, ( LPPOINT ) &rcClient+1 );

   SetProp( hwndNew, PROP_LEFT, rcClient.left );
   SetProp( hwndNew, PROP_TOP, rcClient.top );
   SetProp( hwndNew, PROP_WIDTH, rcClient.right - rcClient.left );
   SetProp( hwndNew, PROP_HEIGHT, rcClient.bottom - rcClient.top );

   /* Reposition the new window */
   GetClientRect( GetParent( hwndNew ), &rcClient );
   lStyle = GetWindowLong( hwndNew, GWL_STYLE );
   AdjustWindowRect( &rcClient, lStyle, FALSE );
   SetWindowPos( hwndNew,
      ( HWND ) NULL,
      rcClient.left, rcClient.top,
      rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
      SWP_NOREDRAW | SWP_NOACTIVATE );

   /* Mark maximized in the window style */
   SetWindowLong( hwndNew, GWL_STYLE, lStyle | WS_MAXIMIZE );

   /* Update menu */
   MdiZoomMenu( hwndNew );

   /* Display the window */
   ShowWindow( hwndNew, SW_SHOW );

   /* Client area is of the old window */
   InvalidateRect( hwndNew, ( LPRECT ) NULL, TRUE );

   /* OLD WINDOW */

   /* Mark normal size in the window style */
   lStyle = GetWindowLong( hwndCur, GWL_STYLE );
   SetWindowLong( hwndCur, GWL_STYLE, lStyle & ~WS_MAXIMIZE );

   /* Move the window back to where it was before it zoomed */
   SetWindowPos( hwndCur,
      ( HWND ) NULL,
      GetProp( hwndCur, PROP_LEFT ),
      GetProp( hwndCur, PROP_TOP ),
      GetProp( hwndCur, PROP_WIDTH ),
      GetProp( hwndCur, PROP_HEIGHT ),
      SWP_NOACTIVATE | SWP_NOZORDER );

   /* Update menu */
   MdiRestoreMenu( hwndCur );
}

/**/

/*
 * MdiUnhideChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Decide on the next document to activate when the current one is
 * hidden or closed.
 *
 */

HWND MdiChooseNewActiveChild( 
   HWND        hwndChild )
{
   HWND        hwndIter;         /* Iterate document windows */

   /* Choose a new child window for activation */
   for ( hwndIter = GetWindow( hwndChild, GW_HWNDFIRST );
      hwndIter != NULL;
      hwndIter = GetWindow( hwndIter, GW_HWNDNEXT ) )
   {
      /*
      ** Get next visible, MDI child in internal windows chain
      ** that's not the current one.
      */
      if ( GetProp( hwndIter, PROP_ISMDI )
         && hwndIter != hwndChild
         && IsWindowVisible( hwndIter ) )
      {
         break;
      }
   }
   return hwndIter;
}

/**/

/*
 * MdiHideChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Hide a specific document from view.  This routine also handles
 * activating the next appropriate document window.  If none exist
 * then it restores the original menu on the MDI desktop and sets
 * the focus to the desktop.
 *
 */

void MdiHideChild( 
   HWND        hwndChild )
{
   int         iCount;           /* Number of hidden documents */
   HMENU       hmenuMain;        /* MDI desktop menu */
   HMENU       hmenuOld;         /* Document menu */
   HWND        hwndMain;         /* Handle to MDI desktop */
   HWND        hwndNext;         /* Next active document window */

   /* One less visible MDI child */
   hwndMain = GetParent( hwndChild );
   iCount = GetProp( hwndMain, PROP_HIDDEN );
   SetProp( hwndMain, PROP_HIDDEN, ++iCount );

   /* We're no longer around */
   MdiDeactivateChild( hwndChild );
   MdiRemoveWindowFromMenu( hwndChild, FALSE );

   /* Find us a new active MDI child */
   hwndNext = MdiChooseNewActiveChild( hwndChild );

   /* Juggle the windows around */
   if ( hwndNext )
   {
      /* There is another window to put up */
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         MdiSwitchZoom( hwndNext, hwndChild );
      }
      ShowWindow( hwndChild, SW_HIDE );
      MdiActivateChild( hwndNext, FALSE );
   }
   else
   {
      /* Hiding the last MDI child */
      if ( GetProp( hwndMain, PROP_ZOOM ) )
      {
         SetProp( hwndMain, PROP_ZOOM, FALSE );
         MdiRestoreChild( hwndChild, FALSE );
      }
      ShowWindow( hwndChild, SW_HIDE );

      /* Back to bare-bones menu */
      hmenuOld = GetMenu( hwndMain );
      hmenuMain = GetProp( hwndMain, PROP_MAINMENU );
      MdiWindowMenu( hwndMain, hmenuMain, TRUE );
      SetMenu( hwndMain, hmenuMain );
      MdiWindowMenu( hwndMain, hmenuOld, FALSE );
      SetProp( hwndMain, PROP_ACTIVE, NULL );

      /* Focus back to parent */
      SetFocus( hwndMain );
   }
}

/**/

/*
 * MdiUnhideChild( hwndChild ) : void;
 *
 *    hwndChild      Handle to document
 *
 * Bring a previous hidden document back to view.
 *
 */

void MdiUnhideChild( 
   HWND        hwndChild )
{
   int         iCount;           /* Number of hidden documents */
   HWND        hwndMain;         /* Handle to MDI desktop */

   /* One more visible MDI child */
   hwndMain = GetParent( hwndChild );
   iCount = GetProp( hwndMain, PROP_HIDDEN );
   SetProp( hwndMain, PROP_HIDDEN, --iCount );

   /* Adjust menu */
   MdiReinsertWindowInMenu( hwndChild );

   /* We're activating */
   if ( GetProp( hwndMain, PROP_ZOOM ) )
   {
      MdiSwitchZoom( hwndChild, GetProp( hwndMain, PROP_ACTIVE ) );
   }
   MdiActivateChild( hwndChild, TRUE );
}

/**/

/*
 * MdiDlgUnhide( hwndUnhide, message, wParam, lParam ) : int;
 *
 *    hwndUnhide     Handle to dialog box
 *    message        Current message
 *    wParam         Word parameter to the message
 *    lParam         Long parameter to the message
 *
 * Handle the UNHIDE dialog box.  This routine assumes that no two
 * document windows have the same title.  When the user selects a title
 * from the listbox, this routine iterates the existing documents for
 * a window with the same title.  It then unhides that one.
 *
 */

int FAR PASCAL MdiDlgUnhide(
   HWND        hwndUnhide,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   char        sTitle[80];       /* Title of document windows */
   char        sSelected[80];    /* Selected document in list box */
   int         iIndex;           /* Index into list box */
   HWND        hwndIter;         /* Iterate document windows */
   HWND        hwndMain;         /* Handle to MDI desktop */

   switch ( message )
   {
   case WM_INITDIALOG:
      /* Put the titles of the MDI children in the list box */
      hwndMain = GetParent( hwndUnhide );
      for ( hwndIter = GetTopWindow( hwndMain );
         hwndIter != NULL;
         hwndIter = GetWindow( hwndIter, GW_HWNDNEXT ) )
      {
         /* Ignore non-MDI or visible children */
         if ( !GetProp( hwndIter, PROP_ISMDI )
            || IsWindowVisible( hwndIter ) )
         {
            continue;
         }

         /* A potential window to unhide */
         GetWindowText( hwndIter, sTitle, sizeof( sTitle ) );
         SendDlgItemMessage( hwndUnhide,
            DLGUNHIDE_LB,
            LB_ADDSTRING,
            0,
            ( LONG ) ( LPSTR ) sTitle );
      }

      /* Select the first MDI child in the list box */
      SendDlgItemMessage( hwndUnhide, DLGUNHIDE_LB, LB_SETCURSEL, 0, 0L );
      return TRUE;
   
   case WM_COMMAND:
      switch( wParam )
      {
      case IDOK:
         /* Which MDI child is selected in the list box? */
         iIndex = ( int ) SendDlgItemMessage( hwndUnhide,
            DLGUNHIDE_LB,
            LB_GETCURSEL,
            0,
            0L );
         if ( iIndex == LB_ERR )
         {
            EndDialog( hwndUnhide, NULL );
            break;
         }

         /* Get the title of the MDI child selected */
         SendDlgItemMessage( hwndUnhide,
            DLGUNHIDE_LB,
            LB_GETTEXT,
            iIndex,
            ( LONG ) ( LPSTR ) sSelected );

         /* Compare the title to all our MDI children */
         hwndMain = GetParent( hwndUnhide );
         for ( hwndIter = GetTopWindow( hwndMain );
            hwndIter != NULL;
            hwndIter = GetWindow( hwndIter, GW_HWNDNEXT ) )
         {
            /* Ignore non-MDI or visible children */
            if ( !GetProp( hwndIter, PROP_ISMDI )
               || IsWindowVisible( hwndIter ) )
            {
               continue;
            }

            /* Found one that's hidden */
            GetWindowText( hwndIter, sTitle, sizeof( sTitle ) );
            if ( strcmp( sTitle, sSelected ) == 0 )
            {
               /* Found it.  This relies on the fact that
               ** no two MDI children can have the same
               ** title.
               */
               EndDialog( hwndUnhide, ( int ) hwndIter );
               return FALSE;
            }
         }

         /* No MDI child selected */
         EndDialog( hwndUnhide, NULL );
         break;
      
      case IDCANCEL:
         EndDialog( hwndUnhide, NULL );
         break;

      case DLGUNHIDE_LB:
         /* Double click on list box same as OK */
         if ( HIWORD( lParam ) == LBN_DBLCLK )
         {
            SendMessage( hwndUnhide, WM_COMMAND, IDOK, 0L );
         }
         break;
      }
      break;
   }
   return FALSE;
}
