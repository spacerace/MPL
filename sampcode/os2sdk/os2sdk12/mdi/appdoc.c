/*
    appdoc.c - MDI application
    Created by Microsoft Corporation, 1989
*/
#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINFRAMEMGR
#define INCL_WINPOINTERS
#define INCL_WINMENUS
#define INCL_WINWINDOWMGR
#define INCL_WINACCELERATORS
#define INCL_WININPUT
#define INCL_WINHEAP
#define INCL_WINSCROLLBARS
#define INCL_WINRECTANGLES
#define INCL_WINCOUNTRY
#define INCL_GPIPRIMITIVES
#define INCL_GPILOGCOLORTABLE

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "app.h"
#include "appdata.h"
#include "mdi.h"
#include "mdidata.h"

/* Function prototypes */
VOID AppHScroll(HWND hwnd, MPARAM mp1, MPARAM mp2);
VOID AppVScroll(HWND hwnd, MPARAM mp1, MPARAM mp2);
VOID AppEraseBackground(HWND hwnd, HPS hps);
VOID AppPaint(HWND hwnd);
VOID MDIClose(HWND hwndClient);
BOOL MDICreate(HWND);
BOOL MDIDestroy(HWND);
BOOL MDIActivate(HWND, BOOL);

/*
 * The array of RGB values for the rounded
 * rectangles.
 */
LONG aclrRGB[16] = {
    RGB_RED, RGB_WHITE, RGB_GREEN, RGB_BLACK,
    RGB_BLUE, RGB_WHITE, RGB_YELLOW, RGB_BLACK,
    RGB_CYAN, RGB_BLACK, RGB_PINK, RGB_BLACK,
    RGB_WHITE, RGB_PINK, RGB_BLACK, RGB_RED
};



MRESULT EXPENTRY DocWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

    switch (msg) {

    case WM_COMMAND:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;

    case WM_CREATE:
        if (MDICreate(hwnd) == FALSE)
            return ( (MRESULT) TRUE);
        break;

    case WM_DESTROY:
        MDIDestroy(hwnd);
        break;

    case WM_CLOSE:
        MDIClose(hwnd);
        break;

    case WM_HSCROLL:
        AppHScroll(hwnd, mp1, mp2);
        break;

    case WM_VSCROLL:
        AppVScroll(hwnd, mp1, mp2);
        break;

    case WM_ERASEBACKGROUND:
        AppEraseBackground(hwnd, (HPS)mp1);
        break;

    case WM_PAINT:
        AppPaint(hwnd);
        break;

    case WM_ACTIVATE:
        MDIActivate(hwnd, (BOOL)SHORT1FROMMP(mp1));
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }

    return (0L);
}


VOID AppEraseBackground(HWND hwnd, HPS hps)
{
    RECTL rclPaint;
    HWND hwndFrame, hwndClient;
    register NPDOC npdoc;

    npdoc = NPDOCFROMCLIENT(hwnd);
    hwndFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);

    /*
     * We know the main client is around so
     * go ahead and erase it.
     */
    WinQueryWindowRect(hwnd, &rclPaint);
    WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL)&rclPaint, 2);
    WinFillRect(hps, &rclPaint, npdoc->clrBackground);

    /*
     * Now check to see which of the other client windows
     * are around and erase them.
     *
     * We do all this to avoid erasing the splitbars.
     */
    if (npdoc->fs & DF_SPLITVERT) {

        hwndClient = WinWindowFromID(hwndFrame, ID_CLIENT2);
        /*
         * If it became invisible due to the frame
         * window getting too small, then don't
         * bother drawing.
         */
        if (WinIsWindowVisible(hwndClient) != FALSE) {
            WinQueryWindowRect(hwndClient, &rclPaint);
            WinMapWindowPoints(hwndClient, hwndFrame,
                    (PPOINTL)&rclPaint, 2);
            WinFillRect(hps, &rclPaint, npdoc->clrBackground);
        }
    }

    if (npdoc->fs & DF_SPLITHORZ) {

        hwndClient = WinWindowFromID(hwndFrame, ID_CLIENT3);
        if (WinIsWindowVisible(hwndClient) != FALSE) {
            WinQueryWindowRect(hwndClient, &rclPaint);
            WinMapWindowPoints(hwndClient, hwndFrame,
                    (PPOINTL)&rclPaint, 2);
            WinFillRect(hps, &rclPaint, npdoc->clrBackground);
        }
    }

    /*
     * If we're split in both directions, then there's
     * a ID_CLIENT4 window.
     */
    if ((npdoc->fs & (DF_SPLITHORZ | DF_SPLITVERT)) ==
            (DF_SPLITHORZ | DF_SPLITVERT)) {

        hwndClient = WinWindowFromID(hwndFrame, ID_CLIENT4);
        if (WinIsWindowVisible(hwndClient) != FALSE) {
            WinQueryWindowRect(hwndClient, &rclPaint);
            WinMapWindowPoints(hwndClient, hwndFrame,
                    (PPOINTL)&rclPaint, 2);
            WinFillRect(hps, &rclPaint, npdoc->clrBackground);
        }
    }
}


