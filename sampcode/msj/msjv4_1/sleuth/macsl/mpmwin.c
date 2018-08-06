/*-----------------------------------------------------------------*/
/* MpmWin.c                                                        */
/* Miscellaneous windowing functions                               */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

LOCAL BOOL      MpmShowWindow( HWND hwnd, BOOL fShow, BOOL fPaint );

/*-----------------------------------------------------------------*/
/* Default window function.  The only message currently handled    */
/* specially here is WM_CLOSE, which posts a WM_QUIT message.      */
/* All other messages just return 0L.                              */
/*-----------------------------------------------------------------*/

MRESULT APIENTRY WinDefWindowProc( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return 0L;

    switch( msg )
    {
      case WM_CLOSE:
        WinPostQueueMsg( (HAB)NULL, WM_QUIT, 0L, 0L );
        break;
    }
    
    return 0L;
}

/*-----------------------------------------------------------------*/
/* Enable or disable hwnd for keyboard/mouse input, according to   */
/* fEnable.
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinEnableWindow( hwnd, fEnable )
    HWND        hwnd;
    BOOL        fEnable;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    if( ISDESKTOPWINDOW(hwnd) || ISOBJECTWINDOW(hwnd) )
      return FALSE;

    fEnable = !! fEnable;  /* normalize boolean value */

    /* Notify window if state is changing */

    if( fEnable == !( MYWNDOF(hwnd).flStyle & WS_DISABLED ) )
      WinSendMsg( hwnd, WM_ENABLE, MPFROMSHORT(fEnable), 0 );

    /* Get rid of input focus if this window has it and it's
       being disabled */

    if( ! fEnable  &&  hwnd == _hwndFocus )
      WinSetFocus( _hwndDesktop, NULL );

    /* Tweak the style bits */

    if( fEnable )
      MYWNDOF(hwnd).flStyle &= ~WS_DISABLED;
    else
      MYWNDOF(hwnd).flStyle |= WS_DISABLED;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Set or reset hwnd's WS_VISIBLE flag, without causing any window */
/* painting.                                                       */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinEnableWindowUpdate( hwnd, fEnable )
    HWND        hwnd;
    BOOL        fEnable;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    if( ISDESKTOPWINDOW(hwnd) || ISOBJECTWINDOW(hwnd) )
      return FALSE;

    return MpmShowWindow( hwnd, fEnable, FALSE ); 
}

