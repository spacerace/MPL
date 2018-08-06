/***************************************************************************\
* mdi.c - MDI application
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#define INCL_WINHEAP

#include <os2.h>
#include "app.h"
#include "appdata.h"
#include "mdi.h"
#include "mdidata.h"

VOID MDIDesktopSize(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    HWND hwndDoc;
    RECTL rclWindow;
    POINTL ptlBorderSize;
    HENUM henum;
    SWP swp;
    ULONG ulStyle;

    /*
     * This code keeps maximized MDI windows maximized within the
     * main client window and bottom-left aligns minimized windows.
     */
    WinQueryWindowRect(hwnd, (PRECTL)&rclWindow);

    WinSendMsg(hwndMDIFrame, WM_QUERYBORDERSIZE, (MPARAM)&ptlBorderSize,
            0L);
    WinInflateRect(NULL, (PRECTL)&rclWindow, (SHORT)ptlBorderSize.x,
            (SHORT)ptlBorderSize.y);
    rclWindow.yTop += cyTitlebar;

    swp.hwndInsertBehind = NULL;

    henum = WinBeginEnumWindows(hwnd);
    while (hwndDoc = WinGetNextWindow(henum)) {
        WinLockWindow(hwndDoc, FALSE);
        ulStyle = WinQueryWindowULong(hwndDoc, QWL_STYLE);
        if (ulStyle & WS_MAXIMIZED) {
            swp.hwnd = hwndDoc;
            swp.x = (SHORT)rclWindow.xLeft;
            swp.y = (SHORT)rclWindow.yBottom;
            swp.cx = (SHORT)rclWindow.xRight - (SHORT)rclWindow.xLeft;
            swp.cy = (SHORT)rclWindow.yTop - (SHORT)rclWindow.yBottom;
            swp.fs = SWP_MOVE | SWP_SIZE;
            WinSetMultWindowPos(NULL, (PSWP)&swp, 1);
        } else if (ulStyle & WS_MINIMIZED) {

            WinQueryWindowPos(hwndDoc, &swp);

            swp.x = 0;
            swp.y = 0;
            swp.fs = SWP_MOVE | SWP_ZORDER;
            swp.hwndInsertBehind = HWND_BOTTOM;

       /*
            swp.y -= (SHORT2FROMMP(mp2) - SHORT2FROMMP(mp1));
            swp.fs = SWP_MOVE;
       */
            WinSetMultWindowPos(NULL, (PSWP)&swp, 1);

        }
    }
    WinEndEnumWindows(henum);

    /*
     * Adjust yNextNewDoc to keep it top-left aligned.
     */
    yNextNewDoc += (SHORT2FROMMP(mp2) - SHORT2FROMMP(mp1));
}


VOID MDIDesktopSetFocus(HWND hwnd, MPARAM mp2)
{
    HWND hwndTopDoc;

    /*
     * If we're getting the focus and there is a document window,
     * set the focus to the top-most document window so the main
     * client window never has the focus.
     */
    if ((SHORT1FROMMP(mp2) != FALSE) &&
            (hwndTopDoc = WinQueryWindow(hwnd, QW_TOP, FALSE))) {
        WinSetFocus(HWND_DESKTOP, hwndTopDoc);
    }
}


VOID MDIDesktopActivateDoc(SHORT idMenuitem)
{
    register NPDOC npdoc;

    npdoc = npdocFirst;
    while (npdoc != NULL) {
        if (npdoc->idMI == (USHORT) idMenuitem) {
            WinSetFocus(HWND_DESKTOP, npdoc->hwndFrame);

            /*
             * If the document is minimized then
             * restore it as well.
             */
            if (WinQueryWindowULong(npdoc->hwndFrame, QWL_STYLE) &
                    WS_MINIMIZED) {
                WinSetWindowPos(npdoc->hwndFrame, NULL,
                        0, 0, 0, 0, SWP_RESTORE);
            }
            break;
        }
        npdoc = npdoc->npdocNext;
    }
}

MRESULT EXPENTRY MainFrameWndProc(HWND hwnd, USHORT msg, MPARAM mp1,
        MPARAM mp2)
{
    HWND hwndAppMenu;
    NPDOC npdoc;

    switch (msg) {

    case WM_NEXTMENU:
        /*
         * Connect child sysmenu with application menus
         */

        npdoc=NPDOCFROMCLIENT(WinWindowFromID(hwndActiveDoc, FID_CLIENT));

        if (npdoc->fAabSysMenu == TRUE) {
            /* child sysmenu is already in app menu */
            return (*pfnMainFrameWndProc)(hwnd, msg, mp1, mp2);
            }

#define hwndCurrent HWNDFROMMP(mp1)

        hwndAppMenu=NPDOCFROMCLIENT(WinWindowFromID(hwndActiveDoc, FID_CLIENT))
                        ->hwndMainMenu;

        if (((hwndCurrent == hwndSysMenu) && !SHORT1FROMMP(mp2)) ||
                ((hwndCurrent == hwndAppMenu) && SHORT1FROMMP(mp2))) {

            if (hwndActiveDoc)
                return (WinWindowFromID(hwndActiveDoc, FID_SYSMENU));
        }

#undef hwndCurrent

        break;
    }
    return (*pfnMainFrameWndProc)(hwnd, msg, mp1, mp2);
}
