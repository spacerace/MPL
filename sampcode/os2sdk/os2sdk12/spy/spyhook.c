/***********************************************************************\
* spyhook.c - Spy global input hook interface library
* Created by Microsoft Corporation, 1989
*
* This dynlink is needed because global input hooks must reside in a DLL.
\***********************************************************************/
#define INCL_DOSPROCESS
#define	INCL_WINHEAP
#define	INCL_WINHOOKS
#define	INCL_WINWINDOWMGR
#include <os2.h>
#include "spyhook.h"

ULONG spyhookSem = 0L;          /* global semaphore for this module */
HMODULE hmodSpy;                /* dynlink module handle            */
SHORT    SpybHooks;             /* Which hook do we use?            */

HAB habOwner = (HAB)0;          /* Hook owner's anchor block        */
HMQ hmqOwner = (HMQ)0;          /* Hook owner's message queue       */
BOOL fRecording = FALSE;        /* Are we recording now?            */
BOOL fAnythingToWatch = FALSE;  /* Is there anything to watch?      */
BOOL fDispMsgsNotInList = TRUE; /* Disp msgs not in list?           */

/* Define array of window handles that we are going to process */
BOOL fProcessAllFrames = FALSE; /* Are we processing all frame messages?*/
BOOL fProcessAllWindows = FALSE;/* Are we processing all windows?*/
HWND *pahwndSpy = NULL;         /* Array of hwnds to spy on, no max here */
int  chwndSpy = 0;  		/* Max windows to spy */

/* Define array of HMQ's to spy on */
HMQ *pahmqSpy = NULL;           /* HMQ list to spy on */
int chmqSpy = 0;                /* Count of hmqs we are spying on */


/* Define message filter array */
UCHAR rgMessageFilter[MAXMSGFILTERBYTES] = "";
UCHAR  GMsgEnableAndType(USHORT);       /* Is message in message list?*/
VOID   GrabMsgDataWords(SHORT, UCHAR);  /* Get extra words for message */
VOID   CopyStruct(UCHAR FAR *pbSrc, UCHAR FAR *pbDst, SHORT cb);/* Copy bytes */

/* Define an array of messages to pass to whomever calls us */
ULONG spyMsgSem =  0L;          /* global semaphore for this module */

QMSGSPY  rgqmsgSpy[MAXMSGCNT];  /* Array of messages to process */
SHORT cmsgSpy = 0;              /* Count of messages yet to be read */
SHORT imsgWrite = 0;            /* Index to message to write out */
SHORT imsgRead = 0;             /* Index to message to read */

HHEAP hheapSpyHook;             /* Heap for SpyHook DLL */
SHORT acbMPTypes[MP_MASK+1] = { /* Number of desired bytes to extract */
    0,                          /* Normal fields, no pointers */
    sizeof(SWP),                /* MPT_SWP */
    sizeof(RECTL),              /* MPT_RECTL */
    0,                          /* MPT_WNDPRMS we don't process yet */
    sizeof(QMSG)                /* MPT_QMSG */
};


/* Information for getting selector information in different PID context */
PID     pidLNSpy = 0xffff;      /* Which process */
USHORT  selectorLNSpy = 0;      /* Which Selector */
WHOISINFO   whoIsLNSpy;         /* Symbol information */
int     rcLNSpy = -1;           /* Return code */


/* Define all external functions */
extern VOID  BuildStackTrace(CHAR FAR *, SHORT, SHORT);

/* Define all function prototypes */
VOID SpyCheckPIDForSymbol(VOID);
BOOL FAR PASCAL SpyInputHook(HAB, PQMSG, USHORT);
BOOL FAR PASCAL SpySendMsgHook(HAB, PSMHSTRUCT, USHORT);
VOID PASCAL Init(HMODULE);

