/*-----------------------------------------------------------------*/
/* MpmMenu.c                                                       */
/* Menu functions                                                  */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

SHORT       _sAppleCount;

/*-----------------------------------------------------------------*/
/* Window function for the menu window class.                      */
/* Note that this handles the Mac's menu, the zoom box, and the    */
/* close box, since these are all menus in PM.                     */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY MpmFnwpMenu( hwnd, msg, mp1, mp2 )
    HWND        hwnd;
    USHORT      msg;
    MPARAM      mp1;
    MPARAM      mp2;
{
    Point       point;
    HWND        hwndOwner;
    LONG        lMenuID;
    USHORT      usMenuID;
    MenuHandle  hmenu;

    if( ! MpmValidateWindow(hwnd) )
      return FALSE;

    hwndOwner = MYWNDOF(hwnd).hwndOwner;
    ASSERT( hwndOwner,
            "MpmFnwpMenu: no owner" );

    switch( msg )
    {
      /* Button down - see which it is and handle it */

      case WM_BUTTON1DOWN:
        point.h = MOUSEMSG(&msg)->x;
        point.v = _alSysVal[SV_CYSCREEN] - MOUSEMSG(&msg)->y;
        switch( MYWNDOF(hwnd).id )
        {
          /* Menu bar: track mouse, then open a desk accessory or
             send a WM_COMMAND msg */

          case FID_MENU:
            WinSendMsg( hwndOwner, WM_INITMENU,
                        MPFROMSHORT(usMenuID),
                        MPFROMP(hwnd) );
            lMenuID = MenuSelect( point );
            hmenu = GetMHandle( HIUSHORT(lMenuID) );
            if( hmenu == (**(MbarHandle)MenuList).mlist[0].hmenu  &&
                LOUSHORT(lMenuID) > _sAppleCount )
            {
              Str255  strDAName;
              GetItem( hmenu, LOUSHORT(lMenuID), (char*)strDAName );
              OpenDeskAcc( (char*)strDAName );
              return 0L;
            }
            if( ! HIUSHORT(lMenuID) )
              return 0L;
            usMenuID = ( HIUSHORT(lMenuID) << 8 )  +
                       ( LOUSHORT(lMenuID) & 0xFF );
            WinSendMsg( hwndOwner, WM_COMMAND,
                        MPFROMSHORT(usMenuID),
                        MPFROM2SHORT( CMDSRC_MENU, FALSE /*?*/ ) );
            HiliteMenu( 0 );
            return 0L;

          /* Zoom box: track mouse, send SC_RESTORE or SC_MAXIMIZE */

          case FID_MINMAX:
            if( TrackBox( MYWNDOF(hwnd).pwin, point,
                          SHORT2FROMMP(mp2) ) )
              WinSendMsg( hwndOwner, WM_SYSCOMMAND,
                          MPFROMSHORT( SHORT2FROMMP(mp2) == inZoomIn
                                         ? SC_RESTORE
                                         : SC_MAXIMIZE ),
                          MPFROM2SHORT( CMDSRC_MENU, FALSE /*?*/ ) );
            return 0L;

          /* Close box: track mouse and send SC_CLOSE */

          case FID_SYSMENU:
            if( TrackGoAway( MYWNDOF(hwnd).pwin, point ) )
              WinSendMsg( hwndOwner, WM_SYSCOMMAND,
                          MPFROMSHORT( SC_CLOSE ),
                          MPFROM2SHORT( CMDSRC_MENU, FALSE /*?*/ ) );
            return 0L;
        }
        return 0L;

      /* WM_CREATE and WM_DESTROY manipulate the static _hwndMenu,
         only for the menu bar itself */

      case WM_CREATE:
        if( MYWNDOF(hwnd).id == FID_MENU )
        {
          ASSERT( ! _hwndMenu,
                  "MpmFnwpMenu: more than one menu" );
          _hwndMenu = hwnd;
        }
        return 0L;
        
      case WM_DESTROY:
        if( MYWNDOF(hwnd).id == FID_MENU )
          _hwndMenu = NULL;
        return 0L;

      /* WM_INITMENU is just passed along to the owner */
        
      case WM_INITMENU:
        return WinSendMsg( hwndOwner, msg, mp1, mp2 );
    }
    return 0L;
    
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
/* Load the menu bar from the resource file, using idResources.    */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmMenuLoad( hwndFrame, idResources )
    HWND        hwndFrame;
    USHORT      idResources;
{
    MbarHandle  hmbar;
    MbarPtr     pmbar;
    SHORT       sMenuCount;
    MenuHandle  hmenu;

    hmbar = (MbarHandle)GetNewMBar( idResources );
    if( ! hmbar )
      return FALSE;

    sMenuCount = MENUCOUNTOFHMBAR( hmbar );
    ASSERT( sMenuCount > 0,
            "MpmMenuLoad: null menu bar?" );

    SetMenuBar( hmbar );
    DisposHandle( (Handle)hmbar );

    hmbar = (MbarHandle)MenuList;

    hmenu = (**hmbar).mlist[0].hmenu;

    ASSERT( hmenu,
            "MpmMenuLoad: no apple menu?" );

    _sAppleCount = CountMItems( hmenu );

    AddResMenu( hmenu, 'DRVR' );

    DrawMenuBar();

    return TRUE;
}

/*-----------------------------------------------------------------*/
