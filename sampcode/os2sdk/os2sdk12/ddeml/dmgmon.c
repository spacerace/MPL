/****************************** Module Header ******************************\
* Module Name: DMGMON.C
*
* This module contains functions used for DDE monitor control.
*
* Created:  8/2/88    sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "ddemlp.h"
#define MSGF_DDEPOSTMSG     3

#define freeMonStr(psz) MyFreeMem(hheapDmg, (NPBYTE)(USHORT)psz, MAX_MONITORSTR + 1)


BOOL EXPENTRY DdeSendHookProc(hab, psmh, fInterTask)
HAB hab;
PSMHSTRUCT psmh;
BOOL fInterTask;
{
    PSZ psz;
    PSZ pszSave;
    PSZ pszLast;

    UNUSED hab;
    UNUSED fInterTask;
    
    if (psmh->msg == WM_DDE_INITIATE || psmh->msg == WM_DDE_INITIATEACK) {
        if (allocMonStr(&pszSave, &pszLast)) {
            psz = timestamp(pszSave, pszLast);
            psz = lstrcat(psz, " ", pszLast);
            psz = ltoa((ULONG)psmh->mp1, psz, pszLast);
            psz = lstrcat(psz, " -> ", pszLast);
            psz = ltoa((ULONG)psmh->hwnd, psz, pszLast);
            psz = lstrcat(psz, "\n\r", pszLast);
            psz = ddeMsgToPsz(psmh->msg, psz, pszLast);
            psz = pddesToPsz(psmh->msg, (PDDESTRUCT)psmh->mp2, psz, pszLast);
            psz = lstrcat(psz, ")\n\r", pszLast);
            MonitorBroadcast(pszSave);
            freeMonStr(pszSave);
        }
    }
    return(FALSE);
}




BOOL EXPENTRY DdePostHookProc(hab, pqmsg, fs)
HAB hab;
PQMSG pqmsg;
USHORT fs;
{
    PSZ psz;
    PSZ pszSave;
    PSZ pszLast;

    UNUSED hab;

    if (fs && pqmsg->msg >= WM_DDE_FIRST && pqmsg->msg <= WM_DDE_LAST) {
        pszLast = psz + MAX_MONITORSTR;  
        if (allocMonStr(&pszSave, &pszLast)) {
            psz = timestamp(pszSave, pszLast);
            psz = lstrcat(psz, " ", pszLast);
            psz = ltoa((ULONG)pqmsg->mp1, psz, pszLast);
            psz = lstrcat(psz, " -> ", pszLast);
            psz = ltoa((ULONG)pqmsg->hwnd, psz, pszLast);
            psz = lstrcat(psz, "\n\r", pszLast);
            psz = ddeMsgToPsz(pqmsg->msg, psz, pszLast);
            psz = pddesToPsz(pqmsg->msg, (PDDESTRUCT)pqmsg->mp2, psz, pszLast);
            psz = lstrcat(psz, ")\n\r", pszLast);
            MonitorBroadcast(pszSave);
            freeMonStr(pszSave);
        }
    }
    return(FALSE);
}

/*
 * This guy sends a UM_MONITOR to all the monitor windows (up to MAX_MONITOR)
 * The cheap restriction is due to needing to not be in the semaphore
 * while the monitor is in control yet needing to keep access to pai in
 * the semaphore.
 */
void MonitorBroadcast(psz)
PSZ psz;
{
    HWND hwnd[MAX_MONITORS];
    PAPPINFO pai;
    register USHORT i = 0;

    SemCheckOut();
    SemEnter();
    pai = pAppInfoList;
    while (pai && i < cMonitor && i < MAX_MONITORS) {
        if (pai->hwndMonitor) {
            hwnd[i] = pai->hwndMonitor;
            i++;
        }
        pai = pai->next;
    }
    SemLeave();
    
    for (i = 0; i < cMonitor; i++)
        WinSendMsg(hwnd[i], UM_MONITOR, (MPARAM)psz, 0L);
}


/*
 * We need to allocate the string buffer so that recursive calls will work.
 * We also need to do this because the DLL DS is shared between all potential
 * monitor processes.
 *
 * This also initializes the psz for us with a null terminator and checks
 * cMonitor for us.  If this fails, no monitor action is done.
 *
 * ppsz will contain a pointer to the begining of the allocated buffer.
 * ppszLast will contain a pointer to the end of the allocated buffer.
 */
BOOL allocMonStr(ppsz, ppszLast)
PSZ far *ppsz;
PSZ far *ppszLast;
{
    SemEnter();
    if (cMonitor == 0 ||
            ((*ppsz = FarAllocMem(hheapDmg, MAX_MONITORSTR + 1)) == NULL)) {
        SemLeave();
        return(FALSE);
    }
    *ppszLast = *ppsz + MAX_MONITORSTR;
    **ppsz = '\0';
    SemLeave();
    return(TRUE);
}



MRESULT EXPENTRY MonitorWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
register MPARAM mp1;
MPARAM mp2;
{
    PAPPINFO pai;
    HDMGDATA hDmgData;

    pai = GetCurrentAppInfo(FALSE);

    switch (msg) {
    case WM_CREATE:
        mp1  = (PSZ)"\n\rMonitor Created\n\r\n\r";
        goto MonOut;
        break;

    case WM_DESTROY:
        mp1 = (PSZ)"\n\r\n\rMonitor Destroyed\n\r";
        /* fall through */

    case UM_MONITOR:
        /*
         * mp1 = psz to print
         */
MonOut:
        hDmgData = DdePutData((PSZ)mp1, (ULONG)(lstrlen(mp1) + 1),
                0L, (HSZ)0L, DDEFMT_TEXT, 0);
        pai->cInCallback++;
        DoCallback(pai, 0, 0, 0, DDEFMT_TEXT, XTYP_MONITOR, hDmgData);
        if (pai->cInCallback > 0)   /* test incase exitlist processing messed things up */
            pai->cInCallback--;
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }

    return(0);
}


