/***************************************************************************\
* mdidoc.c - MDI application
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

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
#define INCL_WINDIALOGS
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

/* Function Prototypes */
VOID BuildWindowMenu(VOID);
VOID TrackSplitbars(HWND, USHORT, SHORT, SHORT);
MRESULT MDIFormatFrame(HWND hwnd, PSWP aswp, MPARAM mp2);
MRESULT MDIMinMaxFrame(HWND hwnd, MPARAM mp1, MPARAM mp2);

#define	WM_WINDOWPOSCHANGED	0x55

VOID SetMainTitleText(HWND hwndDocFrame)
{
    char szDocTitle[80], szMainTitle[80];

    /*
     * Get the titlebar text for the specified document window.
     */
    WinQueryWindowText(hwndDocFrame, 80, (PSZ)szDocTitle);

    /*
     * Build up the correct text for the main window.
     */
    strcpy(szMainTitle, " - ");
    strcat(szMainTitle, szDocTitle);

    /*
     * Set the main window's titlebar to the new text.
     */
    WinSetWindowText(hwndMDIFrame, szMainTitle);
}


VOID ClearMainTitleText(VOID)
{
    WinSetWindowText(hwndMDIFrame, "");
}


VOID AddToWindowMenu(NPDOC npdocNew)
{
    MENUITEM mi;
    char szItemText[1];
    char szTitleNew[128], szTitleCompare[128];
    register NPDOC npdoc, npdocPrev;
    USHORT usRes;

    cDocs++;

    /* build menuitem structure for this document */
    mi.iPosition = MIT_END;
    mi.afStyle = MIS_TEXT;
    mi.afAttribute = 0;
    mi.id = CMD_WINDOWITEMS + cDocs;
    mi.hwndSubMenu = NULL;
    mi.hItem = NULL;
    szItemText[0] = '\0';

    /*
     * Insert a blank item into the menu.
     */
    WinSendMsg(hwndWindowMenu, MM_INSERTITEM, MPFROMP(&mi),
            MPFROMP(szItemText));

    /*
     * Add the document to the DOCLIST.
     */
    if (npdocFirst == NULL) {
        npdocFirst = npdocNew;

        /*
         * Fill in the DOCLIST for this doc.
         */
        npdocNew->idMI = NULL;
        npdocNew->npdocNext = NULL;

    } else {

        /*
         * Add the item in the correct
         * sorted location.
         */
        npdoc = npdocFirst;
        npdocPrev = NULL;

        while (npdoc != NULL) {
            WinQueryWindowText(npdoc->hwndFrame, 128, szTitleCompare);
            WinQueryWindowText(npdocNew->hwndFrame, 128, szTitleNew);
            WinUpper(NULL, NULL, NULL, szTitleCompare);
            WinUpper(NULL, NULL, NULL, szTitleNew);

            usRes = WinCompareStrings(NULL, NULL, NULL, szTitleNew,
                    szTitleCompare, NULL);

            if (usRes == WCS_LT) {
                if (npdocPrev == NULL) {
                    npdocFirst = npdocNew;
                    npdocFirst->npdocNext = npdoc;
                } else {
                    npdocPrev->npdocNext = npdocNew;
                    npdocNew->npdocNext = npdoc;
                }

                /*
                 * Fill in the DOC for this document.
                 */
                npdocNew->idMI = NULL;
                break;
            } else if (npdoc->npdocNext == NULL) {
                /*
                 * If we've made it to the end,
                 * just tack it on here.
                 */
                npdoc->npdocNext =  npdocNew;

                /*
                 * Fill in the DOC for this document.
                 */
                npdocNew->idMI = NULL;
                npdocNew->npdocNext = NULL;
            }

            npdocPrev = npdoc;
            npdoc = npdoc->npdocNext;
        }
    }

    BuildWindowMenu();
}


VOID RemoveFromWindowMenu(NPDOC npdocRemove)
{
    register NPDOC npdoc, npdocPrev;

    VOID BuildWindowMenu(VOID);

    /*
     * If the window menu is gone no need to 
     * do any of this stuff.  This can happen
     * when the MDI app is closing.
     */
    if (!WinIsWindow(hab, hwndWindowMenu))
        return;

    /* Delete item from menu */
    WinSendMsg(hwndWindowMenu, MM_DELETEITEM,
            MPFROM2SHORT(npdocRemove->idMI, FALSE), NULL);

    /*
     * Remove the DOC from the linked-list.
     */
    if (npdocFirst != NULL) {

        /*
         * Initialize these for our while loop.
         */
        npdoc = npdocFirst;
        npdocPrev = NULL;

        while (npdoc != NULL) {

            /*
             * If we've found the element unlink
             * it from the list.
             */
            if (npdoc == npdocRemove) {

                /*
                 * Unlink this document.
                 */
                if (npdocPrev != NULL)
                    npdocPrev->npdocNext = npdoc->npdocNext;
                else
                    npdocFirst = npdoc->npdocNext;

                /*
                 * Break out of the while loop,
                 * we're done here.
                 */
                break;
            }
            npdocPrev = npdoc;
            npdoc = npdoc->npdocNext;
        }
    }

    BuildWindowMenu();
}


