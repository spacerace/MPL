/****************************** Module Header ******************************\
* Module Name: DMGDDE.C
*
* This module contains functions used for interfacing with DDE structures
* and such.
*
* Created:  12/23/88    sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "ddemlp.h"

/***************************** Private Function ****************************\
* timeout()
*
* This routine creates a timer for hwndTimeout.  It then runs a modal loop
* which will exit once the WM_TIMER message is received by hwndTimeout.
* hwndTimeout can be any window that doesn't use timers itself with TID_TIMEOUT
* or TID_ABORT since its window proc doesn't need to do
* anything for this to work.  Only the client and server windows use these
* so were cool.
* Only one timeout window is allowed per thread.  This is checked by the
* pai passed in.
*
* Returns fSuccess, ie TRUE if TID_TIMEOUT was received before TID_ABORT.
*
* PUBDOC START
* Synchronous client transaction modal loops:
*
* During Synchronous transactions, a client application will enter a modal
* loop while waiting for the server to respond to the request.  If an
* application wishes to filter messages to the modal loop, it may do so
* by setting a message filter tied to MSGF_DDE.  Applications should
* be aware however that the DDE modal loop processes private messages
* in the WM_USER range, WM_DDE messages, and WM_TIMER messages with timer IDs
* using the TID_ constants defined in ddeml.h.
* These messages must not be filtered by an application!!!
*
* PUBDOC END
*
* History:
*   Created     sanfords    12/19/88
\***************************************************************************/
BOOL timeout(pai, ulTimeout, hwndTimeout)
PAPPINFO pai;
ULONG ulTimeout;
HWND hwndTimeout;
{
    QMSG qmsg;

    SemEnter();
    if (pai->hwndTimer) {
        pai->LastError = DMGERR_REENTRANCY;
        AssertF(FALSE, "Recursive timeout call");
        SemLeave();
        return(FALSE);
    }
    pai->hwndTimer = hwndTimeout;
    SemLeave();

    WinStartTimer(DMGHAB, hwndTimeout, TID_TIMEOUT, (USHORT)ulTimeout);

    WinGetMsg(DMGHAB, &qmsg, (HWND)NULL, 0, 0);

    /*
     * stay in modal loop until a timeout happens.
     */
    while (qmsg.hwnd != hwndTimeout ||
            qmsg.msg != WM_TIMER ||
            (LOUSHORT(qmsg.mp1) != TID_TIMEOUT &&
            LOUSHORT(qmsg.mp1) != TID_ABORT)) {
            
        if (!WinCallMsgFilter(DMGHAB, &qmsg, MSGF_DDE))
            WinDispatchMsg(DMGHAB, &qmsg);
            
        WinGetMsg(DMGHAB, &qmsg, (HWND)NULL, 0, 0);
    }
    
    WinStopTimer(DMGHAB, hwndTimeout, TID_TIMEOUT);
    SemEnter();
    pai->hwndTimer = 0;
    SemLeave();
    /*
     * post a callback check incase we blocked callbacks due to being
     * in a timeout.
     */
    WinPostMsg(pai->hwndDmg, UM_CHECKCBQ, (MPARAM)pai, 0L);
    return(LOUSHORT(qmsg.mp1) == TID_TIMEOUT);
}


/***************************** Private Function ****************************\
*
*  Based on pii, this sends an INITIATE message to either an exact
*  target window (hwndSend), a target frame window (hwndFrame) or to all
*  top level frames (both hwnds are NULL).  It fills in pci info as apropriate.
*  Note that pii->pCC must NOT be NULL and is assumed to be properly set.
*
*  Returns FALSE if SendDDEInit failed.
*  On success pci->ci.xad.state is CONVST_CONNECTED.
*
* History:
*   created 12/21/88        sanfords
\***************************************************************************/
BOOL ClientInitiate(hwnd, pii, pci)
HWND hwnd;
PINITINFO pii;
PCLIENTINFO pci;
{
    BOOL fRet = TRUE;

    if (pii->pCC->cb < sizeof(CONVCONTEXT))
        return(FALSE);
        
    SemEnter();
    /*
     * we need to set this info BEFORE we do the synchronous initiate
     * so the INITIATEACK msg is done correctly.
     */
    pci->ci.xad.state = CONVST_INIT1;
    pci->ci.xad.LastError = DMGERR_NO_ERROR;
    pci->ci.hszServerApp = pii->hszAppName;
    pci->ci.hszTopic = pii->hszTopic;
    pci->ci.cc.cb = sizeof(CONVCONTEXT);
    CopyBlock((PBYTE)&pii->pCC->fsContext, (PBYTE)&pci->ci.cc.fsContext,
        sizeof(CONVCONTEXT) - sizeof(USHORT));
    pci->ci.hwndFrame = pii->hwndFrame;
    SemLeave();
    
    fRet = SendDDEInit(hwnd,
            WinIsWindow(DMGHAB, pii->hwndSend) ? pii->hwndSend : pii->hwndFrame,
            pci);
    SemEnter();
    /*
     * If we failed to initiate directly with the server, try the frame.
     */
    if (!fRet && WinIsWindow(DMGHAB, pii->hwndSend) &&
            WinIsWindow(DMGHAB, pii->hwndFrame)) {
        SemLeave();
        fRet = SendDDEInit(hwnd, pii->hwndFrame, pci);
        if (fRet) {
            /*
             * OK, client is locked in so he wont go away on a terminate
             * from a random window. If the new server is not the same
             * window as the origonal, send it a terminate so it can
             * go away nicely.
             */
            if (pii->hwndSend != pci->ci.hwndPartner) 
                WinSendMsg(pii->hwndSend, WM_DDE_TERMINATE, 0L, 0L);
        }
        SemEnter();
        
    }
    if (!fRet) 
        pci->ci.xad.state = CONVST_NULL;
    else {
        /*
         * successful initiate means we want to keep these around awhile.
         * removed at window closing time.
         */
        IncHszCount(pci->ci.hszServerApp);
        IncHszCount(pci->ci.hszTopic);
    }
    SemLeave();
    return(fRet);
}