VOID AppHScroll(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    HWND hwndFrame;
    NPDOC npdoc;
    RECTL rclPaintBottom, rclPaintTop;
    RECTL rclWindowBottom, rclWindowTop;
    HWND hwndClientBottom, hwndClientTop;
    HWND hwndScrollbar;
    register NPVIEW npviewBottom, npviewTop;
    SHORT posSlider, xOriginOld;
    USHORT cmd, idScrollbar;

    hwndFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);
    npdoc = NPDOCFROMCLIENT(hwnd);

    idScrollbar = SHORT1FROMMP(mp1);

    switch (idScrollbar) {

    case FID_HORZSCROLL:
        hwndClientTop = hwnd;
        if (npdoc->fs & DF_SPLITHORZ) {
            hwndClientBottom = WinWindowFromID(hwndFrame, ID_CLIENT3);
        } else {
            hwndClientBottom = NULL;
        }
        break;

    case ID_HORZSCROLL2:
        hwndClientTop = WinWindowFromID(hwndFrame, ID_CLIENT2);
        if (npdoc->fs & DF_SPLITHORZ) {
            hwndClientBottom = WinWindowFromID(hwndFrame, ID_CLIENT4);
        } else {
            hwndClientBottom = NULL;
        }
        break;
    }

    hwndScrollbar = WinWindowFromID(hwndFrame, idScrollbar);

    npviewTop = NPVIEWFROMCLIENT(hwndClientTop);
    WinQueryWindowRect(hwndClientTop, &rclWindowTop);

    if (hwndClientBottom != NULL) {
        npviewBottom = NPVIEWFROMCLIENT(hwndClientBottom);
        WinQueryWindowRect(hwndClientBottom, &rclWindowBottom);
    }

    posSlider = (SHORT) (ULONG) WinSendMsg(hwndScrollbar, SBM_QUERYPOS, NULL, NULL);

    cmd = SHORT2FROMMP(mp2);
    switch (cmd) {

    case SB_LINELEFT:
        posSlider -= 16;
        break;

    case SB_LINERIGHT:
        posSlider += 16;
        break;

    case SB_PAGELEFT:
        posSlider -= ((SHORT)rclWindowTop.xRight - 16);
        break;

    case SB_PAGERIGHT:
        posSlider += ((SHORT)rclWindowTop.xRight - 16);
        break;

    case SB_SLIDERPOSITION:
        posSlider = SHORT1FROMMP(mp2);
        break;
    }

    WinSendMsg(hwndScrollbar, SBM_SETPOS, MPFROMSHORT(posSlider), NULL);

    xOriginOld = npviewTop->xOrigin;
    npviewTop->xOrigin = (SHORT) (ULONG) WinSendMsg(hwndScrollbar, SBM_QUERYPOS, NULL, NULL);
    WinScrollWindow(hwndClientTop, xOriginOld - npviewTop->xOrigin, 0,
            NULL, NULL, NULL, &rclPaintTop, NULL);

    if (hwndClientBottom != NULL) {
        xOriginOld = npviewBottom->xOrigin;
        npviewBottom->xOrigin = npviewTop->xOrigin;
        WinScrollWindow(hwndClientBottom, xOriginOld - npviewBottom->xOrigin,
                0, NULL, NULL, NULL, &rclPaintBottom, NULL);
    }

    WinMapWindowPoints(hwndClientTop, hwndFrame, (PPOINTL)&rclPaintTop, 2);
    WinInvalidateRect(hwndFrame, &rclPaintTop, TRUE);

    if (hwndClientBottom != NULL) {
        WinMapWindowPoints(hwndClientBottom, hwndFrame, (PPOINTL)&rclPaintBottom, 2);
        WinInvalidateRect(hwndFrame, &rclPaintBottom, TRUE);
    }
}