VOID BuildWindowMenu(VOID)
{
    register NPDOC npdoc;
    char szDocTitle[80];
    char szItemText[80];
    USHORT idMI, posMI, cWindows;

    /*
     * Get the position of the first
     * window in the Window menu.
     */
    posMI = (USHORT)WinSendMsg(hwndWindowMenu, MM_ITEMPOSITIONFROMID,
            MPFROM2SHORT(CMD_WINDOWITEMS, TRUE), NULL);
    posMI++;

    npdoc = npdocFirst;
    cWindows = 1;
    while (npdoc != NULL) {

        /*
         * Fill in the menuitem ID.
         */
        idMI = (USHORT)WinSendMsg(hwndWindowMenu, MM_ITEMIDFROMPOSITION,
                MPFROMSHORT(posMI), NULL);
        npdoc->idMI = idMI;

        /*
         * Build the item string.
         */
        szItemText[0] = '~';
        itoa(cWindows, szItemText + 1, 10);
        strcat(szItemText, " ");
        WinQueryWindowText(npdoc->hwndFrame, 80, (PSZ)szDocTitle);
        strcat(szItemText, szDocTitle);

        /*
         * Set the item text for the menuitem.
         */
        WinSendMsg(hwndWindowMenu, MM_SETITEMTEXT, MPFROMSHORT(idMI),
                MPFROMP(szItemText));

        /*
         * Clear out the previous attributes.
         */
        WinSendMsg(hwndWindowMenu, MM_SETITEMATTR, MPFROM2SHORT(idMI, FALSE),
                MPFROM2SHORT(MIA_CHECKED, 0));

        posMI++;
        cWindows++;
        npdoc = npdoc->npdocNext;
    }
}


NPDOC MdiNewDocument(USHORT fsStyle, PSZ pszClassName)
{
    ULONG ctlData, flStyle;
    HWND hwndNewFrame, hwndNewClient, hwndSysMenu;
    register NPDOC npdocNew;
    register NPVIEW npview;
    char szDocTitle[80], szDocNumber[4];

    usDocNumber++;

    /*
     * Setup the window's titlebar text.
     */
    itoa(usDocNumber, szDocNumber, 10);
    strcpy(szDocTitle, "Untitled");
    strcat(szDocTitle, szDocNumber);

    ctlData = FCF_TITLEBAR | FCF_MINMAX | FCF_SIZEBORDER |
            FCF_VERTSCROLL | FCF_HORZSCROLL;

    hwndNewFrame = WinCreateStdWindow(hwndMDI,
            FS_ICON | FS_ACCELTABLE,
            (VOID FAR *)&ctlData,
            pszClassName, szDocTitle,
            WS_VISIBLE,
            (HMODULE)NULL, IDR_MDIDOC,
            (HWND FAR *)&hwndNewClient);

    npdocNew = (NPDOC)WinAllocMem(hHeap, sizeof(DOC));
    if (npdocNew == NULL)
        return (FALSE);

    npdocNew->hwndFrame = hwndNewFrame;
    npdocNew->cxVertSplitPos = 0;
    npdocNew->cyHorzSplitPos = 0;
    npdocNew->fs = 0;
    npdocNew->fsStyle = fsStyle;

    npview = NPVIEWFROMCLIENT(hwndNewClient);
    npview->npdoc = npdocNew;

    /*
     * Link in the VIEW for FID_CLIENT since we
     * know it exists.
     */
    npdocNew->npviewFirst = npview;

    /*
     * Create the 'splitbar' controls for the frame.
     * This includes the splitbar and the extra client
     * and scrollbar windows.  This routine will also
     * link the appropriate VIEWs into the DOC structure.
     */
    if (fsStyle & (DS_HORZSPLITBAR | DS_VERTSPLITBAR)) {
        if (CreateSplitbarWindows(hwndNewFrame, npdocNew) == FALSE) {
            WinDestroyWindow(hwndNewFrame);
            return (FALSE);
        }
    }

    /*
     * Load in the document window's system menu.
     */
    hwndSysMenu = WinLoadMenu(hwndNewFrame, (HMODULE)NULL, IDM_DOCSYSMENU);

    /*
     * Make it look like a normal system menu to the frame manager so
     * gets formatted correctly etc.
     */
    flStyle = WinQueryWindowULong(hwndSysMenu, QWL_STYLE);
    WinSetWindowULong(hwndSysMenu, QWL_STYLE, flStyle | MS_TITLEBUTTON);
    WinSetWindowUShort(hwndSysMenu, QWS_ID, FID_SYSMENU);

    /*
     * Set the bitmap to the SBMP_CHILDSYSMENU bitmap.
     */
    WinSendMsg(hwndSysMenu, MM_SETITEMHANDLE, (MPARAM)SC_DOCSYSMENU,
            (MPARAM)hbmChildSysMenu);

    /*
     * Set the menu window handles in the DOC structure so these
     * controls can be hidden/shown at the correct time.
     */
    npdocNew->hwndSysMenu = hwndSysMenu;
    npdocNew->hwndMinmax = WinWindowFromID(hwndNewFrame, FID_MINMAX);

    /*
     * Subclass the frame so we can handle the accelerators
     * and other MDI stuff.
     */
    pfnFrameWndProc = WinSubclassWindow(hwndNewFrame,
            (PFNWP)DocFrameWndProc);

    /*
     * Add the window to the Window menu.
     */
    AddToWindowMenu(npdocNew);

    return (npdocNew);
}


