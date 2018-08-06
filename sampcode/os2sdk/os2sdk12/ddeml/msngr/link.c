/****************************** MODULE Header ******************************\
* Module Name:  link.c - Messenger application - link module
*
* Created: 8/1/89  sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#include "msngr.h"
#include "string.h"

extern HWND hwndMsngr;
extern HPOINTER hptrLink;
extern SHORT cyText;
extern HAB hab;
extern HSZ hszAppName;
extern HSZ hszEmailName;
extern NPUSERLIST gnpUL;
extern PFNWP lpfnSysEFWndProc;   /* holds the system edit control proc */

typedef struct _LINKDATA {
    NPUSERLIST pUserItem;
    USHORT state;
    HWND hwndTextIn;
    char **ppszText;
    char szOut[MAX_LINKSTR + 1];
    USHORT cLines;
    USHORT iTop;
} LINKDATA;
typedef LINKDATA *NPLINKDATA;

PFNWP lpfnStaticWndProc;

char **AllocTextIn(USHORT cLines);
void FreeTextIn(char **ppsz, USHORT cLines);
void AddTextIn(NPLINKDATA pLinkData, PSZ pszTextIn);
MRESULT EXPENTRY MyTextInWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);



/***************************** Private Function ****************************\
* DESCRIPTION:
*   This creates a link window with control data set up as dictated by the
*   parameters given.  state incidates whether the window was created by
*   user or ddeml action.
*
* History:      1/18/89     Created         sanfords
\***************************************************************************/
BOOL CreateLinkWindow(pUserItem, state)
NPUSERLIST pUserItem;
USHORT state;
{
    NPLINKDATA pLinkData;


    if (pUserItem->hwndLink != 0) {
        WinDestroyWindow(pUserItem->hwndLink);
        pUserItem->hwndLink = 0;
    }
        
    if (!(pLinkData = (NPLINKDATA)WinAllocMem(hheap, sizeof(LINKDATA)))) {
        NotifyUser(SZMEMORYERROR);
        return(0);
    }
    
    pLinkData->pUserItem = pUserItem;
    pLinkData->state = state;
    pUserItem->hwndLink = WinLoadDlg(HWND_DESKTOP, hwndMsngr, LinkDlgProc,
            0L, IDD_LINK, (PVOID)pLinkData);
            
    if (!WinPostMsg(pUserItem->hwndLink, UM_LINK1, 0L, 0L)) {
        WinDestroyWindow(pUserItem->hwndLink);
        return(FALSE);
    }
    return(TRUE);
}


char **AllocTextIn(cLines)
USHORT cLines;
{
    char **ppsz, *psz;
    
    if (!(ppsz = (char **)WinAllocMem(hheap,
            cLines * (MAX_LINKSTR + 1 + sizeof(NPSZ)))))
        return(0);
    psz = (char *)(ppsz + cLines);
    while (cLines--) {
        ppsz[cLines] = psz;
        *psz = '\0';
        psz += MAX_LINKSTR + 1;
    }
    return(ppsz);
}


void FreeTextIn(ppsz, cLines)
char **ppsz;
USHORT cLines;
{
    WinFreeMem(hheap, (NPBYTE)ppsz, cLines * (MAX_LINKSTR + 1 + sizeof(NPSZ)));
}


void AddTextIn(pLinkData, pszTextIn)
NPLINKDATA pLinkData;
PSZ pszTextIn;
{
    HPS hps;
    RECTL rcl;
    USHORT cchOut, cLines;
    CHAR ch;
    
    hps = WinGetPS(pLinkData->hwndTextIn);
    
    cLines = 0;
    WinQueryWindowRect(pLinkData->hwndTextIn, &rcl);
    while (*pszTextIn) {
        cchOut = WinDrawText(hps, -1, pszTextIn, &rcl, 0L, 0L,
                DT_LEFT | DT_BOTTOM | DT_WORDBREAK | DT_QUERYEXTENT);
        ch = pszTextIn[cchOut];
        pszTextIn[cchOut] = '\0';
        lstrcpy(pLinkData->ppszText[pLinkData->iTop++], pszTextIn);
        if (pLinkData->iTop == pLinkData->cLines)
            pLinkData->iTop = 0;
        pszTextIn[cchOut] = ch;
        pszTextIn += cchOut;
        while (*pszTextIn == ' ')
           pszTextIn++;
        cLines++;
    }
    WinReleasePS(hps);
    WinQueryWindowRect(pLinkData->hwndTextIn, &rcl);
    WinScrollWindow(pLinkData->hwndTextIn, 0, cyText * cLines,
            NULL, &rcl, NULL, NULL, SW_INVALIDATERGN);
}



