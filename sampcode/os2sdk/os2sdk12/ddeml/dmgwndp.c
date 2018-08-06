/****************************** Module Header ******************************\
*
* Module Name: DMGWNDP.C
*
* This module contains all the window procs for the DDE manager.
*
* Created: 12/23/88 sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#include "ddemlp.h"


ULONG defid = QID_SYNC;
XFERINFO defXferInfo = {
        &defid,
        1L,
        XTYP_INIT,
        DDEFMT_TEXT,
        0L,
        0L,
        0L,
        NULL
};

void InitAck(HWND hwnd, PCLIENTINFO pci, HWND hwndServer, PDDEINIT pddei);
MRESULT ClientXferReq(PXFERINFO pXferInfo, PCLIENTINFO pci, HWND hwnd);
USHORT SendClientReq(PXADATA pXad, HWND hwndServer, HWND hwnd, PAPPINFO pai);
void DoClientDDEmsg(PCLIENTINFO pci, HWND hwnd, USHORT msg, HWND hwndFrom,
        PDDESTRUCT pddes);
BOOL fExpectedMsg(PXADATA pXad, PDDESTRUCT pddes, USHORT msg, PCLIENTINFO pci);
BOOL AdvanceXaction(HWND hwnd, PCLIENTINFO pci, PXADATA pXad,
        PDDESTRUCT pddes, USHORT msg, PUSHORT pErr);
MRESULT ClientXferRespond(PCLIENTINFO pci, PXADATA pXad, PUSHORT pErr);
void FrameInitConv(HWND hwndClient, PDDEINIT pddei);

/*
 * ----------------CLIENT SECTION------------------
 *
 * Each client conversation has associated with it a window and a queue.
 * A conversation has one synchronous transaction and may have many
 * asynchronous transactions.  A transaction is differientiated by its
 * state and other pertinant data.  A transaction may be synchronous,
 * asynchronous, (initiated by DdeClientXfer()), or it may be external,
 * (initiated by an advise loop.)
 *
 * A transaction is active if it is in the middle of tranfer, otherwise
 * it is shutdown.  A shutdown transaction is either successful or
 * failed.  When an asynchronous transaction shuts down, the client
 * is notified via the callback function. (XTYP_XFERCOMPLETE)
 *
 * The synchronous transaction, when active, is in a timeout loop which
 * can shut-down the transaction at the end of a predefined time period.
 * Shutdown synchronous transactions imediately transfer their information
 * to the client application by returning to DdeClientXfer().
 *
 * asynchronous transactions remain in the client queue until removed
 * by the client application via DdeCheckQueue().  
 *
 * external transactions take place when the client is in an advise
 * data loop.  These transactions pass through the callback function to
 * the client to be accepted.(XTYP_ADVDATA)
 */


/***************************** Private Function ****************************\
* MRESULT EXPENTRY ClientWndProc(hwnd, msg, mp1, mp2);
*
*   This window controls a single DDE conversation from the CLIENT side.
*   If closed, it will automaticly abort any conversationn in progress.
*   It maintains an internal list of any extra WM_DDEINITIATEACK messages
*   it receives so that it can be queried later about this information.
*   Any extra WM_DDEINITIATEACK messages comming in will be immediately
*   terminated.
*   It also maintains an internal list of all items which currently are
*   in active ADVISE loops.
*
* History:
*   Created     12/16/88    Sanfords
\***************************************************************************/
MRESULT EXPENTRY ClientWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    register PCLIENTINFO pci;
    PAPPINFO pai;
    MRESULT mrData;
    PDDESTRUCT pddes;

    pci = (PCLIENTINFO)WinQueryWindowULong(hwnd, QWL_USER);

    switch (msg) {
    case WM_CREATE:
        /*
         * allocate and initialize the client window info.
         */
        pai = GetCurrentAppInfo(FALSE);
        SemEnter();
        pci = (PCLIENTINFO)FarAllocMem(pai->hheapApp, sizeof(CLIENTINFO));
        SemLeave();
        if (pci == NULL) {
            pai->LastError = DMGERR_MEMORY_ERROR;
            return(1);          /* aboart creation - low memory */
        }
        WinSetWindowULong(hwnd, QWL_USER, (ULONG)pci);
        pci->ci.pai = pai;
        pci->ci.xad.state = CONVST_NULL;
        pci->ci.xad.pXferInfo = &defXferInfo;
        pci->ci.fs = 0;
        pci->ci.hwndPartner = NULL;
        pci->ci.hszServerApp = NULL;
        pci->ci.hszTopic = NULL;
        pci->pQ = NULL;    /* don't create until we need one */
        if (!(pci->ci.pAdviseList = CreateLst(pai->hheapApp, sizeof(ADVLI)))) {
            FarFreeMem(pai->hheapApp, (PBYTE)pci, sizeof(CLIENTINFO));
            pai->LastError = DMGERR_MEMORY_ERROR;
            return(1);          /* aboart creation - low memory */
        }
        break;

    case UMCL_INITIATE:
        if (pci->ci.xad.state == CONVST_NULL) {
            return(ClientInitiate(hwnd, (PINITINFO)mp1, pci));
        }
        break;

    case WM_DDE_INITIATEACK:
        InitAck(hwnd, pci, mp1, mp2);
        DosFreeSeg(SELECTOROF(mp2));
        return(1);
        break;

    case WM_DESTROY:
        SemCheckOut();
        if (pci->ci.fs & ST_CONNECTED) {
            /*
             * stop any advises in progress
             */
            if (pci->ci.fs & ST_ADVISE) {
                pddes = AllocDDESel(0, 0, 0, 0L, NULL);
                MyDdePostMsg(pci->ci.hwndPartner, hwnd, WM_DDE_UNADVISE,
                        (PMYDDES)pddes, pci->ci.pai, MDPM_FREEHDATA);
            }
            WinSendMsg(hwnd, UMCL_TERMINATE, 0L, 0L);
            /*
             * decrement the use counts on hszs we are done with.
             */
            FreeHsz(pci->ci.hszServerApp);
            FreeHsz(pci->ci.hszTopic);
        }
        
        DestroyLst(pci->ci.pAdviseList);
        
        SemEnter();
        DestroyQ(pci->pQ);
        FarFreeMem(pci->ci.pai->hheapApp, (PBYTE)pci, sizeof(CLIENTINFO));
        SemLeave();
        break;

    case UMCL_TERMINATE:
        /*
         * terminates any conversation in progress
         */
        if (pci->ci.fs & ST_CONNECTED) {
            pci->ci.fs = pci->ci.fs & ~ST_CONNECTED;
            pci->ci.xad.state = CONVST_TERMINATED;
            if (WinIsWindow(DMGHAB, pci->ci.hwndPartner))
                WinDdePostMsg(pci->ci.hwndPartner, hwnd, WM_DDE_TERMINATE, 0L, FALSE);
        }
        break;

    case UMCL_XFER:
        if (!(pci->ci.fs & ST_CONNECTED)) {
            pci->ci.pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
            return(0);
        }
        return(ClientXferReq((PXFERINFO)mp1, pci, hwnd));
        break;

    case WM_DDE_DATA:
    case WM_DDE_ACK:
        DoClientDDEmsg(pci, hwnd, msg, (HWND)mp1, (PDDESTRUCT)mp2);
        break;

    case WM_DDE_TERMINATE:
        SemCheckOut();
        /*
         * only respond if this is for us.
         */
        if ((HWND)mp1 != pci->ci.hwndPartner) {
           DosFreeSeg(SELECTOROF(mp2));
           break;
        }
        WinSendMsg(hwnd, UMCL_TERMINATE, 0L, 0L);
        DosFreeSeg(SELECTOROF(mp2));
        break;

    case UM_QUERY:
        /*
         * LOUSHORT(mp1) = info index.
         * mp2 = pData.     If pData==0, return data else copy into pData.
         */
        switch (LOUSHORT(mp1)) {
        case Q_STATUS:
             mrData = (MRESULT)pci->ci.fs;
             break;

        case Q_CLIENT:
             mrData = TRUE;
             break;

        case Q_APPINFO:
             mrData = pci->ci.pai;
             break;

        case Q_APPNAME:
             mrData = *(PHSZ)PTOPPILEITEM(pci->ci.pai->pAppNamePile);
             break;
             
        case Q_ALL:
             mrData = (MRESULT)(CLIENTINFO FAR *)pci;
             break;
        }
        if (mp2 == 0)
            return(mrData);
        else
            *(MRESULT FAR *)mp2 = mrData;
        return(1);
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}



