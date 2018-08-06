/***************************************************************************\
* split.c - Code for window splitting
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WINMENUS
#define INCL_WININPUT
#define INCL_WINHEAP
#define INCL_WINSCROLLBARS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#define INCL_GPIBITMAPS
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include "app.h"
#include "appdata.h"
#include "mdi.h"
#include "mdidata.h"

VOID InvertRect(HPS, PRECTL);
VOID TrackSplitbars(HWND, USHORT, SHORT, SHORT);
VOID FillSplitbarInteriors(USHORT, HWND, HWND);
HWND QueryBotLeftClient(HWND hwndFrame, NPDOC npdoc);
VOID DrawTrackRects(HPS, PRECTL, SHORT, SHORT, USHORT);
VOID MoveTrackRects(HPS, PRECTL, USHORT, SHORT, SHORT, SHORT, SHORT);
VOID CalcTrackRect(PRECTL, PRECTL, SHORT, USHORT);
VOID SetSplitbarPos(HWND, PRECTL, SHORT, SHORT, USHORT);
HWND QueryBotLeftClient(HWND hwndFrame, NPDOC npdoc);

MRESULT EXPENTRY SplitbarWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    HPS hps;
    POINTL ptl;
    RECTL rclPaint;
    HWND hwndClient, hwndFrame, hwndOrigin;
    BOOL fControl;
    register NPDOC npdoc;

    switch (msg) {

    case WM_PAINT:
        hwndFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);
        hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

        npdoc = NPDOCFROMCLIENT(hwndClient);

        hps = WinBeginPaint(hwnd, NULL, NULL);

        WinQueryWindowRect(hwnd, &rclPaint);

        if (WinQueryWindowUShort(hwnd, QWS_ID) == ID_VERTSPLITBAR) {
            if (npdoc->fs & DF_SPLITVERT) {
                WinDrawBorder(hps, &rclPaint, 1, 0, SYSCLR_WINDOWFRAME,
                            SYSCLR_WINDOW, DB_STANDARD);
                /*
                 * Make the interiors of the splitbars
                 * visually correct.
                 */
                FillSplitbarInteriors(npdoc->fs, hwnd,
                        WinWindowFromID(hwndFrame, ID_HORZSPLITBAR));
            } else {
                WinFillRect(hps, &rclPaint, SYSCLR_WINDOWFRAME);
            }
        } else {
            if (npdoc->fs & DF_SPLITHORZ) {
                WinDrawBorder(hps, &rclPaint, 0, 1, SYSCLR_WINDOWFRAME,
                        SYSCLR_WINDOW, DB_STANDARD | DB_INTERIOR);
            } else {
                WinFillRect(hps, &rclPaint, SYSCLR_WINDOWFRAME);
            }
        }

        WinEndPaint(hps);
        break;

    case WM_MOUSEMOVE:
        hwndFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);
        hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

        npdoc = NPDOCFROMCLIENT(hwndClient);

        ptl.x = SHORT1FROMMP(mp1);
        ptl.y = SHORT2FROMMP(mp1);

        hwndOrigin = QueryBotLeftClient(hwndFrame, npdoc);
        WinMapWindowPoints(hwnd, hwndOrigin, (PPOINTL)&ptl, 1);

        if (WinQueryWindowUShort(hwnd, QWS_ID) == ID_VERTSPLITBAR) {

            if ((npdoc->fs & DF_SPLITHORZ) &&
                    ((SHORT)ptl.y > (npdoc->cyHorzSplitPos - (cyHorzSplitbar))) &&
                    ((SHORT)ptl.y <= (npdoc->cyHorzSplitPos + (cyHorzSplitbar * 2)))) {
                WinSetPointer(HWND_DESKTOP, hptrHVSplit);
            } else {
                WinSetPointer(HWND_DESKTOP, hptrVertSplit);
            }
        } else {
            if ((npdoc->fs & DF_SPLITVERT) &&
                    ((SHORT)ptl.x > (npdoc->cxVertSplitPos - (cxVertSplitbar))) &&
                    ((SHORT)ptl.x <= (npdoc->cxVertSplitPos + (cxVertSplitbar * 2)))) {
                WinSetPointer(HWND_DESKTOP, hptrHVSplit);
            } else {
                WinSetPointer(HWND_DESKTOP, hptrHorzSplit);
            }
        }
        break;

    case WM_BUTTON1DOWN:
    case WM_BUTTON1DBLCLK:
        /*
         * Get the control key state here so the user doesn't
         * have to hold it down the whole time to get the
         * desired effect.
         */
        fControl = (WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000);

        hwndFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);
        hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

        /*
         * Update the entire window before we start
         * tracking with the splitter bars.
         */
        WinUpdateWindow(hwndFrame);

        npdoc = NPDOCFROMCLIENT(hwndClient);

        ptl.x = SHORT1FROMMP(mp1);
        ptl.y = SHORT2FROMMP(mp1);

        hwndOrigin = QueryBotLeftClient(hwndFrame, npdoc);
        WinMapWindowPoints(hwnd, hwndOrigin, (PPOINTL)&ptl, 1);

        if (WinQueryWindowUShort(hwnd, QWS_ID) == ID_VERTSPLITBAR) {
            if ((npdoc->fs & DF_SPLITHORZ) &&
                    ((SHORT)ptl.y > (npdoc->cyHorzSplitPos - (cyHorzSplitbar))) &&
                    ((SHORT)ptl.y <= (npdoc->cyHorzSplitPos + (cyHorzSplitbar * 2)))) {
                TrackSplitbars(hwndClient, SPS_VERT | SPS_HORZ, (SHORT)ptl.x,
                        (SHORT)ptl.y);
            } else {
                TrackSplitbars(hwndClient, SPS_VERT, (SHORT)ptl.x,
                        (SHORT)ptl.y);
            }
        } else {
            if ((npdoc->fs & DF_SPLITVERT) &&
                    ((SHORT)ptl.x > (npdoc->cxVertSplitPos - (cxVertSplitbar))) &&
                    ((SHORT)ptl.x <= (npdoc->cxVertSplitPos + (cxVertSplitbar * 2)))) {
                TrackSplitbars(hwndClient, SPS_VERT | SPS_HORZ, (SHORT)ptl.x,
                        (SHORT)ptl.y);
            } else {
                TrackSplitbars(hwndClient, SPS_HORZ, (SHORT)ptl.x,
                        (SHORT)ptl.y);
            }
        }

        /*
         * If the control key is down, we leave
         * the focus where it is.
         */
        if (fControl == FALSE)
            WinSetFocus(HWND_DESKTOP, hwndClient);

        break;

    case WM_MOVE:
    case WM_SIZE:
        WinInvalidateRect(hwnd, NULL, FALSE);
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }

    return (0L);
}


