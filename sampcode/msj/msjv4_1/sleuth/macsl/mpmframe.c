/*-----------------------------------------------------------------*/
/* MpmFrame.c                                                      */
/* Frame window functions                                          */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

LOCAL BOOL      MpmFrameFixCtl( HWND hwndFrame, ULONG flFrameFlags,
                                ULONG flFlagMask, PVOID pCtlData,
                                PSZ pszClass, PHWND pahwndCtls,
                                USHORT usFid, ULONG flCtlStyle );
LOCAL MRESULT   MpmFrameFormat( HWND hwndFrame );
LOCAL MRESULT   MpmFrameUpdate( HWND hwndFrame, ULONG flFrameFlags );
LOCAL SHORT     MpmFrameQueryCtls( HWND hwndFrame,
                                   PHWND pahwndCtls );
LOCAL VOID      MpmZoomWindow( HWND hwnd, SHORT sPart );

/*-----------------------------------------------------------------*/
/* Create a frame window and the specified frame controls.         */
/*-----------------------------------------------------------------*/

HWND APIENTRY WinCreateStdWindow( hwndParent, flStyle, pCtlData,
                                  pszClassClient, pszTitle,
                                  flStyleClient, hmod, idResources,
                                  phwndClient )
    HWND        hwndParent;
    ULONG       flStyle, flStyleClient;
    PVOID       pCtlData;
    PSZ         pszClassClient, pszTitle;
    HMODULE     hmod;
    USHORT      idResources;
    PHWND       phwndClient;
{
    HWND        hwndFrame, hwndClient;
    SHORT       x, y, cx, cy;
    ULONG       flFrameFlags;

    /* Pick up frame flags, take care of special hwndParent values */

    flFrameFlags = ( pCtlData ? *(PULONG)pCtlData : 0 );

    if( ! hwndParent || hwndParent == HWND_DESKTOP )
      hwndParent = _hwndDesktop;

    ASSERT( hwndParent == _hwndDesktop,
            "WinCreateStdWindow: Must be a top level window" );
    ASSERT( ! hmod,
            "WinCreateStdWindow: hmod must be NULL" );
    ASSERT( ! (flStyle & WS_CLIPCHILDREN),
            "WinCreateStdWindow: WS_CLIPCHILDREN not allowed" );

    /* Assign default position if visible */

    if( flStyle & WS_VISIBLE )
    {
      x = 10;       /* TEMP HACK */
      y = screenBits.bounds.bottom - 200;
      cx = 250;
      cy = 150;
    }
    else
      x = y = cx = cy = 0;

    /* Create the frame window itself */

    hwndFrame =
      WinCreateWindow( hwndParent, WC_FRAME, _szNull,
                       flStyle & ~WS_VISIBLE, x, y, cx, cy,
                       NULL, HWND_TOP, idResources, pCtlData, NULL );

    if( ! hwndFrame )
      return NULL;

    /* Create frame controls according to flFrameFlags */

    MpmFrameUpdate( hwndFrame, flFrameFlags );

    /* Create client window if requested */

    if( pszClassClient && *pszClassClient )
    {
      *phwndClient = hwndClient =
        WinCreateWindow(
          hwndFrame, pszClassClient, _szNull, flStyleClient,
          0, 0, 0, 0, hwndFrame, HWND_BOTTOM, FID_CLIENT, NULL, NULL
        );
      if( ! hwndClient )
        goto exitDestroy;
    }

    /* Create menu and initialize title */

    if( flFrameFlags & FCF_MENU )
      if( ! MpmMenuLoad( hwndFrame, idResources ) )
        goto exitDestroy;

    if( flFrameFlags & FCF_TITLEBAR )
      WinSetWindowText( hwndFrame, pszTitle );

    /* Make window visible if requested */

    if( flStyle & WS_VISIBLE )
    {
      WinSendMsg( hwndFrame, WM_FORMATFRAME, 0L, 0L );
      WinShowWindow( hwndFrame, TRUE );
    }

    return hwndFrame;

exitDestroy:
    if( pszClassClient && *pszClassClient )
      *phwndClient = NULL;
    WinDestroyWindow( hwndFrame );
    return NULL;
}