/*
 * Client response to a WM_DDE_INITIATEACK message when expected.
 */
void InitAck(hwnd, pci, hwndServer, pddei)
HWND hwnd;
PCLIENTINFO pci;
HWND hwndServer;
PDDEINIT pddei;
{
    SemCheckOut();
    
    if (pci->ci.fs & ST_CONNECTED) {
        /*
         * extra server - spawn another client window.  (we assume we
         * will only get extras if enumerating.)
         */
        AssertF(WinQueryWindow(hwnd, QW_PARENT, FALSE) != pci->ci.pai->hwndDmg,
            "Improper client spawn")
        if (hwndServer != pci->ci.hwndPartner) {
            WinSendMsg(hwndServer, WM_DDE_TERMINATE, hwnd, 0L);
            GetDDEClientWindow(WinQueryWindow(hwnd, QW_PARENT, FALSE),
                    hwndServer, hwndServer, pci->ci.hszServerApp,
                    pci->ci.hszTopic, &pci->ci.cc);
        }
        return;
    }

    if (pci->ci.xad.state != CONVST_INIT1) 
        return;
        
    /*
     * first one back... lock in!
     */
    pci->ci.hwndPartner = hwndServer;
    pci->ci.xad.state = CONVST_CONNECTED;
    pci->ci.fs |= ST_CONNECTED;
    if (WinQueryWindowPtr(hwndServer, QWP_PFNWP) == ServerWndProc) 
        pci->ci.fs |= ST_INTRADLL;
    
    /*
     * If the connection was made using a wild app name, we want to
     * hack in an apropriate name so QueryConvInfo can give the app
     * something useful to refer to this guy as.
     *
     * - the protocol is little help here.
     */
    if (pci->ci.hszServerApp == 0) {
        if (WinQueryWindowPtr(hwndServer, QWP_PFNWP) == ServerWndProc) {
            /*
             * one of ours! simple.
             */
            pci->ci.hszServerApp = (PAPPINFO)WinSendMsg(pci->ci.hwndPartner,
                    UM_QUERY, (MPARAM)Q_APPNAME, 0L);
        } else {
            /*
             * Try the psz in pddei.  Maybe the server set it properly
             * before returning it.
             */
            if (!(pci->ci.hszServerApp =
                    GetHsz(PSZAPP(pddei), pci->ci.cc.idCountry,
                            pci->ci.cc.usCodepage, TRUE))) {
                        
                PSZ pszT;
                USHORT cb;
                
                /*
                 * WORST CASE:
                 * Until a better way is found, we set the hszServerApp to
                 * the title of the frame window.
                 */
                if (pszT = FarAllocMem(pci->ci.pai->hheapApp,
                        cb = WinQueryWindowTextLength(pci->ci.hwndFrame) + 1)) {
                    WinQueryWindowText(pci->ci.hwndFrame, cb, (PSZ)pszT);
                    pci->ci.hszServerApp = GetHsz(pszT, pci->ci.cc.idCountry,
                            pci->ci.cc.usCodepage, TRUE);
                    FarFreeMem(pci->ci.pai->hheapApp, (PBYTE)pszT, cb);
                }
            }
        }
    }
    /*
     * Now what if the topic was wild?
     */
    if (pci->ci.hszTopic == 0) {
        if (WinQueryWindowPtr(hwndServer, QWP_PFNWP) == ServerWndProc) {
            /*
             * one of ours! simple.
             */
            pci->ci.hszTopic = (PAPPINFO)WinSendMsg(pci->ci.hwndPartner,
                    UM_QUERY, (MPARAM)Q_TOPIC, 0L);
        } else {
            /*
             * Try the psz in pddei.  Maybe the server set it properly
             * before returning it.  If this doesn't work were out of
             * luck, keep it wild.
             */
            pci->ci.hszServerApp = GetHsz(PSZAPP(pddei), pci->ci.cc.idCountry,
                    pci->ci.cc.usCodepage, TRUE);
        }                
    }
}



/***************************** Private Function ****************************\
* Processes a client transfer request issued by one of the ClientXfer
* functions.  This may be synchronous or asynchronous.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
MRESULT ClientXferReq(pXferInfo, pci, hwnd)
PXFERINFO pXferInfo;
PCLIENTINFO pci;
HWND hwnd;
{
    PCQDATA pcqd;
    MRESULT retVal;

    if (pXferInfo->ulTimeout == TIMEOUT_ASYNC) {
        /*
         * add a client queue item to track this transaction and return
         * the ID.
         */
        if (pci->pQ == NULL)
            pci->pQ = CreateQ(sizeof(CQDATA));
        if (pci->pQ == NULL) {
            pci->ci.pai->LastError = DMGERR_MEMORY_ERROR;
            return(0);
        }
        pcqd = (PCQDATA)Addqi(pci->pQ);
        if (pcqd == NULL) {
            pci->ci.pai->LastError = DMGERR_MEMORY_ERROR;
            return(0);
        }
        CopyBlock((PBYTE)pXferInfo, (PBYTE)&pcqd->XferInfo, sizeof(XFERINFO));
        pcqd->xad.state = CONVST_CONNECTED;
        pcqd->xad.pddes = 0;
        pcqd->xad.LastError = DMGERR_NO_ERROR;
        pcqd->xad.pXferInfo = &pcqd->XferInfo;
        /*
         * Get transaction started - if it fails, quit now.
         */
        if ((pcqd->xad.LastError = SendClientReq(&pcqd->xad,
                pci->ci.hwndPartner, hwnd, pci->ci.pai)) == DMGERR_SERVER_DIED) {
            pci->ci.fs = pci->ci.fs & ~ST_CONNECTED;
            Deleteqi(pci->pQ, MAKEID(pcqd));
            pci->ci.pai->LastError = DMGERR_SERVER_DIED;
            return(0);
        }
        return((MRESULT)MAKEID(pcqd));
    }
    
    /*
     * if not quiesent, yet synchronous, tell him were busy.
     * (this case could happen on a recursive call.)
     */
    if (pci->ci.xad.state != CONVST_CONNECTED) {
        pci->ci.pai->LastError = DMGERR_BUSY;
        return(0);
    }
    /*
     * Set this so messages comming in during the conversation know whats up
     */
    pci->ci.xad.pXferInfo = pXferInfo;
    
    if ((pci->ci.pai->LastError = SendClientReq(&pci->ci.xad,
            pci->ci.hwndPartner, hwnd, pci->ci.pai)) == DMGERR_SERVER_DIED) {
        pci->ci.fs = pci->ci.fs & ~ST_CONNECTED;
    }
    
    if (pci->ci.pai->LastError != DMGERR_NO_ERROR)
        return(0);
    /*
     *  reset the LastError here so we know if we had problems while
     *  in the modal loop.
     */
    pci->ci.pai->LastError = DMGERR_NO_ERROR;
    /*
     * timeout - modal loop.
     */
    if (!timeout(pci->ci.pai, pXferInfo->ulTimeout, hwnd)) {
        /*
         * reentrency or client has unregistered
         */
        return(0);
    }
    /*
     * check results - lasterror already set by timeout().
     * Synchronous conversation must be reset to quiesent by the time we
     * give up.
     */
    if (pci->ci.xad.state == CONVST_INCOMPLETE) {
        pci->ci.xad.state = CONVST_CONNECTED;
        return(0);
    }

    retVal = ClientXferRespond(pci, &pci->ci.xad, &pci->ci.pai->LastError);
    if (pci->ci.xad.state == CONVST_INCOMPLETE) 
        pci->ci.xad.state = CONVST_CONNECTED;
        
    return(retVal);
}



     
/***************************** Private Function ****************************\
* This routine sends the apropriate initiation messages for starting a
* client request according to the transaction data given.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
USHORT SendClientReq(pXad, hwndServer, hwnd, pai)     
PXADATA pXad;
HWND hwndServer;
HWND hwnd;
PAPPINFO pai;
{
    USHORT fsStatus = 0;
    USHORT msg;
    BOOL fCopy;
    PDDESTRUCT pddes;
    
    switch (pXad->pXferInfo->usType) {
    case XTYP_REQUEST:
        msg = WM_DDE_REQUEST;
        pXad->state = CONVST_REQSENT;
        fCopy = FALSE;
        break;

    case XTYP_POKE:
        msg = WM_DDE_POKE;
        pXad->state = CONVST_POKESENT;
        fCopy = TRUE;
        break;

    case XTYP_EXEC:
        msg = WM_DDE_EXECUTE;
        pXad->state = CONVST_EXECSENT;
        fCopy = TRUE;
        break;

    case XTYP_ADVSTART:
    case XTYP_ADVSTART | XTYPF_NODATA:
    case XTYP_ADVSTART | XTYPF_ACKREQ:
    case XTYP_ADVSTART | XTYPF_NODATA | XTYPF_ACKREQ:
        fsStatus = pXad->pXferInfo->usType & (DDE_FACKREQ | DDE_FNODATA);
        msg = WM_DDE_ADVISE;
        pXad->state = CONVST_ADVSENT;
        fCopy = FALSE;
        break;

    case XTYP_ADVSTOP:
        msg = WM_DDE_UNADVISE;
        pXad->state = CONVST_UNADVSENT;
        fCopy = FALSE;
        break;

    default:
        return(DMGERR_INVALIDPARAMETER);
        break;
    }
    
    /*
     * Send transfer
     */
    if ((pddes = AllocDDESel(fsStatus, pXad->pXferInfo->usFmt,
            pXad->pXferInfo->hszItem, fCopy ? pXad->pXferInfo->cb : 0, NULL))
            == 0) {
        pXad->state = CONVST_CONNECTED;
        return(DMGERR_MEMORY_ERROR);
    }

    if (fCopy)
        CopyHugeBlock((PBYTE)pXad->pXferInfo->pData, DDES_PABDATA(pddes),
                pXad->pXferInfo->cb);

    if (WinIsWindow(DMGHAB, hwndServer)) {
        if (!MyDdePostMsg(hwndServer, hwnd, msg, (PMYDDES)pddes, pai, MDPM_FREEHDATA)) {
            pXad->state = CONVST_CONNECTED;
            return(DMGERR_POSTMSG_FAILED);
        }
    } else {
        /*
         * We lost the server, we are TERMINATED arnold!
         */
        pXad->state = CONVST_TERMINATED;
        FreeData((PMYDDES)pddes, pai);
        return(DMGERR_SERVER_DIED);
    }
    return(DMGERR_NO_ERROR);
}





