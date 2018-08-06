/*-----------------------------------------------------------------*/
/* MpmCreate.c                                                     */
/* Window creation functions                                       */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

HMYCLASS    _hFirstClass = NULL;

Rect    _rectAdjPlainDBox    = {  1, 1, -1, -1 };  /* wrong... */
Rect    _rectAdjDocumentProc = { 19, 1, -2, -2 };
Rect    _rectAdjDBoxProc     = {  5, 5, -5, -5 };  /* check this */
Rect    _rectAdjAltDBoxProc  = {  1, 1, -3, -3 };  /* this too! */

/*-----------------------------------------------------------------*/

LOCAL HMYCLASS  MpmFindClass( PSZ pszName, USHORT usHash );
LOCAL VOID      MpmFixName( PSZ* ppszName, PSZ pszBuf,
                            PUSHORT pusHash );

/*-----------------------------------------------------------------*/
/* Create a window.                                                */
/*-----------------------------------------------------------------*/

HWND APIENTRY
WinCreateWindow( hwndParent, pszClass,  pszName, flStyle,
                 x, y, cx, cy, hwndOwner, hwndPrevSibling, id,
                 pCtlData, pPresParams )
    HWND        hwndParent, hwndOwner, hwndPrevSibling;
    PSZ         pszClass, pszName;
    ULONG       flStyle;
    SHORT       x, y, cx, cy;
    USHORT      id;
    PVOID       pCtlData, pPresParams;
{
    HWND        hwnd, hwndNextSibling, hwndMac;
    PMYWND      pwnd;
    HMYCLASS    hClass;
    PMYCLASS    pClass;
    USHORT      usLen;
    SHORT       sProcID;
    Rect        rect, rectAdj;
    WindowPeek  pwin, pwinBehind;
    CHAR        szTitle[256];
    BOOL        fGoAway, fFrameWindow;
    UCHAR       ucKind;
    USHORT      usHash;
    CHAR        szClassBuf[10];
    ULONG       flFrameFlags;

    /* Figure out which WK_ code to use */

    if( pszClass == WC_DESKTOP )
      ucKind = WK_DESKTOP;
    else
    {
      if( ! hwndParent  ||  hwndParent == HWND_DESKTOP )
        hwndParent = _hwndDesktop;
      if( hwndParent == _hwndDesktop )
        ucKind = WK_MAIN;
      else
        ucKind = WK_CHILD;
    }

    /* Fix up WC_ names, calculate hash, and find window class */

    MpmFixName( &pszClass, szClassBuf, &usHash );

    hClass = MpmFindClass( pszClass, usHash );
    if( ! hClass )
      return NULL;

    /* Grab frame flags if it's the window frame class */

    fFrameWindow = MpmIsFrameClass( pszClass );
    flFrameFlags =
      ( fFrameWindow && pCtlData ? *(PULONG)pCtlData : 0 );

    /* Allocate window structure */

    usLen = sizeof(MYWND) + (**hClass).class.cbWindowData;
    hwnd = (HWND)MpmNewHandleZ( usLen );
    if( ! hwnd )
      return NULL;

    /* Handle WK_ specific stuff */

    switch( ucKind )
    {
      /* Desktop window: just set window position */

      case WK_DESKTOP:
        SetRect( &rect, 0, 0, cx, cy );
        SetRect( &rectAdj, 0, 0, 0, 0 );
        break;

      /* Main Mac window: figure out which style of Mac window
         to use and create it */

      case WK_MAIN:
        fGoAway = FALSE;
        if( ! ( flStyle & FS_BORDER )  &&
            ! ( flFrameFlags & FCF_DOCBITS ) )
        {
          sProcID = plainDBox;  /* shouldn't have border! */
          rectAdj = _rectAdjPlainDBox;
        }
        else if( flFrameFlags & FCF_DOCBITS )
        {
          sProcID = documentProc;
          rectAdj = _rectAdjDocumentProc;
          if( flFrameFlags & FCF_MAXBUTTON )
            sProcID += 8;
          if( flFrameFlags & FCF_SYSMENU )
            fGoAway = TRUE;
        }
        else if( flStyle & FS_DLGBORDER )
        {
          sProcID = dBoxProc;
          rectAdj = _rectAdjDBoxProc;
        }
        else
        {
          sProcID = altDBoxProc;
          rectAdj = _rectAdjAltDBoxProc;
        }

        rect.left   = rectAdj.left   + x;
        rect.right  = rectAdj.right  + x + cx;
        rect.top    = rectAdj.top    + rect.bottom - cy;
        rect.bottom = rectAdj.bottom + screenBits.bounds.bottom - y;
    
        /* Should check rect for out of screen boundaries! */
    
        strncpy( szTitle, pszName, 255 );
        szTitle[255] = '\0';
    
        if( hwndPrevSibling == HWND_TOP )
          pwinBehind = (WindowPeek)(-1);
        else if( hwndPrevSibling == HWND_BOTTOM )
          pwinBehind = (WindowPeek)NULL;
        else if( ISMAINWINDOW(hwndPrevSibling) )
          pwinBehind = PWINOFHWND(hwndPrevSibling);
        else
          ERROR( "WinCreateWindow: Invalid hwndPrevSibling" );
    
        pwin = 
          (WindowPeek)NewWindow( NULL, &rect, CtoPstr(szTitle),
                                 FALSE, sProcID, pwinBehind,
                                 fGoAway, (LONG)hwnd );
        if( ! pwin )
        {
          DisposHandle( (Handle)hwnd );
          return NULL;
        }
        PWINOFHWND(hwnd) = pwin;

        break;

      /* Child window: set up all the "kin" windows */

      case WK_CHILD:
        if( hwndPrevSibling == HWND_TOP )
        {
          MYWNDOF(hwnd).hwndNextSibling = hwndNextSibling =
            MYWNDOF(hwndParent).hwndTopChild;
          MYWNDOF(hwndNextSibling).hwndPrevSibling = hwnd;
          MYWNDOF(hwndParent).hwndTopChild = hwnd;
        }
        else if( hwndPrevSibling == HWND_BOTTOM )
        {
          MYWNDOF(hwnd).hwndPrevSibling = hwndPrevSibling =
            MYWNDOF(hwndParent).hwndBottomChild;
          MYWNDOF(hwndPrevSibling).hwndNextSibling = hwnd;
          MYWNDOF(hwndParent).hwndBottomChild = hwnd;
        }
        else
        {
          if( ! MpmValidateWindow(hwndPrevSibling) )
            return NULL;
          if( MYWNDOF(hwndPrevSibling).hwndParent != hwndParent )
            return NULL;
          MYWNDOF(hwnd).hwndNextSibling = hwndNextSibling =
            MYWNDOF(hwndPrevSibling).hwndNextSibling;
          MYWNDOF(hwnd).hwndPrevSibling = hwndPrevSibling;
          MYWNDOF(hwndPrevSibling).hwndNextSibling = hwnd;
          if( hwndNextSibling )
            MYWNDOF(hwndNextSibling).hwndPrevSibling = hwnd;
          else
            MYWNDOF(hwndParent).hwndBottomChild = hwnd;
        }
        if( ! MYWNDOF(hwndParent).hwndTopChild )
          MYWNDOF(hwndParent).hwndTopChild = hwnd;
        if( ! MYWNDOF(hwndParent).hwndBottomChild )
          MYWNDOF(hwndParent).hwndBottomChild = hwnd;
        for( hwndMac = hwndParent;
             ISCHILDWINDOW(hwndMac);
             hwndMac = MYWNDOF(hwndMac).hwndParent );
        PWINOFHWND(hwnd) = PWINOFHWND(hwndMac);
        rectAdj = MYWNDOF(hwndMac).rectAdj;
        break;
    }

    /* Fill in the window structure fields */

    pClass = *hClass;
    pwnd = PMYWNDOF(hwnd);

    pwnd->signature = WND_SIGNATURE;
    pwnd->ucKind = ucKind;
    pwnd->pfnwp = pClass->class.pfnWindowProc;
    pwnd->hclass = hClass;
    pwnd->flStyle =
      ( flStyle & ~WS_VISIBLE ) |
      ( pClass->class.flClassStyle & CLASSWINDOWBITS );
    pwnd->hwndOwner = hwndOwner;
    pwnd->hwndParent = hwndParent;
    pwnd->id = id;
    pwnd->rectAdj = rectAdj;
    pwnd->flFrameFlags = flFrameFlags;

    /* Now the window is here for real, so send the WM_CREATE */

    if( WinSendMsg( hwnd, WM_CREATE,
                    MPFROMP(pCtlData), MPFROMP(&hwndParent) ) )
    {
      WinDestroyWindow( hwnd );
      return NULL;
    }

    /* Send the WM_ADJUSTWINDOWPOS if it's not the desktop window
       and it has a nonzero size */

    if( cx  &&  cy  &&  ucKind != WK_DESKTOP )
      WinSetWindowPos( hwnd, NULL, x, y, cx, cy, SWP_MOVE | SWP_SIZE );

    /* Make the window visible if it's supposed to be visible */

    if( flStyle & WS_VISIBLE )
      WinShowWindow( hwnd, TRUE );

    return hwnd;
}