/***********************************************************************\
* BOOL FAR PASCAL SpyInputHook( hab, lpqmsg, fs )
* HAB hab;
* PQMSG  lpqmsg;
* USHORT    fs;
*
* Effect:  This is the global input hook procedure.  Note that hook
*   procedures can be chained, so we always return FALSE to pass the
*   message to the next guy in the chain.
*
* Return value:  FALSE to pass message to next hook procedure.
\***********************************************************************/
BOOL FAR PASCAL SpyInputHook( hab, lpqmsg, fs )
HAB	hab;
PQMSG	lpqmsg;
USHORT  fs;
{
    UCHAR       bMPType;

    /*
     * Check first to see if we are looking for a process's symbols
     */
    if (pidLNSpy != 0xffff)
        SpyCheckPIDForSymbol();

    /*
     * First check to see if we are processing any hook messages
     */
    if (!fRecording || !fAnythingToWatch || (cmsgSpy >= MAXMSGCNT))
        return (FALSE); /* No, let the next one have it now */

    /*
     * See if we have any messages to process - re-check to make sure we
     * don't overwrite our queue.
     */
    if (DosSemRequest((HSEM)(PULONG)&spyhookSem, 100L) != 0)
        return (FALSE);
    if (cmsgSpy < MAXMSGCNT) {
        if (SpyFWindowInList(lpqmsg->hwnd, FALSE) &&
                (bMPType = GMsgEnableAndType(lpqmsg->msg))) {
            rgqmsgSpy[imsgWrite].fs = fs;       /* Save the flags */
            rgqmsgSpy[imsgWrite].qmsg = *lpqmsg;   /* Save the message */

            GrabMsgDataWords(imsgWrite, bMPType);
            imsgWrite++;

            if (imsgWrite == MAXMSGCNT)
                imsgWrite = 0;  /* Wrap around */
            cmsgSpy++;
            DosSemClear((HSEM)(PULONG)&spyMsgSem);
        }
    }

    DosSemClear( (HSEM)(PULONG)&spyhookSem );
    return FALSE;   /* Let system take normal action */
}


/***********************************************************************\
* BOOL FAR PASCAL SpySendMsgHook( hab, lpsmh, fInterTask )
* HAB		hab;
* PSMHSTRUCT	lpsmh;
* BOOL		fInterTask;
*
* Effect:  This is the global input hook procedure.  Note that hook
*   procedures can be chained, so we always return FALSE to pass the
*   message to the next guy in the chain.
*
* Return value:  FALSE to pass message to next hook procedure.
\***********************************************************************/
BOOL FAR PASCAL SpySendMsgHook( hab, lpsmh, fInterTask )
HAB		hab;
PSMHSTRUCT      lpsmh;
BOOL            fInterTask;
{
    UCHAR           bMPType;
    PIDINFO         pidinfo;

    /*
     * Check first to see if we are looking for a process's symbols
     */
    if (pidLNSpy != 0xffff)
        SpyCheckPIDForSymbol();

    /*
     * First check to see if we are processing any hook messages
     */
    if (!fRecording || !fAnythingToWatch || (cmsgSpy >= MAXMSGCNT))
        return (FALSE); /* No, let the next one have it now */

    /*
     * See if we have any messages to process
     */
    if (DosSemRequest((HSEM)(PULONG)&spyhookSem, 100L) != 0)
        return (FALSE);
    /* Make sure no one got in by mistake */
    if (cmsgSpy < MAXMSGCNT) {
        if (SpyFWindowInList(lpsmh->hwnd, FALSE) &&
                (bMPType = GMsgEnableAndType(lpsmh->msg))) {
            /*
             * Store out message, must move pieces separately
             */
            rgqmsgSpy[imsgWrite].fs = fInterTask;
            rgqmsgSpy[imsgWrite].qmsg.hwnd = lpsmh->hwnd;
            rgqmsgSpy[imsgWrite].qmsg.msg  = lpsmh->msg;
            rgqmsgSpy[imsgWrite].qmsg.mp1  = lpsmh->mp1;
            rgqmsgSpy[imsgWrite].qmsg.mp2  = lpsmh->mp2;
            rgqmsgSpy[imsgWrite].qmsg.time = (ULONG)-1;
            rgqmsgSpy[imsgWrite].qmsg.ptl.x  = 0;
            rgqmsgSpy[imsgWrite].qmsg.ptl.y  = 0;

            GrabMsgDataWords(imsgWrite, bMPType);

            /* Now get the PID/TID info, and do the stack backtrace */
            DosGetPID(&pidinfo);
            rgqmsgSpy[imsgWrite].pidSend = pidinfo.pid;
            rgqmsgSpy[imsgWrite].tidSend = pidinfo.tid;
            BuildStackTrace((CHAR FAR *)rgqmsgSpy[imsgWrite].pvoidStack,
                    CALLSTOSKIP, MAXSTRACE);

            imsgWrite++;
            if (imsgWrite == MAXMSGCNT)
                imsgWrite = 0;  /* Wrap around */
            cmsgSpy++;
            DosSemClear((HSEM)(PULONG)&spyMsgSem);
        }
    }

    DosSemClear( (HSEM)(PULONG)&spyhookSem );
    return FALSE;   /* Let system take normal action */
}


