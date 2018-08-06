/*-----------------------------------------------------------------*/
/* MpmPaint.c                                                      */
/* Window painting functions                                       */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

LOCAL VOID      MpmInvalidateWindow( HWND hwnd );
LOCAL VOID      MpmPaintWindow( HWND hwnd );

/*-----------------------------------------------------------------*/
/* Set up a PS for painting.  Since the Mac's BeginUpdate function */
/* has already tweaked the visRgn, we just have to get the HPS and */
/* pass back the bounding rect.                                    */
/*-----------------------------------------------------------------*/

HPS APIENTRY WinBeginPaint( hwnd, hpsIn, prclPaint )
    HWND        hwnd;
    HPS         hpsIn;
    PRECTL      prclPaint;
{
    HPS         hps;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    ASSERT( ! hpsIn,
            "WinBeginPaint: hps must be NULL" );

    hps = WinGetPS( hwnd );

    MpmMapRclOfMac( hwnd, prclPaint,
                    &(**PGRAFOFHPS(hps)->visRgn).rgnBBox );

    return hps;
}

/*-----------------------------------------------------------------*/
/* Clean up after painting.  Just does a WinReleasePS, since there */
/* is going to be a call to the Mac's EndUpdate function to do the */
/* real work.                                                      */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinEndPaint( hps )
    HPS         hps;
{
    return WinReleasePS( hps );
}

/*-----------------------------------------------------------------*/
/* Get a PS handle for hwnd.  This makes a copy of the GrafPort to */
/* avoid potential problems.  There's only one (static) PS in our  */
/* PS cache.                                                       */
/*-----------------------------------------------------------------*/

HPS APIENTRY WinGetPS( hwnd )
    HWND        hwnd;
{
    GrafPtr     pgraf;
    RgnHandle   hrgn;
    Rect        rect;

    ASSERT( ! ( _ps1.flags & PSF_INUSE ),
            "WinGetPS: PS already in use" );

    if( ! hwnd  ||  hwnd == HWND_DESKTOP )
      hwnd = _hwndDesktop;

    if( ! MpmValidateWindow(hwnd) )
      return NULL;

    /* Clear the cache PS and mark it as in use */

    memzero( &_ps1 );
    _ps1.hwnd = hwnd;
    _ps1.flags |= PSF_INUSE;

    /* Copy the Mac window's GrafPort */

    if( hwnd == _hwndDesktop )
      GetWMgrPort( &pgraf );
    else
      pgraf = &PWINOFHWND(hwnd)->port;

    _ps1.port = *pgraf;

    _ps1.port.visRgn = NewRgn();
    CopyRgn( pgraf->visRgn, _ps1.port.visRgn );

    _ps1.port.clipRgn = NewRgn();
    CopyRgn( pgraf->clipRgn, _ps1.port.clipRgn );

    /* Clip the visRgn down to this window's rectangle in case it's
       a child window */

    if( ! ( MYWNDOF(hwnd).flStyle & WS_PARENTCLIP ) )
    {
      hrgn = NewRgn();
      MpmQueryMacRect( hwnd, &rect );
      RectRgn( hrgn, &rect );
      SectRgn( _ps1.port.visRgn, hrgn, _ps1.port.visRgn );
      DisposeRgn( hrgn );
    }

    /* Handle WS_CLIPCHILDREN and WS_CLIPSIBLINGS here? */

    return _hps1;
}

/*-----------------------------------------------------------------*/
/* Invalidate a rectangle in hwnd so it will be painted.  The      */
/* Mac's InvalRect function does the real work.                    */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinInvalidateRect( hwnd, prcl, fKids )
    HWND        hwnd;
    PRECTL      prcl;
    BOOL        fKids;
{
    Rect        rect;
    RECTL       rcl;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    if( prcl )
      rcl = *prcl;
    else
      WinQueryWindowRect( hwnd, &rcl );

    MpmMapMacOfRcl( hwnd, &rect, &rcl );

    thePort = &PWINOFHWND(hwnd)->port;

    InvalRect( &rect );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Release the cache PS by disposing of its regions and marking it */
/* as free.                                                        */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinReleasePS( hps )
    HPS         hps;
{
    if( ! MpmValidatePS(hps) )
      return FALSE;

    ASSERT( _ps1.flags & PSF_INUSE,
            "WinReleasePS: PS not in use" );

    DisposeRgn( _ps1.port.visRgn );
    DisposeRgn( _ps1.port.clipRgn );

    _ps1.flags &= ~PSF_INUSE;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Force a window update on hwnd and its children.  Actually, we   */
/* go up to the top level Mac window if hwnd is a child, and       */
/* all children of that top level window.                          */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinUpdateWindow( hwnd )
    HWND        hwnd;
{
    WindowPeek  pwin;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    pwin = PWINOFHWND(hwnd);
    hwnd = HWNDOFPWIN(pwin);  /* get main Mac window */

    BeginUpdate( pwin );

    MpmDrawGrowIcon( hwnd );

    UpdtControl( pwin, pwin->port.visRgn );

    MpmPaintWindow( hwnd );

    EndUpdate( pwin );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Draw the size box in hwnd if it has one.                        */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmDrawGrowIcon( hwnd )
    HWND        hwnd;
{
    if( MpmIsFrameClass( MYCLASSOF(hwnd).szName ) )
      if( MYWNDOF(hwnd).flFrameFlags & FCF_SIZEBORDER )
        DrawGrowIcon( PWINOFHWND(hwnd) );
}

/*-----------------------------------------------------------------*/
/* Invalidate a window.  (Not implemented)                         */
/*-----------------------------------------------------------------*/

#ifdef INVAL

LOCAL VOID MpmInvalidateWindow( hwnd )
    HWND        hwnd;
{
    Rect        rect;

    switch( MYWNDOF(hwnd).ucKind )
    {
      case WK_CHILD:
        MpmQueryMacRect( hwnd, &rect );
        break;

      case WK_MAIN:
      /*if( MpmIsFrameClass(MYCLASSOF(hwnd).szName) )*/
        if( WinWindowFromID( hwnd, FID_SIZEBORDER ) )
        {
          rect = PWINOFHWND(hwnd)->port.portRect;
          rect.top = rect.bottom - _alSysVal[SV_CYHSCROLL];
          rect.left = rect.right - _alSysVal[SV_CXVSCROLL];
        }
        break;

      default:
        return;
    }

    thePort = &PWINOFHWND(hwnd)->port;

    InvalRect( &rect );
    EraseRect( &rect );
}

#endif

/*-----------------------------------------------------------------*/
/* Send WM_PAINT messages to hwnd and all its children.            */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmPaintWindow( hwnd )
    HWND        hwnd;
{
    HWND        hwndChild;

    if( ! ( MYWNDOF(hwnd).flStyle & WS_VISIBLE ) )
      return;

    WinSendMsg( hwnd, WM_PAINT, 0, 0 );

    for( hwndChild = MYWNDOF(hwnd).hwndTopChild;
         hwndChild;
         hwndChild = MYWNDOF(hwndChild).hwndNextSibling )
      MpmPaintWindow( hwndChild );
}

/*-----------------------------------------------------------------*/