/***************************** Private Function ****************************\
* This handles client window processing of WM_DDE_ACK and WM_DDE_DATA msgs.
* On exit pddes is freed.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
void DoClientDDEmsg(pci, hwnd, msg, hwndFrom, pddes)
PCLIENTINFO pci;
HWND hwnd;
USHORT msg;
HWND hwndFrom;
PDDESTRUCT pddes;
{
    PCQDATA pqd;
    int i;
    HSZ hszItem;
    PADVLI pAdviseItem;
    
    /*
     * make sure its for us.
     */
    if (hwndFrom != pci->ci.hwndPartner || !(pci->ci.fs & ST_CONNECTED)) {
        FreeData((PMYDDES)pddes, pci->ci.pai);
        return;
    }
    /*
     * Check if it fits the synchronous transaction data
     */
    if (fExpectedMsg(&pci->ci.xad, pddes, msg, pci)) {
        if (AdvanceXaction(hwnd, pci, &pci->ci.xad, pddes, msg,
                &pci->ci.pai->LastError))
            WinPostMsg(hwnd, WM_TIMER, (MPARAM)TID_TIMEOUT, 0L);
        return;
    }
     
    /*
     * See if it fits any asynchronous transaction data - if any exist
     */
    if (pci->pQ != NULL && pci->pQ->pqiHead != NULL) {
        SemEnter();
        pqd = (PCQDATA)pci->pQ->pqiHead;
        /*
         * cycle from oldest to newest.
         */
        for (i = pci->pQ->cItems; i; i--) {
            pqd = (PCQDATA)pqd->next;
            if (!fExpectedMsg(&pqd->xad, pddes, msg, pci))
                continue;
            if (AdvanceXaction(hwnd, pci, &pqd->xad, pddes, msg,
                    &pqd->xad.LastError)) {
                ClientXferRespond(pci, &pqd->xad, &pqd->xad.LastError);
                SemLeave();
                MakeCallback(pci->ci.pai, hwnd, (HSZ)0L, (HSZ)0L, 0,
                        XTYP_XFERCOMPLETE, (HDMGDATA)MAKEID(pqd),
                        0, 0, hwndFrom);
                return;
            }
            SemLeave();
            return;
        }
        SemLeave();
    }
    /*
     * It doesn't fit anything, check for an advise data message.
     */
    if (msg == WM_DDE_DATA) {
        hszItem = GetHszItem((PMYDDES)pddes, &pci->ci.cc, TRUE);
        if (pAdviseItem = (PADVLI)FindAdvList(pci->ci.pAdviseList, hszItem,
                pddes->usFormat)) {
            MakeCallback(pci->ci.pai, (HCONV)hwnd, pci->ci.hszTopic,
                hszItem, pddes->usFormat, XTYP_ADVDATA, pddes, msg,
                pddes->fsStatus, pci->ci.hwndPartner);
        } else {
            FreeHsz(hszItem);
        }
        return;
    }
    /*
     * throw it away
     */
    FreeData((PMYDDES)pddes, pci->ci.pai);
    return;
}



/***************************** Private Function ****************************\
* This routine matches a conversation transaction with a DDE message.  If
* the state, usType, format, itemname dde structure data and the message
* received all agree, TRUE is returned.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
BOOL fExpectedMsg(pXad, pddes, msg, pci)
PXADATA pXad;
PDDESTRUCT pddes;
USHORT msg;
PCLIENTINFO pci;
{
    HSZ hsz = 0;
    BOOL fRet = FALSE;
    
    if (!(pXad->state > CONVST_INIT1 &&
            pddes->usFormat == pXad->pXferInfo->usFmt &&
            (hsz = GetHszItem((PMYDDES)pddes, &pci->ci.cc, TRUE)) ==
            pXad->pXferInfo->hszItem)) {
        goto Exit;
    }
    switch (pXad->state) {
    case CONVST_REQSENT:
        if (msg == WM_DDE_DATA && !(pddes->fsStatus & DDE_FRESPONSE))
            /*
             * Not data in response to a request!
             */
            break;
        fRet = (msg == WM_DDE_ACK || msg == WM_DDE_DATA);
        break;
        
    case CONVST_POKESENT:
    case CONVST_EXECSENT:
    case CONVST_ADVSENT:
    case CONVST_UNADVSENT:
        fRet = (msg == WM_DDE_ACK);
        break;
    }
    
Exit:    
    FreeHsz(hsz);
    return(fRet);
}



