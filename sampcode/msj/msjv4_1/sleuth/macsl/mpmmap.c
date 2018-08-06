/*-----------------------------------------------------------------*/
/* MpmMap.c                                                        */
/* Coordinate mapping functions                                    */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/
/* Map a list of PM window points from hwndFrom's coordinates to   */
/* hwndTo's coordinates.  cwpt is the number of points.            */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinMapWindowPoints( hwndFrom, hwndTo, paptl, cwpt )
    HWND        hwndFrom, hwndTo;
    PPOINTL     paptl;
    SHORT       cwpt;
{
    POINTL      ptlFrom, ptlTo;
    PMYWND      pwnd;

    /* Fix up special case window handles */

    if( ! hwndFrom  ||  hwndFrom == HWND_DESKTOP )
      hwndFrom = _hwndDesktop;

    if( ! hwndTo  ||  hwndTo == HWND_DESKTOP )
      hwndTo = _hwndDesktop;

    if( ! MpmValidateWindow(hwndFrom)  ||
        ! MpmValidateWindow(hwndTo) )
      return FALSE;

    /* Get absolute positions for both windows */

    pwnd = PMYWNDOF(hwndFrom);
    ptlFrom.x = pwnd->x;
    ptlFrom.y = pwnd->y;
    MpmMapAbsOfWin( pwnd->hwndParent, &ptlFrom, 1 );

    pwnd = PMYWNDOF(hwndTo);
    ptlTo.x = pwnd->x;
    ptlTo.y = pwnd->y;
    MpmMapAbsOfWin( pwnd->hwndParent, &ptlTo, 1 );

    /* Adjust the points */

    for( ;  cwpt > 0;  paptl++, cwpt-- )
    {
      paptl->x += ptlFrom.x - ptlTo.x;
      paptl->y += ptlFrom.y - ptlTo.y;
    }

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Map a list of points from hwnd's coordinates to absolute        */
/* coordinates.                                                    */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmMapAbsOfWin( hwnd, pptl, cwpt )
    HWND        hwnd;
    PPOINTL     pptl;
    SHORT       cwpt;
{
    POINTL      ptlAdj;

    ptlAdj.x = ptlAdj.y = 0;

    for( ;  hwnd;  hwnd = MYWNDOF(hwnd).hwndParent )
    {
      ptlAdj.x += MYWNDOF(hwnd).x;
      ptlAdj.y += MYWNDOF(hwnd).y;
    }

    for( ;  cwpt > 0;  cwpt--, pptl++ )
    {
      pptl->x += ptlAdj.x;
      pptl->y += ptlAdj.y;
    }
}

/*-----------------------------------------------------------------*/
/* Map a PM POINTL to a Mac Point, using hwnd's coordinates.       */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmMapMacOfPtl( hwnd, ppoint, pptl )
    HWND        hwnd;
    Point*      ppoint;
    PPOINTL     pptl;
{
    RECTL       rclMain;
    HWND        hwndMain;
    Rect*       prectAdj;
    POINTL      ptl;

    ptl = *pptl;

    hwndMain = MAINHWND(hwnd);
    prectAdj = &MYWNDOF(hwndMain).rectAdj;
    WinQueryWindowRect( hwndMain, &rclMain );
    WinMapWindowPoints( hwnd, hwndMain, &ptl, 1 );

    ppoint->h = ptl.x - prectAdj->left;
    ppoint->v = rclMain.yTop - ptl.y - prectAdj->top;
}

/*-----------------------------------------------------------------*/
/* Map a PM RECTL to a Mac Rect, using hwnd's coordinates.         */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmMapMacOfRcl( hwnd, prect, prcl )
    HWND        hwnd;
    Rect*       prect;
    PRECTL      prcl;
{
    RECTL       rclMain;
    HWND        hwndMain;
    Rect*       prectAdj;
    RECTL       rcl;

    rcl = *prcl;

    hwndMain = MAINHWND(hwnd);
    prectAdj = &MYWNDOF(hwndMain).rectAdj;
    WinQueryWindowRect( hwndMain, &rclMain );
    WinMapWindowRect( hwnd, hwndMain, &rcl );

    prect->left   = rcl.xLeft                  - prectAdj->left;
    prect->right  = rcl.xRight                 - prectAdj->left;
    prect->top    = rclMain.yTop - rcl.yTop    - prectAdj->top;
    prect->bottom = rclMain.yTop - rcl.yBottom - prectAdj->top;
}

/*-----------------------------------------------------------------*/
/* Map a Mac Point to a PM POINTL, using hwnd's coordinates.       */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmMapPtlOfMac( hwnd, pptl, ppoint )
    HWND        hwnd;
    PPOINTL     pptl;
    Point*      ppoint;
{
    Rect        rect;

    MpmQueryMacRect( hwnd, &rect );

    pptl->x = ppoint->h - rect.left;
    pptl->y = rect.bottom - ppoint->v;
}

/*-----------------------------------------------------------------*/
/* Map a Mac Rect to a PM RECTL, using hwnd's coordinates.         */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmMapRclOfMac( hwnd, prcl, prect )
    HWND        hwnd;
    PRECTL      prcl;
    Rect*       prect;
{
    Rect        rect;

    MpmQueryMacRect( hwnd, &rect );

    prcl->xLeft   = prect->left  - rect.left;
    prcl->xRight  = prect->right - rect.left;
    prcl->yBottom = rect.bottom  - prect->bottom;
    prcl->yTop    = rect.bottom  - prect->top;
}

/*-----------------------------------------------------------------*/
/* Map a list of PM window points from absolute coordinates to     */
/* hwnd's coordinates.                                             */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmMapWinOfAbs( hwnd, pptl, cwpt )
    HWND        hwnd;
    PPOINTL     pptl;
    SHORT       cwpt;
{
    POINTL      ptlAdj;

    ptlAdj.x = ptlAdj.y = 0;

    for( ;  hwnd;  hwnd = MYWNDOF(hwnd).hwndParent )
    {
      ptlAdj.x -= MYWNDOF(hwnd).x;
      ptlAdj.y -= MYWNDOF(hwnd).y;
    }

    for( ;  cwpt > 0;  cwpt--, pptl++ )
    {
      pptl->x += ptlAdj.x;
      pptl->y += ptlAdj.y;
    }
}

/*-----------------------------------------------------------------*/