/***********************************************************************\
* BOOL FAR PASCAL SpyInstallHook( hab, hmq, fSendMessage, bHooks)
*
* Effect:  This routine installs a system-wide HK_INPUT hook.  The hab
*   hmq are remembered for message posting.  Note that we only allow
*   one input hook to be installed through this routine, but other
*   apps may call WinSetHook directly.
*
* Returns value:  TRUE if hook installed successfully, FALSE otherwise.
\***********************************************************************/
BOOL FAR PASCAL SpyInstallHook( hab, hmq, bHooks )
HAB     hab;
HMQ     hmq;
USHORT  bHooks;
{
    BOOL fRet;
    SEL  selHeap;

    DosSemRequest( (HSEM)(PULONG)&spyhookSem, -1L);

    /*
     * If this is the first time through, we need to create a local heap
     * to store message information.
     */
    if (hheapSpyHook == NULL) {
        selHeap = (SEL)(((ULONG)((PSZ)&hheapSpyHook)) >> 16);
        hheapSpyHook = WinCreateHeap(selHeap, 1024, 0, 0, 100,
            HM_MOVEABLE | HM_VALIDSIZE);
        if (hheapSpyHook == NULL)
            return (FALSE);
    }



    /*
     * Look at hook index.
     *
     * if it equals  HK_INPUT then
     *		install a system-wide input hook
     * else
     *		set system wide SENDMSG hook
     */
    SpybHooks = bHooks;
    if (SpybHooks & SPYH_INPUT)
        fRet = WinSetHook( hab, (HMQ)0, HK_INPUT, (PFN)SpyInputHook,
                    hmodSpy );

    if (SpybHooks & SPYH_SENDMSG)
        fRet = WinSetHook( hab, (HMQ)0, HK_SENDMSG, (PFN)SpySendMsgHook,
                    hmodSpy );
    if (fRet) {
        habOwner = hab;
        hmqOwner = hmq;
    }

    DosSemSet((HSEM)(PULONG)&spyMsgSem);    /* Init, no messages avail */
    DosSemClear( (HSEM)(PULONG)&spyhookSem );

    return fRet;
}



/***********************************************************************\
* BOOL FAR PASCAL SpySetWindowList( chwnd, rghwnd)
*
* Effect:  This routine sets the list of window that we are interested
*   in watching the messages for.
*
* Returns value:  TRUE
\***********************************************************************/
BOOL FAR PASCAL SpySetWindowList( chwnd, rghwnd )
SHORT       chwnd;
HWND FAR    *rghwnd;
{
    SHORT   i;
    HWND    *phwndT;

    DosSemRequest( (HSEM)(PULONG)&spyhookSem, -1L);

    if (pahwndSpy) {
        WinFreeMem(hheapSpyHook, (char *)pahwndSpy, sizeof(HWND) * chwndSpy);
        pahwndSpy = NULL;
    }


    chwndSpy = chwnd;

    if (chwndSpy > 0 ) {

        phwndT = pahwndSpy = (HWND *) WinAllocMem(hheapSpyHook,
                sizeof(HWND) * chwndSpy);

        if (phwndT != NULL) {
            for (i=0; i < chwnd; i++) {
                *phwndT++ = *rghwnd++;
            }
        } else
            chwndSpy = 0;
    }

    fAnythingToWatch = (chwndSpy > 0) || (chmqSpy > 0) ||
            fProcessAllWindows || fProcessAllFrames;

    DosSemClear( (HSEM)(PULONG)&spyhookSem );
    return(TRUE);
}


/***********************************************************************\
* BOOL FAR PASCAL SpyGetWindowList( chwnd, rghwnd)
*
* Effect:  This routine sets the list of window that we are interested
*   in watching the messages for.
*
* Returns value:  TRUE
\***********************************************************************/
SHORT FAR PASCAL SpyGetWindowList( chwnd, rghwnd )
SHORT       chwnd;
HWND FAR    *rghwnd;
{
    SHORT   i;
    HWND    *phwndT;

    DosSemRequest( (HSEM)(PULONG)&spyhookSem, -1L);

    if (chwnd > chwndSpy)
            chwnd = chwndSpy;

    phwndT = pahwndSpy;
    for (i=0; i < chwnd; i++) {
        *rghwnd++ = *pahwndSpy++;
    }

    DosSemClear( (HSEM)(PULONG)&spyhookSem );

    return(chwnd);
}