/***************************** Private Function ****************************\
* This function assumes that msg is an apropriate message for the transaction
* referenced by pXad.  It acts on msg as apropriate.  pddes is the DDESTRUCT
* associated with msg.
*
* Returns fSuccess ie: transaction is ready to close up.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
BOOL AdvanceXaction(hwnd, pci, pXad, pddes, msg, pErr)
HWND hwnd;
PCLIENTINFO pci;
PXADATA pXad;
PDDESTRUCT pddes;
USHORT msg;
PUSHORT pErr;
{
    switch (msg) {
    case WM_DDE_ACK:
        switch (pXad->state) {
        case CONVST_ADVSENT:
        case CONVST_EXECSENT:
        case CONVST_POKESENT:
        case CONVST_REQSENT:
        case CONVST_UNADVSENT:
            if (pddes->fsStatus & DDE_FACK) {
                /*
                 * handle successes
                 */
                switch (pXad->state) {
                case CONVST_POKESENT:
                    pXad->state = CONVST_POKEACKRCVD;
                    break;

                case CONVST_EXECSENT:
                    pXad->state = CONVST_EXECACKRCVD;
                    break;

                case CONVST_ADVSENT:
                    pXad->state = CONVST_ADVACKRCVD;
                    break;

                case CONVST_UNADVSENT:
                    pXad->state = CONVST_UNADVACKRCVD;
                    break;

                case CONVST_REQSENT:
                    /*
                     * requests are not expected to send a +ACK.  only
                     * -ACK or data.  We ignore a +ACK to a request.
                     */
                    FreeData((PMYDDES)pddes, pci->ci.pai);
                    return(FALSE);
                }
            } else {
                /*
                 * handle the expected ACK failures.
                 */
                *pErr = DMGERR_NOTPROCESSED;
                if (pddes->fsStatus & DDE_FBUSY)
                    *pErr = DMGERR_BUSY;
                pXad->state = CONVST_INCOMPLETE;
            }
        }
        FreeData((PMYDDES)pddes, pci->ci.pai);
        return(TRUE);
        break;

    case WM_DDE_DATA:
        switch (pXad->state) {
        case CONVST_REQSENT:
            /*
             * send an ack if requested - we dare not return the given
             * pddes because it may be a data item sent to several
             * clients and we would mess up the fsStatus word for
             * all processes involved.
             */
            if (pddes->fsStatus & DDE_FACKREQ) {
                MyDdePostMsg(pci->ci.hwndPartner, hwnd, WM_DDE_ACK,
                        (PMYDDES)AllocDDESel(DDE_FACK, pddes->usFormat,
                        pXad->pXferInfo->hszItem, 0L, NULL),
                        pci->ci.pai, MDPM_FREEHDATA);
            }
            pXad->state = CONVST_DATARCVD;
            /*
             * We do NOT free the selector here yet because it will be 
             * given to the client via pXad->pddes.
             */
            pXad->pddes = pddes;
            return(TRUE);
            break;
        }
    }
    return(FALSE);
}


    
/***************************** Private Function ****************************\
* This function assumes that a client transfer request has been completed -
* or should be completed by the time this is called.
*
* pci contains general client info
* pXad contains the transaction info
* pErr points to where to place the LastError code.
*
* Returns 0 on failure
* Returns TRUE or a Data Selector on success.
* On failure, the conversation is left in a CONVST_INCOMPLETE state.
* On success, the conversation is left in a CONVST_CONNECTED state.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
MRESULT ClientXferRespond(pci, pXad, pErr)
PCLIENTINFO pci;
PXADATA pXad;
PUSHORT pErr;
{
    if (pXad->state == CONVST_INCOMPLETE) 
        return(0);
        
    switch (pXad->pXferInfo->usType) {
    case XTYP_REQUEST:
        if (pXad->state != CONVST_DATARCVD) {
            if (*pErr == DMGERR_NO_ERROR)
                *pErr = DMGERR_DATAACKTIMEOUT;
            goto failexit;
        }
        pXad->state = CONVST_CONNECTED;
        return(pXad->pddes);
        break;

    case XTYP_POKE:
        if (pXad->state != CONVST_POKEACKRCVD) {
            if (*pErr == DMGERR_NO_ERROR)
                *pErr = DMGERR_POKEACKTIMEOUT;
            goto failexit;
        }
        pXad->state = CONVST_CONNECTED;
        return(TRUE);
        break;

    case XTYP_EXEC:
        if (pXad->state != CONVST_EXECACKRCVD) {
            if (*pErr == DMGERR_NO_ERROR)
                *pErr = DMGERR_EXECACKTIMEOUT;
            goto failexit;
        }
        pXad->state = CONVST_CONNECTED;
        return(TRUE);
        break;

    case XTYP_ADVSTART:
    case XTYP_ADVSTART | XTYPF_NODATA:
    case XTYP_ADVSTART | XTYPF_ACKREQ:
    case XTYP_ADVSTART | XTYPF_NODATA | XTYPF_ACKREQ:
        if (pXad->state != CONVST_ADVACKRCVD) {
            if (*pErr == DMGERR_NO_ERROR)
                *pErr = DMGERR_ADVACKTIMEOUT;
            goto failexit;
        }
        if (!AddAdvList(pci->ci.pAdviseList, pXad->pXferInfo->hszItem,
                pXad->pXferInfo->usType & (DDE_FACKREQ | DDE_FNODATA),
                pXad->pXferInfo->usFmt)) {
            pXad->state = CONVST_INCOMPLETE;
            pci->ci.pai->LastError = DMGERR_MEMORY_ERROR;
            return(FALSE);
        } else {
            pXad->state = CONVST_CONNECTED;
            pci->ci.fs |= ST_ADVISE;
            return(TRUE);
        }
        break;

    case XTYP_ADVSTOP:
        if (pXad->state != CONVST_UNADVACKRCVD) {
            if (*pErr == DMGERR_NO_ERROR)
                *pErr = DMGERR_UNADVACKTIMEOUT;
            goto failexit;
        }
        if (!DeleteAdvList(pci->ci.pAdviseList, pXad->pXferInfo->hszItem,
                pXad->pXferInfo->usFmt))
            pci->ci.fs &= ~ST_ADVISE;
        pXad->state = CONVST_CONNECTED;
        return(TRUE);
        break;

    }
    
failexit:
    pXad->state = CONVST_INCOMPLETE;
    return(0);
}






/*
 * ----------------------------SERVER SECTION--------------------------------
 */



