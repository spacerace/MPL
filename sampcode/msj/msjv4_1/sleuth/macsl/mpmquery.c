/*-----------------------------------------------------------------*/
/* MpmQuery.c                                                      */
/* Information gathering functions                                 */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/
/* Stuff pahwndKids with the child window handles of hwndParent    */
/* from idMin to idMax.                                            */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinMultWindowFromIDs( hwndParent, pahwndKids,
                                    idMin, idMax )
    HWND        hwndParent;
    PHWND       pahwndKids;
    USHORT      idMin, idMax;
{
    HWND        hwnd;
    USHORT      id;
    SHORT       sFound;

    memset( pahwndKids, 0, (idMax - idMin + 1) * sizeof(HWND) );

    if( ! MpmValidateWindow(hwndParent) )
      return 0;

    if( ISDESKTOPWINDOW(hwndParent) )
      return 0;  /* could handle this case... */

    sFound = 0;

    for( hwnd = MYWNDOF(hwndParent).hwndTopChild;
         hwnd;
         hwnd = MYWNDOF(hwnd).hwndNextSibling )
    {
      id = MYWNDOF(hwnd).id;
      if( id >= idMin  &&  id <= idMax )
      {
        pahwndKids[ id - idMin ] = hwnd;
        sFound++;
      }
    }
    
    return sFound;
}

/*-----------------------------------------------------------------*/
/* Return the active window handle.  We ignore fLock.              */
/*-----------------------------------------------------------------*/

HWND APIENTRY WinQueryActiveWindow( hwndDesktop, fLock )
    HWND        hwndDesktop;
    BOOL        fLock;
{
    ASSERT( ! hwndDesktop  ||
            hwndDesktop == HWND_DESKTOP  ||
            hwndDesktop == _hwndDesktop,
            "WinQueryActiveWindow: bad hwndDesktop" );

    return _hwndActive;
}

/*-----------------------------------------------------------------*/
/* Fill pszName (max len sNameMax) with hwnd's window class name.  */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinQueryClassName( hwnd, sNameMax, pszName )
    HWND        hwnd;
    SHORT       sNameMax;
    PSZ         pszName;
{
    SHORT       sNameLen;

    pszName[0] = '\0';

    if( ! MpmValidateWindow(hwnd) )
      return 0;

    if( ! sNameMax )
      return 0;

    sNameLen = strlen( MYCLASSOF(hwnd).szName );
    if( sNameLen > sNameMax-1 )
      sNameLen = sNameMax - 1;

    memcpy( pszName, MYCLASSOF(hwnd).szName, sNameLen );
    pszName[sNameLen] = '\0';

    return sNameLen;
}

/*-----------------------------------------------------------------*/
/* Return the desktop window handle.                               */
/*-----------------------------------------------------------------*/

HWND APIENTRY WinQueryDesktopWindow( hab, hdc )
    HAB         hab;
    HDC         hdc;
{
    ASSERT( ! hdc,
            "WinQueryDesktopWindow: hdc must be NULL" );

    /* ignore hab */

    return _hwndDesktop;
}

/*-----------------------------------------------------------------*/
/* Return the object window handle.                                */
/*-----------------------------------------------------------------*/

HWND APIENTRY WinQueryObjectWindow( hwndDesktop )
    HWND        hwndDesktop;
{
    ASSERT( ! hwndDesktop  ||
            hwndDesktop == HWND_DESKTOP  ||
            hwndDesktop == _hwndDesktop,
            "WinQueryObjectWindow: bad hwndDesktop" );

    return _hwndObject;
}

/*-----------------------------------------------------------------*/
/* Return one of the SV_ system values.                            */
/*-----------------------------------------------------------------*/

LONG APIENTRY WinQuerySysValue( hwndDesktop, iSysValue )
    HWND        hwndDesktop;
    SHORT       iSysValue;
{
    if( iSysValue < 0 || iSysValue >= SV_CSYSVALUES )
      return 0;

    return _alSysVal[iSysValue];
}

/*-----------------------------------------------------------------*/
/* Return one of hwnd's relatives as determined by cmd.            */
/* This is done differently depending on hwnd's WK_ window kind,   */
/* since we use the Mac's window information for WK_MAIN windows,  */
/* and our own fields for child windows.                           */
/* We ignore fLock.                                                */
/*-----------------------------------------------------------------*/