/*-----------------------------------------------------------------*/
/* Create or destroy one frame control for MpmFrameUpdate.         */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmFrameFixCtl( hwndFrame, flFrameFlags, flFlagMask,
                             pCtlData, pszClass, pahwndCtls, usFid,
                             flCtlStyle )
    HWND        hwndFrame;
    ULONG       flFrameFlags, flFlagMask, flCtlStyle;
    PVOID       pCtlData;
    PSZ         pszClass;
    PHWND       pahwndCtls;
    USHORT      usFid;
{
    PHWND       phwnd;

    ASSERT( usFid >= FID_CTL_MIN && usFid <= FID_CTL_MAX,
            "MpmFrameFixCtl: bad usFid" );

    phwnd = pahwndCtls + usFid - FID_CTL_MIN;

    if( flFrameFlags & flFlagMask )
    {
      if( *phwnd )
        return TRUE;

      *phwnd =
        WinCreateWindow( hwndFrame, pszClass, _szNull, flCtlStyle,
                         0, 0, 0, 0, hwndFrame, HWND_BOTTOM,
                         usFid, pCtlData, NULL );
      return !! *phwnd;
    }
    else  /* ! ( flFrameFlags & flFlagMask ) */
    {
      if( ! *phwnd )
        return TRUE;
      WinDestroyWindow( *phwnd );
      *phwnd = NULL;
      return TRUE;
    }
}

/*-----------------------------------------------------------------*/
/* Adjust the positions of all the frame controls (and client      */
/* window) in hwndFrame according to its current size, by setting  */
/* up a SWP structure and calling WinSetMultWindowPos.             */
/*-----------------------------------------------------------------*/

LOCAL MRESULT MpmFrameFormat( hwndFrame )
    HWND        hwndFrame;
{
    HWND        ahwnd[FID_CTL_COUNT];
    SWP         aswp[FID_CTL_COUNT];
    PHWND       phwnd;
    SHORT       i;
    LONG        flStyle;
    RECTL       rclFrame, rclClient, rclNewClient, rclVert, rclHorz;
    Rect        rectAdj;

    /* Clear the SWP structure, get the list of frame controls */

    memzero( &aswp );

    if( ! MpmFrameQueryCtls( hwndFrame, ahwnd ) )
      return 0L;

    for( i = 0;  i < FID_CTL_COUNT;  i++ )
      aswp[i].hwnd = ahwnd[i];

    ASSERT( ahwnd[I_CLIENT],
            "MpmFrameFormat: Must have client window" );

    /* Give the client window a shot at if it wants to take over */
            
    if( WinSendMsg( ahwnd[I_CLIENT], WM_FORMATFRAME, 0, 0 ) )
      return MRFROMLONG(1);

    /* Pick up the frame window size and style */

    flStyle = MYWNDOF(hwndFrame).flStyle;
    rectAdj = MYWNDOF(hwndFrame).rectAdj;
    WinQueryWindowRect( hwndFrame, &rclFrame );
    
    if( ahwnd[I_SIZEBORDER] )
    {
      ASSERT( ahwnd[I_VERTSCROLL] || ahwnd[I_HORZSCROLL],
              "MpmFrameFormat: FID_SIZEBORDER requires scroll bar" );
    }

    /* Take a first cut at the client window position/size */

    rclClient.xLeft   = rclFrame.xLeft   + rectAdj.left;
    rclClient.xRight  = rclFrame.xRight  + rectAdj.right;
    rclClient.yBottom = rclFrame.yBottom - rectAdj.bottom;
    rclClient.yTop    = rclFrame.yTop    - rectAdj.top;

    rclNewClient = rclClient;

    /* Position the vertical scroll bar */

#define swp aswp[I_VERTSCROLL]
    if( ahwnd[I_VERTSCROLL] )
    {
      swp.cx = _alSysVal[SV_CXVSCROLL];
      swp.x = rclNewClient.xRight =
        rclClient.xRight - swp.cx + _alSysVal[SV_CXBORDER];
      swp.y = rclClient.yBottom - _alSysVal[SV_CYBORDER];
      if( ahwnd[I_HORZSCROLL] || ahwnd[I_SIZEBORDER] )
        swp.y += _alSysVal[SV_CYHSCROLL] - _alSysVal[SV_CYBORDER];
      swp.cy = rclClient.yTop - swp.y + _alSysVal[SV_CYBORDER];
      swp.fs = SWP_MOVE | SWP_SIZE;
    }
#undef swp

    /* Position the horizontal scroll bar */

#define swp aswp[I_HORZSCROLL]
    if( ahwnd[I_HORZSCROLL] )
    {
      swp.cy = _alSysVal[SV_CYHSCROLL];
      swp.y = rclClient.yBottom - _alSysVal[SV_CYBORDER];
      rclNewClient.yBottom =
        rclClient.yBottom + swp.cy - _alSysVal[SV_CYBORDER];
      swp.x = rclClient.xLeft - _alSysVal[SV_CXBORDER];
      swp.cx = rclClient.xRight - swp.x + _alSysVal[SV_CXBORDER];
      if( ahwnd[I_VERTSCROLL] || ahwnd[I_SIZEBORDER] )
        swp.cx -= _alSysVal[SV_CXVSCROLL] - _alSysVal[SV_CXBORDER];
      swp.fs = SWP_MOVE | SWP_SIZE;
    }
#undef swp

    /* Now that we know which (if any) scroll bars are present,
       set the final client size */

#define swp aswp[I_CLIENT]
    if( ahwnd[I_CLIENT] )
    {
      swp.x = rclNewClient.xLeft;
      swp.cx = rclNewClient.xRight - rclNewClient.xLeft;
      swp.y = rclNewClient.yBottom;
      swp.cy = rclNewClient.yTop - rclNewClient.yBottom;
      swp.fs = SWP_MOVE | SWP_SIZE;
    }
#undef swp

    /* Everything's calculated, play bumper car with the windows */

    WinSetMultWindowPos( (HAB)NULL, &aswp[0], FID_CTL_COUNT );

    return MRFROMSHORT(TRUE);
}