/***************************** Public  Function ****************************\
* MRESULT EXPENTRY ServerWndProc(hwnd, msg, mp1, mp2)
* HWND hwnd;
* USHORT msg;
* MPARAM mp1;
* MPARAM mp2;
*
* DESCRIPTION:
*   This processes DDE conversations from the server end.
*   It stores internal information and acts much like a state machine.
*   If closed, it will automaticly abort any conversation in progress.
*   It also maintains an internal list of all items which currently are
*   in active ADVISE loops.
* PUBDOC START
*   These server windows have the feature that a conversation can be
*   re-initiated with them by a client.  The Client merely terminates
*   the conversation and then re-initiates by using a SendMsg to this
*   window.  This allows a client to change the topic of the conversation
*   or to pass the conversation on to another client window without
*   loosing the server it initiated with.   This is quite useful for
*   wild initiates.
* PUBDOC END
*
* History:
* 10/18/89 sanfords Added hack to make hszItem==0L when offszItem==offabData.
* 1/4/89   sanfords created 
\***************************************************************************/
MRESULT EXPENTRY ServerWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
#define PDDES ((PDDESTRUCT)mp2)    
    register PSERVERINFO psi;
    MPARAM mrData;
    PADVLI pAdviseItem;
    PSZ pszApp, pszTopic;
    HSZ hsz;
    USHORT cchApp, cchTopic;
    USHORT usType;
    HDMGDATA hDmgData = 0L;
    BOOL fResult;
    

    psi = (PSERVERINFO)WinQueryWindowULong(hwnd, QWL_USER);

    switch (msg) {
    case WM_DDE_REQUEST:
    case WM_DDE_ACK:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
        /*
         * only respond if this is for us.
         */
        if ((HWND)mp1 != psi->ci.hwndPartner || !(psi->ci.fs & ST_CONNECTED)) {
            FreeData((PMYDDES)mp2, psi->ci.pai);
            return(0);
        }
    }

    switch (msg) {
    case WM_CREATE: {
            PAPPINFO pai;
            
            /*
             * allocate and initialize the server window info.
             */
            pai = GetCurrentAppInfo(FALSE);
            SemEnter();

            if (!(psi = (PSERVERINFO)FarAllocMem(pai->hheapApp, sizeof(SERVERINFO))))
                goto LowMem;
            FillBlock((PBYTE)&psi->ci, sizeof(COMMONINFO), 0);
            if (!(psi->ci.pAdviseList = CreateLst(pai->hheapApp, sizeof(ADVLI)))) {
                FarFreeMem(pai->hheapApp, (PBYTE)psi, sizeof(SERVERINFO));
LowMem:                        
                pai->LastError = DMGERR_MEMORY_ERROR;
                SemLeave();
                return(1);          /* abort creation - low memory */
            }
            SemLeave();
            psi->ci.pai = pai;
            psi->ci.xad.state = CONVST_NULL;
            WinSetWindowULong(hwnd, QWL_USER, (ULONG)psi);
        }
        break;

    case UMSR_INITIATE:
        /*
         * This was sent by the subclassed frame of the server app.
         * The frame has already queried the server for permission
         * to create this window.
         *
         * If mp2 is NULL, this is a hot server window waiting for
         * a WM_DDE_INITIATE.
         */
#define pii ((PINITINFO)mp1)         
        IncHszCount(psi->ci.hszServerApp = pii->hszAppName);
        IncHszCount(psi->ci.hszTopic = pii->hszTopic);
        psi->ci.hwndPartner = (HWND)mp2;
        psi->ci.hwndFrame = FindFrame(psi->ci.hwndPartner);
        psi->ci.cc.fsContext = pii->pCC->fsContext;
        psi->ci.cc.idCountry = pii->pCC->idCountry;
        psi->ci.cc.usCodepage = pii->pCC->usCodepage;
        psi->ci.fs |= ST_CONNECTED;
        psi->ci.xad.state = CONVST_CONNECTED;
        
        SemEnter();
        pszApp = pszFromHsz(psi->ci.hszServerApp, &cchApp);
        pszTopic = pszFromHsz(psi->ci.hszTopic, &cchTopic);
        SemLeave();

        if (mp2)
            WinDdeRespond((HWND)mp2, hwnd, pszApp, pszTopic);
        
        SemEnter();
        FarFreeMem(hheapDmg, (PBYTE)pszApp, cchApp);
        FarFreeMem(hheapDmg, (PBYTE)pszTopic, cchTopic);
        SemLeave();
        
        return(1);
#undef pii        
        break;

    case WM_DDE_INITIATE:
        /*
         * This will happen when a client tries to re-initiate a conversation
         * with this server.  We allow about 10 seconds after termination
         * for another client to connect specifically with this window.
         * This allows a client to swap windows on its end of the conversation.
         */

        if (psi->ci.xad.state == CONVST_TERMINATED &&
                (psi->ci.hszServerApp == GetHsz(PSZAPP(mp2), psi->ci.cc.idCountry,
                        psi->ci.cc.usCodepage, FALSE))) {
                    
            WinStopTimer(DMGHAB, hwnd, TID_SELFDESTRUCT);
            hsz = psi->ci.hszTopic;
            psi->ci.hszTopic = GetHsz(PSZTOPIC(mp2), psi->ci.cc.idCountry,
                    psi->ci.cc.usCodepage, TRUE);
            FreeHsz(hsz);
            psi->ci.hwndPartner = (HWND)mp1;
            psi->ci.hwndFrame = FindFrame(psi->ci.hwndPartner);
            psi->ci.fs |= ST_CONNECTED;
            psi->ci.xad.state = CONVST_CONNECTED;
            if (WinQueryWindowPtr(psi->ci.hwndPartner, QWP_PFNWP) == ClientWndProc) 
                psi->ci.fs |= ST_INTRADLL;
            WinDdeRespond((HWND)mp1, hwnd, PSZAPP(mp2), PSZTOPIC(mp2));
            fResult = TRUE;
        } else
            fResult = FALSE;
            
        FreeData((PMYDDES)mp2, psi->ci.pai);
        return(fResult);
        break;

    case WM_DDE_TERMINATE:
        /*
         * only respond if this is for us.
         */
        if ((HWND)mp1 != psi->ci.hwndPartner) 
            break;
        /* fall through */

    case UMSR_TERMINATE:
        /*
         * terminates any conversation in progress
         * Note that we keep around all the other conversation data so
         * a later re-connection is possible.
         */
        if (psi->ci.fs & ST_CONNECTED) {
            psi->ci.fs &= ~ST_CONNECTED;
            psi->ci.xad.state = CONVST_TERMINATED;
            if (WinIsWindow(DMGHAB, psi->ci.hwndPartner))
                WinDdePostMsg(psi->ci.hwndPartner, hwnd, WM_DDE_TERMINATE, 0L, FALSE);
        }
        if (psi->ci.fs & ST_ADVISE) {
            FlushLst(psi->ci.pAdviseList);
            psi->ci.fs &= ~ST_ADVISE;
        }
        /*
         * Mr. Phelps, if this window isn't reconnected within 10 odd
         * seconds, it will self-destruct.  This gives the client time
         * to reconnect with another client window.  This often happens
         * with wild initiates.
         */
        WinStartTimer(DMGHAB, hwnd, TID_SELFDESTRUCT, 0xa000);
        break;

    case WM_TIMER:
        if (LOUSHORT(mp1) == TID_SELFDESTRUCT && !(psi->ci.fs & ST_CONNECTED))
            DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        SemCheckOut();
        /*
         * Send ourselves a terminate and free local data.
         */
        WinSendMsg(hwnd, UMSR_TERMINATE, 0L, 0L);
        MakeCallback(psi->ci.pai, hwnd, psi->ci.hszTopic, (HSZ)NULL, 0, XTYP_TERM,
                0L, 0, 0, psi->ci.hwndPartner);
        SemEnter();
        DestroyLst(psi->ci.pAdviseList);
        FreeHsz(psi->ci.hszServerApp);
        FreeHsz(psi->ci.hszTopic);
        FarFreeMem(psi->ci.pai->hheapApp, (PBYTE)psi, sizeof(SERVERINFO));
        SemLeave();
        break;

    case WM_DDE_REQUEST:
        usType = XTYP_REQUEST;
        goto Callback;
        
    case WM_DDE_EXECUTE:
        usType = XTYP_EXEC;
        hDmgData = mp2;
        goto Callback;
        
    case WM_DDE_POKE:
        usType = XTYP_POKE;
        hDmgData = mp2;
        goto Callback;
        
    case WM_DDE_ADVISE:
        usType = XTYP_ADVSTART; /* set ST_ADVISE AFTER app oks advise loop */
        goto Callback;
        
    case WM_DDE_UNADVISE:
        /*
         * Terminate the advise now, but notify the server in callback so
         * messages don't get out of order.
         */
        if (!DeleteAdvList(psi->ci.pAdviseList,
                GetHszItem(mp2, &psi->ci.cc, FALSE),
                PDDES->usFormat))
            psi->ci.fs &= ~ST_ADVISE;
        usType = XTYP_ADVSTOP;
        goto Callback;
        
    case WM_DDE_ACK:
        /*
         * This is an ack in response to the FACKREQ bit being set.
         * See if this refers to one of the advise loops.
         */
        if ((pAdviseItem = FindAdvList(psi->ci.pAdviseList,
                GetHszItem(mp2, &psi->ci.cc, FALSE),
                PDDES->usFormat)) &&
                (pAdviseItem->fsStatus & DDE_FACKREQ)) {
            /*
             * Update advise loop status - no longer waiting for an ack.
             */
            pAdviseItem->fsStatus &= ~ADVST_WAITING;
            if (pAdviseItem->fsStatus & ADVST_CHANGED) {
                pAdviseItem->fsStatus |= ADVST_POSTED; 
                /*
                 * The client is out of date.  Send the data
                 * again (simulate a post advise call).
                 * Don't bother the server with ACK info.
                 */
                MakeCallback(psi->ci.pai, (HCONV)hwnd, psi->ci.hszTopic,
                        pAdviseItem->hszItem, 
                        pAdviseItem->usFmt, XTYP_ADVREQ, 
                        0L, UMSR_POSTADVISE,
                        pAdviseItem->fsStatus & ~DDE_FRESERVED,
                        psi->ci.hwndPartner);
                FreeData((PMYDDES)mp2, psi->ci.pai);
                return(0);
            }
        }
        usType = XTYP_ACK;
        hDmgData = PDDES->fsStatus;
        
Callback:        
        MakeCallback(psi->ci.pai, (HCONV)hwnd, psi->ci.hszTopic,
#if 0
                /*
                 * hack for EXCEL which makes its items and data equal for
                 * execute acks which SHOULD use NULL as the item name.
                 */
                (PDDES->offszItemName == PDDES->offabData) ?
                    0L :
#endif
                GetHszItem((PMYDDES)mp2, &psi->ci.cc, TRUE),
                PDDES->usFormat, usType,
                hDmgData, msg, PDDES->fsStatus,
                psi->ci.hwndPartner);
        /*
         * now free the incomming selector IF it wasn't passed on to
         * MakeCallback as hDmgData.
         */
        if (hDmgData != mp2)
            FreeData((PMYDDES)mp2, psi->ci.pai);
        break;
        
    case UMSR_POSTADVISE:
        /*
         * This message came from DdePostAdvise()
         *
         * Advise loops are tricky because of the desireable FACKREQ feature
         * of DDE.  The advise loop list holds information in its fsStatus
         * field to maintain the state of the advise loop.
         *
         * if the ADVST_POSTED bit is set, it means that the server already
         * has an ADVREQ message in its callback queue.  This prevents
         * unnecessary ADVREQ messages from getting thrown into the callback
         * queue.
         *
         * if the ADVST_WAITING bit is set, the server is still waiting for
         * the client to give it the go-ahead for more data with an
         * ACK message on this item. (FACKREQ is set)  Without a go-ahead,
         * the server will not send any more advise data to the client but
         * will instead set the ADVST_CHANGED bit which will cause another
         * WM_DDE_DATA message to be sent to the client as soon as the
         * go-ahead ACK is received.  This keeps the client up to date
         * but never overloads it.
         */

        if (!(psi->ci.fs & ST_ADVISE) ||
                !(pAdviseItem = FindAdvList(psi->ci.pAdviseList, (HSZ)mp1, 0)))
            break;

        do {
            /*
             * for each format for this item that has an advise loop:
             */
            if (pAdviseItem->fsStatus & ADVST_POSTED)
                continue;
                
            if ((pAdviseItem->fsStatus & DDE_FACKREQ) &&
                    (pAdviseItem->fsStatus & ADVST_WAITING)) {
                /*
                 * if the client has not yet finished with the last data
                 * we gave him, just update the advise loop status
                 * instead of sending data now.
                 */
                pAdviseItem->fsStatus |= ADVST_CHANGED;
                continue;
            }
            if (pAdviseItem->fsStatus & DDE_FNODATA) {
                /*
                 * In the nodata case, we don't bother the server.  Just
                 * pass the client an apropriate DATA message.
                 */
                MyDdePostMsg(psi->ci.hwndPartner, hwnd, WM_DDE_DATA,
                        (PMYDDES)AllocDDESel(pAdviseItem->fsStatus & ~(DDE_FNODATA | DDE_FACKREQ),
                        pAdviseItem->usFmt, (HSZ)mp1, 0L, 0),
                        psi->ci.pai, MDPM_FREEHDATA);
                continue;
            }
            /*
             * Otherwise, lets get the data from the server.
             */
            pAdviseItem->fsStatus |= ADVST_POSTED;
            MakeCallback(psi->ci.pai, (HCONV)hwnd, psi->ci.hszTopic,
                    (HSZ)mp1, pAdviseItem->usFmt, XTYP_ADVREQ,
                    0, UMSR_POSTADVISE,
                    pAdviseItem->fsStatus & (DDE_FACKREQ | DDE_FNODATA),
                    psi->ci.hwndPartner);
        } while (pAdviseItem = FindNextAdv(pAdviseItem, (HSZ)mp1));
        break;

    case UM_QUERY:
        /*
         * LOUSHORT(mp1) = info index.
         * mp2 = pData.     If pData==0, return data else copy into pData.
         */
        switch (LOUSHORT(mp1)) {
        case Q_STATUS:
             mrData = (MRESULT)psi->ci.fs;
             break;

        case Q_CLIENT:
             mrData = FALSE;
             break;

        case Q_APPINFO:
             mrData = psi->ci.pai;
             break;
              
        case Q_APPNAME:
             mrData = psi->ci.hszServerApp;
             break;

        case Q_TOPIC:
             mrData = psi->ci.hszTopic;
             break;
             
        case Q_ALL:
             mrData = (MRESULT)(SERVERINFO FAR *)psi;
             break;
        }
        if (mp2 == 0)
            return(mrData);
        else
            *(MRESULT FAR *)mp2 = mrData;
        return(1);
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
#undef PDDES    
}