HWND APIENTRY WinQueryWindow( hwnd, cmd, fLock )
    HWND        hwnd;
    SHORT       cmd;
    BOOL        fLock;
{
    UCHAR       ucKind;
    WindowPeek  pwin;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    ucKind = MYWNDOF(hwnd).ucKind;

top:
    switch( cmd )
    {
      case QW_NEXT:
        switch( ucKind )
        {
          case WK_DESKTOP:
          case WK_OBJECT:
            return NULL;
          case WK_MAIN:
            pwin = PWINOFHWND(hwnd)->nextWindow;
            return pwin ? HWNDOFPWIN(pwin) : NULL;
          case WK_CHILD:
            return MYWNDOF(hwnd).hwndNextSibling;
        }
        break;

      case QW_PREV:
        switch( ucKind )
        {
          case WK_DESKTOP:
          case WK_OBJECT:
            return NULL;
          case WK_MAIN:
            return NULL;  /* later take care of this... */
          case WK_CHILD:
            return MYWNDOF(hwnd).hwndPrevSibling;
        }
        break;

      case QW_TOP:
        switch( ucKind )
        {
          case WK_DESKTOP:
            return HWNDOFPWIN( WindowList );
          case WK_OBJECT:
          case WK_MAIN:
          case WK_CHILD:
            return MYWNDOF(hwnd).hwndTopChild;
        }
        break;

      case QW_BOTTOM:
        switch( ucKind )
        {
          case WK_DESKTOP:
            return NULL;  /* later... */
          case WK_OBJECT:
          case WK_MAIN:
          case WK_CHILD:
            return MYWNDOF(hwnd).hwndBottomChild;
        }
        break;

      case QW_OWNER:
        return MYWNDOF(hwnd).hwndOwner;

      case QW_PARENT:
        switch( ucKind )
        {
          case WK_DESKTOP:
          case WK_OBJECT:
          case WK_MAIN:
            return NULL;
          case WK_CHILD:
            return MYWNDOF(hwnd).hwndParent;
        }
        break;

      case QW_NEXTTOP:
        switch( ucKind )
        {
          case WK_DESKTOP:
          case WK_OBJECT:
            return NULL;
          case WK_MAIN:
            cmd = QW_NEXT;
            goto top;
          case WK_CHILD:
            while( ISCHILDWINDOW(hwnd) )
              hwnd = MYWNDOF(hwnd).hwndParent;
            return hwnd;
        }
        break;

      case QW_PREVTOP:
        switch( ucKind )
        {
          case WK_DESKTOP:
          case WK_OBJECT:
            return NULL;
          case WK_MAIN:
            cmd = QW_PREV;
            goto top;
          case WK_CHILD:
            cmd = QW_NEXTTOP;
            goto top;
        }
        break;
    }

    return NULL;
}

/*-----------------------------------------------------------------*/
/* Return the lock count for a window.  Always 0 in MacPM!         */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinQueryWindowLockCount( hwnd )
    HWND        hwnd;
{
    MpmValidateWindow( hwnd );

    return 0;
}