VOID MDISetInitialDocPos(HWND hwndNewFrame)
{
    WinSetWindowPos(hwndNewFrame, NULL, xNextNewDoc, yNextNewDoc,
            cxNewDoc, cyNewDoc, SWP_MOVE | SWP_SIZE | SWP_SHOW);

    /*
     * Insert logic to change xNextNewDoc/yNextNewDoc and cxNewDoc/cyNewDoc.
     */
}


VOID AddAabSysMenu(VOID)
{
    if (fAabSysMenu == FALSE) {
        WinSendMsg(hwndMainMenu, MM_INSERTITEM, MPFROMP(&miAabSysMenu),
                (MPARAM)NULL);
        fAabSysMenu = TRUE;
    }
}


VOID RemoveAabSysMenu(VOID)
{
    if (fAabSysMenu == TRUE) {
        WinSendMsg(hwndMainMenu, MM_REMOVEITEM,
                MPFROMSHORT(IDM_AABDOCSYSMENU), (MPARAM)FALSE);
        fAabSysMenu = FALSE;
    }
}


BOOL MDICreate(HWND hwndClient)
{
    register NPVIEW npview;

    /*
     * Allocate the DOC structure for this window and stick it in
     * the window structure of the client.
     */
    npview = (NPVIEW)WinAllocMem(hHeap, sizeof(VIEW));
    if (npview == NULL)
        return (FALSE);

    npview->xOrigin = 0;
    npview->yOrigin = 0;
    npview->fs = 0;
    npview->npviewNext = NULL;
    npview->hwndClient = hwndClient;

    WinSetWindowUShort(hwndClient, QWS_USER, (USHORT)npview);

    return (TRUE);
}


VOID MDIActivate(HWND hwndClient, BOOL fActivate)
{
    HWND hwndFrame;
    register NPDOC npdoc;

    hwndFrame = WinQueryWindow(hwndClient, QW_PARENT, FALSE);

    /*
     * If our active status is changing, show/hide the AAB Sysmenu, check
     * the correct item on the Window menu, and show/hide the titlebar
     * frame controls.
     */
    npdoc = NPDOCFROMCLIENT(hwndClient);
    if (fActivate != FALSE) {

        hwndActiveDoc = hwndFrame;
        if (WinQueryWindowULong(hwndFrame, QWL_STYLE) & WS_MAXIMIZED)
            AddAabSysMenu();
        else
            RemoveAabSysMenu();

        /*
         * Check the appropriate item on the Window menu.
         */
        WinSendMsg(hwndWindowMenu, MM_SETITEMATTR,
                MPFROM2SHORT(npdoc->idMI, FALSE),
                MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));

        /*
         * Show the titlebar frame controls.
         */
        WinSetParent(npdoc->hwndSysMenu, hwndFrame, FALSE);
        WinSetParent(npdoc->hwndMinmax, hwndFrame, FALSE);
        WinSendMsg(hwndFrame, WM_UPDATEFRAME, 0L, 0L);

    } else {
        /*
         * Uncheck the appropriate item on the Window menu.
         */
        WinSendMsg(hwndWindowMenu, MM_SETITEMATTR,
                MPFROM2SHORT(npdoc->idMI, FALSE),
                MPFROM2SHORT(MIA_CHECKED, 0));
        hwndActiveDoc = NULL;

        /*
         * Hide the titlebar frame controls.
         */
        WinSetParent(npdoc->hwndSysMenu, HWND_OBJECT, FALSE);
        WinSetParent(npdoc->hwndMinmax, HWND_OBJECT, FALSE);
        WinSendMsg(hwndFrame, WM_UPDATEFRAME, 0L, 0L);
    }
}