/*
 * This assumes hwnd is a DDE window.  It tries to locate the proper
 * top-level frame window that this window is associated with useing
 * process and thread IDs.
 */
HWND FindFrame(
HWND hwnd)
{
    PID pid, pidFrame;
    TID tid, tidFrame;
    HWND hwndMaybe = NULL;
    HWND hwndBetter = NULL;
    HWND hwndFrame;
    HENUM hEnum;
    ULONG ul;
    
    WinQueryWindowProcess(hwnd, &pid, &tid);
    hEnum = WinBeginEnumWindows(HWND_DESKTOP);
    while (hwndFrame = WinGetNextWindow(hEnum)) {
        /*
         * for all top level windows ...
         */
        ul = (ULONG)WinSendMsg(hwndFrame, WM_QUERYFRAMEINFO, 0L, 0L);
        if (FI_FRAME & ul) {
            /*
             * that are frames ...
             */
            WinQueryWindowProcess(hwndFrame, &pidFrame, &tidFrame);
            if (pidFrame == pid) {
                /*
                 * in this process - maybe - ...
                 */
                hwndMaybe = hwndFrame;
                if (tidFrame == tid) {
                    /*
                     * in this thread - better - ...
                     */
                    hwndBetter = hwndFrame;
                    if (WinQueryWindowPtr(hwndFrame, QWP_PFNWP) ==
                            subframeWndProc) {
                        /*
                         * that are subclassed by us - certainly!
                         */
                        hwndBetter = hwndFrame;
                        break;
                    }
                }
            }
        }
    }
    WinEndEnumWindows(hEnum);
    return(hwndBetter ? hwndBetter : hwndMaybe);
}