/*-----------------------------------------------------------------*/
/* Set the active window to hwndSetActive or its top-level parent. */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinSetActiveWindow( hwndDesktop, hwndSetActive )
    HWND        hwndDesktop, hwndSetActive;
{
    QMSG        qmsg;

    if( ! MpmValidateWindow(hwndSetActive) )
      return FALSE;

    /* Get to the main Mac window if this is a child */

    hwndSetActive = MAINHWND(hwndSetActive);

    /* Get out if this window is already active */

    if( hwndSetActive == _hwndActive )
      return FALSE;

    /* Tell the Mac to make it the active window */

    SelectWindow( PWINOFHWND(hwndSetActive) );

    /* Force the activate event to get to the window function now */

    while( WinPeekMsg( NULL, &qmsg, NULL,
                       WM_ACTIVATE, WM_ACTIVATE, PM_REMOVE ) )
      WinDispatchMsg( NULL, &qmsg );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Set the input focus to hwndSetFocus.                            */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinSetFocus( hwndDesktop, hwndSetFocus )
    HWND        hwndDesktop, hwndSetFocus;
{
    if( hwndSetFocus && ! MpmValidateWindow(hwndSetFocus) )
      return FALSE;

    /* Nothing to do if it already has the focus */

    if( hwndSetFocus == _hwndFocus )
      return TRUE;

    _sSetFocusDepth++;  /* Avoid recursive call to WinSetFocus! */

    /* Notify the previous focus window it's losing it */

    if( _hwndFocus )
      WinSendMsg( _hwndFocus, WM_SETFOCUS,
                  MPFROMHWND(_hwndFocus), MPFROMSHORT(FALSE) );

    /* Set the active window and the new focus */

    if( hwndSetFocus )
      WinSetActiveWindow( NULL, hwndSetFocus );

    _hwndFocus = hwndSetFocus;

    /* Notify the window it's receiving the focus */

    if( _hwndFocus )
      WinSendMsg( _hwndFocus, WM_SETFOCUS,
                  MPFROMHWND(_hwndFocus), MPFROMSHORT(TRUE) );

    _sSetFocusDepth--;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Set the position and size of the window(s) listed in *pswp.     */
/* This function is lazy - instead of invalidating only the child  */
/* windows actually being moved or resized, it simply invalidates  */
/* the entire main Mac window.  (See the code under #ifdef INVAL)  */
/* Also, the function doesn't handle Z-ordering.                   */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinSetMultWindowPos( hab, pswp, cswp )
    HAB         hab;
    PSWP        pswp;
    SHORT       cswp;
{
    HWND        hwnd;
    PMYWND      pwnd;
    UCHAR       ucKind;
    HWND        hwndParent;
    SHORT       xParent, yParent, cxParent, cyParent;
    SHORT       cxOld, cyOld;
    ULONG       flStyle;
    Rect        rect;
    Rect*       prect;
    Rect*       prectAdj;

    ucKind = MYWNDOF(pswp->hwnd).ucKind;

    ASSERT( ucKind >= WK_MAIN,
            "WinSetMultWindowPos: Desktop or object window illegal" );

    /* Don't worry, be happy - just invalidate the whole thing */

#ifndef INVAL
    {
      WindowPeek  pwin;
      thePort = &PWINOFHWND(pswp->hwnd)->port;
      rect = thePort->portRect;
      InvalRect( &rect );
      EraseRect( &rect );
    }
#endif

    hwndParent = NULL;

    /* Loop through all the windows in *pswp */

    for( ;  cswp > 0;  pswp++, cswp-- )
    {
      hwnd = pswp->hwnd;
      if( ! hwnd )
        continue;

      if( ! MpmValidateWindow(hwnd) )
        return FALSE;

      flStyle = MYWNDOF(hwnd).flStyle;

      /* Pick up parent window info, make sure all windows have the
         same parent */

      if( ! hwndParent )
      {
        hwndParent = MYWNDOF(hwnd).hwndParent;
        pwnd = PMYWNDOF(hwndParent);
        xParent  = pwnd->x;
        yParent  = pwnd->y;
        cxParent = pwnd->cx;
        cyParent = pwnd->cy;
      }

      ASSERT( MYWNDOF(hwnd).hwndParent == hwndParent,
              "WinSetMultWindowPos: Windows have different parents" );

      /* Handle moving and sizing if requested */

      if( pswp->fs & (SWP_MOVE|SWP_SIZE) )
      {
#ifdef INVAL
        if( flStyle & WS_VISIBLE )
          MpmInvalidateWindow( hwnd );
#endif

        if( ! MYWNDOF(hwnd).hwndTopChild && (flStyle & WS_VISIBLE) )
          MYWNDOF(hwnd).flStyle &= ~WS_VISIBLE;  /* hack! */

        /* Set up new coordinates */

        pwnd = PMYWNDOF(hwnd);
        cxOld = pwnd->cx;
        cyOld = pwnd->cy;
        if( pswp->fs & SWP_MOVE )
        {
          pwnd->x = pswp->x;
          pwnd->y = pswp->y;
        }
        if( pswp->fs & SWP_SIZE )
        {
          pwnd->cx = pswp->cx;
          pwnd->cy = pswp->cy;
        }

        /* Give the window a chance to tweak the coordinates with
           a WM_ADJUSTWINDOWPOS message */

        if( ! ( pswp->fs & SWP_NOADJUST ) )
          WinSendMsg( hwnd, WM_ADJUSTWINDOWPOS, MPFROMP(pswp), 0L );

        /* any fancy BitBlts could go here... */

        /* For a top-level window, ask the Mac to move and size it */

        if( ucKind == WK_MAIN )
        {
          pwnd = PMYWNDOF(hwnd);
          prectAdj  = &pwnd->rectAdj;
          MoveWindow( MYWNDOF(hwnd).pwin,
                      pwnd->x + prectAdj->left,
                      _alSysVal[SV_CYSCREEN] - pwnd->y - pwnd->cy
                        + prectAdj->top,
                      FALSE );
          SizeWindow( MYWNDOF(hwnd).pwin,
                      pwnd->cx - prectAdj->left + prectAdj->right,
                      pwnd->cy - prectAdj->top  + prectAdj->bottom,
                      FALSE );
        }

        /* Notify the window of the new position and size */

        if( MYCLASSOF(hwnd).class.flClassStyle & CS_MOVENOTIFY )
          WinSendMsg( hwnd, WM_MOVE, 0L, 0L );

        WinSendMsg( hwnd, WM_SIZE,
                    MPFROM2SHORT( cxOld, cyOld ),
                    MPFROM2SHORT( pwnd->cx, pwnd->cy ) );

#ifdef INVAL
        if( flStyle & WS_VISIBLE )
          MpmInvalidateWindow( hwnd );
#endif

        if( ! MYWNDOF(hwnd).hwndTopChild && (flStyle & WS_VISIBLE) )
          MYWNDOF(hwnd).flStyle |= WS_VISIBLE;  /* hack! */
      }
    }

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Set one window's position and size.  This is easy - just call   */
/* WinSetMultWindowPos!  (Note that a SWP structure is defined to  */
/* exactly match the parameters on the stack for WinSetWindowPos.) */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinSetWindowPos( hwnd, hwndBehind, x, y, cx, cy, fs )
    HWND        hwnd, hwndBehind;
    SHORT       x, y, cx, cy;
    USHORT      fs;
{
    return WinSetMultWindowPos( (HAB)NULL, (PSWP)&hwnd, 1 );
}

/*-----------------------------------------------------------------*/
/* Set a window's text, by sending a WM_SETWINDOWPARAMS message.   */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinSetWindowText( hwnd, pszText )
    HWND        hwnd;
    PSZ         pszText;
{
    WNDPARAMS   wprm;

    memzero( &wprm );

    wprm.fsStatus = WPM_TEXT | WPM_CCHTEXT;
    wprm.pszText = pszText;
    wprm.cchText = strlen( pszText );

    return (BOOL)WinSendMsg( hwnd, WM_SETWINDOWPARAMS,
                                   MPFROMP(&wprm), 0 );
}

/*-----------------------------------------------------------------*/
/* Show or hide hwnd, according to fShow.                          */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinShowWindow( hwnd, fShow )
    HWND        hwnd;
    BOOL        fShow;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    if( ISDESKTOPWINDOW(hwnd) || ISOBJECTWINDOW(hwnd) )
      return FALSE;

    return MpmShowWindow( hwnd, fShow, TRUE );  
}

/*-----------------------------------------------------------------*/
/* Do the real work for WinShowWindow and WinEnableWindowUpdate.   */
/* Recurse through all the child windows to take care of them, set */
/* WS_VISIBLE as requested, ask the Mac to show or hide a top      */
/* level window, and send the WM_SHOW message.                     */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmShowWindow( hwnd, fShow, fPaint )
    HWND        hwnd;
    BOOL        fShow;
    BOOL        fPaint;
{
    HWND        hwndChild;

    /* Call this function recursively for all children */

    for( hwndChild = MYWNDOF(hwnd).hwndTopChild;
         hwndChild;
         hwndChild = MYWNDOF(hwndChild).hwndNextSibling )
      MpmShowWindow( hwndChild, fShow, FALSE );

    /* Tweak the WS_VISIBLE flag */

    if( fShow )
    {
      fShow = TRUE;  /* normalize fShow */

      if( MYWNDOF(hwnd).flStyle & WS_VISIBLE )
        return FALSE;

      MYWNDOF(hwnd).flStyle |= WS_VISIBLE;
    }
    else
    {
      if( !( MYWNDOF(hwnd).flStyle & WS_VISIBLE ) )
        return FALSE;

      MYWNDOF(hwnd).flStyle &= ~WS_VISIBLE;
    }

    /* Update the screen if requested */

    if( fPaint )
    {
      if( ISCHILDWINDOW(hwnd) )
#ifdef INVAL
        MpmInvalidateWindow( hwnd );
#else
        ;
#endif
      else if( fShow )
        ShowWindow( PWINOFHWND(hwnd) );
      else
        HideWindow( PWINOFHWND(hwnd) );
    }

    /* Let the window know with a WM_SHOW */

    WinSendMsg( hwnd, WM_SHOW, MPFROMSHORT(fShow), 0L );

    return TRUE;  
}

/*-----------------------------------------------------------------*/
/* Window function for button controls.  (Unimplemented!)          */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpButton( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for the desktop window.  Nothing to do!         */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpDesktop( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for dialog box class.  (Unimplemented!)         */
/*-----------------------------------------------------------------*/

#ifdef FUTURE

MRESULT EXPENTRY MpmFnwpDialog( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

#endif

/*-----------------------------------------------------------------*/
/* Window function for edit fields.  (Unimplemented!)              */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpEntryField( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for list boxes.  (Unimplemented!)               */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpListBox( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for object window.  Nothing to do!              */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpObject( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for static text fields.  (Unimplemented!)       */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpStatic( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