VOID FillSplitbarInteriors(USHORT fs, HWND hwndVert, HWND hwndHorz)
{
    HPS hps;
    RECTL rclVertSplitbar, rclHorzSplitbar;

    if (fs & DF_SPLITVERT) {
        hps = WinGetPS(hwndVert);
        WinQueryWindowRect(hwndVert, &rclVertSplitbar);
        WinInflateRect(NULL, &rclVertSplitbar, -cxBorder, 0);
        WinFillRect(hps, &rclVertSplitbar, SYSCLR_WINDOW);
        WinReleasePS(hps);
    }

    if (fs & DF_SPLITHORZ) {
        hps = WinGetPS(hwndHorz);
        WinQueryWindowRect(hwndHorz, &rclHorzSplitbar);
        WinInflateRect(NULL, &rclHorzSplitbar, 0, -cyBorder);
        WinFillRect(hps, &rclHorzSplitbar, SYSCLR_WINDOW);
        WinReleasePS(hps);
    }
}


BOOL CreateSplitbarWindows(HWND hwndFrame, NPDOC npdoc)
{
    USHORT fsStyle;
    register NPVIEW npview, npviewNew;
    HWND hwndNewClient;

    fsStyle = npdoc->fsStyle;
    npview = npdoc->npviewFirst;

    if (fsStyle & DS_VERTSPLITBAR) {
        if (WinCreateWindow(hwndFrame, WC_SCROLLBAR, "",
                SBS_HORZ, 0, 0, 0, 0, hwndFrame,
                WinWindowFromID(hwndFrame, FID_HORZSCROLL),
                ID_HORZSCROLL2, NULL, NULL) == NULL)
            return (FALSE);

        if (WinCreateWindow(hwndFrame, szSplitbarClass, "",
                (ULONG)SPS_VERT, 0, 0, 0, 0, hwndFrame,
                WinWindowFromID(hwndFrame, FID_MINMAX),
                ID_VERTSPLITBAR, NULL, NULL) == NULL)
            return (FALSE);

        hwndNewClient = WinCreateWindow(hwndFrame, szDocClass, "",
                0L, 0, 0, 0, 0, hwndFrame,
                WinWindowFromID(hwndFrame, FID_CLIENT),
                ID_CLIENT2, NULL, NULL);
        if (hwndNewClient == NULL)
            return (FALSE);

        /*
         * Link ID_CLIENT2 into npdoc's view list.
         */
        npviewNew = NPVIEWFROMCLIENT(hwndNewClient);
        npview->npviewNext = npviewNew;
        npview = npviewNew;
    }

    if (fsStyle & DS_HORZSPLITBAR) {
        if (WinCreateWindow(hwndFrame, WC_SCROLLBAR, "",
                SBS_VERT, 0, 0, 0, 0, hwndFrame,
                WinWindowFromID(hwndFrame, FID_VERTSCROLL),
                ID_VERTSCROLL2, NULL, NULL) == NULL)
            return (FALSE);

        if (WinCreateWindow(hwndFrame, szSplitbarClass, "",
                (ULONG)SPS_HORZ, 0, 0, 0, 0, hwndFrame,
                WinWindowFromID(hwndFrame, FID_MINMAX),
                ID_HORZSPLITBAR, NULL, NULL) == NULL)
            return (FALSE);

        /*
         * If we're split vertically as well, then we want to
         * insert ID_CLIENT3 behind ID_CLIENT2 and create
         * ID_CLIENT4 behind CLIENT3, otherwise we just create
         * ID_CLIENT3 behind FID_CLIENT.
         */
        if (fsStyle & DS_VERTSPLITBAR) {
            hwndNewClient = WinCreateWindow(hwndFrame, szDocClass, "",
                    0L, 0, 0, 0, 0, hwndFrame,
                    WinWindowFromID(hwndFrame, ID_CLIENT2),
                    ID_CLIENT3, NULL, NULL);
            if (hwndNewClient == NULL)
                return (FALSE);

            /*
             * Link ID_CLIENT3 into npdoc's view list.
             */
            npviewNew = NPVIEWFROMCLIENT(hwndNewClient);
            npview->npviewNext = npviewNew;
            npview = npviewNew;

            hwndNewClient = WinCreateWindow(hwndFrame, szDocClass, "",
                    0L, 0, 0, 0, 0, hwndFrame,
                    WinWindowFromID(hwndFrame, ID_CLIENT3),
                    ID_CLIENT4, NULL, NULL);
            if (hwndNewClient == NULL)
                return (FALSE);

            /*
             * Link ID_CLIENT4 into npdoc's view list.
             */
            npviewNew = NPVIEWFROMCLIENT(hwndNewClient);
            npview->npviewNext = npviewNew;
            npview = npviewNew;

        } else {
            hwndNewClient = WinCreateWindow(hwndFrame, szDocClass, "",
                    0L, 0, 0, 0, 0, hwndFrame,
                    WinWindowFromID(hwndFrame, FID_CLIENT),
                    ID_CLIENT3, NULL, NULL);
            if (hwndNewClient == NULL)
                return (FALSE);

            /*
             * Link ID_CLIENT3 into npdoc's view list.
             */
            npviewNew = NPVIEWFROMCLIENT(hwndNewClient);
            npview->npviewNext = npviewNew;
            npview = npviewNew;
        }
    }

    return (TRUE);
}