/***************************** Private Function ****************************\
* DESCRIPTION:
*   This is a state-driven dialog which allows user linking.  At createion
*   time the control data is set to contain information on who this link
*   is with, and why this window was created. (user action or another user
*   requesting a link.
*
* History:      1/18/89     Created         sanfords
\***************************************************************************/
MRESULT EXPENTRY LinkDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    register NPLINKDATA pLinkData;
    register NPUSERLIST pUserItem;
    char szName[MAX_NAMESTR + 1];
    char szTitle[MAX_TITLESTR + 1];
    SWP swp;

    if (pLinkData = (NPLINKDATA)WinQueryWindowUShort(hwnd, QWS_USER))
        pUserItem = pLinkData->pUserItem;

    switch(msg) {
    case WM_INITDLG:
        /*
         * This is required because currently, automatic ICON resource loading
         * is not supported for dialogs.
         */
        if (hptrLink == 0) {
            hptrLink = WinLoadPointer(HWND_DESKTOP, NULL, IDD_LINK);
        }
        WinSendMsg(hwnd, WM_SETICON, (MPARAM)hptrLink, 0L);

        /*
         * we are initialized with a (near) pointer to a LINKDATA structure.
         * Keep it in our window words.
         */
        pLinkData = (NPLINKDATA)(USHORT)mp2;
        WinSetWindowUShort(hwnd, QWS_USER, (USHORT)pLinkData);
        pLinkData->hwndTextIn = WinWindowFromID(hwnd, IDC_TEXTIN);

        /*
         * set the output window so it is a integral multiple of cyText
         * high.
         */
        WinQueryWindowPos(pLinkData->hwndTextIn, &swp);
        swp.cy -= swp.cy % cyText;
        pLinkData->cLines = swp.cy / cyText;
        pLinkData->iTop = 0;
        swp.fs = SWP_SIZE;
        WinSetMultWindowPos(hab, &swp, 1);
        
        /*
         * set up constants used for text output.
         */
        lpfnStaticWndProc = WinSubclassWindow(pLinkData->hwndTextIn,
                MyTextInWndProc);
        pLinkData->cLines = swp.cy / cyText;
        pLinkData->ppszText = AllocTextIn(pLinkData->cLines);
        
        /*
         * subclass the edit control so that enter keys send us a message.
         */
        lpfnSysEFWndProc =
                WinSubclassWindow(WinWindowFromID(hwnd, IDC_EF_TEXTOUT),
                EnhancedEFWndProc);
                
        WinSendDlgItemMsg(hwnd, IDC_EF_TEXTOUT, EM_SETTEXTLIMIT,
                MPFROMSHORT(MAX_LINKSTR), 0L);

        /*
         * return 1 so we don't get the focus - we are still invisible!
         */
        return(1);
        break;

    case WM_DESTROY:
        /*
         *      Terminate advise loop.
         *      Destroy connection if made
         *          advise loops first.
         *      free control data
         */
        if (pUserItem->hConvLink) {
            if (pLinkData->state == LNKST_LINKED) {
                DdeClientXfer(0, 0L, pUserItem->hConvLink,
                        hszEmailName, 
                        DDEFMT_TEXT, XTYP_ADVSTOP, ulTimeout, NULL);
            }
            DdeDisconnect(pUserItem->hConvLink);
            pUserItem->hConvLink = 0;
        }
        FreeTextIn(pLinkData->ppszText, pLinkData->cLines);
        WinFreeMem(hheap, (NPBYTE)pLinkData, sizeof(LINKDATA));
        pUserItem->hwndLink = 0;
        break;

    case UM_LINK1:
        /*
         * connect on link channel - topic = server name.
         */
        if (!pUserItem->hConvLink && !(pUserItem->hConvLink =
                DdeConnect(hszAppName, pUserItem->hsz, NULL, (HAPP)mp1))) {
            if (pLinkData->state == LNKST_USERSTART) 
                NotifyUser(SZCANTLINK);
            return(FALSE);
        }
        
        if (pLinkData->state == LNKST_USERSTART) {
            return(WinSendMsg(hwnd, UM_DOADVSTART, 0L, 0L));
        } else {
            /*
             * We were started by a ddeml advise message - return now
             * and post an advise start message to ourselves so we send
             * it AFTER we returned from his advise.
             */
            WinPostMsg(hwnd, UM_DOADVSTART, 0L, 0L);
            return(TRUE);
        }
        break;

    case UM_DOADVSTART:
        /*
         * Its time to start the advise loop...return fSuccess 
         *
         * Item = client name.
         */
        if (DdeClientXfer(0, 0L, pUserItem->hConvLink, hszEmailName,
                DDEFMT_TEXT, XTYP_ADVSTART, ulTimeout, NULL)) {
            DdeGetHszString(hszEmailName, szName, MAX_NAMESTR + 1L);
            lstrcat(szTitle, szName, SZMSGLINKWITH);
            DdeGetHszString(pUserItem->hsz, szName, MAX_NAMESTR + 1L);
            lstrcat(szTitle, szTitle, szName);
            WinSetDlgItemText(hwnd, FID_TITLEBAR, szTitle);
            if (pLinkData->state == LNKST_DMGSTART) {
                pLinkData->state = LNKST_LINKED;
                WinSetDlgItemText(hwnd, IDC_INFO, SZLINKED);
            }
            WinShowWindow(hwnd, TRUE);
            return(TRUE);

        } else {
            if (pLinkData->state == LNKST_USERSTART)
                MyPostError(DdeGetLastError());
            else
                /*
                 * destroy ouselves lest we have an invisible link window
                 * hanging around.
                 */
                WinDestroyWindow(hwnd);
            return(FALSE);
        }
        break;

    case UM_ADVRCVD:
        if (pLinkData->state == LNKST_USERSTART) {
            pLinkData->state = LNKST_LINKED;
            WinSetDlgItemText(hwnd, IDC_INFO, SZLINKED);
        }
        break;

    case UM_LINKDATAIN:
        AddTextIn(pLinkData, (PSZ)mp1);
        break;

    case UM_BREAKLINK:
        if (pLinkData->state == LNKST_LINKED) {
            NotifyUser(SZLINKBROKEN);
            if (WinIsWindow(hab, hwnd))
                WinDestroyWindow(hwnd);
        }
        break;

    case WM_WINDOWPOSCHANGED:
        /*
         * hide the Edit control when minimized since it messes up the icon.
         */
        if ((LONG)mp2 & AWP_MINIMIZED)
            WinShowWindow(WinWindowFromID(hwnd, IDC_EF_TEXTOUT), FALSE);
        else if ((LONG)mp2 & AWP_RESTORED)
            WinShowWindow(WinWindowFromID(hwnd, IDC_EF_TEXTOUT), TRUE);
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
        
    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case IDC_TERMINATE:
            if (pLinkData->state == LNKST_LINKED) {
                pLinkData->state = LNKST_UNLINKED;
                DdeClientXfer(0, 0L, pUserItem->hConvLink, hszEmailName,
                        DDEFMT_TEXT, XTYP_ADVSTOP, ulTimeout, NULL);
            }
            WinDestroyWindow(hwnd);
            break;
        }
        break;

    case ENHAN_ENTER:
        /*
         * when the user hits the enter key, it will be passed from the
         * entryfield to here and we will send it to the linkee.
         */
        WinQueryDlgItemText(hwnd, IDC_EF_TEXTOUT, MAX_LINKSTR + 1,
                pLinkData->szOut);
        WinSetDlgItemText(hwnd, IDC_EF_TEXTOUT, "");
        if (pLinkData->state == LNKST_LINKED)
            /*
             * we postadvise to OUR server which is talking to HIS client
             * which has the topic being OUR name and the item being HIS.
             * This results in a UM_LINKDATAOUT being sent to us.
             */
            DdePostAdvise(hszEmailName, pUserItem->hsz);
        break;

    case UM_LINKDATAOUT:
        /*
         * returns hDmgData filled.
         */
        if (pLinkData->state == LNKST_LINKED) {
            return(DdePutData(pLinkData->szOut,
                    (ULONG)(lstrlen(pLinkData->szOut) + 1), 0L,
                    pUserItem->hsz, DDEFMT_TEXT, 0));
        }
        break;

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}