VOID MDIDestroy(HWND hwndClient)
{
    WinFreeMem(hHeap, (NPBYTE)WinQueryWindowUShort(hwndClient, QWS_USER),
            sizeof(VIEW));
}


VOID MDIClose(HWND hwndClient)
{
    WinDestroyWindow(WinQueryWindow(hwndClient, QW_PARENT, FALSE));
}


MRESULT CALLBACK DocFrameWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    MRESULT mres;
    USHORT cFrameCtls;
    HWND hwndParent, hwndClient;
    register NPDOC npdoc;
    RECTL rclClient;

    switch (msg) {

    case WM_SYSCOMMAND:
        if (SHORT1FROMMP(mp1) == SC_SPLIT) {
            WinSetPointer(HWND_DESKTOP, hptrHVSplit);
            TrackSplitbars(WinWindowFromID(hwnd, FID_CLIENT),
                    SPS_HORZ | SPS_VERT, -1, -1);
            WinSetPointer(HWND_DESKTOP, hptrArrow);
        } else if (SHORT1FROMMP(mp2) == CMDSRC_ACCELERATOR) {

            /*
             * If the command was sent because of an accelerator
             * we need to see if it goes to the document or the main
             * frame window.
             */
            if ((WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000)) {

                /*
                 * If the control key is down we'll send it
                 * to the document's frame since that means
                 * it's either ctl-esc or one of the document
                 * window's accelerators.
                 */
                return (*pfnFrameWndProc)(hwnd, msg, mp1, mp2);
            } else if (SHORT1FROMMP(mp1) == SC_DOCSYSMENU) {

                /*
                 * If the window is maximized then we want
                 * to pull down the system menu on the main
                 * menu bar.
                 */
                if ((WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MAXIMIZED) &&
                        (SHORT1FROMMP(mp1) == SC_DOCSYSMENU)) {
                    WinPostMsg(miAabSysMenu.hwndSubMenu, MM_STARTMENUMODE,
                            MPFROM2SHORT(TRUE, FALSE), 0L);
                    return (0L);
                } else {
                    WinPostMsg(WinWindowFromID(hwnd, FID_SYSMENU),
                            MM_STARTMENUMODE, MPFROM2SHORT(TRUE, FALSE), 0L);
                }
            } else {
                /*
                 * Control isn't down so send it the main
                 * frame window.
                 */
                return WinSendMsg(hwndMDIFrame, msg, mp1, mp2);
            }
        } else {
            /*
             * WM_SYSCOMMAND not caused by an accelerator
             * so hwnd is the window we want to send the
             * message to.
             */
            return (*pfnFrameWndProc)(hwnd, msg, mp1, mp2);
        }
        break;

    case WM_NEXTMENU:
        /*
         * Connect child sysmenu with application menus for cursor motion.
         * Only return a value if we're the System Menu.  We don't want the
         * MinMax menu or any others that might be there to freak out.
         */
        if (WinQueryWindowUShort(HWNDFROMMP(mp1), QWS_ID) == FID_SYSMENU) {
            if (SHORT1FROMMP(mp2)) {
                return (MRESULT)hwndSysMenu;
            } else {
                return (MRESULT)hwndAppMenu;
            }
        } else {
            return (0L);
        }
        break;

    case WM_MINMAXFRAME:
        return (MDIMinMaxFrame(hwnd, mp1, mp2));
        break;

    case WM_WINDOWPOSCHANGED: 

#define aswp    ((PSWP)mp1)

        npdoc = NPDOCFROMCLIENT(WinWindowFromID(hwnd, FID_CLIENT));
        if (((PSWP)mp1)->fs & SWP_SIZE) {

            if (npdoc->fs & (DF_SPLITHORZ | DF_HSPLITOVERFLOW)) {
                /*
                 * If we were sized then adjust the horizontal splitbar
                 * to be top-aligned.
                 */
                npdoc->cyHorzSplitPos += (aswp[0].cy - aswp[1].cy);

                /*
                 * Set or clear the DF_HSPLITOVERFLOW and DF_SPLITHORZ flags.
                 */
                if (npdoc->cyHorzSplitPos < 0) {
                    npdoc->fs |= DF_HSPLITOVERFLOW;
                    npdoc->fs &= ~DF_SPLITHORZ;
                } else {
                    npdoc->fs &= ~DF_HSPLITOVERFLOW;
                    npdoc->fs |= DF_SPLITHORZ;
                }
            }

            if (npdoc->fs & (DF_SPLITVERT | DF_VSPLITOVERFLOW)) {

                WinQueryWindowRect(hwnd, &rclClient);
                WinCalcFrameRect(hwnd, &rclClient, TRUE);

                /*
                 * Set or clear the DF_VSPLITOVERFLOW and DF_SPLITVERT flags.
                 */
                if (npdoc->cxVertSplitPos >
                        (((SHORT)rclClient.xRight - (SHORT)rclClient.xLeft) -
                        cxVertSplitbar)) {
                    npdoc->fs |= DF_VSPLITOVERFLOW;
                    npdoc->fs &= ~DF_SPLITVERT;
                } else {
                    npdoc->fs &= ~DF_VSPLITOVERFLOW;
                    npdoc->fs |= DF_SPLITVERT;
                }
            }
        }

#undef aswp

        return (*pfnFrameWndProc)(hwnd, msg, mp1, mp2);
        break;

    case WM_CALCVALIDRECTS:

        /*
         * We do our own WM_CALCVALIDRECTS processing
         * because the frame manager uses the window
         * rectangle of FID_CLIENT, which in our case
         * might be smaller than the 'client area' due
         * to window splitting.
         */

#define prclOld ((PRECTL)&(((PRECTL)mp1)[0]))
#define prclNew ((PRECTL)&(((PRECTL)mp1)[1]))

        /*
         * Calculate the client rectangle of hwnd in its
         * parent's coordinates.
         */
        WinQueryWindowRect(hwnd, (PRECTL)prclOld);
        hwndParent = WinQueryWindow(hwnd, QW_PARENT, FALSE);
        WinMapWindowPoints(hwnd, hwndParent, (PPOINTL)prclOld, 2);
        WinCalcFrameRect(hwnd, (PRECTL)prclOld, TRUE);

        /*
         * Calculate the client rect for the
         * destination of the frame window.
         */
        WinCalcFrameRect(hwnd, (PRECTL)prclNew, TRUE);

        /*
         * Top align destination bits.
         */
        prclNew->yBottom += (prclNew->yTop - prclNew->yBottom) -
                            (prclOld->yTop - prclOld->yBottom);

#undef prclOld
#undef prclNew

        /*
         * Return 0 since we've already aligned the bits.
         */
        return(0);
        break;

    case WM_QUERYFRAMECTLCOUNT:
        cFrameCtls = (int)(*pfnFrameWndProc)(hwnd, WM_QUERYFRAMECTLCOUNT, mp1, mp2);

        /*
         * Max number of additional frame controls is seven
         * Two splitbars, two additional scrollbars, and three
         * additional client windows.  Throw in 3 more just to
         * be sure we don't trash memory
         */
        return (MRFROMSHORT(cFrameCtls + 7 + 3));

    case WM_FORMATFRAME:
        return (MDIFormatFrame(hwnd, ((PSWP)PVOIDFROMMP(mp1)), mp2));
        break;

    case WM_BUTTON1UP:
        mres =  (*pfnFrameWndProc)(hwnd, msg, mp1, mp2);

        /*
         * If we're minimized we need to do activation ourself
         * and bring up the system menu.
         */
        if (!(WinQueryWindowUShort(hwnd, QWS_FLAGS) & FF_ACTIVE)) {

            /*
             * Only do this if we're minimized.
             */
            if (WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MINIMIZED) {

                WinSetActiveWindow(HWND_DESKTOP, hwnd);

                /*
                 * Bring up the system menu if there is one.
                 */
                WinSendDlgItemMsg(hwnd, FID_SYSMENU, MM_STARTMENUMODE,
                        MPFROMSHORT(TRUE), 0L);
            }
        }
        break;

    case WM_DESTROY:
        /*
         * If this document is maximized, remove the AabSysMenu.
         */
        if (WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MAXIMIZED)
            RemoveAabSysMenu();

        hwndClient = WinWindowFromID(hwnd, FID_CLIENT);
        npdoc = NPDOCFROMCLIENT(hwndClient);

        /*
         * Make sure these windows are on the frame so they
         * get destroyed.
         */
        WinSetParent(npdoc->hwndSysMenu, hwnd, FALSE);
        WinSetParent(npdoc->hwndMinmax, hwnd, FALSE);

        RemoveFromWindowMenu(npdoc);

        WinFreeMem(hHeap, (NPBYTE)npdoc, sizeof(DOC));

        return (*pfnFrameWndProc)(hwnd, msg, mp1, mp2);

    default:
        return (*pfnFrameWndProc)(hwnd, msg, mp1, mp2);
    }
}

