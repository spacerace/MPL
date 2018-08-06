/*-----------------------------------------------------------------*/
/* MpmScroll.c                                                     */
/* Window scrolling functions                                      */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

#define P pascal

P LOCAL VOID    MpmTrackScrollBar( ControlHandle hctl, SHORT sPart );
  LOCAL VOID    MpmTrackScrollBarNotify( SHORT cmd );
P LOCAL VOID    MpmTrackThumb( VOID );
  LOCAL SHORT   MpmTrackThumbCalc( Point point );
  LOCAL VOID    MpmTrackThumbHideShow( VOID );
  LOCAL VOID    MpmTrackThumbNotify( SHORT cmd, SHORT sValue );

#undef P

/*-----------------------------------------------------------------*/

HWND        _hwndTrack    = NULL;
ControlHandle _hctlTrack;

#if 0  /* part of failed attempt at fancy thumb tracking */

RgnHandle   _hrgnTrack;
Point       _pointTrack1;
SHORT       _sTrackValue, _sTrackValue1;
struct {
    Rect      rectLimit;
    Rect      rectSlop;
    short     sAxis;
} _thumbTrack;

#endif

/*-----------------------------------------------------------------*/
/* Scroll a window.  The three prcl parameters must all be NULL.   */
/* dx and dy give the scroll increments, fOptions tells whether to */
/* scroll child windows or not (SW_SCROLLCHILDREN flag).           */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinScrollWindow( hwnd, dx, dy, prclScroll, prclClip,
                                  hrgnUpdate, prclUpdate, fOptions )
    HWND        hwnd;
    SHORT       dx, dy;
    PRECTL      prclScroll, prclClip, prclUpdate;
    HRGN        hrgnUpdate;
    USHORT      fOptions;
{
    Rect        rect;
    RgnHandle   hrgn;
    HWND        hwndChild;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    ASSERT( ! prclScroll && ! prclClip && ! hrgnUpdate && ! prclUpdate,
            "WinScrollWindow: hrgn and prcl params must be NULL" );

    /* Adjust child window coordinates if requested */

    if( fOptions & SW_SCROLLCHILDREN )
      for( hwndChild = MYWNDOF(hwnd).hwndTopChild;
           hwndChild;
           hwndChild = MYWNDOF(hwndChild).hwndNextSibling )
      {
        MYWNDOF(hwndChild).x += dx;
        MYWNDOF(hwndChild).y += dy;
      }

    /* Scroll the window, Mac-style */

    MpmQueryMacRect( hwnd, &rect );

    hrgn = NewRgn();
    thePort = &PWINOFHWND(hwnd)->port;
    ScrollRect( &rect, dx, -dy, hrgn );
    InvalRgn( hrgn );
    DisposeRgn( hrgn );

    return 2;  /* not really right, but no one cares */
}

/*-----------------------------------------------------------------*/
/* Redisplay all scroll bar controls within hwnd (and hwnd itself, */
/* if it's a scroll bar), with the active or inactive appearance   */
/* according to fActive.                                           */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmActivateScrollBars( hwnd, fActive )
    HWND        hwnd;
    BOOL        fActive;
{
    if( strcmp( MYCLASSOF(hwnd).szName, "#8" ) == 0 )
      HiliteControl( MYWNDOF(hwnd).hctl, fActive ? 0 : 255 );

    for( hwnd = MYWNDOF(hwnd).hwndTopChild;
         hwnd;
         hwnd = MYWNDOF(hwnd).hwndNextSibling )
      MpmActivateScrollBars( hwnd, fActive );
}

/*-----------------------------------------------------------------*/
/* Mac call-back function for scroll bar tracking.  Sends the      */
/* appropriate WM_HSCROLL or WM_VSCROLL message with the SB_ code  */
/* for the particular action in progress.                          */
/*-----------------------------------------------------------------*/

pascal VOID MpmTrackScrollBar( hctl, sPart )
    ControlHandle hctl;
    SHORT       sPart;
{
    SHORT       cmd;

    ASSERT( (HWND)(**hctl).contrlRfCon == _hwndTrack,
            "MpmTrackScrollBar: bad hctl or _hwndTrack" );

    switch( sPart )
    {
      case inUpButton:    cmd = SB_LINEUP;    break;
      case inDownButton:  cmd = SB_LINEDOWN;  break;
      case inPageUp:      cmd = SB_PAGEUP;    break;
      case inPageDown:    cmd = SB_PAGEDOWN;  break;
      default:            return;
    }
    MpmTrackScrollBarNotify( cmd );
}