/***************************** Private Function ****************************\
* Allocates and sends a WM_DDE_INITIATE message to hwndTo.  Any failures
* cause FALSE to be returned.  If hwndTo is NULL, performs equivalent of
* WinDdeInitiate2().
*
* History:
*   created     12/22/88        sanfords
*   2/2/89  sanfords    added SEG_GETABLE during monitoring.
\***************************************************************************/
BOOL SendDDEInit(hwndFrom, hwndTo, pci)
HWND hwndFrom;
HWND hwndTo;
PCLIENTINFO pci;
{
    PID pidTo;
    TID tid;
    SEL sel;
    PDDEINIT pddeinit;
    HENUM henum;
    ULONG ul;
    USHORT cchApp, cchTopic;
    PSZ pszApp, pszTopic;
    BOOL fEnumerating;  /* set if extra acks are ok */

    SemCheckOut();

    if (hwndTo == NULL) {
        /*
         * Call on self for all top level frame windows until we are connected.
         * (if enumerating, do em all anyway.)
         */
        fEnumerating = WinQueryWindow(hwndFrom, QW_PARENT, FALSE) !=
                pci->ci.pai->hwndDmg;
        if (henum = WinBeginEnumWindows(HWND_DESKTOP)) {
            while ((hwndTo = WinGetNextWindow(henum)) &&
                    (fEnumerating || pci->ci.xad.state == CONVST_INIT1)) {
                if (hwndTo != pci->ci.pai->hwndFrame &&
                        (ul = (ULONG)WinSendMsg(hwndTo, WM_QUERYFRAMEINFO, 0L, 0L)) &&
                        (ul & FI_FRAME))
                    SendDDEInit(hwndFrom, hwndTo, pci);
            }
            WinEndEnumWindows(henum);
        }
        return(TRUE);
    }
    
    if (WinQueryWindowProcess(hwndTo, &pidTo, &tid) == NULL)
        return(FALSE);

    SemEnter();
    pszApp = pszFromHsz(pci->ci.hszServerApp, &cchApp);
    pszTopic = pszFromHsz(pci->ci.hszTopic, &cchTopic);
    if (DosAllocSeg(sizeof(DDEINIT) + sizeof(CONVCONTEXT) + cchApp + cchTopic,
                &sel, SEG_GIVEABLE) != 0) {
        SemLeave();
        return(FALSE);
    }
    pddeinit = MAKEP(sel, 0);
    pddeinit->cb = sizeof(DDEINIT);
    pddeinit->offConvContext = sizeof(DDEINIT);
    pddeinit->pszAppName = (PSZ)pddeinit + sizeof(DDEINIT) + sizeof(CONVCONTEXT);
    pddeinit->pszTopic = pddeinit->pszAppName + cchApp;
    CopyBlock((PBYTE)&pci->ci.cc, (PBYTE)DDEI_PCONVCONTEXT(pddeinit), sizeof(CONVCONTEXT));
    CopyBlock((PBYTE)pszApp, (PBYTE)pddeinit->pszAppName, cchApp);
    CopyBlock((PBYTE)pszTopic, (PBYTE)pddeinit->pszTopic, cchTopic);
    FarFreeMem(hheapDmg, pszApp, cchApp);
    FarFreeMem(hheapDmg, pszTopic, cchTopic);
    SemLeave();

    if (DosGiveSeg(sel, pidTo, &sel) != 0) {
        DosFreeSeg(sel);
        return(FALSE);
    }

    WinSendMsg(hwndTo, WM_DDE_INITIATE, (MPARAM)hwndFrom, pddeinit);
    if (pidTo != pci->ci.pai->pid)
        DosFreeSeg(sel);
    return(TRUE);
}