HWND QueryBotLeftClient(HWND hwndFrame, NPDOC npdoc)
{
    if (npdoc->fs & DF_SPLITHORZ) {
        /*
         * If there isn't enough room to split
         * then only the main client will be
         * visible so we'll use it.
         */
        if (WinIsWindowVisible(WinWindowFromID(hwndFrame, ID_HORZSPLITBAR)))
            return (WinWindowFromID(hwndFrame, ID_CLIENT3));
        else
            return (WinWindowFromID(hwndFrame, FID_CLIENT));

    } else {
        return (WinWindowFromID(hwndFrame, FID_CLIENT));
    }
}



VOID TrackSplitbars(HWND hwndClient, register USHORT fsTrack, SHORT xMouse,
        SHORT yMouse)
{
    SHORT x, y;
    SHORT xNew, yNew;
    HPS hps;
    RECTL rclClient, rclClientScreen;
    QMSG qmsg;
    AREABUNDLE abnd;
    HWND hwndFrame, hwndOrigin;
    register NPDOC npdoc;
    POINTL ptlPointer;

    hwndFrame = WinQueryWindow(hwndClient, QW_PARENT, FALSE);

    WinLockWindowUpdate(HWND_DESKTOP, hwndFrame);

    /*
     * If we're split horizontally then we want the
     * origin to be a ID_CLIENT3's origin, otherwise
     * we want FID_CLIENT's origin.
     */
    npdoc = NPDOCFROMCLIENT(hwndClient);
    hwndOrigin = QueryBotLeftClient(hwndFrame, npdoc);

    /*
     * We use WinCalcFrameRect() to get the client rectangle
     * rather than WinQueryWindowRect() because the window
     * might already be split, so hwndClient's rectangle wouldn't
     * necessarily include the area we want.
     */
    WinQueryWindowRect(hwndFrame, &rclClient);

    /*
     * Map rectangle to screen coordinates because
     * WinCalcFrameRect() will byte-align the client.
     */
    WinMapWindowPoints(hwndFrame, HWND_DESKTOP, (PPOINTL)&rclClient, 2);
    WinCalcFrameRect(hwndFrame, &rclClient, TRUE);

    WinCopyRect(NULL, &rclClientScreen, &rclClient);
    WinMapWindowPoints(HWND_DESKTOP, hwndOrigin, (PPOINTL)&rclClient, 2);

    WinSetCapture(HWND_DESKTOP, hwndOrigin);

    /*
     * We get a PSF_PARENTCLIP PS so we can draw the
     * splitbars through the other client windows,
     * but still get the PS origin we want.
     */
    hps = WinGetClipPS(hwndOrigin, NULL,
            PSF_PARENTCLIP | PSF_LOCKWINDOWUPDATE);

    abnd.usSymbol = PATSYM_HALFTONE;
    abnd.lColor = CLR_TRUE;
    abnd.lBackColor = CLR_FALSE;
    GpiSetAttrs(hps, PRIM_AREA, ABB_SYMBOL | ABB_COLOR | ABB_BACK_COLOR, 0L,
            (PBUNDLE)&abnd);

    if (fsTrack & SPS_HORZ) {
        if (npdoc->fs & DF_SPLITHORZ) {
            y = npdoc->cyHorzSplitPos + (cyHorzSplitbar / 2);
        } else if (npdoc->fs & DF_HSPLITOVERFLOW) {
            y = 0;
            /*
             * Clear the overflow flag since the
             * user can't track to an overflown
             * position.
             */
            npdoc->fs &= ~DF_HSPLITOVERFLOW;
        } else {
            y = (SHORT)rclClient.yTop - (cyHorzSplitbar / 2);
        }
    }

    if (fsTrack & SPS_VERT) {
        if (npdoc->fs & DF_SPLITVERT) {
            x = npdoc->cxVertSplitPos + (cxVertSplitbar / 2);
        } else if (npdoc->fs & DF_VSPLITOVERFLOW) {
            x = (SHORT)rclClient.xRight - cxVertSplitbar;
            /*
             * Clear the overflow flag since the
             * user can't track to an overflown
             * position.
             */
            npdoc->fs &= ~DF_VSPLITOVERFLOW;
        } else {
            x = (cxVertSplitbar / 2);
        }
    }

    /*
     * If xMouse is -1, then we want to set the pointer
     * position to the position of the splitbars.
     */
    if (xMouse == -1) {
        xMouse = x;
        yMouse = y;
        ptlPointer.x = x;
        ptlPointer.y = y;
        WinMapWindowPoints(hwndOrigin, HWND_DESKTOP, (PPOINTL)&ptlPointer, 1);
        WinSetPointerPos(HWND_DESKTOP, (SHORT)ptlPointer.x, (SHORT)ptlPointer.y);
    }

    DrawTrackRects(hps, &rclClient, x, y, fsTrack);

    while (WinGetMsg(NULL, (PQMSG)&qmsg, NULL, NULL, NULL)) {
        switch (qmsg.msg) {

        case WM_BUTTON1UP:
            DrawTrackRects(hps, &rclClient, x, y, fsTrack);
            WinLockWindowUpdate(HWND_DESKTOP, NULL);
            SetSplitbarPos(hwndClient, &rclClient, x, y, fsTrack);
            goto exit_no_unlock;

        case WM_CHAR:
            if ((((USHORT) (ULONG) qmsg.mp1 &
                    (KC_KEYUP | KC_DEADKEY | KC_COMPOSITE | KC_INVALIDCOMP))
                    == 0) && ((USHORT) (ULONG) qmsg.mp1 & KC_VIRTUALKEY)) {

                WinQueryPointerPos(HWND_DESKTOP, (PPOINTL)&ptlPointer);
                switch (SHORT2FROMMP(qmsg.mp2)) {

                case VK_UP:
                    ptlPointer.y += (cyHorzSplitbar * 2);
                    break;

                case VK_DOWN:
                    ptlPointer.y -= (cyHorzSplitbar * 2);
                    break;

                case VK_RIGHT:
                    ptlPointer.x += (cxVertSplitbar * 2);
                    break;

                case VK_LEFT:
                    ptlPointer.x -= (cxVertSplitbar * 2);
                    break;

                case VK_ENTER:
                case VK_NEWLINE:
                    DrawTrackRects(hps, &rclClient, x, y, SPS_HORZ | SPS_VERT);
                    WinLockWindowUpdate(HWND_DESKTOP, NULL);
                    SetSplitbarPos(hwndClient, &rclClient, x, y,
                            SPS_HORZ | SPS_VERT);
                    goto exit_no_unlock;

                case VK_ESC:
                    DrawTrackRects(hps, &rclClient, x, y, SPS_HORZ | SPS_VERT);
                    goto exit;
                }
                if ((SHORT)ptlPointer.x < (SHORT)rclClientScreen.xLeft)
                    ptlPointer.x = rclClientScreen.xLeft;
                else if ((SHORT)ptlPointer.x > (SHORT)rclClientScreen.xRight)
                    ptlPointer.x = rclClientScreen.xRight;

                if ((SHORT)ptlPointer.y < (SHORT)rclClientScreen.yBottom)
                    ptlPointer.y = rclClientScreen.yBottom;
                else if ((SHORT)ptlPointer.y > (SHORT)rclClientScreen.yTop)
                    ptlPointer.y = rclClientScreen.yTop;

                WinSetPointerPos(HWND_DESKTOP, (SHORT)ptlPointer.x,
                        (SHORT)ptlPointer.y);
            }
            break;

        case WM_MOUSEMOVE:
            xNew = x + (SHORT1FROMMP(qmsg.mp1) - xMouse);
            xMouse = SHORT1FROMMP(qmsg.mp1);
            yNew = y + (SHORT2FROMMP(qmsg.mp1) - yMouse);
            yMouse = SHORT2FROMMP(qmsg.mp1);

            MoveTrackRects(hps, &rclClient, fsTrack, x, y, xNew, yNew);
            x = xNew;
            y = yNew;
            break;

        default:
            WinDispatchMsg(NULL, (PQMSG)&qmsg);
            break;
        }
    }

exit:
    WinLockWindowUpdate(HWND_DESKTOP, NULL);
exit_no_unlock:
    WinReleasePS(hps);
    WinSetCapture(HWND_DESKTOP, NULL);
}