/***********************************************************************\
* BOOL FAR PASCAL SpySetQueueList( chmq, rghmq )
*
* Effect:  This routine sets the list of queues whose messages we watch.
*
* Returns value:  TRUE
\***********************************************************************/
BOOL FAR PASCAL SpySetQueueList( chmq, rghmq )
SHORT       chmq;
HMQ FAR    *rghmq;
{
    SHORT   i;
    HMQ     *phmqT;

    DosSemRequest( (HSEM)(PULONG)&spyhookSem, -1L);

    /*
     * If we previously had a list of HMQs to spy on, free the old list
     * now
     */
    if (pahmqSpy != NULL) {
        WinFreeMem(hheapSpyHook, (char *)pahmqSpy, sizeof(HMQ) * chmqSpy);
        pahmqSpy = NULL;
    }

    chmqSpy = chmq;

    if (chmqSpy > 0) {
        phmqT = pahmqSpy = (HWND *) WinAllocMem(hheapSpyHook, sizeof(HMQ) * chmqSpy);

        if (phmqT != NULL) {
            for (i=0; i < chmq; i++) {
                *phmqT++ = *rghmq++;
            }
        } else
            chmqSpy = 0;
    }

    fAnythingToWatch = (chwndSpy > 0) || (chmqSpy > 0) ||
            fProcessAllWindows || fProcessAllFrames;
    DosSemClear( (HSEM)(PULONG)&spyhookSem );

    return(TRUE);
}


/***********************************************************************\
* BOOL FAR PASCAL SpyGetQueueList( chmq, rghmq )
*
* Effect:  This routine gets the list of queues whose messages we watch.
*
* Returns value:  TRUE
\***********************************************************************/
SHORT FAR PASCAL SpyGetQueueList( chmq, rghmq )
SHORT       chmq;
HMQ FAR    *rghmq;
{
    SHORT   i;
    HMQ     *phmqT;

    DosSemRequest( (HSEM)(PULONG)&spyhookSem, -1L);

    if (chmq > chmqSpy)
        chmq = chmqSpy;

    phmqT = pahmqSpy;

    for (i=0; i < chmq; i++) {
        *rghmq++ = *phmqT++;
    }

    DosSemClear( (HSEM)(PULONG)&spyhookSem );

    return(chmq);
}


/***********************************************************************\
* BOOL FAR PASCAL SpySetMessageList(UCHAR FAR *prgNewMsgFilter,
*                   BOOL fNewDispMsgsNotInList)
*
* Effect:  This routine sets the list of windows that whose message we watch.
*
* Returns value:  TRUE
\***********************************************************************/
BOOL FAR PASCAL SpySetMessageList(prgNewMsgFilter, fNewDispMsgsNotInList)
UCHAR FAR           *prgNewMsgFilter;
BOOL                fNewDispMsgsNotInList;
{
    SHORT           i;
    unsigned char   *prgb;

    DosSemRequest( (HSEM)(PULONG)&spyhookSem, -1L);

    prgb = rgMessageFilter;


    fDispMsgsNotInList = fNewDispMsgsNotInList;

    for (i=0; i < MAXMSGFILTERBYTES; i++) {
        *prgb++ = *prgNewMsgFilter++;
    }

    DosSemClear( (HSEM)(PULONG)&spyhookSem );

    return(TRUE);
}