/*-----------------------------------------------------------------*/
/* Handy function to get the list of controls in a frame window    */
/*-----------------------------------------------------------------*/

SHORT MpmFrameQueryCtls( hwndFrame, pahwndCtls )
    HWND        hwndFrame;
    PHWND       pahwndCtls;
{
    return WinMultWindowFromIDs( hwndFrame, pahwndCtls,
                                 FID_CTL_MIN, FID_CTL_MAX );
}

/*-----------------------------------------------------------------*/
/* Create or destroy the appropriate frame controls in hwndFrame   */
/* according to the bits in flFrameFlags.                          */
/*-----------------------------------------------------------------*/

LOCAL MRESULT MpmFrameUpdate( hwndFrame, flFrameFlags )
    HWND        hwndFrame;
    ULONG       flFrameFlags;
{
    BOOL        fOK;

    HWND        ahwndCtls[FID_CTL_COUNT];
    SBCDATA     sbcd;

    MpmFrameQueryCtls( hwndFrame, ahwndCtls );

    sbcd.cb = sizeof(sbcd);
    sbcd.sHilite = FALSE;
    sbcd.posFirst = sbcd.posLast = sbcd.posThumb = 0;

    fOK =
      (
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_SIZEBORDER, NULL,
                        WC_SIZEBORDER, ahwndCtls, FID_SIZEBORDER,
                        WS_PARENTCLIP )
      &&
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_TITLEBAR, NULL,
                        WC_TITLEBAR, ahwndCtls, FID_TITLEBAR,
                        WS_PARENTCLIP )
      &&
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_SYSMENU, NULL,
                        WC_MENU, ahwndCtls, FID_SYSMENU,
                        WS_PARENTCLIP | MS_ACTIONBAR | MS_TITLEBUTTON )
      &&
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_MINMAX, NULL,
                        WC_MENU, ahwndCtls, FID_MINMAX,
                        WS_PARENTCLIP | MS_ACTIONBAR | MS_TITLEBUTTON )
      &&
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_VERTSCROLL, &sbcd,
                        WC_SCROLLBAR, ahwndCtls, FID_VERTSCROLL,
                        WS_PARENTCLIP | SBS_VERT )
      &&
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_HORZSCROLL, &sbcd,
                        WC_SCROLLBAR, ahwndCtls, FID_HORZSCROLL,
                        WS_PARENTCLIP | SBS_HORZ )
      &&
        MpmFrameFixCtl( hwndFrame, flFrameFlags, FCF_MENU, NULL,
                        WC_MENU, ahwndCtls, FID_MENU,
                        WS_PARENTCLIP | MS_ACTIONBAR )
      );

    ASSERT( fOK,
            "MpmFrameUpdate: MpmFrameFixCtl failed" );

    if( ! ahwndCtls[I_CLIENT] )
      return MRFROMLONG(TRUE);

    return WinSendMsg( ahwndCtls[I_CLIENT], WM_FORMATFRAME, 0, 0 );
}