/*-----------------------------------------------------------------*/
/* Fill *pswp with the position and size of hwnd.                  */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinQueryWindowPos( hwnd, pswp )
    HWND        hwnd;
    PSWP        pswp;
{
    PMYWND      pwnd;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    pwnd = PMYWNDOF(hwnd);
    pswp->fs = SWP_MOVE | SWP_SIZE;
    if( pwnd->ucKind == WK_MAIN )
    {
      if( pwnd->pwin == (WindowPeek)FrontWindow() )
        pswp->fs |= SWP_ACTIVATE;
      else
        pswp->fs |= SWP_DEACTIVATE;
    }
    pswp->x  = pwnd->x;
    pswp->y  = pwnd->y;
    pswp->cx = pwnd->cx;
    pswp->cy = pwnd->cy;
    pswp->hwnd = hwnd;
    pswp->hwndInsertBehind = NULL;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Fill *ppid and *ptid with the process and task ID's for hwnd.   */
/* In MacPM, these are fake values.                                */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinQueryWindowProcess( hwnd, ppid, ptid )
    HWND        hwnd;
    PPID        ppid;
    PTID        ptid;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    *ppid = MY_PID;
    *ptid = MY_PID;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Fill *prcl with the window rectangle for hwnd.                  */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinQueryWindowRect( hwnd, prcl )
    HWND        hwnd;
    PRECTL      prcl;
{
    PMYWND      pwnd;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    pwnd = PMYWNDOF(hwnd);

    prcl->xLeft   = 0;
    prcl->yBottom = 0;
    prcl->xRight  = pwnd->cx;
    prcl->yTop    = pwnd->cy;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Fill *pszText with hwnd's window text, by sending hwnd a        */
/* WM_QUERYWINDOWPARAMS message.                                   */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinQueryWindowText( hwnd, sMax, pszText )
    HWND        hwnd;
    SHORT       sMax;
    PSZ         pszText;
{
    WNDPARAMS   wprm;

    pszText[0] = '\0';

    memzero( &wprm );

    wprm.fsStatus = WPM_TEXT | WPM_CCHTEXT;
    wprm.pszText = pszText;
    wprm.cchText = sMax;

    if( WinSendMsg( hwnd, WM_QUERYWINDOWPARAMS, MPFROMP(&wprm), 0 ) )
      return wprm.cchText;
    else
      return 0;
}

/*-----------------------------------------------------------------*/
/* Return hwnd's window text length, by sending hwnd a             */
/* WM_QUERYWINDOWPARAMS message.                                   */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinQueryWindowTextLength( hwnd )
    HWND        hwnd;
{
    WNDPARAMS   wprm;

    memzero( &wprm );

    wprm.fsStatus = WPM_CCHTEXT;

    if( WinSendMsg( hwnd, WM_QUERYWINDOWPARAMS, MPFROMP(&wprm), 0 ) )
      return wprm.cchText;
    else
      return 0;
}

/*-----------------------------------------------------------------*/
/* Return one of hwnd's long values, as determined by index.       */
/*-----------------------------------------------------------------*/

ULONG APIENTRY WinQueryWindowULong( hwnd, index )
    HWND        hwnd;
    SHORT       index;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    if( index >= QWL_USER )
      return *(PULONG)&MYWNDOF(hwnd).bExtra[index];

    switch( index )
    {
      case QWL_STYLE:
        return MYWNDOF(hwnd).flStyle;

      case QWP_PFNWP:
        return (ULONG)MYWNDOF(hwnd).pfnwp;

      case QWL_HMQ:
        return (ULONG)(HMQ)1;
    }

    return 0L;
}

/*-----------------------------------------------------------------*/
/* Return one of hwnd's short values, as determined by index.      */
/*-----------------------------------------------------------------*/

USHORT APIENTRY WinQueryWindowUShort( hwnd, index )
    HWND        hwnd;
    SHORT       index;
{
    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    if( index >= QWL_USER )
      return *(PUSHORT)&MYWNDOF(hwnd).bExtra[index];

    switch( index )
    {
      case QWS_ID:
        return MYWNDOF(hwnd).id;
    }

    return 0;
}

/*-----------------------------------------------------------------*/
/* Return the child of hwndParent with the specified window ID.    */
/*-----------------------------------------------------------------*/

HWND APIENTRY WinWindowFromID( hwndParent, id )
    HWND        hwndParent;
    USHORT      id;
{
    HWND        hwnd;

    if( ! MpmValidateWindow(hwndParent) )
      return NULL;

    if( ISDESKTOPWINDOW(hwndParent) )
      return NULL;  /* could handle this case... */

    for( hwnd = MYWNDOF(hwndParent).hwndTopChild;
         hwnd;
         hwnd = MYWNDOF(hwnd).hwndNextSibling )
      if( MYWNDOF(hwnd).id == id )
        return hwnd;

    return NULL;
}

/*-----------------------------------------------------------------*/
/* Fill *prect with the Mac rectangle for hwnd.                    */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmQueryMacRect( hwnd, prect )
    HWND        hwnd;
    Rect*       prect;
{
    RECTL       rcl;

    WinQueryWindowRect( hwnd, &rcl );
    MpmMapMacOfRcl( hwnd, prect, &rcl );
}

/*-----------------------------------------------------------------*/
