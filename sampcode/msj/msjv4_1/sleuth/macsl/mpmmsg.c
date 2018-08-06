/*-----------------------------------------------------------------*/
/* MpmMsg.c                                                        */
/* Messaging functions                                             */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

LOCAL BOOL      MpmMsgActivate( PQMSG pqmsg, EventRecord *pEvent,
                                USHORT fs );
LOCAL BOOL      MpmMsgKey( PQMSG pqmsg, EventRecord *pEvent,
                           USHORT usCode );
LOCAL BOOL      MpmMsgFindChild( PQMSG pqmsg, PPOINTL pptl );
LOCAL BOOL      MpmMsgMouse( PQMSG pqmsg, EventRecord *pEvent,
                             USHORT msg );
LOCAL BOOL      MpmMsgPaint( PQMSG pqmsg, EventRecord *pEvent,
                             USHORT fsPeek );
LOCAL BOOL      MpmMsgPosted( PQMSG pqmsg, EventRecord *pEvent );
LOCAL BOOL      MpmPeekEvent( SHORT sEventMask, EventRecord *pEvent,
                              USHORT fs );

/*-----------------------------------------------------------------*/
/* Dispatch a message to its destination window.                   */
/*-----------------------------------------------------------------*/

ULONG APIENTRY WinDispatchMsg( hab, pqmsg )
    HAB         hab;
    PQMSG       pqmsg;
{
    if( ! pqmsg->hwnd  ||  ! MpmValidateWindow(pqmsg->hwnd) )
      return 0L;

    return
      (ULONG)( *MYWNDOF(pqmsg->hwnd).pfnwp )(
        pqmsg->hwnd, pqmsg->msg, pqmsg->mp1, pqmsg->mp2
     );
}

/*-----------------------------------------------------------------*/
/* Get the next message, waiting for one if necessary.             */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinGetMsg( hab, pqmsg, hwnd, msgMin, msgMax )
    HAB         hab;
    PQMSG       pqmsg;
    HWND        hwnd;
    USHORT      msgMin;
    USHORT      msgMax;
{
    while( ! WinPeekMsg( hab, pqmsg, hwnd,
                         msgMin, msgMax, PM_REMOVE ) )
      WinWaitMsg( hab, msgMin, msgMax );

    return pqmsg->msg != WM_QUIT;
}

/*-----------------------------------------------------------------*/
/* See if a message is available and pass it back in *pqmsg if so. */
/* This function does the dirty work of converting from Mac events */
/* to PM messages.                                                 */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinPeekMsg( hab, pqmsg, hwnd, msgMin, msgMax, fs )
    HAB         hab;
    PQMSG       pqmsg;
    HWND        hwnd;
    USHORT      msgMin;
    USHORT      msgMax;
    USHORT      fs;
{
    EventRecord event;
    SHORT       sEventMask;
    BOOL        fMine;
    static Point  pointMouse = { -999, -999 };

    sEventMask = everyEvent;
    if( msgMin == WM_ACTIVATE  &&  msgMax == WM_ACTIVATE )
      sEventMask = activMask;
    else
      ASSERT( ! hwnd && ! msgMin && ! msgMax,
              "WinPeekMsg: hwnd, msgMin, and msgMax must be NULL" );

    /* Let desk accessories run */

    SystemTask();

    /* Peek at the next Mac event.  If it's a null event, check
       for mouse movement. */

    fMine = MpmPeekEvent( sEventMask, &event, fs );

    if( event.what == nullEvent )
      if( event.where.v != pointMouse.v ||
          event.where.h != pointMouse.h )
      {
        if( fs & PM_REMOVE )
          pointMouse = event.where;
        pqmsg->ptl.x = event.where.h;
        pqmsg->ptl.y = screenBits.bounds.bottom - event.where.v;
        pqmsg->time = event.when * 100 / 6;  /* hack? */
        return MpmMsgMouse( pqmsg, &event, WM_MOUSEMOVE );
      }

    if( ! fMine )
      return FALSE;

    /* Set up *pqmsg and process the specific type of event */

    pqmsg->ptl.x = event.where.h;
    pqmsg->ptl.y = screenBits.bounds.bottom - event.where.v;
    pqmsg->time = event.when * 100 / 6;  /* hack? */

    switch( event.what )
    {
      case mouseDown:
        return MpmMsgMouse( pqmsg, &event, WM_BUTTON1DOWN );

      case mouseUp:
        return MpmMsgMouse( pqmsg, &event, WM_BUTTON1UP );

      case keyDown:
        return MpmMsgKey( pqmsg, &event, 0 );

      case keyUp:
        return MpmMsgKey( pqmsg, &event, KC_KEYUP | KC_PREVDOWN );

      case autoKey:
        return MpmMsgKey( pqmsg, &event, KC_PREVDOWN );

      case updateEvt:
        return MpmMsgPaint( pqmsg, &event, fs );

      case activateEvt:
        return MpmMsgActivate( pqmsg, &event, fs );

      case postedEvt:
        return MpmMsgPosted( pqmsg, &event );
    }

    return FALSE;
}