/***************************** Private Function ****************************\
* DESCRIPTION:
*   This function gains control whenever externaly trigered data transfers
*   are sent to the hszEmailName topic.
*
* History:      1/17/89     Created         sanfords
\***************************************************************************/
HDMGDATA LinkXfer(pXferInfo, hszPartner)
PXFERINFO pXferInfo;
HSZ hszPartner;
{
    NPUSERLIST pUserItem;
    char szTextIn[MAX_LINKSTR + 1];

    /*
     * always allow creation of servers on this topic.
     */
    if (pXferInfo->usType == XTYP_INIT)
        return(TRUE);

    /*
     * we only allow text
     */
    if (pXferInfo->usFmt != DDEFMT_TEXT)
        return(0);

    /*
     * if he's gone, this should never happen.
     */
    if (!(pUserItem = FindUser(gnpUL, hszPartner))) {
        NotifyUser(SZINTERNALERROR);
        return(0);
    }

    if (pXferInfo->usType == XTYP_ADVSTART) {
        /*
         * link Advise startup - if no hwndLink, he started it, we must
         * create a link window and a counter advise loop.  If we do
         * have a hwndLink, we started it and it is his counter advise
         * link request.
         */
        if (!WinIsWindow(hab, pUserItem->hwndLink)) {
            return((HDMGDATA)CreateLinkWindow(pUserItem, LNKST_DMGSTART));
        } else {
            WinSendMsg(pUserItem->hwndLink, UM_ADVRCVD, 0L, 0L);
            return(DDE_FACK);
        }
    }

    /*
     * The rest of these are only applicable if we are in an active link
     * situation, so quit if no link window is present.
     */
    if (!WinIsWindow(hab, pUserItem->hwndLink)) 
        return(0);

    switch (pXferInfo->usType) {
    case XTYP_ADVDATA:
        /*
         * incomming link data
         */
        DdeGetData(pXferInfo->hDmgData, szTextIn, (ULONG)(MAX_LINKSTR + 1), 0L);
        DdeFreeData(pXferInfo->hDmgData);
        WinSendMsg(pUserItem->hwndLink, UM_LINKDATAIN, (MPARAM)szTextIn, 0L);
        return(DDE_FACK);
        break;

    case XTYP_REQUEST:
    case XTYP_ADVREQ:
        /*
         * Outgoing link data
         */
        return((HDMGDATA)WinSendMsg(pUserItem->hwndLink, UM_LINKDATAOUT, 0L, 0L));
        break;

    case XTYP_ADVSTOP:
        WinSendMsg(pUserItem->hwndLink, UM_BREAKLINK, 0L, 0L);
        return(TRUE);
        break;
    }
    return(0);
}



MRESULT EXPENTRY MyTextInWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    register NPLINKDATA pLinkData;
    RECTL rcl;
    HPS hps;
    USHORT iLine, cLines;
    
    switch (msg) {
    case WM_PAINT:
        hps = WinBeginPaint(hwnd, NULL, &rcl);
        pLinkData = (NPLINKDATA)WinQueryWindowUShort(
                WinQueryWindow(hwnd, QW_PARENT, FALSE), QWS_USER);
        WinQueryWindowRect(hwnd, &rcl);
        rcl.yBottom = rcl.yTop - cyText;
        
        iLine = pLinkData->iTop;
        cLines = pLinkData->cLines;
        while (cLines--) {
            WinDrawText(hps, -1, pLinkData->ppszText[iLine++], &rcl, 0L, 0L,
                    DT_LEFT | DT_VCENTER | DT_ERASERECT | DT_TEXTATTRS);
            if (iLine == pLinkData->cLines)
                iLine = 0;
            WinOffsetRect(hab, &rcl, 0, -cyText);
        }
        WinEndPaint(hps);
        break;

    default:
        return((*lpfnStaticWndProc)(hwnd, msg, mp1, mp2));
    }
}