VOID SetSplitbarPos(HWND hwndClient, PRECTL prclClient, SHORT x, SHORT y,
        USHORT fsTrack)
{
    register NPDOC npdoc;
    register NPVIEW npview1, npview2, npview3, npview4;
    HWND hwndFrame;
    RECTL rclTrack;

    npview1 = (NPVIEW)WinQueryWindowUShort(hwndClient, QWS_USER);
    npdoc = npview1->npdoc;
    hwndFrame = WinQueryWindow(hwndClient, QW_PARENT, FALSE);

    if (npdoc->fsStyle & DS_VERTSPLITBAR) {
        npview2 = (NPVIEW)WinQueryWindowUShort(
                WinWindowFromID(hwndFrame, ID_CLIENT2), QWS_USER);
    }

    if (npdoc->fsStyle & DS_HORZSPLITBAR) {
        npview3 = (NPVIEW)WinQueryWindowUShort(
                WinWindowFromID(hwndFrame, ID_CLIENT3), QWS_USER);
    }

    if (npdoc->fsStyle & (DS_HORZSPLITBAR | DS_VERTSPLITBAR)) {
        npview4 = (NPVIEW)WinQueryWindowUShort(
                WinWindowFromID(hwndFrame, ID_CLIENT4), QWS_USER);
    }

    if (fsTrack & SPS_VERT) {
        CalcTrackRect(prclClient, &rclTrack, x, SPS_VERT);
        npdoc->cxVertSplitPos = (SHORT) rclTrack.xLeft;
        if ((npdoc->cxVertSplitPos < (SHORT) cxVertSplitbar) ||
                (npdoc->cxVertSplitPos >
                ((SHORT)prclClient->xRight - (cxVertSplitbar * 2)))) {
            npdoc->fs &= ~DF_SPLITVERT;
        } else {
            npdoc->fs |= DF_SPLITVERT;

            /*
             * Setup ID_CLIENT2 with the same yOrigin
             * as FID_CLIENT.
             */
            npview2->yOrigin = npview1->yOrigin;

            /*
             * If we were horizontally split already, then
             * set ID_CLIENT4 to the same yOrigin
             * as ID_CLIENT3.
             */
            if (npdoc->fs & DF_SPLITHORZ)
                npview4->yOrigin = npview3->yOrigin;
        }
    }

    if (fsTrack & SPS_HORZ) {
        CalcTrackRect(prclClient, &rclTrack, y, SPS_HORZ);
        npdoc->cyHorzSplitPos = (SHORT)rclTrack.yBottom;
        if ((npdoc->cyHorzSplitPos >
                ((SHORT)prclClient->yTop - (cyHorzSplitbar * 2))) ||
                (npdoc->cyHorzSplitPos < (SHORT) cyHorzSplitbar)) {
            npdoc->fs &= ~DF_SPLITHORZ;
        } else {
            npdoc->fs |= DF_SPLITHORZ;

            /*
             * Setup ID_CLIENT3 with the same xOrigin
             * as FID_CLIENT.
             */
            npview3->xOrigin = npview1->xOrigin;

            /*
             * If we were vertically split already, then
             * set ID_CLIENT4 to the same xOrigin
             * as ID_CLIENT2.
             */
            if (npdoc->fs & DF_SPLITHORZ)
                npview4->xOrigin = npview2->xOrigin;
        }
    }

    WinSendMsg(hwndFrame, WM_UPDATEFRAME, 0L, 0L);
}