/*-----------------------------------------------------------------*/
/* Find out if pszClassName is the frame window class.             */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmIsFrameClass( pszClassName )
    PSZ         pszClassName;
{
    return strcmp( pszClassName, "#1" ) == 0;
}

/*-----------------------------------------------------------------*/
/* Handle a WM_SYSCOMMAND/SC_MAXIMIZE request for a frame window.  */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmZoomWindow( hwnd, sPart )
    HWND        hwnd;
    SHORT       sPart;
{
    PMYWND      pwnd;
    WindowPeek  pwin;
    SWP         swp;
    Rect*       prect;

    pwin = MYWNDOF(hwnd).pwin;

    /* Get the old size for the WM_SIZE message */

    WinQueryWindowPos( hwnd, &swp );

    /* Do the Mac zooming */

    thePort = &pwin->port;
    EraseRect( &pwin->port.portRect );
    ZoomWindow( pwin, sPart, FALSE );

    /* Bring the MYWND information up to date */

    pwnd = PMYWNDOF(hwnd);
    prect = &(**pwin->strucRgn).rgnBBox;

    pwnd->x  = prect->left;
    pwnd->y  = (SHORT)_alSysVal[SV_CYSCREEN] - prect->bottom;
    pwnd->cx = prect->right - prect->left;
    pwnd->cy = prect->bottom - prect->top;

    /* Send the WM_MOVE and WM_SIZE messages */

    if( MYCLASSOF(hwnd).class.flClassStyle & CS_MOVENOTIFY )
      WinSendMsg( hwnd, WM_MOVE, 0, 0 );

    WinSendMsg( hwnd, WM_SIZE,
                MPFROM2SHORT( swp.cx, swp.cy ),
                MPFROM2SHORT( MYWNDOF(hwnd).cx, MYWNDOF(hwnd).cy ) );
}