/***********************************************************************\
* BOOL FAR PASCAL SpyFWindowInList (hwnd, fInWindowListOnly)
*
* Effect:  This function checks our current list of windows, and returns
*          TRUE if the window is in the list, else returns FALSE.
*
* Returns value:  TRUE if window is in list.  Also, if we are in the special
*           state we will pass through all frame windows.
\***********************************************************************/
BOOL FAR PASCAL SpyFWindowInList (hwnd, fInWindowListOnly)
register HWND  hwnd;
BOOL            fInWindowListOnly;
{
    register int    i;
    char            szClassName[10];    /* Class name of window */
    CLASSINFO       classinfo;          /* Information about class */
    HMQ             hmqWindow;          /* HMQ of window        */
    HMQ             *phmqT;             /* Temporary pointer in hmq list */
    HWND            *phwndT;            /* Temporary pointer to HWND list */

    phwndT = pahwndSpy;
    for (i=0; i < chwndSpy; i++) {
        if (hwnd == *phwndT++)
            return (TRUE);
    }

    /* See if we are restricting to only windows in list */
    if (fInWindowListOnly)
        return (FALSE);

    if (fProcessAllWindows)
        return (TRUE);      /* All windows pass through */

    /*
     * See if we are watching any message queues
     */
    if (phmqT = pahmqSpy) {
        hmqWindow = (HMQ)WinQueryWindowULong(hwnd, QWL_HMQ);
        for (i=0; i < chmqSpy; i++) {
            if (*phmqT++ == hmqWindow)
                return (TRUE);
        }
    }

    if (fProcessAllFrames) {
        /* See if frame class */
        if (hwnd == NULL)
            return (TRUE);      /* pass queue messages through */
        WinQueryClassName(hwnd, sizeof(szClassName),
            (PSZ)szClassName);
        WinQueryClassInfo((HAB)NULL, (PSZ)szClassName,
            &classinfo);
        if (classinfo.flClassStyle & CS_FRAME)
            return (TRUE);
    }

    return (FALSE);
}


/***********************************************************************\
* BOOL FAR PASCAL SpyReleaseHook(fZeroQueue)
*
* Effect:  This routine releases the input hook, if it is installed.
*
* Returns value: TRUE if hook is released, FALSE otherwise.
\***********************************************************************/
BOOL FAR PASCAL SpyReleaseHook(fZeroQueue)
BOOL    fZeroQueue;
{
    BOOL fRet;

    DosSemRequest ( (HSEM)(PULONG)&spyhookSem, -1L );

    if ( habOwner ) {
        if (SpybHooks & SPYH_INPUT)
                fRet = WinReleaseHook( habOwner, (HMQ)0, HK_INPUT,
                    (PFN)SpyInputHook, hmodSpy );

        if (SpybHooks & SPYH_SENDMSG)
                fRet = WinReleaseHook( habOwner, (HMQ)0, HK_SENDMSG,
                    (PFN)SpySendMsgHook, hmodSpy );
            if ( fRet ) {
                habOwner = (HAB)0;
                hmqOwner = (HMQ)0;
        }
    }

    /*
     * When the hook is freed, we want to clear message count out,
     * make sure any process waiting will abort
     * Only do this if the Zeroqueu flag was passed
     */
    if (fZeroQueue) {
        cmsgSpy = 0;
        imsgWrite = 0;
        imsgRead = 0;
        if (pahwndSpy != NULL)
            WinFreeMem(hheapSpyHook, (char *)pahwndSpy,
                    sizeof(HWND) * chwndSpy);
        pahwndSpy = NULL;
        chwndSpy = 0;           /* Max windows to spy */
        fRecording = FALSE;     /* Set recording to off */
        if (pahmqSpy != NULL)
            WinFreeMem(hheapSpyHook, (char *)pahmqSpy, sizeof(HMQ) * chmqSpy);

        pahmqSpy = NULL;
        chmqSpy = 0;
        fAnythingToWatch = FALSE;
        DosSemClear((HSEM)(PULONG)&spyMsgSem);    /* Free any process */
    }
    DosSemClear((HSEM)(PULONG)&spyhookSem);

    return fRet;
}


/***********************************************************************\
* BOOL FAR PASCAL SpyHookOnOrOff( fOn)
*
* Effect:  This routine allows the application to turn the hook
*   processing on or off.
*
* Returns:	TRUE
\***********************************************************************/
BOOL FAR PASCAL SpyHookOnOrOff(fOn)
BOOL fOn;
{
    DosSemRequest ( (HSEM)(PULONG)&spyhookSem, -1L);

    fRecording = fOn;

    DosSemClear( (HSEM)(PULONG)&spyhookSem );
    return (TRUE);
}



/***********************************************************************\
* BOOL FAR PASCAL SpySetAllFrameOpt(fAllFrames)
*
* Effect:  Special state if TRUE will cause the window filter to pass
*       all frame windows through.  This is useful when debugging
*       interactions between frame windows, when the windows are
*       net yet created.
*
* Returns:	TRUE
\***********************************************************************/
BOOL FAR PASCAL SpySetAllFrameOpt(fAllFrames)
BOOL fAllFrames;
{
    fProcessAllFrames = fAllFrames;
    fAnythingToWatch = (chwndSpy > 0) || (chmqSpy > 0) ||
            fProcessAllWindows || fProcessAllFrames;
    return (TRUE);
}