VOID CalcTrackRect(PRECTL prclClient, PRECTL prcl, SHORT coord, USHORT fsTrack)
{
    if (fsTrack & SPS_VERT) {
        WinSetRect(NULL, prcl, coord - (cxVertSplitbar / 2), 0,
                coord + (cxVertSplitbar - (cxVertSplitbar / 2)),
                (SHORT)prclClient->yTop);

        if ((SHORT)prcl->xLeft < 0)
            WinOffsetRect(NULL, prcl, (SHORT)-(prcl->xLeft), 0);
        else if ((SHORT)prcl->xRight > (SHORT)prclClient->xRight)
            WinOffsetRect(NULL, prcl,
                    -((SHORT)prcl->xRight - (SHORT)prclClient->xRight), 0);
    } else if (fsTrack & SPS_HORZ) {
        WinSetRect(NULL, prcl, 0, coord - (cyHorzSplitbar / 2),
                (SHORT)prclClient->xRight,
                coord + (cyHorzSplitbar - (cyHorzSplitbar / 2)));

        if ((SHORT)prcl->yBottom < 0)
            WinOffsetRect(NULL, prcl, 0, (SHORT)-(prcl->yBottom));
        else if ((SHORT)prcl->yTop > (SHORT)prclClient->yTop)
            WinOffsetRect(NULL, prcl,
                    0, -((SHORT)prcl->yTop - (SHORT)prclClient->yTop));
    }
}