/*-----------------------------------------------------------------*/
/* Window function for the frame window class.                     */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpFrame( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    HWND        hwndClient, hwndTitle;
    HPS         hps;

    if( ! MpmValidateWindow(hwnd) )
      return 0L;

    switch( msg )
    {
      /* these msgs are passed through to client window */
      case WM_COMMAND:
      case WM_HSCROLL:
      case WM_INITMENU:
      case WM_VSCROLL:
        hwndClient = WinWindowFromID( hwnd, FID_CLIENT );
        if( ! hwndClient )
          return 0L;
        return WinSendMsg( hwndClient, msg, mp1, mp2 );

      case WM_FORMATFRAME:
        return MpmFrameFormat( hwnd );

      case WM_PAINT:
        hwndClient = WinWindowFromID( hwnd, FID_CLIENT );
        if( hwndClient &&
            ! WinSendMsg( hwndClient, WM_ERASEBACKGROUND, 0, 0 ) )
        {
          hps = WinGetPS( hwndClient );  /* this is probably wrong! */
          ASSERT( hps,
                  "MpmFnwpFrame: WinGetPS failed" );
          GpiErase( hps );
          WinReleasePS( hps );
        }
        return 0L;

      case WM_QUERYWINDOWPARAMS:
      case WM_SETWINDOWPARAMS:
        hwndTitle = WinWindowFromID( hwnd, FID_TITLEBAR );
        if( ! hwndTitle )
          return 0L;
        return WinSendMsg( hwndTitle, msg, mp1, mp2 );

      case WM_SIZE:
        WinSendMsg( hwnd, WM_FORMATFRAME, 0L, 0L );
        return 0L;

      case WM_SYSCOMMAND:
        switch( COMMANDMSG(&msg)->cmd )
        {
          case SC_CLOSE:
            hwndClient = WinWindowFromID( hwnd, FID_CLIENT );
            WinSendMsg( hwndClient ? hwndClient : hwnd,
                        WM_CLOSE, 0L, 0L );
            return 0L;

          case SC_MAXIMIZE:
            MpmZoomWindow( hwnd, inZoomOut );
            return 0L;

          case SC_RESTORE:
            MpmZoomWindow( hwnd, inZoomIn );
            return 0L;
        }
        return 0L;

      case WM_UPDATEFRAME:
        hwndClient = WinWindowFromID( hwnd, FID_CLIENT );
        if( hwndClient &&
            WinSendMsg( hwndClient, WM_UPDATEFRAME, 0, 0 ) )
          return MRFROMLONG(TRUE);
        return MpmFrameUpdate( hwnd, LONGFROMMP(mp1) );
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for the sizing border (size box) window class.  */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpSizeBorder( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    LONG        lNewPoint;
    Point       point, pointOld;
    Rect        rect;
    WindowPeek  pwin;
    SWP         swp;
    HWND        hwndFrame;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
      /* Let user resize the window; notify it of the new size */

      case WM_BUTTON1DOWN:
        hwndFrame = MYWNDOF(hwnd).hwndOwner;
        pwin = MYWNDOF(hwnd).pwin;
        pointOld.h = pwin->port.portRect.right;
        pointOld.v = pwin->port.portRect.bottom;
        WinQueryWindowPos( hwndFrame, &swp );
        swp.fs &= ~( SWP_ACTIVATE | SWP_DEACTIVATE );
        point.h = MOUSEMSG(&msg)->x;
        point.v = _alSysVal[SV_CYSCREEN] - MOUSEMSG(&msg)->y;
        /* should do a WM_QUERYMINMAXINFO... */
        rect.top = 100;
        rect.left = 100;
        rect.bottom = 32767;
        rect.right = 32767;
        lNewPoint = GrowWindow( pwin, point, &rect );
        if( ! lNewPoint )
          return 0L;
        point.h = LOUSHORT(lNewPoint) - pointOld.h;
        point.v = HIUSHORT(lNewPoint) - pointOld.v;
        if( ! point.h && ! point.v )
          return 0L;
        swp.cx += point.h;
        swp.cy += point.v;
        swp.y  -= point.v;
        WinSetMultWindowPos( (HAB)NULL, &swp, 1 );
        return 0L;
    }

    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Window function for the title bar window class.                 */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpTitleBar( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    Point       point, pointOld;
    Rect        rect;
    HWND        hwndFrame;
    WindowPeek  pwin;
    Str255      str;
    PWNDPARAMS  pwprm;
    USHORT      usLen;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    switch( msg )
    {
      /* Let user move the window; notify it of new position */

      case WM_BUTTON1DOWN:
        hwndFrame = MYWNDOF(hwnd).hwndOwner;
        pwin = MYWNDOF(hwnd).pwin;
        pointOld.h = (**pwin->contRgn).rgnBBox.left;
        pointOld.v = (**pwin->contRgn).rgnBBox.top;
        point.h = MOUSEMSG(&msg)->x;
        point.v = _alSysVal[SV_CYSCREEN] - MOUSEMSG(&msg)->y;
        /* should do a WM_QUERYMINMAXINFO... */
        rect.top = 24;
        rect.left = 4;
        rect.bottom = _alSysVal[SV_CYSCREEN] - 4;
        rect.right = _alSysVal[SV_CXSCREEN] - 4;
        DragWindow( pwin, point, &rect );
        point.h = (**pwin->contRgn).rgnBBox.left - pointOld.h;
        point.v = (**pwin->contRgn).rgnBBox.top  - pointOld.v;
        if( point.h || point.v )
        {
          MYWNDOF(hwndFrame).x += point.h;
          MYWNDOF(hwndFrame).y -= point.v;
          if( MYCLASSOF(hwndFrame).class.flClassStyle & CS_MOVENOTIFY )
            WinSendMsg( hwndFrame, WM_MOVE, 0, 0 );
        }
        return 0L;

      /* Pick up the window text from the Mac and return it */

      case WM_QUERYWINDOWPARAMS:
        pwprm = PVOIDFROMMP(mp1);
        if( ! ( pwprm->fsStatus & (WPM_TEXT|WPM_CCHTEXT) ) )
          return 0L;
        GetWTitle( MYWNDOF(hwnd).pwin, (char*)str );
        if( pwprm->fsStatus & WPM_TEXT )
        {
          usLen = pwprm->cchText;
          if( usLen > (BYTE)str[0] )
            usLen = (BYTE)str[0];
          memcpy( pwprm->pszText, str+1, usLen );
          pwprm->pszText[usLen] = '\0';
        }
        pwprm->cchText = usLen;
        return MRFROMSHORT(1);

      /* Set the Mac's window text */

      case WM_SETWINDOWPARAMS:
        pwprm = PVOIDFROMMP(mp1);
        if( ! ( pwprm->fsStatus & WPM_TEXT ) )
          return 0L;
        usLen = pwprm->cchText;
        if( usLen > 255 )
          usLen = 255;
        memcpy( str+1, pwprm->pszText, usLen );
        str[0] = usLen;
        SetWTitle( MYWNDOF(hwnd).pwin, (char*)str );
        return MRFROMSHORT(1);
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