/***********************************************************************\
* BOOL FAR PASCAL SpySetAllWindowOpt(fAllWindows)
*
* Effect:  Special state if TRUE will cause the window filter to pass
*       all windows through.  This is useful when debugging
*       interactions between all of the windows.
*
* Returns:	TRUE
\***********************************************************************/
BOOL FAR PASCAL SpySetAllWindowOpt(fAllWindows)
BOOL fAllWindows;
{
    fProcessAllWindows = fAllWindows;
    fAnythingToWatch = (chwndSpy > 0) || (chmqSpy > 0) ||
            fProcessAllWindows || fProcessAllFrames;
    return (TRUE);
}


/***********************************************************************\
* BOOL FAR PASCAL SpySetLNSymbolPID(PID pid, SELECTOR sel);
*
*
* Effect:  Tell the hook to be on the lookout for a specific PID.
*       If found running in the context of the specified process, we
*       will get the selector information for the specified selector.
\***********************************************************************/
BOOL FAR PASCAL SpySetLNSymbolPID(pid, selector)
PID     pid;
USHORT  selector;
{
    if (pidLNSpy == 0xffff) {
        selectorLNSpy = selector;
        rcLNSpy = -1;               /* Assume error for now */
        pidLNSpy = pid;             /* Save PID to look for */
        return (TRUE);
    } else
        return (FALSE);
}



/***********************************************************************\
* VOID SpyCheckPIDForSymbol(VOID)
*
* Effect:  Only called when we are looking for a symbol
*
\***********************************************************************/
VOID SpyCheckPIDForSymbol()
{
    PIDINFO         pidinfo;

    /*
     * Get the spy semaphore to serialize access and see if we are the
     * correct PID;
     */
    DosGetPID(&pidinfo);
    if (DosSemRequest((HSEM)(PULONG)&spyhookSem, 100L) != 0)
        return;

    if (pidinfo.pid == pidLNSpy) {
        /*
         * We are the correct process, so continue to try go get the
         * symbol information.
         */
        pidLNSpy = 0xffff;       /* reset to not enter again */
        rcLNSpy = IdentifyCodeSelector( selectorLNSpy,
            (PWHOISINFO)&whoIsLNSpy);
    }

    DosSemClear( (HSEM)(PULONG)&spyhookSem );
}



/***********************************************************************\
* int FAR PASCAL SpyGetLNSymbolSelector(pwhoinfo)
*
* Effect:  Get the information about the specified selector.
*       If the specified process has not been executed since the
*       function SpySetLNSymbolPID has been called, it will return -1,
*       else it will return what was returned from IdentifyCodeSelector.
\***********************************************************************/
int FAR PASCAL SpyGetLNSymbolSelector(pwhoIsinfo)
PWHOISINFO    pwhoIsinfo;
{
    *pwhoIsinfo = whoIsLNSpy;
    return (rcLNSpy);
}




/**********************************************************************\
* VOID GrabMsgDataWords (SHORT imsg, UCHAR bMPType)
*
* Effect:  Gets the next message from the list.
*
*	if timeout != 0 on message processing threads, problems may occur.
*	if lpqmsg==NULL, this function acts like a query or wait function.
\***********************************************************************/
VOID GrabMsgDataWords(imsg, bMPType)
SHORT       imsg;
UCHAR       bMPType;
{
    UCHAR   bMP1Type;
    UCHAR   bMP2Type;
    SHORT   cbMPs;
    SHORT   cbMP1;
    SHORT   cbMP2;
    UCHAR   *rgBuf;

    /*
     * This function will use the MPType data to know if MP1 and/or MP2
     * are pointers to any known data, that we want to extract off
     * and save for spy to display later
     */
    rgqmsgSpy[imsg].bMPType = bMPType;
    bMP1Type = (UCHAR)(bMPType & MP_MASK);
    cbMPs = cbMP1 = rgqmsgSpy[imsg].cbDataMP1 = acbMPTypes[bMP1Type];

    bMP2Type = (UCHAR)((bMPType >> 3) & MP_MASK);
    cbMPs += (cbMP2 = rgqmsgSpy[imsg].cbDataMP2 = acbMPTypes[bMP2Type]);

    if (cbMPs > 0) {
        /* Allocate memory to save the data into */
        rgBuf = rgqmsgSpy[imsg].rgData = WinAllocMem(hheapSpyHook, cbMPs);

        if (rgBuf != NULL) {
            /* Copy the data down, Note: if count is 0 will NOP */
            if (cbMP1) {
                CopyStruct((CHAR FAR *)rgqmsgSpy[imsg].qmsg.mp1,
                        rgBuf, cbMP1);
                rgBuf += cbMP1;
            }

            if (cbMP2) {
                CopyStruct((CHAR FAR *)rgqmsgSpy[imsg].qmsg.mp2,
                        rgBuf, cbMP2);
            }
        }

    } else {
        rgqmsgSpy[imsg].rgData = NULL;
    }
}