/*-----------------------------------------------------------------*/
/* Send the actual WM_?SCROLL message for MpmTrackScrollBar.       */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmTrackScrollBarNotify( cmd )
    SHORT       cmd;
{
    PMYWND      pwnd;

    pwnd = PMYWNDOF(_hwndTrack);

    if( pwnd->hwndOwner )
      WinSendMsg(
        pwnd->hwndOwner,
        pwnd->flStyle & SBS_VERT ? WM_VSCROLL : WM_HSCROLL,
        MPFROMSHORT(pwnd->id),
        MPFROM2SHORT( (**pwnd->hctl).contrlValue, cmd )
      );
}

/*-----------------------------------------------------------------*/
/* Part of failed attempt at scroll bar thumb tracking.            */
/*-----------------------------------------------------------------*/

#if 0

LOCAL pascal VOID MpmTrackThumb()
{
    Point       point;
    SHORT       sValue;

    GetMouse( &point );
    sValue = MpmTrackThumbCalc( point );
    if( sValue == _sTrackValue )
      return;

    _sTrackValue = sValue;

    MpmTrackThumbHideShow();

    MpmTrackThumbNotify( SB_SLIDERTRACK, sValue );

    MpmTrackThumbHideShow();
}

#endif

/*-----------------------------------------------------------------*/
/* Part of failed attempt at scroll bar thumb tracking.            */
/*-----------------------------------------------------------------*/

#if 0

LOCAL SHORT MpmTrackThumbCalc( point )
    Point       point;
{
    SHORT       sValueMin, sValueRange, sPix, sPixMax;

    if( ! PtInRect( point, &_thumbTrack.rectSlop ) )
      return _sTrackValue1;

    if( _thumbTrack.sAxis & hAxisOnly )
    {
      sPix = point.h - _thumbTrack.rectLimit.left;
      sPixMax = _thumbTrack.rectLimit.right -
                _thumbTrack.rectLimit.left;
    }
    else
    {
      sPix = point.v - _thumbTrack.rectLimit.top;
      sPixMax = _thumbTrack.rectLimit.bottom -
                _thumbTrack.rectLimit.top;
    }

    if( sPix < 0 )
      sPix = 0;

    if( sPix > sPixMax )
      sPix = sPixMax;

    sValueMin = (**_hctlTrack).contrlMin;
    sValueRange = (**_hctlTrack).contrlMax - sValueMin;

    return (SHORT)(
      ( (LONG)sPix * (LONG)sValueRange + (LONG)sValueRange/2 ) /
      (LONG)sPixMax
    ) + sValueMin;
}

#endif

/*-----------------------------------------------------------------*/
/* Part of failed attempt at scroll bar thumb tracking.            */
/*-----------------------------------------------------------------*/

#if 0

LOCAL VOID MpmTrackThumbHideShow()
{
    RgnHandle   hrgn;
    Pattern     patSave;
    SHORT       sModeSave;

return;

    hrgn = NewRgn();
    CopyRgn( _hrgnTrack, hrgn );
    InsetRgn( hrgn, 1, 1 );
    DiffRgn( _hrgnTrack, hrgn, hrgn );

    thePort = &MYWNDOF(_hwndTrack).pwin->port;
    memcpy( patSave, thePort->pnPat, sizeof(Pattern) );
    sModeSave = thePort->pnMode;
    memcpy( thePort->pnPat, dkGray, sizeof(Pattern) );
    thePort->pnMode = notPatXor;

    PaintRgn( hrgn );

    memcpy( thePort->pnPat, patSave, sizeof(Pattern) );
    thePort->pnMode = sModeSave;

    DisposeRgn( hrgn );
}

#endif

/*-----------------------------------------------------------------*/
/* Part of failed attempt at scroll bar thumb tracking.            */
/*-----------------------------------------------------------------*/

#if 0

LOCAL VOID MpmTrackThumbNotify( cmd, sValue )
    SHORT       cmd;
    SHORT       sValue;
{
}

#endif