VOID AppVScroll(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    HWND hwndFrame;
    NPDOC npdoc;
    RECTL rclPaintRight, rclPaintLeft;
    RECTL rclWindowRight, rclWindowLeft;
    HWND hwndClientRight, hwndClientLeft;
    HWND hwndScrollbar;
    register NPVIEW npviewRight, npviewLeft;
    SHORT posSlider, yOriginOld;
    USHORT cmd, idScrollbar;

    hwndFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);
    npdoc = NPDOCFROMCLIENT(hwnd);

    idScrollbar = SHORT1FROMMP(mp1);

    switch (idScrollbar) {

    case FID_VERTSCROLL:
        hwndClientLeft = hwnd;
        if (npdoc->fs & DF_SPLITVERT) {
            hwndClientRight = WinWindowFromID(hwndFrame, ID_CLIENT2);
        } else {
            hwndClientRight = NULL;
        }
        break;

    case ID_VERTSCROLL2:
        hwndClientLeft = WinWindowFromID(hwndFrame, ID_CLIENT3);
        if (npdoc->fs & DF_SPLITVERT) {
            hwndClientRight = WinWindowFromID(hwndFrame, ID_CLIENT4);
        } else {
            hwndClientRight = NULL;
        }
        break;
    }

    hwndScrollbar = WinWindowFromID(hwndFrame, idScrollbar);

    npviewLeft = NPVIEWFROMCLIENT(hwndClientLeft);
    WinQueryWindowRect(hwndClientLeft, &rclWindowLeft);

    if (hwndClientRight != NULL) {
        npviewRight = NPVIEWFROMCLIENT(hwndClientRight);
        WinQueryWindowRect(hwndClientRight, &rclWindowRight);
    }

    posSlider = (SHORT) (ULONG) WinSendMsg(hwndScrollbar, SBM_QUERYPOS, NULL, NULL);

    cmd = SHORT2FROMMP(mp2);
    switch (cmd) {

    case SB_LINEUP:
        posSlider -= 16;
        break;

    case SB_LINEDOWN:
        posSlider += 16;
        break;

    case SB_PAGEUP:
        posSlider -= ((SHORT)rclWindowLeft.yTop - 16);
        break;

    case SB_PAGEDOWN:
        posSlider += ((SHORT)rclWindowLeft.yTop - 16);
        break;

    case SB_SLIDERPOSITION:
        posSlider = SHORT1FROMMP(mp2);
        break;
    }

    WinSendMsg(hwndScrollbar, SBM_SETPOS, MPFROMSHORT(posSlider), NULL);

    yOriginOld = npviewLeft->yOrigin;
    npviewLeft->yOrigin = (SHORT) (ULONG) WinSendMsg(hwndScrollbar, SBM_QUERYPOS, NULL, NULL);
    WinScrollWindow(hwndClientLeft, 0,  npviewLeft->yOrigin - yOriginOld,
            NULL, NULL, NULL, &rclPaintLeft, NULL);

    if (hwndClientRight != NULL) {
        yOriginOld = npviewRight->yOrigin;
        npviewRight->yOrigin = npviewLeft->yOrigin;
        WinScrollWindow(hwndClientRight, 0, npviewRight->yOrigin - yOriginOld,
                NULL, NULL, NULL, &rclPaintRight, NULL);
    }

    WinMapWindowPoints(hwndClientLeft, hwndFrame, (PPOINTL)&rclPaintLeft, 2);
    WinInvalidateRect(hwndFrame, &rclPaintLeft, TRUE);

    if (hwndClientRight != NULL) {
        WinMapWindowPoints(hwndClientRight, hwndFrame, (PPOINTL)&rclPaintRight, 2);
        WinInvalidateRect(hwndFrame, &rclPaintRight, TRUE);
    }
}


VOID AppPaint(HWND hwnd)
{
    HPS hps;
    RECTL rclPaint, rclWindow, rclTest, rclDst;
    POINTL ptl, ptlPatternRef;
    register NPVIEW npview;
    AREABUNDLE abnd;
    LONG clrStart, clrEnd, clrInc, clr;
    SHORT i, j;

    hps = WinBeginPaint(hwnd, (HPS)NULL, &rclPaint);

    /*
     * Go into RGB mode.
     */
    GpiCreateLogColorTable(hps, 0L, LCOLF_RGB, 0L, 0L, NULL);

    /*
     * Make rclPaint an inclusive-inclusive rectangle
     * since that's how GpiBox() will be output.
     */
    rclPaint.xLeft--;
    rclPaint.yBottom--;

    npview = NPVIEWFROMCLIENT(hwnd);

    /*
     * Set the pattern to be at the top-left
     * since we're top-left aligning the bits.
     */
    WinQueryWindowRect(hwnd, (PRECTL)&rclWindow);
    ptlPatternRef.x = rclWindow.xLeft - npview->xOrigin;
    ptlPatternRef.y = rclWindow.yTop + npview->yOrigin;
    GpiSetPatternRefPoint(hps, &ptlPatternRef);

    for (i = 0; i < 8; i++) {

        clr = clrStart = aclrRGB[i * 2];
        clrEnd = aclrRGB[(i * 2) + 1];
        clrInc = (clrEnd - clrStart) / 8;

        for (j = 0; j < 8; j++) {
            abnd.lColor = clr + (j * clrInc);
            GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0L, (PBUNDLE)&abnd);

            /*
             * Only draw the box if it's going to
             * be visible in the update region.
             */
            WinSetRect(NULL, &rclTest, 10 + (i * 75),
                    (SHORT)rclWindow.yTop - 75 - (j * 75), 75 + (i * 75),
                    (SHORT)rclWindow.yTop - 10 - (j * 75));

            WinOffsetRect(NULL, &rclTest, -npview->xOrigin, npview->yOrigin);

            if (WinIntersectRect(NULL, &rclDst, &rclTest, &rclPaint)) {

                ptl.x = rclTest.xLeft;
                ptl.y = rclTest.yTop;
                GpiSetCurrentPosition(hps, (PPOINTL)&ptl);

                ptl.x = rclTest.xRight;
                ptl.y = rclTest.yBottom;
                GpiBox(hps, DRO_OUTLINEFILL, (PPOINTL)&ptl, 40L, 40L);
            }
        }
    }

    WinEndPaint(hps);
}