/***************************** Private Function ****************************\
*
*  Alocates and fills in a MYDDES. if pai == 0, the MYDDES is considered
* unowned.
*
* History:  created     1/4/89  sanfords
* 10/18/89  sanfords Added hack so that if usFmt==DDEFMT_TEXT and hszItem==0L,
*                    the data and item strings are one.
*                    (This allows for excel EXEC compatibility)
*   2/2/89  sanfords Added GETABLE during monitoring.
*  6/13/90  sanfords Altered to not expand hszItem at this point.
\***************************************************************************/
PDDESTRUCT AllocDDESel(fsStatus, usFmt, hszItem, cbData, pai)
USHORT fsStatus;
USHORT usFmt;
HSZ hszItem;
ULONG cbData;
PAPPINFO pai;
{
    PMYDDES pmyddes = NULL;
    ULONG cbTotal;
    ULONG cchItem;
    SEL sel;

    SemEnter();
    cchItem = DdeGetHszString(hszItem, NULL, 0L) + 1L;

    /*
     * This hack makes execs conform to EXCELs way.
     */
    if (!hszItem && usFmt == DDEFMT_TEXT)
        cchItem = 0L;

    cbTotal = sizeof(MYDDES) + cchItem + cbData + 1;
    if (cbTotal <= 0xFFFF) {
        if (DosAllocSeg((USHORT)cbTotal, &sel, SEG_GIVEABLE) != 0)
            goto allocDdeExit;
    } else {
        if (DosAllocHuge((USHORT)(cbTotal >> 16), (USHORT)cbTotal, &sel,
                0, SEG_GIVEABLE) != 0)
            goto allocDdeExit;
    }

    pmyddes = MAKEP(sel, 0);
    pmyddes->cbData = cbData;
    pmyddes->fsStatus = fsStatus;
    pmyddes->usFormat = usFmt;
    pmyddes->offszItemName = sizeof(MYDDES);
    pmyddes->offabData = sizeof(MYDDES) + (USHORT)cchItem;
    pmyddes->ulRes1 = 0L;
    pmyddes->magic = MYDDESMAGIC;
    pmyddes->hszItem = hszItem;
    pmyddes->pai = pai;
    pmyddes->fs = 0;
    *DDES_PABDATA((PDDESTRUCT)pmyddes) = '\0'; /* in case data is never placed */
    *DDES_PSZITEMNAME((PDDESTRUCT)pmyddes) = '\0';  /* we expand this at post time if necessary */

allocDdeExit:
    SemLeave();
    return((PDDESTRUCT)pmyddes);
}




/***************************** Private Function ****************************\
* This routine returns the hwnd of a newly created and connected DDE
* client or NULL if failure.
*
* History:  created     1/6/89  sanfords
\***************************************************************************/
HCONV GetDDEClientWindow(hConvList, hwndFrame, hwndSend, hszApp, hszTopic, pCC)
HCONVLIST hConvList;
HWND hwndFrame;
HWND hwndSend;
HSZ hszApp;
HSZ hszTopic;
PCONVCONTEXT pCC;
{
    HCONV hConv;
    INITINFO ii;
    CONVCONTEXT cc;

    SemCheckOut();
    
    hConv = WinCreateWindow(hConvList, SZCLIENTCLASS, "", 0L,
            0, 0, 0, 0, (HWND)NULL, HWND_TOP, WID_CLIENT, 0L, 0L);
            
    if (hConv == NULL)
        return(NULL);
        
    ii.hszTopic = hszTopic;
    ii.hszAppName = hszApp;
    ii.hwndSend = hwndSend;
    ii.hwndFrame = hwndFrame;
    if (pCC == NULL) {
        pCC = &cc;
        cc.cb = sizeof(CONVCONTEXT);
        cc.fsContext = 0;
        /*##LATER - may want to use process codepage instead */
        cc.usCodepage = syscc.codepage;
        cc.idCountry = syscc.country;
    }
    if (pCC->usCodepage == 0) 
        pCC->usCodepage = syscc.codepage;
    if (pCC->idCountry == 0) 
        pCC->idCountry = syscc.country;
        
    ii.pCC = pCC;
    WinSendMsg(hConv, UMCL_INITIATE, (MPARAM)&ii, 0L);
    
    if (!((USHORT)WinSendMsg(hConv, UM_QUERY, (MPARAM)Q_STATUS, 0L) &
            ST_CONNECTED)) {
        WinDestroyWindow(hConv);
        return(NULL);
    }
    return(hConv);
}