VOID DrawTrackRects(HPS hps, PRECTL prclClient, SHORT x, SHORT y,
        USHORT fsTrack)
{
    RECTL rclFill;

    VOID CalcTrackRect(PRECTL, PRECTL, SHORT, USHORT);

    if (fsTrack & SPS_VERT) {
        CalcTrackRect(prclClient, (PRECTL)&rclFill, x, SPS_VERT);

        InvertRect(hps, (PRECTL)&rclFill);
    }

    if (fsTrack & SPS_HORZ) {
        CalcTrackRect(prclClient, (PRECTL)&rclFill, y, SPS_HORZ);

        InvertRect(hps, (PRECTL)&rclFill);
    }
}


VOID MoveTrackRects(HPS hps, PRECTL prclClient, USHORT fsTrack, SHORT xOld,
        SHORT yOld, SHORT xNew, SHORT yNew)
{
    RECTL rclOld, rclNew, rclScratch;
    VOID CalcTrackRect(PRECTL, PRECTL, SHORT, USHORT);

    if (fsTrack & SPS_VERT) {
        CalcTrackRect(prclClient, (PRECTL)&rclOld, xOld, SPS_VERT);
        CalcTrackRect(prclClient, (PRECTL)&rclNew, xNew, SPS_VERT);

        if (WinSubtractRect(NULL, &rclScratch, &rclOld, &rclNew))
            InvertRect(hps, (PRECTL)&rclScratch);
        if (WinSubtractRect(NULL, &rclScratch, &rclNew, &rclOld))
            InvertRect(hps, (PRECTL)&rclScratch);
    }

    if (fsTrack & SPS_HORZ) {
        CalcTrackRect(prclClient, (PRECTL)&rclOld, yOld, SPS_HORZ);
        CalcTrackRect(prclClient, (PRECTL)&rclNew, yNew, SPS_HORZ);

        if (WinSubtractRect(NULL, &rclScratch, &rclOld, &rclNew))
            InvertRect(hps, (PRECTL)&rclScratch);
        if (WinSubtractRect(NULL, &rclScratch, &rclNew, &rclOld))
            InvertRect(hps, (PRECTL)&rclScratch);
    }
}