/*-----------------------------------------------------------------*/
/* Window function for scroll bar controls.                        */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpScrollBar( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    USHORT      id;
    ControlHandle hctl, hctl1;
    Rect        rect;
    Rect*       prect;
    SHORT       sPart;
    POINTL      ptl;
    SHORT       cmd;
    Handle      hcdef;
    LONG        lPoint;
    Point       point;
    static BOOL fTrackThumb = FALSE;
    static SHORT sInitValue;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    id   = MYWNDOF(hwnd).id;
    hctl = MYWNDOF(hwnd).hctl;

    switch( msg )
    {
      /* Return the current position */

      case SBM_QUERYPOS:
        return MRFROMSHORT( fTrackThumb ? sInitValue
                                        : (**hctl).contrlValue );

      /* Return the scroll bar range */

      case SBM_QUERYRANGE:
        return MRFROM2SHORT( (**hctl).contrlMin, (**hctl).contrlMax );

      /* Set the scroll bar position and range */

      case SBM_SETSCROLLBAR:
        (**hctl).contrlMin = SHORT1FROMMP(mp2);
        (**hctl).contrlMax = SHORT2FROMMP(mp2);
        /* fall through */

      /* Set the scroll bar position only */

      case SBM_SETPOS:
        if( MYWNDOF(hwnd).flStyle & WS_VISIBLE )
          SetCtlValue( hctl, SHORT1FROMMP(mp1) );
        else
          (**hctl).contrlValue = SHORT1FROMMP(mp1);
        fTrackThumb = FALSE;
        return MRFROMSHORT(1);

      /* Handle mouse button down: call TrackControl to track it */

      case WM_BUTTON1DOWN:
        ptl.x = SHORT1FROMMP(mp1);
        ptl.y = SHORT2FROMMP(mp1);
        MpmMapMacOfPtl( hwnd, &point, &ptl );
        sPart = FindControl( point, PWINOFHWND(hwnd), &hctl1 );
        ASSERT( sPart  &&  hctl1 == hctl,
                "MpmFnwpScrollBar: FindControl failed" );
        _hwndTrack = hwnd;
        if( sPart == inThumb )
        {
          fTrackThumb = TRUE;
          sInitValue = (**hctl).contrlValue;
          if( TrackControl( hctl, point, NULL ) )
            MpmTrackScrollBarNotify( SB_SLIDERPOSITION );
          fTrackThumb = FALSE;
        }
        else
        {
          TrackControl( hctl, point, (ProcPtr)MpmTrackScrollBar );
          MpmTrackScrollBarNotify( SB_ENDSCROLL );
        }
        return 0L;

      /* Handle scroll bar creation: call NewControl to create it */

      case WM_CREATE:
        rect.left = rect.top = 0;
        if( MYWNDOF(hwnd).flStyle & SBS_VERT )
          rect.right = _alSysVal[SV_CXVSCROLL], rect.bottom = 100;
        else
          rect.bottom = _alSysVal[SV_CYHSCROLL], rect.right = 100;
        hctl = NewControl( PWINOFHWND(hwnd), &rect, _szNull,
                           MYWNDOF(hwnd).flStyle & WS_VISIBLE,
                           0, 0, 1, scrollBarProc, (long)hwnd );
        if( ! hctl )
          return MRFROMSHORT(1);
        MYWNDOF(hwnd).hctl = hctl;
        return 0L;

      /* Destroy scroll bar */

      case WM_DESTROY:
        MYWNDOF(hwnd).hctl = NULL;
        DisposeControl( hctl );
        return 0L;

      /* Handle window movement: use MoveControl to move the bar */

      case WM_MOVE:
        MpmQueryMacRect( hwnd, &rect );
        if( MYWNDOF(hwnd).flStyle & WS_VISIBLE )
          MoveControl( hctl, rect.left, rect.top );
        else
        {
          prect = &(**hctl).contrlRect;
          prect->right  += rect.left - prect->left;
          prect->left    = rect.left;
          prect->bottom += rect.top  - prect->top;
          prect->top     = rect.top;
        }
        return 0L;

      /* Show or hide the scroll bar */

      case WM_SHOW:
        if( mp1 )
          ShowControl( hctl );
        else
          HideControl( hctl );
        return 0L;

      /* Handle window resizing: use SizeControl to resize it */

      case WM_SIZE:
        if( SHORT1FROMMP(mp2) && SHORT2FROMMP(mp2) )
        {
          if( MYWNDOF(hwnd).flStyle & WS_VISIBLE )
            SizeControl( hctl,
                         SHORT1FROMMP(mp2),
                         SHORT2FROMMP(mp2) );
          else
          {
            prect = &(**hctl).contrlRect;
            prect->right  = prect->left + SHORT1FROMMP(mp2);
            prect->bottom = prect->top  + SHORT2FROMMP(mp2);
          }
        }
        return 0L;
    }
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