/*-----------------------------------------------------------------*/
/* Destroy a window.                                               */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinDestroyWindow( hwnd )
    HWND        hwnd;
{
    HWND        hwndChild, hwndNextSibling, hwndPrevSibling;
    PMYWND      pwnd, pwndParent;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    /* Hide the window if it's visible */

    if( MYWNDOF(hwnd).flStyle & WS_VISIBLE )
      WinShowWindow( hwnd, FALSE );

    /* Destroy all children */

    while( hwndChild = MYWNDOF(hwnd).hwndTopChild )
      WinDestroyWindow( hwndChild );

    /* Let the window know it's being destroyed */

    WinSendMsg( hwnd, WM_DESTROY, 0L, 0L );

    /* For top level windows, destroy the Mac window; for
       child windows, clean up the "kin" pointers */

    if( ISMAINWINDOW(hwnd) )
    {
      DisposeWindow( PWINOFHWND(hwnd) );
    }
    else if( ISCHILDWINDOW(hwnd) )
    {
      pwnd            = PMYWNDOF( hwnd );
      pwndParent      = PMYWNDOF( pwnd->hwndParent );
      hwndNextSibling = pwnd->hwndNextSibling;
      hwndPrevSibling = pwnd->hwndPrevSibling;

      if( pwndParent->hwndTopChild == hwnd )
        pwndParent->hwndTopChild = hwndNextSibling;

      if( pwndParent->hwndBottomChild == hwnd )
        pwndParent->hwndBottomChild = hwndPrevSibling;

      if( hwndNextSibling )
        MYWNDOF(hwndNextSibling).hwndPrevSibling = hwndPrevSibling;

      if( hwndPrevSibling )
        MYWNDOF(hwndPrevSibling).hwndNextSibling = hwndNextSibling;
    }
    else
      ERROR( "WinDestroyWindow: Cannot destroy desktop window." );

    /* All done, free up the window structure */

    DisposHandle( (Handle)hwnd );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Get the window class information for a class name               */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinQueryClassInfo( hab, pszClassName, pClassInfo )
    HAB         hab;
    PSZ         pszClassName;
    PCLASSINFO  pClassInfo;
{
    HMYCLASS    hClass;
    CHAR        szClassBuf[10];
    USHORT      usHash;

    /* Fix up WC_ names and calculate hash */

    MpmFixName( &pszClassName, szClassBuf, &usHash );

    /* Find the class info and copy it back */

    hClass = MpmFindClass( pszClassName, usHash );
    if( ! hClass )
      return FALSE;

    *pClassInfo = (**hClass).class;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Register a window class.                                        */
/*-----------------------------------------------------------------*/

BOOL APIENTRY
WinRegisterClass( hab, pszClassName, pfnwp,
                    flStyle, cbWindowData )
    HAB         hab;
    PSZ         pszClassName;
    PFNWP       pfnwp;
    ULONG       flStyle;
    USHORT      cbWindowData;
{
    HMYCLASS    hClass;
    PMYCLASS    pClass;
    CHAR        szClassBuf[10];
    USHORT      usHash;

    /* Fix up WC_ names and calculate hash */

    MpmFixName( &pszClassName, szClassBuf, &usHash );

    /* Make sure the class doesn't already exist */

    if( MpmFindClass( pszClassName, usHash ) )
      return FALSE;

    /* Allocate the class structure and fill it in */

    hClass =
      (HMYCLASS)MpmNewHandleZ( sizeof(MYCLASS) +
                               strlen(pszClassName) );
    if( ! hClass )
      return FALSE;

    pClass = *hClass;
    
    pClass->usHash = usHash;
    pClass->class.flClassStyle = flStyle;
    pClass->class.pfnWindowProc = pfnwp;
    pClass->class.cbWindowData = cbWindowData;
    strcpy( pClass->szName, pszClassName );

    /* Link the class into the class chain */

    pClass->hNextClass = _hFirstClass;
    _hFirstClass = hClass;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Find the MYCLASS structure for a class name                     */
/*-----------------------------------------------------------------*/

LOCAL HMYCLASS MpmFindClass( pszClassName, usHash )
    PSZ         pszClassName;
    USHORT      usHash;
{
    HMYCLASS    hClass;

    for( hClass = _hFirstClass;
         hClass;
         hClass = (**hClass).hNextClass )
      if( usHash == (**hClass).usHash  &&
          strcmp( pszClassName, (**hClass).szName ) == 0 )
        return hClass;

    return NULL;
}

/*-----------------------------------------------------------------*/
/* Fix up the special WC_ class "names" and calculate the hash     */
/* value for a class name.                                         */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmFixName( ppszName, pszBuf, pusHash )
    PSZ*        ppszName;
    PSZ         pszBuf;
    PUSHORT     pusHash;
{
    PSZ         pszName;
    USHORT      usHash;

    pszName = *ppszName;

    if( HIUSHORT(pszName) == 0xFFFF )
    {
      sprintf( pszBuf, "#%u", LOUSHORT(pszName) );
      pszName = pszBuf;
    }

    *ppszName = pszName;

    for( usHash = 0;  *pszName;  pszName++ )
      usHash = (usHash << 1) ^ *pszName;

    *pusHash = usHash;
}

/*-----------------------------------------------------------------*/