VOID InvertRect(HPS hps, PRECTL prcl)
{
    RECTL rclParm[2];

    WinCopyRect(NULL, (PRECTL)&rclParm[0], (PRECTL)prcl);
    rclParm[1].xLeft = 0;
    rclParm[1].yBottom = 0;
    GpiBitBlt(hps, (HPS)NULL, 3L, (PPOINTL)rclParm, ROP_PATINVERT, 0L);
}


VOID FindSwp(PSWP aswp, register USHORT cswp, USHORT id, PSWP FAR *ppswp)
{
    register i;

    for (i = 0; i < (SHORT) cswp; i++) {
        if (WinQueryWindowUShort(aswp[i].hwnd, QWS_ID) == id) {
            *ppswp = &aswp[i];
            return;
        }
    }
}


/*
 * Set the SWP structure to the passed in parameters.
 *
 * This routine is dependent on the order of elements
 * in the SWP structure.
 */
VOID SetSwpPos(PSWP pswp, HWND hwnd, HWND hwndInsertBehind, SHORT x, SHORT y,
        SHORT cx, SHORT cy, USHORT fs)
{
    /*
	*pswp = *((PSWP)&fs);
    */
    pswp->hwnd			=	hwnd;
    pswp->hwndInsertBehind	=	hwndInsertBehind;
    pswp->x			=	x;
    pswp->y			=	y;
    pswp->cx			=	cx;
    pswp->cy			=	cy;
    pswp->fs			=	fs;
}


VOID HideSwp(PSWP pswp, HWND hwnd, USHORT *piswp)
{
    if (WinIsWindowVisible(hwnd)) {
        SetSwpPos(pswp, hwnd, NULL, 0, 0, 0, 0, SWP_HIDE);
        (*piswp)++;
    }
}