/**********************************************************************\
* SpyGetNextMessage (lpqmsg, lpBuf, cbBuf, lTimeOut)
*
* Effect:  Get the next message from the list.
* 	if timeout != 0 on message processing threads, problems may occur.
*	if lpqmsg==NULL, this function acts like a query or wait function.
*
* Returns:	Whether SpyGetNextMessage succeeds or not.
\***********************************************************************/
BOOL FAR PASCAL SpyGetNextMessage(lpqmsg, lpBuf, cbBuf, lTimeOut)
PQMSGSPY   lpqmsg;      /* Pointer where the user wants the message stored */
PSZ        lpBuf;       /* pointer to buffer */
SHORT      cbBuf;       /* size of buffer in bytes */
LONG    lTimeOut;       /* Timeout value */
{

    SHORT       cbMsg;  /* Count of bytes associated with message */

    /* Quick escape hatch */
    if ((lTimeOut == 0) && (cmsgSpy == 0))
        return (FALSE);     /* Don't Wait */

    /*
     * Now let's possibly wait for a message
     */
    if (cmsgSpy == 0) {
        if (DosSemWait((HSEM)(PULONG)&spyMsgSem, lTimeOut) != 0)
            return (FALSE); /* No messages after timeout */
        if (cmsgSpy == 0)
            return (FALSE); /* Still no messages, return condition */
    }

    /*
     * If the lpqmsg is NULL, the user is simply asking if there is
     * a message and/or waiting for the message, so don't extract
     * the message, but simply return the status.
     */
    if (lpqmsg != NULL) {
        if (DosSemRequest((HSEM)(PULONG)&spyhookSem, lTimeOut) == 0) {
            *lpqmsg = rgqmsgSpy[imsgRead];   /* Extract the message */

            cbMsg = rgqmsgSpy[imsgRead].cbDataMP1
                    + rgqmsgSpy[imsgRead].cbDataMP2;

            if ((cbMsg > 0) && (lpBuf != NULL)) {
                if (cbMsg < cbBuf)
                    cbBuf = cbMsg;  /* Number of bytes to copy */
                CopyStruct(rgqmsgSpy[imsgRead].rgData, lpBuf, cbBuf);
            }

            if (cbMsg > 0)
                WinFreeMem(hheapSpyHook, rgqmsgSpy[imsgRead].rgData, cbMsg);

            /* Also give the caller any additional information on message */

            imsgRead++;
            if (imsgRead == MAXMSGCNT)
                imsgRead = 0;  /* Wrap around */

            /*
             * Decrement count of messages, if we go to zero, set
             * the semaphore, so that the next read will suspend until
             * the next message
             */
            cmsgSpy--;
            if (cmsgSpy == 0)
                DosSemSet((HSEM)(PULONG)&spyMsgSem);

            DosSemClear( (HSEM)(PULONG)&spyhookSem );
        }
    }

    return (TRUE);
}

/***********************************************************************\
* VOID PASCAL Init( hmod )
* 
* Effect:	Saves the Spy module handle
\***********************************************************************/
VOID PASCAL Init( hmod )
HMODULE hmod;
{
    /* Save the module handle */
    hmodSpy = hmod;
}

/***********************************************************************\
* UCHAR GMsgEnableAndType (USHORT msg)
*
* Effect:
*   Should the message be processed?
*
*   If the message is out of range 
*	or bit is set in message bitmask then return TRUE;
\***********************************************************************/
UCHAR GMsgEnableAndType(msg)
USHORT  msg;
{
    if (msg > MAXMSGFILTER)
        return ((UCHAR)(fDispMsgsNotInList? MP_ENABLED : 0));

    return (rgMessageFilter[msg]);   /* One byte per message */
}