/***************************** Private Function ****************************\
* This routine handles server message replys.  This may have been called
* immediately in the case of enabled callbacks, or may have been called
* via DdeEnableCallback in which case the server action has been
* delayed.  QReply is responsible for freeing the pddes given as well as
* the pcbi->hDmgData and pcbi->hszItem.
*
* History:
*   Created     9/12/89    Sanfords
*   6/12/90 sanfords    Added checks for HDATA ownership.
\***************************************************************************/
void QReply(pcbi, pddes)
PCBLI pcbi;
PDDESTRUCT pddes;   /* hDataRet */
{
    PSERVERINFO psi;
    PADVLI pAdviseItem;
    USHORT fsStatus, msg;

    if ((pcbi->usType & XCLASS_MASK) == XCLASS_NOTIFICATION)
        return;

    SemCheckOut();
    psi = WinSendMsg(pcbi->hConv, UM_QUERY, (MPARAM)Q_ALL, 0L);
    
    switch (pcbi->msg) {
    case UMSR_POSTADVISE:
        /*
         * The NODATA case never gets here.
         */
        if ((psi) &&
                (pAdviseItem = FindAdvList(psi->ci.pAdviseList, pcbi->hszItem,
                pcbi->usFmt))) {
            pAdviseItem->fsStatus &= ~ADVST_POSTED;
            if (pddes) {
                pAdviseItem->fsStatus &= ~ADVST_CHANGED;
                MyDdePostMsg(pcbi->hConvPartner, pcbi->hConv, WM_DDE_DATA,
                        (PMYDDES)pddes, psi->ci.pai, MDPM_FREEHDATA);
                if (pAdviseItem->fsStatus & DDE_FACKREQ) 
                    pAdviseItem->fsStatus |= ADVST_WAITING;
            }
        }
        break;
            
    case WM_DDE_REQUEST:
        if (pddes) {
            pddes->fsStatus = (pcbi->fsStatus & DDE_FACKREQ) | DDE_FRESPONSE;
            msg = WM_DDE_DATA;
        } else {
            /*
             * send a -ACK
             */
            pddes = AllocDDESel(((USHORT)pddes & DDE_FAPPSTATUS) |
                    ((USHORT)pddes & DDE_FBUSY ? DDE_FBUSY : DDE_NOTPROCESSED),
                    pcbi->usFmt, pcbi->hszItem, 0L, NULL);
            msg = WM_DDE_ACK;
        }
        MyDdePostMsg(pcbi->hConvPartner, pcbi->hConv, msg, (PMYDDES)pddes,
                psi->ci.pai, MDPM_FREEHDATA);
        break;
        
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
        /*
         * pddes is supposed to be the proper DDE_ constants to return.
         * we just stick them in the given pddes (hDmgData) and return
         * it as an ACK.  This frees pcbi->hDmgData in the process.
         */
        ((PDDESTRUCT)pcbi->hDmgData)->fsStatus = 
                (USHORT)pddes & ~DDE_FRESERVED;
        MyDdePostMsg(pcbi->hConvPartner, pcbi->hConv, WM_DDE_ACK,
                (PMYDDES)pcbi->hDmgData, psi->ci.pai, MDPM_FREEHDATA);
        break;
        
    case WM_DDE_ADVISE:
        /*
         * pddes is fStartAdvise
         * If DDE_FACK, we add the item to the advise loop
         * list and +ACK else we -ACK.
         */
        if ((BOOL)pddes) {
            psi = (PSERVERINFO)WinQueryWindowULong(pcbi->hConv, QWL_USER);
            if (AddAdvList(psi->ci.pAdviseList, pcbi->hszItem,
                    pcbi->fsStatus & (DDE_FNODATA | DDE_FACKREQ),
                    pcbi->usFmt) == NULL) {
                psi->ci.pai->LastError = DMGERR_MEMORY_ERROR;
                fsStatus = DDE_NOTPROCESSED;
            } else {
                psi->ci.fs |= ST_ADVISE;
                fsStatus = DDE_FACK;
            }
        } else {
            fsStatus = DDE_NOTPROCESSED;
        }
        goto AckBack;
        break;

    case WM_DDE_UNADVISE:
        fsStatus = DDE_FACK;
        goto AckBack;
        break;
    
    case WM_DDE_DATA:
        /*
         * must be an advise data item for the CLIENT or maybe some requested
         * data mistakenly sent here due to the client queue being flushed.
         * pddes is fsStatus.
         *
         * send an ack back if requested.
         */
        if (pcbi->fsStatus & DDE_FACKREQ) {
            /*
             * Clean up the status incase the app is messed up.
             */
            fsStatus = (USHORT)pddes & ~DDE_FRESERVED;
            if (fsStatus & (DDE_NOTPROCESSED | DDE_FBUSY))
                fsStatus &= ~DDE_FACK;
AckBack:
            MyDdePostMsg(pcbi->hConvPartner, pcbi->hConv, WM_DDE_ACK,
                (PMYDDES)AllocDDESel(fsStatus, pcbi->usFmt, pcbi->hszItem, 0L, 0),
                psi->ci.pai, MDPM_FREEHDATA);
        }
        break;
    }
}



/*
 * ----------------FRAME SECTION------------------
 *
 * A frame window exists on behalf of every registered thread.  It
 * handles conversation initiation and therefore issues callbacks
 * to the server app as needed to notify or query the server app.
 * The callback queue is always bypassed for these synchronous
 * events.
 */

/***************************** Private Function ****************************\
* MRESULT EXPENTRY subframeWndProc(hwnd, msg, mp1, mp2)
* HWND hwnd;
* USHORT msg;
* MPARAM mp1;
* MPARAM mp2;
*
* This routine takes care of setting up server windows as needed to respond
* to incomming WM_DDE_INTIIATE messages.  It is subclassed from the top
* level frame of the server application.
*
* History:  created 12/20/88    sanfords
\***************************************************************************/
MRESULT EXPENTRY subframeWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    PAPPINFO pai;

    pai = GetCurrentAppInfo(FALSE);
    
    switch (msg) {
    case UM_REGISTER:
    case UM_UNREGISTER:
        /*
         * we pass notification messages through this proc so we can make the
         * xfer call within the correct thread's context.
         */
        MakeCallback(pai, (HCONV)0, (HSZ)0, (HSZ)mp1, 0,
                msg == UM_REGISTER ? XTYP_REGISTER : XTYP_UNREGISTER,
                (HDMGDATA)mp2, msg, 0, 0L);
        return(0);
        break;

    case WM_DDE_INITIATE:
        FrameInitConv((HWND)mp1, (PDDEINIT)mp2);
        FreeData((PMYDDES)mp2, pai);
        break;

    default:
        return((*lpfnFrameWndProc)(hwnd, msg, mp1, mp2));
        break;
    }
}