MRESULT MDIMinMaxFrame(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    PSWP pswp;

    pswp = (PSWP)PVOIDFROMMP(mp1);
    if ((pswp->fs & SWP_MAXIMIZE) &&
            ((WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MAXIMIZED) == 0L)) {
        pswp->cy += cyTitlebar;
        AddAabSysMenu();
        SetMainTitleText(hwnd);
    } else if (((pswp->fs & SWP_RESTORE) || (pswp->fs & SWP_MINIMIZE)) &&
            (WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MAXIMIZED)) {
        RemoveAabSysMenu();
        ClearMainTitleText();
    }
    return (*pfnFrameWndProc)(hwnd, WM_MINMAXFRAME, mp1, mp2);
}

MRESULT MDIFormatFrame(HWND hwnd, PSWP aswp, MPARAM mp2)
{
    SWP swpClient;
    PSWP pswpHScroll, pswpVScroll;
    USHORT chwnd, iswpNext;
    register NPDOC npdoc;
    SHORT cyHorzSplitPos, cxVertSplitPos;

    iswpNext = chwnd = (USHORT)(*pfnFrameWndProc)(hwnd, WM_FORMATFRAME,
                            aswp, mp2);

    FindSwp(aswp, chwnd, FID_HORZSCROLL, (PSWP FAR *)&pswpHScroll);
    FindSwp(aswp, chwnd, FID_VERTSCROLL, (PSWP FAR *)&pswpVScroll);

    npdoc = NPDOCFROMCLIENT(aswp[chwnd - 1].hwnd);

    cyHorzSplitPos = npdoc->cyHorzSplitPos;
    cxVertSplitPos = npdoc->cxVertSplitPos;

    /*
     * Save the client rectangle away because we
     * want to do thing based on the original
     * client rectangle as well as move the
     * client's ordering in the SWP list behind
     * the other client windows.
     */
    swpClient = aswp[chwnd - 1];

    /*
     * Start from the client window's SWP
     * since we're going to move it to
     * the end.
     */
    iswpNext = (chwnd - 1);

    if (npdoc->fsStyle & DS_VERTSPLITBAR) {

        /*
         * If the horizontal scrollbar is being hidden
         * then we certainly don't need to be around...
         */
        if (pswpHScroll->fs & SWP_HIDE) {

            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT2),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSCROLL2),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_VERTSPLITBAR),
                    &iswpNext);

        } else if (npdoc->fs & DF_VSPLITOVERFLOW) {

            /*
             * First hide CLIENT3 and VERTSCROLL2
             * since we know they're going away.
             */
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT2),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSCROLL2),
                    &iswpNext);

            /*
             * Hide CLIENT4 since it won't be needed for now.
             */
            if (npdoc->fsStyle & DS_HORZSPLITBAR) {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT4),
                        &iswpNext);
            }

            /*
             * If there isn't any room for even the hidden
             * representation then get rid of it...
             */
            if (pswpHScroll->cx < (cxVertSplitbar * 3)) {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_VERTSPLITBAR),
                        &iswpNext);
            } else {
                SetSwpPos(&aswp[iswpNext++],
                        WinWindowFromID(hwnd, ID_VERTSPLITBAR), NULL,
                        pswpHScroll->x + pswpHScroll->cx - cxVertSplitbar,
                        pswpHScroll->y + cyBorder, cxVertSplitbar,
                        pswpHScroll->cy - cyBorder, pswpHScroll->fs);

                pswpHScroll->cx -= cxVertSplitbar;
            }

        } else if (npdoc->fs & DF_SPLITVERT) {

            /*
             * Format the client windows.
             *
             * If we're split horzintally as well then we
             * need to show the fourth client.
             */

            /*
             * If we're split horizontally and there was
             * enough room to be split then format
             * the 'fourth' client window.
             */
            if ((npdoc->fs & DF_SPLITHORZ) &&
                    (pswpVScroll->cy > (cyHorzSplitbar * 3))) {
                aswp[iswpNext].hwnd = WinWindowFromID(hwnd, ID_CLIENT4);
                aswp[iswpNext].hwndInsertBehind = NULL;
                aswp[iswpNext].x = swpClient.x +
                        (cxVertSplitPos + cxVertSplitbar);
                aswp[iswpNext].cx = swpClient.cx -
                        (cxVertSplitPos + cxVertSplitbar);
                aswp[iswpNext].y = swpClient.y;
                aswp[iswpNext].cy = cyHorzSplitPos;
                aswp[iswpNext].fs = swpClient.fs | SWP_SHOW;

                iswpNext++;
            }

            aswp[iswpNext].hwnd = WinWindowFromID(hwnd, ID_CLIENT2);
            aswp[iswpNext].hwndInsertBehind = NULL;
            aswp[iswpNext].x = swpClient.x +
                    (cxVertSplitPos + cxVertSplitbar);
            aswp[iswpNext].cx = swpClient.cx -
                    (cxVertSplitPos + cxVertSplitbar);
            /*
             * If we're split horizontally and there was
             * enough room to be split then format
             * the 'second' client window against the
             * 'fourth' client window.
             */
            if ((npdoc->fs & DF_SPLITHORZ) &&
                    (pswpVScroll->cy > (cyHorzSplitbar * 3))) {
                aswp[iswpNext].y = swpClient.y +
                        (cyHorzSplitPos + cyHorzSplitbar);
                aswp[iswpNext].cy = swpClient.cy -
                        (cyHorzSplitPos + cyHorzSplitbar);
            } else {
                aswp[iswpNext].y = swpClient.y;
                aswp[iswpNext].cy = swpClient.cy;
            }
            aswp[iswpNext].fs = swpClient.fs | SWP_SHOW;

            iswpNext++;

        } else {
            /*
             * If we're not split then format the window with the
             * splitbar to the left of the horizontal scrollbar.
             */
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSCROLL2),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT2),
                    &iswpNext);

            /*
             * Hide ID_CLIENT4 since it won't be needed for now.
             */
            if (npdoc->fsStyle & DS_HORZSPLITBAR) {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT4),
                        &iswpNext);
            }

            aswp[iswpNext].hwnd = WinWindowFromID(hwnd, ID_VERTSPLITBAR);
            aswp[iswpNext].hwndInsertBehind = NULL;
            aswp[iswpNext].x = pswpHScroll->x;
            aswp[iswpNext].y = pswpHScroll->y + cyBorder;
            aswp[iswpNext].cx = cxVertSplitbar;
            aswp[iswpNext].cy = pswpHScroll->cy - cyBorder;
            aswp[iswpNext].fs = pswpHScroll->fs;
            pswpHScroll->x += cxVertSplitbar;
            pswpHScroll->cx -= cxVertSplitbar;

            iswpNext++;
        }
    }

    if (npdoc->fsStyle & DS_HORZSPLITBAR) {
        /*
         * If the horizontal scrollbar is being hidden
         * then we certainly don't need to be around...
         */
        if (pswpVScroll->fs & SWP_HIDE) {

            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT3),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_VERTSCROLL2),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSPLITBAR),
                    &iswpNext);

        } else if (npdoc->fs & DF_HSPLITOVERFLOW) {

            /*
             * First hide CLIENT3 and VERTSCROLL2
             * since we know they're going away.
             */
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT3),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_VERTSCROLL2),
                    &iswpNext);

            /*
             * Hide CLIENT4 since it won't be needed for now.
             */
            if (npdoc->fsStyle & DS_VERTSPLITBAR) {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT4),
                        &iswpNext);
            }

            /*
             * If there isn't any room for even the hidden
             * representation then get rid of it
             */
            if (pswpVScroll->cy < (cyHorzSplitbar * 3)) {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSPLITBAR),
                        &iswpNext);
            } else {
                SetSwpPos(&aswp[iswpNext++],
                        WinWindowFromID(hwnd, ID_HORZSPLITBAR), NULL,
                        pswpVScroll->x, pswpVScroll->y, pswpVScroll->cx - cxBorder,
                        cyHorzSplitbar, pswpVScroll->fs);

                pswpVScroll->y += cyHorzSplitbar;
                pswpVScroll->cy -= cyHorzSplitbar;
            }

        } else if (npdoc->fs & DF_SPLITHORZ) {

            if (pswpVScroll->cy > (cyHorzSplitbar * 3)) {
                /*
                 * Format the client windows.
                 */
                SetSwpPos(&aswp[iswpNext++],
                        WinWindowFromID(hwnd, ID_CLIENT3), NULL,
                        swpClient.x, swpClient.y,
                        /*
                         * If we're split vertically then format
                         * the client against the vertical splitbar
                         * as well as the horzontal splitbar.
                         */
                        (npdoc->fs & DF_SPLITVERT) ? cxVertSplitPos : swpClient.cx,
                        cyHorzSplitPos, swpClient.fs | SWP_SHOW);

            } else {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT3),
                        &iswpNext);

                /*
                 * Hide CLIENT4 since it won't be needed for now.
                 */
                if (npdoc->fsStyle & DS_VERTSPLITBAR) {
                    HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT4),
                            &iswpNext);
                }
            }

        } else {
            /*
             * If we're not split then format the window with the
             * splitbar at the top of the vertical scrollbar.
             */
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_VERTSCROLL2),
                    &iswpNext);

            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT3),
                    &iswpNext);

            if (npdoc->fsStyle & DS_VERTSPLITBAR) {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT4),
                    &iswpNext);
            }

            if (pswpVScroll->cy > (cyHorzSplitbar * 3)) {
                SetSwpPos(&aswp[iswpNext++],
                        WinWindowFromID(hwnd, ID_HORZSPLITBAR), NULL,
                        pswpVScroll->x, pswpVScroll->y + pswpVScroll->cy -
                            cyHorzSplitbar,
                        pswpVScroll->cx - cxBorder, cyHorzSplitbar, pswpVScroll->fs);

                pswpVScroll->cy -= cyHorzSplitbar;
            } else {
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSPLITBAR),
                        &iswpNext);
            }
        }
    }

    /*
     * Now format the original client window.
     */
    aswp[iswpNext] = swpClient;

    if (npdoc->fs & (DF_SPLITHORZ | DF_SPLITVERT)) {

        /*
         * Adjust the main client window for the splitbars.
         */
        if (npdoc->fs & DF_SPLITVERT) {
            aswp[iswpNext].cx = cxVertSplitPos;
        }

        if ((npdoc->fs & DF_SPLITHORZ) &&
                (pswpVScroll->cy > (cyHorzSplitbar * 3))) {
            aswp[iswpNext].y += (cyHorzSplitPos + cyHorzSplitbar);
            aswp[iswpNext].cy -= (cyHorzSplitPos + cyHorzSplitbar);
        }
    }

    iswpNext++;

    if (npdoc->fs & DF_SPLITHORZ) {
        /*
         * Format the scrollbars and the splitbar.
         */
        if (pswpVScroll->cy > (cyHorzSplitbar * 3)) {
            SetSwpPos(&aswp[iswpNext++],
                    WinWindowFromID(hwnd, ID_HORZSPLITBAR), NULL,
                    swpClient.x, swpClient.y + cyHorzSplitPos,
                    swpClient.cx + pswpVScroll->cx - cxBorder, cyHorzSplitbar,
                    pswpVScroll->fs | SWP_SHOW);

            SetSwpPos(&aswp[iswpNext++],
                    WinWindowFromID(hwnd, ID_VERTSCROLL2), NULL,
                    pswpVScroll->x, pswpVScroll->y,
                    pswpVScroll->cx, cyHorzSplitPos + (cyBorder * 2),
                    pswpVScroll->fs | SWP_SHOW);

            pswpVScroll->y += (cyHorzSplitPos + cyHorzSplitbar);
            pswpVScroll->cy -= (cyHorzSplitPos + cyHorzSplitbar);

        } else {
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_VERTSCROLL2),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_HORZSPLITBAR),
                    &iswpNext);
            HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT3),
                    &iswpNext);
            if (npdoc->fsStyle & DS_VERTSPLITBAR)
                HideSwp(&aswp[iswpNext], WinWindowFromID(hwnd, ID_CLIENT4),
                        &iswpNext);
        }
    }

    if (npdoc->fs & DF_SPLITVERT) {

        SetSwpPos(&aswp[iswpNext++],
                WinWindowFromID(hwnd, ID_VERTSPLITBAR), NULL,
                swpClient.x + cxVertSplitPos, pswpHScroll->y + cyBorder,
                cxVertSplitbar, swpClient.cy + pswpHScroll->cy - cyBorder,
                pswpHScroll->fs);

        SetSwpPos(&aswp[iswpNext++],
                WinWindowFromID(hwnd, ID_HORZSCROLL2), NULL,
                swpClient.x + cxVertSplitPos + cxVertSplitbar - cyBorder,
                pswpHScroll->y, pswpHScroll->cx - (cxVertSplitPos + cxVertSplitbar),
                pswpHScroll->cy, pswpVScroll->fs | SWP_SHOW);

        pswpHScroll->cx = cxVertSplitPos + (cxBorder * 2);
    }

    return (iswpNext);
}