/***************************** Private Function ****************************\
* This routine institutes a callback directly if psi->fEnableCB is set
* and calls QReply to complete the transaction,
* otherwise it places the data into the queue for processing.
*
* Since hDmgData may be freed by the app at any time once the callback is
* issued, we cannot depend on it being there for QReply.  Therefore we
* save all the pertinant data in the queue along with it.
*
* Returns fSuccess.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
BOOL MakeCallback(pai, hConv, hszTopic, hszItem, usFmt, usType, hDmgData,
    msg, fsStatus, hConvClient)
PAPPINFO pai;
HCONV hConv;
HSZ hszTopic;
HSZ hszItem;
USHORT usFmt;
USHORT usType;
HDMGDATA hDmgData;
USHORT msg;
USHORT fsStatus;
HCONV hConvClient;
{
    PCBLI pcbli;

    SemEnter();
    
    if (!(pcbli = (PCBLI)NewLstItem(pai->plstCB, ILST_LAST))) {
        pai->LastError = DMGERR_MEMORY_ERROR;
        SemLeave();
        return(FALSE);
    }
    pcbli->hConv = hConv;
    pcbli->hszTopic = hszTopic;
    pcbli->hszItem = hszItem;
    pcbli->usFmt = usFmt;
    pcbli->usType = usType;
    pcbli->hDmgData = hDmgData;
    pcbli->msg = msg;
    pcbli->fsStatus = fsStatus;
    pcbli->hConvPartner = hConvClient;
    
    if (pai->fEnableCB && !pai->hwndTimer) {
        SemLeave();
        WinPostMsg(pai->hwndDmg, UM_CHECKCBQ, (MPARAM)pai, 0L);
    } else
        SemLeave();
    
    return(TRUE);
}



/*************************************************************************\
* Attempts to post a DDE message to hwndTo.  Properly frees up pmyddes
* if afCmd has MDPM_FREEHDATA set.  We do not add pmyddes to the target
* thread list since we assume that will be done at the receiving end
* if necessary.
*
* Returns fSuccess.
*
* 6/12/90 sanfords  Created
* 6/13/90 sanfords  Made it convert hszItem to a string at this point
*                   only if hwndTo is not a local guy.
\*************************************************************************/
BOOL MyDdePostMsg(
HWND hwndTo,
HWND hwndFrom,
USHORT msg,
PMYDDES pmyddes,
PAPPINFO paiFrom,
USHORT afCmd)
{
    PID pid;
    TID tid;
    SEL selR;
    BOOL fRet;
    PFNWP pfnwpTo;

    if (!WinQueryWindowProcess(hwndTo, &pid, &tid))
        return FALSE;

    pfnwpTo = (PFNWP)WinQueryWindowPtr(hwndTo, QWP_PFNWP);
    if (cMonitor || (pfnwpTo != ServerWndProc && pfnwpTo != ClientWndProc)) {
        /*
         * its not local - expand hszItem if necessary - always
         * expand if a monitor is installed.
         */
        if (CheckSel(SELECTOROF(pmyddes)) >= sizeof(MYDDES) &&
                pmyddes->magic == MYDDESMAGIC &&
                pmyddes->hszItem &&
                !(pmyddes->fs & HDATA_PSZITEMSET)) {
            pmyddes->fs |= HDATA_PSZITEMSET;
            QueryHszName(pmyddes->hszItem, DDES_PSZITEMNAME(pmyddes),
                    pmyddes->offabData - pmyddes->offszItemName);    
        }
    }
    /*
     * Don't try to share seg with ourselves.
     */
    if (paiFrom->pid != pid) {
        selR = SELECTOROF(pmyddes);
        if (DosGiveSeg(SELECTOROF(pmyddes), pid, &selR))
            return FALSE;
        if (afCmd & MDPM_FREEHDATA) 
            FreeData(pmyddes, paiFrom);
    } else {
        /*
         * just remove hData from our thread list
         */
        if (afCmd & MDPM_FREEHDATA && !(pmyddes->fs & HDATA_APPOWNED))
            FindPileItem(paiFrom->pHDataPile, CmpULONG, (PBYTE)&pmyddes,
                    FPI_DELETE);
    }
    fRet = (BOOL)WinPostMsg(hwndTo, msg, (MPARAM)hwndFrom, (MPARAM)pmyddes);
    if (!fRet) {
        /*
         * make sure this is freed if it is supposed to be - this covers
         * the case where the target is of the same process and only
         * these two threads are registered.
         */
        
        tid = paiFrom->tid;
        do {
            if (FindPileItem(paiFrom->pHDataPile, CmpULONG, (PBYTE)&pmyddes,
                    FPI_COUNT))
                return(FALSE);  /* there is another thread that has this */
            paiFrom = paiFrom->nextThread;
        } while (paiFrom->tid != tid);
        DosFreeSeg(SELECTOROF(pmyddes));
    }
    return(fRet);
}