void FrameInitConv(hwndClient, pddei)
HWND hwndClient;
PDDEINIT pddei;
{
    PAPPINFO pai, paiClient;
    INITINFO ii;
    HSZPAIR hp[2];
    PHSZPAIR php;
    HSZ hsz = 0;
    HDMGDATA hDataCC;
    PDDESTRUCT pddes;
    HWND hwndServer;
    CONVCONTEXT cc;
    BOOL fWild;

    if (!CheckSel(SELECTOROF(pddei))) {
        AssertF(FALSE, "Invalid DDEINIT selector");
        return;
    }
    
    SemCheckOut();
    
    pai = GetCurrentAppInfo(FALSE);
    /*
     * If we are filtering and no app names are registered, quit.
     */
    if ((pai->afCmd & DMGCMD_FILTERINITS) &&
            QPileItemCount(pai->pAppNamePile) == 0) 
        return;
        
    /*
     * filter out inits from ourselves and other agents (if we are an agent)
     */
    if (WinQueryWindowPtr(hwndClient, QWP_PFNWP) == ClientWndProc) {
        paiClient = WinSendMsg(hwndClient, UM_QUERY, (MPARAM)Q_APPINFO, 0L);
        if (paiClient == pai)
            return;
            
        if ((pai->afCmd & DMGCMD_AGENT) && (paiClient->afCmd & DMGCMD_AGENT)) 
            return;
    }

    /*
     * make sure ii.pCC is set up right.
     */
    if (pddei->cb >= sizeof(DDEINIT) && pddei->offConvContext) {
        /*
         * new dde init structure!
         */
        ii.pCC = DDEI_PCONVCONTEXT(pddei);
    } else {
        ii.pCC = &cc;
        cc.cb = sizeof(CONVCONTEXT);
        cc.idCountry = syscc.country;
        cc.usCodepage = syscc.codepage;
        cc.fsContext = 0;
    }


    hp[0].hszApp = GetHsz(PSZAPP(pddei), ii.pCC->idCountry,
            ii.pCC->usCodepage, TRUE);
            
    /*
     * filter out unwanted app names.
     */
    if (hp[0].hszApp && (pai->afCmd & DMGCMD_FILTERINITS) &&
            !FindPileItem(pai->pAppNamePile, CmppHsz, (PBYTE)&hp[0].hszApp, 0)) {
        FreeHsz(hp[0].hszApp);
        return;    
    }

    hp[0].hszTopic = GetHsz(PSZTOPIC(pddei), ii.pCC->idCountry,
            ii.pCC->usCodepage, TRUE);
            
    hp[1].hszApp = hp[1].hszTopic = 0L;

    fWild = (hp[0].hszApp == 0L || hp[0].hszTopic == 0);

    hDataCC = PutData((PBYTE)ii.pCC, (ULONG)sizeof(CONVCONTEXT), 0L, (HSZ)NULL,
            0, 0, pai);

    if (hDataCC == NULL)
        goto CheckOut;
        
    pddes = (PDDESTRUCT)DoCallback(pai, NULL, hp[0].hszTopic,
                    hp[0].hszApp, 0, (fWild ? XTYP_WILDINIT : XTYP_INIT),
                    hDataCC);

    if (pddes == NULL)
        goto CheckOut;

    FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&hDataCC, FPI_DELETE);
    DosFreeSeg(SELECTOROF(hDataCC));
    
    if (fWild) {
        php = (PHSZPAIR)DDES_PABDATA(pddes);
    } else {
        php = &hp[0];
        pddes = NULL;
    }
        
    /*
     * now php points to a 0 terminated list of hszpairs to respond to.
     */
    SemEnter();
    while (QuerylatomLength((LATOM)php->hszApp) &&
            QuerylatomLength((LATOM)php->hszTopic)) {
        SemLeave();
        if ((hwndServer = CreateServerWindow(pai, php->hszTopic)) == 0)
            break;
        /*
         * have the server respond
         */
        ii.hszAppName = php->hszApp;
        ii.hszTopic = php->hszTopic;
        WinSendMsg(hwndServer, UMSR_INITIATE, (MPARAM)&ii, hwndClient);

        /*
         * confirm initialization to server app
         */
        DoCallback(pai, (HCONV)hwndServer, php->hszTopic, php->hszApp,
                0, XTYP_INIT_CONFIRM, 0L);
            
        php++;
        SemEnter();
    }
    SemLeave();
    SemCheckOut();
CheckOut:    
    FreeHsz(hp[0].hszApp);
    FreeHsz(hp[0].hszTopic);
    if (fWild)
        FreeData((PMYDDES)pddes, pai);
}

HWND CreateServerWindow(
PAPPINFO pai,
HSZ hszTopic)
{
    HWND hwndTSvr, hwndServer;
    
    SemCheckOut();
    /*
     * locate or make a Topic server window...
     */
    if ((hwndTSvr =
            HwndFromHsz(hszTopic, pai->pSvrTopicList)) == 0) {
        /*
         * NO - make one.
         */
        if ((hwndTSvr = WinCreateWindow(pai->hwndDmg, SZDEFCLASS, "", 0L,
                0, 0, 0, 0, (HWND)NULL, HWND_BOTTOM, WID_SVRTOPIC,
                0L, 0L)) == 0L) {
            pai->LastError = DMGERR_PMWIN_ERROR;
            return(NULL);
        }
        AddHwndHszList(hszTopic, hwndTSvr, pai->pSvrTopicList);
    }
    
    /*
     * Create the server window
     */
    if ((hwndServer = WinCreateWindow(hwndTSvr, SZSERVERCLASS, "", 0L,
            0, 0, 0, 0, (HWND)NULL, HWND_BOTTOM, WID_SERVER, 0L, 0L)) == 0L) {
        pai->LastError = DMGERR_PMWIN_ERROR;
        return(NULL);
    }
    return(hwndServer);
}

/*
 * main application window - parent of all others in app.
 *
 * 6/12/90 sanfords     Fixed semaphore bug
 */
MRESULT EXPENTRY DmgWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
#define pai ((PAPPINFO)mp1)
    PCBLI pli, pliNext;
    BOOL fException;
    HDMGDATA hDataRet;

    hwnd;
    mp2;
        
    switch (msg) {
    case UM_CHECKCBQ:
        /*
         * We consider everything to be blocked if we are in a client
         * transfer modal loop.   This prevents recursive timeout
         * calls.
         */
        if (pai->hwndTimer)
            return(0);
            
        /*
         * This is where we actually do callbacks.  We do them via this
         * window proc so that we can asynchronously institute callbacks
         * via a PostMsg().
         */
        SemCheckOut();
        SemEnter();
        /*
         * process all enabled conversation callbacks.
         */
        for (pli = (PCBLI)pai->plstCB->pItemFirst; pli; pli = (PCBLI)pliNext) {
            pliNext = (PCBLI)pli->next;
            fException = FindLstItem(pai->plstCBExceptions, CmpULONG, (PLITEM)pli)
                    == NULL ? FALSE : TRUE;
            if (fException == pai->fEnableCB)
                continue; /* blocked */

            pai->cInCallback++;
            SemLeave();
            /*
             * make the actual callback here.
             */
            hDataRet = DoCallback(pai, pli->hConv, pli->hszTopic,
                    pli->hszItem, pli->usFmt, pli->usType, pli->hDmgData);
            SemEnter();
            if (pai->cInCallback > 0)   /* test incase exlst processing messed it up */
                pai->cInCallback--;

            /*
             * If the callback resulted in a BLOCK, disable this conversation.
             */
            if (hDataRet == CBR_BLOCK && !(pli->usType & XTYPF_NOBLOCK)) {
                SemLeave();
                DdeEnableCallback(pli->hConv, FALSE);
                SemEnter();
                continue;
            } else {
                /*
                 * otherwise finish processing the callback.
                 */
                if (WinIsWindow(DMGHAB, pli->hConvPartner)) {
                    SemLeave();
                    QReply(pli, (PDDESTRUCT)hDataRet);
                    SemEnter();
                }
                RemoveLstItem(pai->plstCB, (PLITEM)pli);
            }
        }
        SemLeave();
        return(0);
        break;

    default:
        WinDefWindowProc(hwnd, msg, mp1, mp2);
        break;
    }
#undef pai
}


HDMGDATA DoCallback(
PAPPINFO pai,
HCONV hConv,
HSZ hszTopic,
HSZ hszItem,
USHORT usFmt,
USHORT usType,
HDMGDATA hDmgData)
{
    HDMGDATA hDataRet;
    
    AssertF(IncHszCount(hszTopic) && FreeHsz(hszTopic), "Bad hszTopic on callback");
    AssertF(IncHszCount(hszItem) && FreeHsz(hszItem), "Bad hszItem on callback");

    if (usType & XCLASS_DATAIN) {
        AssertF(CheckSel(SELECTOROF(hDmgData)), "invalid callback data handle");
        ((PMYDDES)hDmgData)->fs |= HDATA_READONLY;
    }
    
#ifdef CRUISER    
    if (pai->afCmd & DMGCMD_32BIT)
        hDataRet = ThkCallback(hConv, hszTopic, hszItem, usFmt, usType, hDmgData,
                pai->pfnCallback);
    else
#endif    
        hDataRet = (*pai->pfnCallback)(hConv, hszTopic, hszItem, usFmt, usType,
                hDmgData);

    if (usType & XCLASS_DATA && CheckSel(SELECTOROF(hDataRet)) > sizeof(MYDDES) &&
            ((PMYDDES)hDataRet)->magic == MYDDESMAGIC) {
        if (((PMYDDES)hDataRet)->pai != pai) {
            AssertF(FALSE, "hData from callback not created by same thread");
            pai->LastError = DMGERR_DLL_USAGE;
            hDataRet = NULL;
        }
    }
    return(hDataRet);
}