/*-----------------------------------------------------------------*/
/* Post a message to a window or the application queue.  This uses */
/* the Mac's event queue to do the actual posting.  Since there    */
/* isn't room in the Mac's event record to carry all the           */
/* information we need, allocate a QMSG structure.  This structure */
/* had better get freed up when the message is pulled from the     */
/* queue!!                                                         */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinPostMsg( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1, mp2;
{
    PQMSG       pqmsg;
    Handle      hqmsg;

    if( hwnd && ! MpmValidateWindow(hwnd) )
      return FALSE;

    hqmsg = MpmNewHandleZ( sizeof(QMSG) );
    if( ! hqmsg )
      return FALSE;

    pqmsg = (PQMSG)*hqmsg;
    pqmsg->hwnd = hwnd;
    pqmsg->msg  = msg;
    pqmsg->mp1  = mp1;
    pqmsg->mp2  = mp2;

    return PostEvent( postedEvt, (LONG)hqmsg ) == 0;
}

/*-----------------------------------------------------------------*/
/* Post a message to the application queue.                        */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinPostQueueMsg( hmq, msg, mp1, mp2 )
    HMQ         hmq;
    USHORT      msg;
    MPARAM      mp1, mp2;
{
    return WinPostMsg( NULL, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Send a message to hwnd's window function.                       */
/*-----------------------------------------------------------------*/

MRESULT APIENTRY WinSendMsg( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    return ( *MYWNDOF(hwnd).pfnwp )( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Wait for the next message; don't remove it from the queue.      */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinWaitMsg( hab, msgMin, msgMax )
    HAB         hab;
    USHORT      msgMin;
    USHORT      msgMax;
{
    QMSG        qmsg;

    while( ! WinPeekMsg( hab, &qmsg, NULL,
                         msgMin, msgMax, PM_NOREMOVE ) )
      ;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Handle an activate event.  Redraw the size box and scroll bars  */
/* as is the practice on the Mac, and generate a WM_ACTIVATE       */
/* message.                                                        */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMsgActivate( pqmsg, pEvent, fs )
    PQMSG       pqmsg;
    EventRecord *pEvent;
    USHORT      fs;
{
    WindowPeek  pwin;
    BOOL        fActive;
    HWND        hwnd;

    fActive = pEvent->modifiers & activeFlag;
    pwin = (WindowPeek)pEvent->message;

    pqmsg->hwnd = hwnd = HWNDOFPWIN(pwin);
    pqmsg->msg = WM_ACTIVATE;
    pqmsg->mp1 = MPFROM2SHORT( fActive, TRUE );
    pqmsg->mp2 = MPFROMSHORT( ! _sSetFocusDepth );

    if( ! ( fs & PM_REMOVE ) )
      return TRUE;

    MpmDrawGrowIcon( hwnd );
    MpmActivateScrollBars( pqmsg->hwnd, fActive );

    if( fActive )
      _hwndActive = pqmsg->hwnd;
    else
      WinSetFocus( NULL, NULL );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Find the child window of pqmsg->hwnd that contains the point    */
/* *pptl.  If there is such a child, change pqmsg->hwnd to that    */
/* window and also adjust *pptl to that window's coordinates.      */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMsgFindChild( pqmsg, pptl )
    PQMSG       pqmsg;
    PPOINTL     pptl;
{
    HWND        hwnd;
    PMYWND      pwnd;

    for( hwnd = MYWNDOF(pqmsg->hwnd).hwndTopChild;
         hwnd;
         hwnd = MYWNDOF(hwnd).hwndNextSibling )
    {
      pwnd = PMYWNDOF(hwnd);
      if( pptl->x >= pwnd->x  &&  pptl->x < pwnd->x + pwnd->cx  &&
          pptl->y >= pwnd->y  &&  pptl->y < pwnd->y + pwnd->cy )
      {
        pptl->x -= pwnd->x;
        pptl->y -= pwnd->y;
        pqmsg->hwnd = hwnd;
        MpmMsgFindChild( pqmsg, pptl );
        return TRUE;
      }
    }

    return FALSE;
}

/*-----------------------------------------------------------------*/
/* Handle a keyboard event by generating a WM_CHAR message.        */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMsgKey( pqmsg, pEvent, usCode )
    PQMSG       pqmsg;
    EventRecord *pEvent;
    USHORT      usCode;
{
    if( _hwndFocus )
      pqmsg->hwnd = _hwndFocus;
    else
      pqmsg->hwnd = _hwndActive;

    pqmsg->msg = WM_CHAR;
    pqmsg->mp1 = 0;
    pqmsg->mp2 = 0;

    /* THIS FUNCTION IS OBVIOUSLY VERY INCOMPLETE!!! */

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Handle a mouse event, either mouseDown, mouseUp, or our         */
/* synthesized mouseMove event.  Figure out who should get the     */
/* event and generate the WM_BUTTON1DOWN, WM_BUTTON1UP, or         */
/* WM_MOUSEMOVE message.  DOESN'T YET HANDLE DOUBLE-CLICKS!!       */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMsgMouse( pqmsg, pEvent, msg )
    PQMSG       pqmsg;
    EventRecord *pEvent;
    USHORT      msg;
{
    SHORT       sArea;
    POINTL      ptl;
    WindowPeek  pwin;
    USHORT      fid, usHitHi;
    HWND        hwnd;

    sArea = FindWindow( pEvent->where, &pwin );

    fid = usHitHi = 0;
    ptl = pqmsg->ptl;
    pqmsg->hwnd = hwnd = ( pwin ? HWNDOFPWIN(pwin) : _hwndDesktop );

    switch( sArea )
    {
      case inContent:
        WinMapWindowPoints( _hwndDesktop, hwnd, &ptl, 1 );
        MpmMsgFindChild( pqmsg, &ptl );
        hwnd = pqmsg->hwnd;
        break;

      case inDesk:
        break;

      case inDrag:
        fid = FID_TITLEBAR;
        break;

      case inGoAway:
        fid = FID_SYSMENU;
        break;

      case inGrow:
        fid = FID_SIZEBORDER;
        break;

      case inMenuBar:
        hwnd = _hwndMenu;
        if( ! hwnd )
          return FALSE;
        break;

      case inSysWindow:
        SystemClick( pEvent, pwin );
        return FALSE;

      case inZoomIn:
      case inZoomOut:
        usHitHi = sArea;
        fid = FID_MINMAX;
        break;

      default:
        return FALSE;
    }

    if( fid )
    {
      hwnd = WinWindowFromID( hwnd, fid );
      ASSERT( hwnd,
              "MpmMsgMouse: missing frame control" );
    }

    if( MYWNDOF(hwnd).flStyle & WS_DISABLED )
      return FALSE;

    pqmsg->hwnd = hwnd;
    pqmsg->msg  = msg;
    pqmsg->mp1  = MPFROM2SHORT( ptl.x, ptl.y );
    pqmsg->mp2  = MPFROM2SHORT( HT_NORMAL, usHitHi );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Handle an update event by generating WM_PAINT messages to the   */
/* window and its children.                                        */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMsgPaint( pqmsg, pEvent, fsPeek )
    PQMSG       pqmsg;
    EventRecord *pEvent;
    USHORT      fsPeek;
{
    WindowPeek  pwin;

    pwin = (WindowPeek)pEvent->message;
    pqmsg->hwnd = HWNDOFPWIN(pwin);
    ASSERT( MpmValidateWindow(pqmsg->hwnd),
            "MpmMsgPaint: bad hwnd" );

    pqmsg->msg = WM_PAINT;
    pqmsg->mp1 = 0;
    pqmsg->mp2 = 0;

    if( ! (fsPeek & PM_REMOVE) )
      return TRUE;

    WinUpdateWindow( pqmsg->hwnd );

    return FALSE;
}

/*-----------------------------------------------------------------*/
/* Handle a posted message.                                        */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMsgPosted( pqmsg, pEvent )
    PQMSG       pqmsg;
    EventRecord *pEvent;
{
    PQMSG       pqmsgPosted;

    pqmsgPosted = (PQMSG)*(Handle)pEvent->message;

    pqmsg->hwnd = pqmsgPosted->hwnd;
    pqmsg->msg  = pqmsgPosted->msg;
    pqmsg->mp1  = pqmsgPosted->mp1;
    pqmsg->mp2  = pqmsgPosted->mp2;

    DisposHandle( (Handle)pEvent->message );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Peek at a Mac event, either pulling it from the queue or not.   */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmPeekEvent( sEventMask, pEvent, fs )
    SHORT       sEventMask;
    EventRecord *pEvent;
    USHORT      fs;
{
    if( fs & PM_REMOVE )
      return GetNextEvent( sEventMask, pEvent );
    else
      return EventAvail( sEventMask, pEvent );
}

/*-----------------------------------------------------------------*/
