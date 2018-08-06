/****************************** Module Header ******************************\
* Module Name: DDE.C
*
* DDE Manager main module - Contains all exported Dde functions.
*
* Created: 12/12/88 Sanford Staab
*
* Copyright (c) 1988, 1989  Microsoft Corporation
* 4/5/89        sanfords        removed need for hwndFrame registration parameter
* 6/5/90        sanfords        Fixed callbacks so they are blocked during
*                               timeouts.
*                               Fixed SendDDEInit allocation bug.
*                               Added hApp to ConvInfo structure.
*                               Allowed QueryConvInfo() to work on server hConvs.
*                               Added FindFrame() to provide an hApp for Server
*                               hConvs.
* 6/14/90       sanfords        Altered hDatas so they will work when shared
*                               between threads of the same process.  Also
*                               added optimization to only have the hsz
*                               in the hData for local conversations.
* 6/21/90       sanfords        Renamed APIs to Dde....
*                               Finished DdeAppNameServer() implementation.
*                       
\***************************************************************************/

#include "ddemlp.h" 
#include "version.h"

/****** Globals *******/

HMODULE hmodDmg = 0;        /* initialized by LoadProc on DLL initialization */
PFNWP lpfnFrameWndProc = 0;             /* system Frame window procedure */

HHEAP hheapDmg = 0;                     /* main DLL heap */

PHATOMTBL aAtbls;
USHORT cAtbls = 0;
USHORT iAtblCurrent = 0;
USHORT   cMonitor = 0;
DOSFSRSEM FSRSemDmg;                    /* used to protect globals */
USHORT cAtoms = 0;                      /* for debugging! */

PAPPINFO pAppInfoList = NULL;           /* registered thread data list */
USHORT usHugeShift;                     /* for huge segment support */
USHORT usHugeAdd;                       /* for huge segment support */
COUNTRYCODE syscc;

BOOL fInSubset(PCQDATA pcqd, ULONG afCmd);

/* PUBDOC START *\
DLL overview:

This DLL supports standard DDE communication on behalf of its client
applications.  It is compatable with most existing DDE programs.  The
API interface features object-like abstractions that allow its implementation
on and across a variety of platforms.

Main features:

  * HSZ manager:    Allows more efficient handling of numerous character
                    strings without the limitations of the atom manager.
                    
  * HDATA manager:  Data container objects allow easy filling, accessing
                    and reuse of huge amounts of data and allow a
                    server to efficiently support several clients.
                    
  * Controled initiates: Supports client to multi-server initiates and
                    allows a client application to pick and choose which
                    conversations to keep.

  * Debugging support: Allows monitoring applications to be easily written
                    and provides readable error messages.  

  * Huge Segment support: Exports a useful huge segment copy function and
                    properly handles huge data transfers.

  * Synchronous communication:  Clients may use a very simple form of
                    transaction processing that requires little application
                    support.

  * Asynchronous communication:  Clients may opt to use queued transfers
                    freeing them to do other tasks in parellel with DDE.

  * Callback control:  Applications may selectively suspend DLL callbacks
                    to themselves when in time critical sections.  Selected
                    conversations can be blocked while being serviced so
                    others can be processed by the same thread.

  * registration notification: All applications using this DLL are notified
                    whenever any other application registers or unregisters
                    itself with this DLL.  This enables nameserver support
                    for DDE.

  * multiple DDE entity support:  An application can register itself with
                    any number of application names and can change what
                    application names it responds to at any time.  Each
                    registered thread is a seperate DDE entity.

  * advise loop control:  Advise loops are tracked by the DLL, however
                    server applications have complete control over
                    advise loop initiation.

  * network agent support: Special agent applications can register with this
                    DLL to represent multiple applications on other machines
                    or platforms.
                    
API specifications:

Callback function:
    
EXPENTRY Callback(
HCONV hConv,        // holds server conversation handle in most cases
HSZ hszTopic,       // holds topic hsz for transaction
HSZ hszItem,        // holds item or application hsz for transaction
USHORT usFormat,    // holds data format when applicable
USHORT usType,      // holds transaction type code
HDMGDATA hDmgData); // holds incomming data in most cases

This is the definition of the data call-back function that an
application must export so that the DDE can initiate interaction
with the application when necessary.  This function is refered to in
the DdeInitialize() call.

The application callback function is very much like a PM window 
procedure for DDE.  The usType specifies the type of transaction being 
done.  By ANDing this parameter with XCLASS_MASK and comparing the 
result with the XCLASS_ constants, the transaction return type 
expected can be classified.

XTYP_ constants also may contain the XTYPF_NOBLOCK flag.  The presence
of this flag indicates that the CBR_BLOCK return value from the callback
will not be honored by the DDE.  (see DdeEnableCallback() for more
information on this concept.)

The various transactions are explained below:


-----XCLASS_NOTIFICATION class:
    These are strictly notification messages to an application.  The return
    value is ignored except in the case of CBR_BLOCK. (if the notification
    is blockable)

XTYP_RTNPKT

   This transaction is sent to agent applications.  hDmgData contains a 
   packet to send to the agent who's handle is in the hszItem parameter.  

XTYP_REGISTER

   Another server name has just been registered with the DLL application 
   name server.  hszItem is set to the application name being 
   registered.  If this is NULL, an application has registered itself as 
   WILD.  hDmgData is set to the application handle that registered.
   This is not blockable by CBR_BLOCK because no hConv is 
   associated with it.  Only if all callbacks are disabled is this 
   transaction blocked.

XTYP_UNREGISTER

   Another server application has just unregistered a name with this 
   DLL.  hszItem is set to the application name being unregistered.
   hDmgData is set to the app handle of the unregistering application.
   This is not blockable by CBR_BLOCK because no hConv is associated 
   with it.  Only if all callbacks are disabled is this transaction 
   blocked.

XTYP_INIT_CONFIRM

   Sent to let a server know that a conversation on application hszItem 
   and Topic hszTopic has been established on hConv.  hConv uniquely 
   identifies this conversation from the server's prospective.  This 
   call cannot be blocked because it is part of the DDE initiate 
   sequence.  This callback is generated by the results of XTYP_INIT and 
   XTYP_WILDINIT callbacks.  

XTYP_TERM

   This is a notification telling a server application that a 
   conversation has been terminated.  hConv is set to identify which 
   conversation was terminated.  

XTYP_ADVSTOP

   This notifies a server that an advise loop is stopping.  hszTopic, 
   hszItem, and usFormat identify the advise loop within hConv.  

XTYP_XFERCOMPLETE

   This notifictaion is sent to a client when an asynchronous data 
   DdeClientXfer() transaction is completed.  hDmgData is the client 
   queue ID of the completed transaction.  hConv is the client 
   conversation handle.  
    
XTYP_MONITOR

   This notifies an app registered as DMGCMD_MONITOR of DDE data that is 
   being transmitted.  hDmgData contains a text string representing the 
   transaction suitable for printing on a terminal, file, or window.  
   Note that this monitors ALL DDE communication and may be extensive.  
   This call cannot be delayed by disabled callbacks.  This transaction 
   is not blockable.  

XTYP_ACK

   This notifies a server that it has received an acknowledge from data 
   it has sent a client.  The hConv, topic, item, and format are set 
   apropriately.  LOUSHORT(hDmgData) will contain the dde flags from the 
   ack.  

-----XCLASS_DATA class:

   Transactions in this class are expected to return an HDMGDATA or 0 as 
   apropriate.  

XTYP_PKT

   This transaction is sent to agent applications.  hDmgData contains a 
   packet to send.  hszItem contains the agent handle to send the data 
   to.  hConv is set to the associated conversation handle.  The return 
   packet received from the partner agent should be returned.  This 
   call is blockable.

   If blocked, the conversation will be unblocked and processed by 
   DdeProcessPkt() when the return packet is received.  It is a good 
   idea for the agent to remember the hConv parameter in case the return 
   packet does not arrive within a reasonable amount of time via 
   XTYP_RTNPKT.  If an agent determines that it wishes to kill a 
   conversation, it should call DdeDisconnect().  

XTYP_REQUEST
XTYP_ADVREQ

   Data is being requested from a server application.  The function 
   should create a hDmgData using the DdePutData() function and 
   return it.  XTYP_ADVREQ origonates from a DdePostAdvise() call 
   while XTYP_REQUEST origonates from a client data request.  

XTYP_WILDINIT

   This is asking a DDE server permission to make multiple connections 
   with a specific client.

   hszItem may be the application name the client is requesting or it 
   may be NULL indicating a wild application name.  hszTopic may be the 
   Topic requested or NULL indicating a wild topic.  If not NULL, 
   hDmgData contains a CONVCONTEXT structure.  All other parameters are 
   0 or NULL.

   For local initiates, (initiates with the server application itself) 
   The server should return a 0 terminated (ie hszApp = hszTopic = 0) 
   array of HSZPAIR structures using DdePutData() and 
   DdeAddData().  Each hsz pair represents an app/topic the server 
   wishes to support.  Each created conversation will result in an 
   XTYP_INIT_CONFIRM notification.  If 0 is returned, no connections are 
   made with the requesting client.  This call is made even if callbacks 
   are disabled due the the synchronous nature of DDE initiates.  This 
   callback cannot be blocked by returning CBR_BLOCK.  

   Agent applications may also process this transaction as a 
   representative initiate.  The agent is expected to package this 
   transaction using DdeCreateInitPkt() and broadcast it to all 
   apropriate agents along its communication channel.  It then must 
   collect the return packets and for each packet and call 
   DdeProcessPkt().  Representative initiates are synchronous in that 
   the agent cannot return from this callback until all initiate return 
   packets are returned.  Representative initiates do NOT result in any 
   XTYP_INIT_CONFIRM notifications to the agent.  The agent is then free 
   to process this transaction for local connections as described.  

-----XCLASS_BOOL class:

   Transactions in this class expect a BOOL return of TRUE or FALSE.  

XTYP_INIT

   This is a query asking a DDE server permission to connect to a 
   specific client.  hszItem is set to the Application name.  hszTopic 
   is set to the topic name.  hDmgData if not NULL, contains CONVCONTEXT 
   data.  All other parameters are 0 or NULL.  A TRUE return value 
   allows the Dde to start up a server on the app/topic specified.  
   This will result in an XTYP_INIT_CONFIRM callback.  This call is made 
   even if callbacks are disabled due the the synchronous nature of DDE 
   initiates.  
    
   Agent applications may process this transaction as a representative 
   transaction.  The agent is expected to package this transaction using 
   DdeCreateInitPkt() and broadcast it to all apropriate agents along 
   its communication channel.  It then must collect the return packets 
   and for each packet call DdeProcessPkt().  This will NOT result in 
   any XTYP_INIT_CONFIRM notifications to the agent.  The agent is then 
   free to process this transaction for local connections.  

    A FALSE return implies no local initiate permissions are granted.

XTYP_ADVSTART

   This transaction requests permission to start a DDE advise loop with 
   a server.  The hszTopic, hszItem, and usFormat identify the advise 
   loop.  If FALSE is returned, the advise loop will not be started.  

-----XCLASS_FLAGS Class:

   Transactions in this class have hDmgData set.  An application should 
   use the DLL Data functions to extract the data from hDmgData.  The 
   return value should be the DDE fsStatus flags the app desires to 
   return to the client application.  If DDE_FACK is set, DDE_FBUSY and 
   DDE_FNOTPROCESSED are ignored.  

   The only flags the Dde expects to be returned are:
        DDE_FACK
        DDE_FBUSY
        DDE_FNOTPROCESSED
        any DDE_APPSTATUS bits
        
   All other bits will be stripped out by the Dde before sending an 
   ack message.  

   A 0 return is equivalent to DDE_NOTPROCESSED.  

XTYP_EXEC

   hDmgData contains an execute string from a client.  hConv, hszTopic, 
   hszItem are set.  If the WM_DDE_EXECUTE message received had the same 
   string for the itemname as for the data, hszItem will be 0L.  This 
   provides for EXCEL EXECUTE compatibility without requireing the 
   creation of an HSZ for the data string.  Applications are advised to 
   ignore the hszItem parameter for execute transactions since newer DDE 
   specifications ignore this value.  
  
XTYP_POKE

   Similar to XTYP_EXEC but hDmgData contains data poked to the server.  
  
XTYP_ADVDATA - advise data for a client!

   Note that XTYP_ADVDATA is for advise loop data intended for the 
   CLIENT not the server.  If the advise loop in progress is of the 
   NODATA type, hDmgData will be 0.  

-----Agent Transfers:

   A DDE agent application is one which registers itself with the 
   DMGCMD_AGENT flag.  Agent applications represent any number of other 
   applications across its communications channel.  Agent applications 
   are only allowed to communicate locally with other non-agent 
   applications.  This prevents communication loops from forming across 
   communication channels.  Any number of agents may register with the 
   DLL but each agent should represent a different communication 
   channel, one which is orthogonal to all other agents.  It is the 
   users responsability to only start up orthogonal agents.  

   Agents are responsible for handling and updating any DDE nameservers 
   associated with their communicaton channels.  Since agent 
   applications can converse directly with non-agent applications, they 
   can set up advise loops on the SysTopic/Topics items of local 
   applications to update the nameserver for the communication channel 
   if they wish to support DDE topics.  This may be impossible with some 
   DDE applications which either do not support the SysTopic/Topics item 
   or which have an unenumerable set of topics they support.  For 
   application name servers the DdeAppNameServer() function is 
   provided to give agents a local application name server from which to 
   draw on.  

   In general, an agent administers two classes of conversations.  One 
   is direct conversations with itself and local non-agent applications.  
   These transactions would be handled by the agent exactly like any 
   non-agent application would handle them.  

   The other class is representative conversations which the agent 
   passes over its communication channel.  In general, representative 
   conversation callbacks to the agent are only XTYP_PKT or XTYP_RTNPKT 
   type transactions or specially handled initiate transactions.  

   Agent applications are responsible for providing a unique ULONG agent 
   handle for every agent it is communicating with on its communication 
   channel.  The handle is provided by the agent whenever it calls 
   DdeProcessPkt().  Agent handles need not be global to the channel 
   since only the agent that created the handle will be expected to use 
   it.  Agent handles should not change over the life of a conversation.

   Should it be necessary to allow agents to alter or convert packet 
   data, the format of the packets can be documented later.  

   To show how an agent would handle its callback function, the 
   following pseudo code is offered as a model: 

ReceivePkt(pBits, cb, hAgentFrom)    \\ gets called when a packet arrives 
{
    hPkt = DdePutData(pBits, cb, 0, 0, 0, 0);
    if (hDmgData = ProcessPkt(hPkt, hAgentFrom))
        PassPkt(hDmgData, hAgentFrom);  \\ agent function to send pkt 
}
    
AgentCallback(hConv, hszTopic, hszItem, usFormat, usType, hDmgData)
{
    switch (usType) {
    case XTYP_INIT:
    case XTYP_WILDINIT:
        \\
        \\ process representative initiates
        \\
        QueryInterestedAgents(hszApp, hszTopic, pAgents);
        hDmgData = DdeCreateInitPkt(hszTopic, hszItem, hDmgData);
        BroadcastPkt(hDmgData, pAgents);
        \\
        \\ agent blocks here till all are in or timeout.
        \\ Packets get sent to ReceivePkt()
        \\
        CollectRtnPkts(pAgents); 
        \\  
        \\ now agent does his own processing of local inits.
        \\ retval == 0 if not interested.
        \\
        return(retval);
        break;

    case XTYP_RTNPKT:
        RemoveFromDeadCheckQ(hConv);
        PassPkt(hDmgData, hszItem); \\ hDmgData==Pkt, hszItem==hAgentTo 
        return(0);
        break;
        
    case XTYP_PKT:
        if (FindIgnoreList(hConv) { \\ was this unblocked due to no rtn pkt? 
            RemoveFromIgnoreList(hConv);
            return(0);  \\ rtn pkt failure.
        }
        if (!PassPkt(hDmgData, hszItem))  \\ hDmgData==Pkt, hszItem==hAgentTo 
            return(0);  \\ packet send failure. 
        AddToDeadCheckQ(hConv, timenow());
        return(CBR_BLOCK);  \\ will be unblocked and handled by ProcessPkt() 
        break;

    case XTYP_REGISTER:
    case XTYP_UNREGISTER:
        \\
        \\ agent updates its communications name server.
        \\
        return(0);
        break;

    default:
        \\
        \\ the rest would reference local conversatoins that the agent
        \\ is maintaining.
        \\
        break;
    }
}

UnblockDeadTransaction(hConv) \\ called when no rtn pkt for hConv has been
                              \\ received for a long time. 
{
    RemoveFromDeadCheckQ(hConv);
    AddToIgnoreList(hConv);
    DdeEnableCallback(CBK_ENABLE, hConv);
}

\* PUBDOC END */


/***************************** Public  Function ****************************\
* PUBDOC START
* USHORT EXPENTRY DdeInitialize(pfnCallback, afCmd, ulRes)
* PFNCALLBACK pfnCallback;  // address to application callback function
* ULONG afCmd;              // registration command flags
* ULONG ulRes;              // currently reserved, must be 0L.
* 
*     This API is used to initialize the DDEML for an application thread.
* 
*     afCmd - is a set of DMGCMD_ flags for special initialization instructions.
* 
*     DMGCMD_AGENT
*         The registering application represents more than one DDE application.
*         Agents are never allowed to establish a conversation with
*         another agent.  See Agent Transactions.
* 
*     DMGCMD_MONITOR
*
*         This defines the registered application as a DDE transaction 
*         monitor.  This is primarily used for debugging DDE 
*         applications.  A monitoring application will have its Callback 
*         function called every time a DDE message is sent.
* 
*         This flag is exclusive of all others.  No other flags should be
*         or'ed in with this one.
* 
*    DMGCMD_CLIENTONLY
*        This should be specified when the application only intends to
*        be a DDE client.  This reduces the resource consumption of the DLL.
*        
*     Registration is on a per-thread basis.  Thus a multi-threaded application
*     could register several threads as seperate DDE entities.  
* 
*     returns any applicable DMGERR_ error code or 0 on success.
*
*     Most other DLL APIs will fail if the calling thread has not called this.
* 
* PUBDOC END
* 
* Registration causes the following windows to be created:
* 
* HWND_OBJECT
*   hwndDmg(s)
*       hwndClient(s)
*   hwndTopicServer(s)
*       hwndServer(s)
*   hwndMonitor(s)
* HWND_DESKTOP
*   hwndFrame(s)
* 
*   See api.doc file for usage info.
* 
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
USHORT EXPENTRY DdeInitialize(pfnCallback, afCmd, ulRes)
PFNCALLBACK pfnCallback;
ULONG afCmd;
ULONG ulRes; 
{
    if (ulRes != 0L || CheckSel(SELECTOROF(pfnCallback)) == 0)
        return(DMGERR_INVALIDPARAMETER);

    return(Register(pfnCallback, afCmd, ulRes, FALSE));
}



USHORT EXPENTRY Register(pfnCallback, afCmd, ulRes, f32bit)
PFNCALLBACK pfnCallback;
ULONG afCmd;
ULONG ulRes;
BOOL f32bit;    /* set if calling app is a 32bit app. */
{
    BOOL        fInit;
    PAPPINFO    pai = 0L, paiT;
    PIDINFO     pidInfo;
    USHORT      usRet = DMGERR_PMWIN_ERROR;
    ULONG       ctlFlags;
    CLASSINFO   ci;
    USHORT      cb;

    UNUSED ulRes;

    SemEnter();
    if (fInit = (hheapDmg == 0L)) {
        /*
         * First time only
         */
        syscc.codepage = syscc.country = 0;
        DosGetCtryInfo(sizeof(COUNTRYCODE), &syscc, (PCOUNTRYINFO)&syscc, &cb);
        if (DosGetHugeShift(&usHugeShift))
            goto Abort;
        usHugeAdd = (1 << usHugeShift) - 1;
        if (!(hheapDmg = MyCreateHeap(0, 4096, 0, 0, 0, HEAPFLAGS)))
            goto Abort;
        if (!WinQueryClassInfo(DMGHAB, WC_FRAME, &ci))
            goto Abort;
        lpfnFrameWndProc = ci.pfnWindowProc;
        if (!AddAtomTable(TRUE)) 
            goto Abort;
    } else {
        
        if ((pai = GetCurrentAppInfo(FALSE)) != NULL) {
            /*
             * re-registration
             */
            return(DMGERR_DLL_USAGE);
        }
        
        /*
         * share the main heap with this process.
         */
        if (DosGetSeg(SELECTOROF(hheapDmg))) {
            SemLeave();
            return(DMGERR_PMWIN_ERROR);
        }
    }

        
    if (DosGetPID(&pidInfo))
        goto Abort;

    if (!(pai = (PAPPINFO)FarAllocMem(hheapDmg, sizeof(APPINFO))))
        goto Abort;
        
    if (!(pai->hheapApp = MyCreateHeap(0, 4096, 0, 0, 0, HEAPFLAGS))) {
        FarFreeMem(hheapDmg, pai, sizeof(APPINFO));
        pai = 0L;
        goto Abort;
    }

    pai->pAppNamePile = NULL;   /* responds to nothing */
    pai->pSvrTopicList = CreateLst(pai->hheapApp, sizeof(HWNDHSZLI));
    pai->pHDataPile = CreatePile(pai->hheapApp, sizeof(HDMGDATA), 8);
    pai->afCmd = (USHORT)afCmd | (f32bit ? DMGCMD_32BIT : 0);
    pai->hwndDmg =
    pai->hwndFrame =
    pai->hwndMonitor =
    pai->hwndTimer = 0;
    pai->pid = pidInfo.pid;
    pai->tid = pidInfo.tid;
    pai->pfnCallback = pfnCallback;
    pai->cInCallback = 0;
    pai->LastError = DMGERR_NO_ERROR;
    pai->fEnableCB = TRUE;
    pai->plstCB = CreateLst(pai->hheapApp, sizeof(CBLI));
    pai->plstCBExceptions = NULL;

    /*
     * make nextThread link.
     */
    paiT = pAppInfoList;
    while (paiT && paiT->pid != pai->pid) {
        paiT = paiT->next;
    }
    pai->nextThread = paiT; /* paiT is NULL or of the same process */
    
    if (paiT) {
        while (paiT->nextThread->tid != pai->nextThread->tid) {
            paiT = paiT->nextThread;
        }
        paiT->nextThread = pai;
    } else {
        /*
         * We must reregister each class for each process that invokes this
         * DLL because we can't register public classes unless we are the
         * shell.
         * Since pai->nextThread is NULL, this is a new process.
         */
        WinRegisterClass(0, SZCLIENTCLASS, ClientWndProc, 0L, 4);
        WinRegisterClass(0, SZSERVERCLASS, ServerWndProc, 0L, 4);
        WinRegisterClass(0, SZDMGCLASS, DmgWndProc, 0L, 4);
        WinRegisterClass(0, SZDEFCLASS, WinDefWindowProc, 0L, 4);
    }
    
    pai->next = pAppInfoList;
    pAppInfoList = pai;
    
    if ((pai->hwndDmg = WinCreateWindow(HWND_OBJECT, SZDMGCLASS, "", 0L,
            0, 0, 0, 0, (HWND)NULL, HWND_BOTTOM, WID_APPROOT, 0L, 0L)) == 0L) {
        goto Abort;
    }

    if (pai->afCmd & DMGCMD_MONITOR) {
        WinRegisterClass(0, SZMONITORCLASS, MonitorWndProc, 0L, 4);
        if ((pai->hwndMonitor = WinCreateWindow(HWND_OBJECT, SZMONITORCLASS, NULL,
                0L, 0, 0, 0, 0, (HWND)NULL, HWND_BOTTOM, WID_MONITOR, 0L, 0L))
                == 0L) {
            goto Abort;
        }
        if (++cMonitor) {
            WinSetHook(DMGHAB, NULL, HK_INPUT, (PFN)DdePostHookProc, hmodDmg);
            WinSetHook(DMGHAB, NULL, HK_SENDMSG, (PFN)DdeSendHookProc, hmodDmg);
        }
    }

    /*
     * create an invisible top-level frame for initiates. (if server ok)
     */
    usRet = DMGERR_PMWIN_ERROR;
    if (!(afCmd & DMGCMD_CLIENTONLY)) {
        ctlFlags = 0;
        if ((pai->hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0L, &ctlFlags,
                (PSZ)NULL, "", 0L, (HMODULE)NULL, 0, (PHWND)NULL)) == (HWND)NULL)
            goto Abort;
        WinSubclassWindow(pai->hwndFrame, subframeWndProc);
    }

    DosExitList(EXLST_ADD, (PFNEXITLIST)ExlstAbort);

    SemLeave();

    return(DMGERR_NO_ERROR);

Abort:
    SemLeave();

    if (pai)
        DdeUninitialize();
    else if (fInit && hheapDmg)
        hheapDmg = MyDestroyHeap(hheapDmg);
    return(usRet);
}


/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeUninitialize(void);
*     This uninitializes an application thread from the DDEML.
*     All DLL resources associated with the application are destroyed.
*     Most other APIs will fail if called after this API by the same thread.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
BOOL EXPENTRY DdeUninitialize()
{
    PAPPINFO pai, paiT;
    PMYDDES pmyddes;
    PIDINFO pi;

    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(FALSE);

    DosExitList(EXLST_REMOVE, (PFNEXITLIST)ExlstAbort);
    
    /*
     * get us out of the semaphore!
     * !!! NOTE: semaphore tid id -1 during exitlist processing!
     */
    DosGetPID(&pi);
    if (FSRSemDmg.cUsage > 0 && FSRSemDmg.pid == pi.pid &&
            (FSRSemDmg.tid == pi.tid || FSRSemDmg.tid == -1)) {
        while (FSRSemDmg.cUsage) {
            SemLeave();
        }
    }
    
    if (pai->hwndTimer)
        WinSendMsg(pai->hwndTimer, WM_TIMER, MPFROMSHORT(TID_ABORT), 0L);

    if (pai->hwndMonitor) {
        DestroyWindow(pai->hwndMonitor);
        if (!--cMonitor) {
            WinReleaseHook(DMGHAB, NULL, HK_INPUT, (PFN)DdePostHookProc, hmodDmg);
            WinReleaseHook(DMGHAB, NULL, HK_SENDMSG, (PFN)DdeSendHookProc, hmodDmg);
        }
    }

    /*
     * inform others of DeRegistration
     */
    if (pai->pAppNamePile != NULL) 
        DdeAppNameServer(NULL, ANS_UNREGISTER);
        
    UnlinkAppInfo(pai);

    DestroyWindow(pai->hwndDmg);
    DestroyWindow(pai->hwndFrame);
    DestroyHwndHszList(pai->pSvrTopicList);
    while (PopPileSubitem(pai->pHDataPile, (PBYTE)&pmyddes)) {
        if (CheckSel(SELECTOROF(pmyddes)) > sizeof(MYDDES) &&
                pmyddes->magic == MYDDESMAGIC &&
                pmyddes->pai == pai) {
            pmyddes->fs &= ~HDATA_APPOWNED;
        }
        FreeData(pmyddes, pai);
    }
    DestroyPile(pai->pHDataPile);
    if (pai->nextThread) {
        paiT = pai;
        while (paiT->nextThread != pai) {
            paiT = paiT->nextThread;
        } 
        paiT->nextThread = pai->nextThread;
        if (paiT->nextThread == paiT) {
            paiT->nextThread = NULL;
        }
    }
    MyDestroyHeap(pai->hheapApp);
    
    DestroyPile(pai->pAppNamePile);
    FarFreeMem(hheapDmg, (PBYTE)pai, sizeof(APPINFO));

    if (pAppInfoList == NULL) {     /* last guy out? - turn the lights out. */
        while (cAtbls--)
            WinDestroyAtomTable(aAtbls[cAtbls]);
        hheapDmg = MyDestroyHeap(hheapDmg);
    } else 
        DosFreeSeg(SELECTOROF(hheapDmg));

    SemCheckOut();

    return(TRUE);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* HCONVLIST EXPENTRY DdeBeginEnumServers(
* HSZ hszAppName,    // app name to connect to, NULL is wild.                   
* HSZ hszTopic,      // topic name to connect to, NULL is wild.                 
* HCONV hConvList,   // previous hConvList for reenumeration, NULL for initial. 
* PCONVCONTEXT pCC,  // language info or NULL for system default.               
* HAPP hApp);        // target application handle or NULL for broadcast init.   
*
*   hszAppName - the DDE application name to connect to - may be 0 for wild.
*   hszTopic - the DDE topic name to connect to - may be 0 for wild.
*   hConvList - The conversation list handle to use for reenumeration.
*       If this is 0, a new hConvList is created.
*   pCC - pointer to CONVCONTEXT structure which provides conversation
*       information needed for international support.  All DDEFMT_TEXT
*       strings within any conversation started by this call should use
*       the codepage referenced in this structure.
*       If NULL is given, the current system values are used.
*   hApp - if not NULL, this directs initiates to only be sent to hApp.
*
*       This routine connects all available conversations on the given 
*       app/topic pair.  Hsz values of 0 indicate wild names.  On reenumeration
*       old hConv's are kept and any new ones created are added to the
*       list.  Duplicate connections are avoided where possible.  A
*       duplicate connection is one which is to the same process and
*       thread on the same application/topic names.  If hApp is provided,
*       initiates are given only to that application.
*       Reenumeration is primarily intended as a response
*       to registration of a new app name to the system.  Reenumeration
*       also removes any terminated conversations from the list.
*
*   returns NULL on failure, hConvList on success.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
HCONVLIST EXPENTRY DdeBeginEnumServers(hszAppName, hszTopic, hConvList,
        pCC, hApp)
HSZ hszAppName;     
HSZ hszTopic;       
HWND hConvList;     
PCONVCONTEXT pCC;   
HAPP hApp;          
{
    PAPPINFO            pai;
    HCONV               hConv, hConvNext, hConvNew;
    HCONVLIST           hConvListNew;
    PCLIENTINFO         pciOld, pciNew;
    PID                 pidOld, pidNew;
    TID                 tidOld, tidNew;

    if ((pai = GetCurrentAppInfo(TRUE)) == 0)
        return(0);

    /*
     * destroy any dead old clients
     */
    if (hConvList) {
        hConv = WinQueryWindow(hConvList, QW_TOP, FALSE);
        while (hConv != NULL) {
            hConvNext = WinQueryWindow(hConv, QW_NEXT, FALSE);
            if (!((USHORT)WinSendMsg(hConv, UM_QUERY, MPFROMSHORT(Q_STATUS), 0L) &
                    ST_CONNECTED))
                WinDestroyWindow(hConv);
            hConv = hConvNext;
        }
    }
    
    if ((hConvListNew = WinCreateWindow(pai->hwndDmg, SZDEFCLASS, "", 0L,
            0, 0, 0, 0, (HWND)NULL, HWND_BOTTOM, WID_CLROOT, 0L, 0L)) == NULL) {
        pai->LastError = DMGERR_PMWIN_ERROR;
        return(0L);
    }
    
    hConvNew = GetDDEClientWindow(hConvListNew, (HWND)hApp, NULL, hszAppName,
            hszTopic, pCC);

    /*
     * If no new hConvs created, quit now.
     */
    if (hConvNew == NULL) {
        if (hConvList && WinQueryWindow(hConvList, QW_TOP, FALSE) == NULL) {
            DestroyWindow(hConvList);
            hConvList = NULL;
        }
        if (hConvList == NULL)
            pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
        return(hConvList);
    }

    /*
     * remove any new ones that duplicate old existing ones
     */
    if (hConvList && (hConv = WinQueryWindow(hConvList, QW_TOP, FALSE))) {
        while (hConv) {
            hConvNext = WinQueryWindow(hConv, QW_NEXT, FALSE);
            pciOld = (PCLIENTINFO)WinQueryWindowULong(hConv, QWL_USER);
            if (!WinIsWindow(DMGHAB, pciOld->ci.hwndPartner)) {
                WinDestroyWindow(hConv);
                hConv = hConvNext;
                continue;
            }
            WinQueryWindowProcess(pciOld->ci.hwndPartner, &pidOld, &tidOld);
            /*
             * destroy any new clients that are duplicates of the old ones.
             */
            hConvNew = WinQueryWindow(hConvListNew, QW_TOP, FALSE);
            while (hConvNew) {
                hConvNext = WinQueryWindow(hConvNew, QW_NEXT, FALSE);
                pciNew = (PCLIENTINFO)WinQueryWindowULong(hConvNew, QWL_USER);
                WinQueryWindowProcess(pciNew->ci.hwndPartner, &pidNew, &tidNew);
                if (pciOld->ci.hszServerApp == pciNew->ci.hszServerApp &&
                        pciOld->ci.hszTopic == pciNew->ci.hszTopic &&
                        pidOld == pidNew &&
                        tidOld == tidNew) {
                    /*
                     * assume same app, same topic, same process, same thread
                     * is a duplicate.
                     */
                    WinDestroyWindow(hConvNew);
                }
                hConvNew = hConvNext;
            }
            /*
             * move the unique old client to the new list
             */
            WinSetParent(hConv, hConvListNew, FALSE);
            hConv = hConvNext;
        }
        WinDestroyWindow(hConvList);
    }
    
    /*
     * If none are left, fail because no conversations were established.
     */
    if (WinQueryWindow(hConvListNew, QW_TOP, FALSE) == NULL) {
        DestroyWindow(hConvListNew);
        pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
        return(NULL);
    } else {
        return(hConvListNew);
    }
}



/***************************** Public  Function ****************************\
* PUBDOC START
* HCONV EXPENTRY DdeGetNextServer(
* HCONVLIST hConvList,  // conversation list being traversed
* HCONV hConvPrev)      // previous conversation extracted or NULL for first
*
* hConvList - handle of conversation list returned by DdeBeginEnumServers().
* hConvPrev - previous hConv returned by this API or 0 to start from the top
*   of hConvList.
*
* This API returns the next conversation handle associated with hConvList.
* A 0 is returned if hConvPrev was the last conversation or if hConvList
* has no active conversations within it.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
HCONV EXPENTRY DdeGetNextServer(hConvList, hConvPrev)
HCONVLIST hConvList;
HCONV hConvPrev;
{
    if (!WinIsWindow(DMGHAB, hConvList))
        return(NULL);
    if (hConvPrev == NULL)
        return(WinQueryWindow(hConvList, QW_TOP, FALSE));
    else
        return(WinQueryWindow(hConvPrev, QW_NEXT, FALSE));
}




/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeEndEnumServers(hConvList)
* HCONVLIST hConvList;  // conversation list to destroy.
*
* hConvList - a conversation list handle returned by DdeBeginEnumServers().
*
* This API destroys hConvList and terminates all conversations associated
* with it.  If an application wishes to save selected conversations within
* hConvList, it should call DdeDisconnect() on all hConv's it does not
* want to use and not call this API.
*
* PUBDOC END
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
BOOL EXPENTRY DdeEndEnumServers(hConvList)
HCONVLIST hConvList;
{
    PAPPINFO pai;
    HCONV hConv, hConvNext;

    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(FALSE);
    if (WinIsWindow(DMGHAB, hConvList)) {
        hConv = WinQueryWindow(hConvList, QW_TOP, FALSE);
        while (hConv != NULL) {
            hConvNext = WinQueryWindow(hConv, QW_NEXT, FALSE);
            DestroyWindow(hConv);
            hConv = hConvNext;
        }
        DestroyWindow(hConvList);
    }
    return(TRUE);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* HCONV EXPENTRY DdeConnect(hszAppName, hszTopic, pCC, hApp)
* HSZ hszAppName;   // app name to connect to, NULL is wild.
* HSZ hszTopic;     // topic name to connect to, NULL is wild.
* PCONVCONTEXT pCC; // language information or NULL for sys default.
* HAPP hApp;        // target application or NULL for broadcast.
*
* hszAppName - DDE application name to connect to.
* hszTopic - DDE Topic name to connect to.
* pCC - CONVCONTEXT information pertinant to this conversation.
*       If NULL, the current system information is used.
* hApp - if not NULL, directs connection to a specific app.
*
* returns - the conversation handle of the connected conversation or 0 on error.
*
* This function allows the simpler aproach of allowing a client to
* talk to the first server it finds on a topic.  It is most efficient when
* an hApp is provided.
*
* PUBDOC END
*
* History:
*   Created     12/16/88    Sanfords
\***************************************************************************/
HCONV EXPENTRY DdeConnect(hszAppName, hszTopic, pCC, hApp)
HSZ hszAppName;
HSZ hszTopic;
PCONVCONTEXT pCC;
HAPP hApp;
{
    PAPPINFO pai;
    HCONV hConv;

    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(0);
        
    hConv = GetDDEClientWindow(pai->hwndDmg, NULL, (HWND)hApp, hszAppName,
            hszTopic, pCC);
            
    if (hConv == 0)
        pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
        
    return(hConv);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeDisconnect(hConv)
* hConv; // conversation handle of conversation to terminate.
*
* This API terminates a conversation started by either DdeConnect() or
* DdeBeginEnumServers().  hConv becomes invalid after this call.
*
* If hConv is a server conversation, any transactions for that conversation
* found on the server callback queue will be deleted prior to terminating
* the conversation.
*
* If hConv is a client conversation, any transactions on the Client Queue
* are purged before termination.
*
* Note that client conversations that are terminated from the server end
* go into a dormant state but are still available so that DdeQueryConvInfo()
* can be used to determine why a conversation is not working.
* Server conversations will destroy themselves if terminated from a client.
*
* returns fSuccess
*
* PUBDOC END
* History:
*   Created     12/16/88    Sanfords
\***************************************************************************/
BOOL EXPENTRY DdeDisconnect(hConv)
HCONV hConv;
{
    PAPPINFO pai;

    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(FALSE);
        
    if (!WinIsWindow(DMGHAB, hConv)) {
        pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
        return(FALSE);
    }
    SemCheckOut();
    return((BOOL)WinSendMsg(hConv, UMCL_TERMINATE, 0L, 0L));
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeQueryConvInfo(hConv, pConvInfo, idXfer)
* HCONV hConv;          // conversation hand to get info on.
* PCONVINFO pConvInfo;  // structure to hold info.
* ULONG idXfer;         // transaction ID if async, QID_SYNC if not.
*
* hConv - conversation handle of a conversation to query.
* pConvInfo - pointer to CONVINFO structure.
* idXfer - Should be a QID_ constant or an ID returned from DdeCheckQueue().
*       if id is QID_SYNC, then the synchronous conversation state is returned.
*
* returns - fSuccess.  The CONVINFO structure is filled in with the
*     conversation's status on success.
*
* Note that a client conversation may have several transactions in progress
* at the same time.  idXfer is used to choose which transaction to refer to.
*
* hConv may be a client or server conversation handle.  Server conversation
* handles ignore idXfer.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
BOOL EXPENTRY DdeQueryConvInfo(hConv, pConvInfo, idXfer)
HCONV hConv;
PCONVINFO pConvInfo;
ULONG idXfer;
{
    PCLIENTINFO pci;
    PAPPINFO pai;
    PXADATA pxad;
    PCQDATA pqd;
    BOOL fClient;

    if ((pai = GetCurrentAppInfo(FALSE)) == 0)
        return(FALSE);

    SemCheckOut();

    /*
     * note that pci may actually be a psi if fClient is false.  Since
     * the common info portions are identical, we don't care except
     * where data is extracted from non-common portions.
     */
    if (!WinIsWindow(DMGHAB, hConv) ||
            !(pci = (PCLIENTINFO)WinSendMsg(hConv, UM_QUERY, (MPARAM)Q_ALL, 0L)) ||
            !WinIsWindow(DMGHAB, pci->ci.hwndPartner)) {
        pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
        return(FALSE);
    }
    
    fClient = (BOOL)WinSendMsg(hConv, UM_QUERY, (MPARAM)Q_CLIENT, 0L);
    
    if (idXfer == QID_SYNC || !fClient) {
        pxad = &pci->ci.xad;
    } else {
        if (pci->pQ != NULL &&
                (pqd = (PCQDATA)Findqi(pci->pQ, idXfer))) {
            pxad = &pqd->xad;
        } else {
            pai->LastError = DMGERR_UNFOUND_QUEUE_ID;
            return(FALSE);
        }
    }
    SemEnter();
    pConvInfo->cb = sizeof(CONVINFO);
    pConvInfo->hConvPartner = IsDdeWindow(pci->ci.hwndPartner);
    pConvInfo->hszAppName = pci->ci.hszServerApp;
    pConvInfo->hszAppPartner = fClient ? pci->ci.hszServerApp : 0;
    pConvInfo->hszTopic = pci->ci.hszTopic;
    pConvInfo->hAgent = pci->ci.hAgent;
    pConvInfo->hApp = pci->ci.hwndFrame;
    if (fClient) {
        pConvInfo->hszItem = pxad->pXferInfo->hszItem;
        pConvInfo->usFmt = pxad->pXferInfo->usFmt;
        pConvInfo->usType = pxad->pXferInfo->usType;
        pConvInfo->usConvst = pxad->state;
        pConvInfo->LastError = pxad->LastError;
    } else {
        pConvInfo->hszItem = NULL;
        pConvInfo->usFmt = 0;
        pConvInfo->usType = 0;
        pConvInfo->usConvst = pci->ci.xad.state;
        pConvInfo->LastError = pci->ci.pai->LastError;
    }
    pConvInfo->usStatus = pci->ci.fs;
    pConvInfo->fsContext = pci->ci.cc.fsContext;
    pConvInfo->idCountry = pci->ci.cc.idCountry;
    pConvInfo->usCodepage = pci->ci.cc.usCodepage;
    SemLeave();
    return(TRUE);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdePostAdvise(hszTopic, hszItem)
* HSZ hszTopic;     // topic of changed data, NULL is all topics
* HSZ hszItem;      // item of changed data, NULL is all items
*
* Causes any clients who have advise loops running on the topic/item name
* specified to receive the apropriate data messages they require.
*
* This should be called by a server application anytime data associated with
* a particular topic/item changes.  This call results in XTYP_ADVREQ
* callbacks being generated.
*
* hszTopic and/or hszItem may be NULL if all topics or items are to be updated.
* This will result in callbacks for all active advise loops that fit the
* hszTopic/hszItem pair.
* 
* The API is intended for SERVERS only!
*
* PUBDOC END
* History:
*   Created     12/16/88    Sanfords
\***************************************************************************/
BOOL EXPENTRY DdePostAdvise(hszTopic, hszItem)
HSZ hszTopic;
HSZ hszItem;
{
    PAPPINFO pai;
    HWND hwndTopic;
    HWND hwndSvr;

    // LATER - add wild hsz support
    
    if (((pai = GetCurrentAppInfo(TRUE)) == 0))
        return(FALSE);
        
    if (pai->afCmd & DMGCMD_CLIENTONLY) {
        pai->LastError = DMGERR_DLL_USAGE;
        return(FALSE);
    }
        
    if ((hwndTopic = HwndFromHsz((HSZ)hszTopic, pai->pSvrTopicList)) == 0)
        return(TRUE);

    hwndSvr = WinQueryWindow(hwndTopic, QW_TOP, FALSE);
    while (hwndSvr) {
        WinPostMsg(hwndSvr, UMSR_POSTADVISE, MPFROMSHORT(hszItem), 0L);
        hwndSvr = WinQueryWindow(hwndSvr, QW_NEXT, FALSE);
    }

    return(TRUE);
}


/***************************** Public  Function ****************************\
* PUBDOC START
* HDMGDATA EXPENTRY DdeClientXfer(pSrc, cb, hConv, hszItem, usFmt,
*         usType, ulTimeout, pidXfer)
* PBYTE pSrc;       // data source, or NULL for non-data cases.
* ULONG cb;         // data size or 0 for non-data cases.
* HCONV hConv;      // associated conversation handle
* HSZ hszItem;      // item of transaction
* USHORT usFmt;     // format for transaction
* USHORT usType;    // transaction type code
* ULONG ulTimeout;  // timeout for synchronous, TIMEOUT_ASSYNC otherwise.
* PULONG pidXfer;   // OUTPUT: assync transfer id, NULL for no output.
*
* This API initiates a transaction from a client to the server connected
* via the conversation specified by hConv.
* 
* Currently usType may be:
*     XTYP_REQUEST
*     XTYP_POKE
*     XTYP_EXEC
*     XTYP_ADVSTART
*     XTYP_ADVSTART | XTYPF_NODATA
*     XTYP_ADVSTART | XTYPF_ACKREQ
*     XTYP_ADVSTART | XTYPF_NODATA | XTYPF_ACKREQ
*     XTYP_ADVSTOP
*
* ulTimeout specifies the maximum time to wait for a response in miliseconds
* and applies to synchronous transactions only.
* if ulTimeout is TIMEOUT_ASSYNC, then the transfer is asynchronous and
* pidXfer may point to where to place the client transaction queue item
* ID created by this request.
* pidXfer may be NULL if no ID is desired.
* 
* If usType is XTYP_REQUEST, synchronous transfers return a valid hDmgData
* on success which holds the data received from the request.
*
* if usType is XTYP_EXEC and hszItem==NULL (wild) and usFmt==DDEFMT_TEXT,
* the item name will be changed to the same as the text data.
* This allows for EXCEL and porthole WINDOWS compatability
* for XTYP_EXEC transactions.  It is suggested that applications always set
* hszItem to NULL for XTYP_EXEC transactions and that servers ignore the
* hszItem perameter in their callback for execute transactions.
*
* returns hDmgData or ACK DDE flags on Success, 0 on failure.
*
* Note: the hDmgData passed in by this call is only valid for the duration
* of the callback.
* 
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
HDMGDATA EXPENTRY DdeClientXfer(pSrc, cb, hConv, hszItem, usFmt,
        usType, ulTimeout, pidXfer)
PBYTE pSrc;
ULONG cb;
HCONV hConv;
HSZ hszItem;
USHORT usFmt;
USHORT usType;
ULONG ulTimeout;
PULONG pidXfer;
{
    PAPPINFO pai;
    XFERINFO xi;
    HDMGDATA hDmgData;

    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(0);

    if (!WinIsWindow(DMGHAB, hConv)) {
        pai->LastError = DMGERR_NO_CONV_ESTABLISHED;
        return(0);
    }
    
    SemCheckOut();
    
    switch (usType) {
    case XTYP_REQUEST:
    case XTYP_POKE:
    case XTYP_EXEC:
    case XTYP_ADVSTART:
    case XTYP_ADVSTART | XTYPF_NODATA:
    case XTYP_ADVSTART | XTYPF_ACKREQ:
    case XTYP_ADVSTART | XTYPF_NODATA | XTYPF_ACKREQ:
    case XTYP_ADVSTOP:
        xi.pidXfer = pidXfer;
        xi.ulTimeout = ulTimeout;
        xi.usType = usType;
        xi.usFmt = usFmt;
        xi.hszItem = hszItem;
        xi.hConv = hConv;
        xi.cb = cb;
        xi.pData = pSrc;        
        hDmgData = (HDMGDATA)WinSendMsg(hConv, UMCL_XFER, (MPARAM)&xi, 0L);
        if (ulTimeout == TIMEOUT_ASYNC) {
            /*
             * Increment the count of hszItem incase the app frees it on
             * return.  This will be decremented when the client Queue
             * entry is removed.
             */
            IncHszCount(hszItem);
        }
        
        /*
         * add the hDmgData to the client's list of handles he needs
         * to eventually free.
         */
        if ((usType & XCLASS_DATA) && (CheckSel(SELECTOROF(hDmgData)))) {
            AddPileItem(pai->pHDataPile, (PBYTE)&hDmgData, CmpULONG);
            if (((PMYDDES)hDmgData)->magic == MYDDESMAGIC) {
                ((PMYDDES)hDmgData)->fs |= HDATA_READONLY;
            }
        }
        
        return(hDmgData);
    }
    pai->LastError = DMGERR_INVALIDPARAMETER;
    return(0);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* USHORT EXPENTRY DdeGetLastError(void)
*
* This API returns the most recent error registered by the DDE manager for
* the current thread.  This should be called anytime a DDE manager API
* returns in a failed state.
*
* returns an error code which corresponds to a DMGERR_ constant found in
* ddeml.h.  This error code may be passed on to DdePostError() to
* show the user the reason for the error or to DdeGetErrorString() to convert
* the error code into an apropriate string.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
USHORT EXPENTRY DdeGetLastError(void)
{
    PAPPINFO pai;
    SHORT err = DMGERR_DLL_NOT_INITIALIZED;

    pai = GetCurrentAppInfo(FALSE);

    if (pai) {
        err = pai->LastError;
        pai->LastError = DMGERR_NO_ERROR;
    }
    return(err);
}


/***************************** Public  Function ****************************\
* PUBDOC START
* void EXPENTRY DdePostError(err)
* ULONG err;    // error code to post.
*
* This API puts up a message box describing the error who's code was
* passed in.
*
* PUBDOC END
*
* History:
*   Created     12/20/88    sanfords
\***************************************************************************/
void EXPENTRY DdePostError(err)
USHORT err;
{
    char szError[MAX_ERRSTR + 1];

    if (err < DMGERR_FIRST || err > DMGERR_LAST)
        return;
    WinLoadString(DMGHAB, hmodDmg, err, MAX_ERRSTR + 1, szError);
    WinMessageBox(HWND_DESKTOP, NULL, szError, SZERRCAPTION, 0,
            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* USHORT EXPENTRY DdeGetErrorString(err, cbMax, psz)
* USHORT err;       // error code to convert
* USHORT cbMax;     // size of string buffer provided by caller
* PSZ psz;          // string buffer address.
*
* This function fills psz with the error string referenced by err of up
* to cbMax characters.  All error strings are <= MAX_ERRSTR in length.
* (not counting the NULL terminator.)
*
* returns length of copied string without NULL terminator or 0 on failure.
*
* PUBDOC END
*
* History:
*   Created     12/20/88    sanfords
\***************************************************************************/
USHORT EXPENTRY DdeGetErrorString(err, cbMax, psz)
USHORT err;
USHORT cbMax;
PSZ psz;
{
    if (err < DMGERR_FIRST || err > DMGERR_LAST)
        return(0);
    return(WinLoadString(DMGHAB, hmodDmg, err, cbMax, psz));
}



/*\
* HDATA stuff:
* 
* Each thread has an hData list that contains all the hData's it has
* been given by the DLL.   This list indicates what hData's an app can
* and must eventually free.
* 
* If an app has multiple threads registered, each threads pai's are linked
* via the nextThread pointer.  The links are circular so the TID
* should be used to know when all the lists are traversed.
* 
* Each hData contains the following flags:
* 
* HDATA_READONLY - set on any hData given to the DLL or created by the DLL.
*         This prevents AddData from working.
*         
* HDATA_APPOWNED - set at creation time by app so app will keep access
*         until it frees it or unregistration happens.
*         This prevents the DLL from freeing the hData before an app is
*         through with it.
* 
* HDATA_APPFREEABLE - set at creation time if logged into thread list.
* 
* Each hData also contains the pai of the thread that created the hData.
*         (set by PutData)  If APPOWNED is set, this identifies the
*         owner thread as well.
* 
* 
* General rules for apps:
* 
*         hDatas from DLL calls are the apps responsibility to free and are
*                 only valid till passed back into the DLL or freed.
*         hDatas from callback calls are only valid during the callback.
*         hDatas created by an app but not APPOWNED are only valid till
*                 passed into the DLL.
*         hDatas created by an app that are APPOWNED are valid until
*                 the creating thread frees it or till that thread unregisters
*                 itself.
*         A process will not loose access to an hData untill all threads that
*                 have received the hData have freed it or passed it back to
*                 the DLL (via callback)
* 
*         
* Register:                                                       DONE
*         creates hDataList for app/thread.
*         creates nextThread links if applicable.
* 
* DdePutData:                                                  DONE
*         Only allows HDATA_APPOWNED flag
*         sets HDATA_APPFREEABLE flag
*         ...Falls into....
* PutData:                                                        DONE
*         Allocates selector
*         sets creator pai
*         sets HDATA_ flags specified
*         if (HDATA_APPFREEABLE)
*             adds to hDataList. 
* 
* DdeAddData:                                                  DONE
*         fails if HDATA_READONLY or a non-dll type selector
* 
* Callback:                                                       DONE
*         Entry:
*             for hData to callback:
*                 Sets HDATA_READONLY if hData valid DLL type selector.
*                 (does NOT add hData to thread list so he can't free it during
*                  the callback)
*                  
*         Exit:
*             for hData to callback:
*                 nothing.
*             for hData from callback:
*                 verifies creator == current
* 
* DdeCreateInitPkt:                                            DONE
* DdeAppNameServer:                                            NOT COMPLETE
* DdeClientXfer:                                               DONE
* DdeCheckQ:                                                   DONE
*         if (valid selector)
*             add to thread list
*             if (valid DLL type selector)
*                 Set READONLY flag.
* 
* ProcessPkt:                                                     NOT IMP.
*         for hData in:
*              After used, calls FreeData()
*         for hData out:
*              before return, adds hData to thread list
*         
* MyPostMsg:                                                      DONE
*         gives if target process != current
*         if target process is not a DLL process/thread, expand hszItem.
*         calls FreeData() on current process if MDPM_FREEHDATA is set.
*         
* 
* DdeFreeData:                                                 DONE
*         if in thread list.
*             remove hData from list
*             if not in any thread lists for this process
*                 free data
*             return pass
*         else 
*             return fail
* 
* FreeData:                                                       DONE
*         if (DLL type selector && HDATA_APPOWNED && creator == current)
*             exit
*         remove hData from thread list if found
*         if not in any thread lists for this process
*             free data
*         
* UnRegister:                                                     DONE
*         for each item in the hDataList:
*             clear HDATA_APPOWNED flag if dll type selector and owned by this
*                 thread.
*             FreeData()
*         destroy list
*         unlink from other thread lists
*         
\*/


/***************************** Public  Function ****************************\
* PUBDOC START
* HDMGDATA EXPENTRY DdePutData(pSrc, cb, cbOff, hszItem, usFmt, afCmd)
* PBYTE pSrc;   // address of data to place into data handle or NULL
* ULONG cb;     // amount of data to copy if pSrc is not NULL.
* ULONG cbOff;  // offset into data handle region to place pSrc data
* HSZ hszItem;  // item associated with this data
* USHORT usFmt; // format associated with this data
* USHORT afCmd; // HDATA_ flags.
* 
* This api allows an application to create a hDmgData apropriate
* for return from its call-back function.
* The passed in data is stored into the hDmgData which is
* returned on success.  Any portions of the data handle not filled are
* undefined.  afCmd contains any of the HDATA_ constants described below:
*
* HDATA_APPOWNED
*   This declares the created data handle to be the responsability of
*   the application to free it.  Application owned data handles may
*   be given to the DLL multiple times.  This allows a server app to be
*   able to support many clients without having to recopy the data for
*   each request.  If this flag is not specified, the data handle becomes
*   invalid once passed to the DLL via any API or the callback function.
*
* NOTES:
*   If an application expects this data handle to hold >64K of data via
*   DdeAddData(), it should specify a cb + cbOff to be as large as
*   the object is expected to get via DdeAddData() calls to avoid
*   unnecessary data copying or reallocation by the DLL.
*
*   if psrc==NULL or cb == 0, no actual data copying takes place.
* 
*   Data handles given to an application via the DdeClientXfer() or
*   DdeCheckQueue() functions are the responsability of the client
*   application to free and MUST NOT be returned from the callback
*   function as server data!  The DLL will only accept data handles
*   returned from the callback function that were created by the
*   called application.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
HDMGDATA EXPENTRY DdePutData(pSrc, cb, cbOff, hszItem, usFmt, afCmd)
PBYTE pSrc;
ULONG cb;
ULONG cbOff;
HSZ hszItem;
USHORT usFmt;
USHORT afCmd;
{
    PAPPINFO pai;

    if (!(pai = GetCurrentAppInfo(FALSE)))
        return(0L);;

    if (afCmd & ~(HDATA_APPOWNED)) {
        pai->LastError = DMGERR_INVALIDPARAMETER;
        return(0L);
    }

    return(PutData(pSrc, cb, cbOff, hszItem, usFmt, afCmd | HDATA_APPFREEABLE, pai));
}


/***************************** Public  Function ****************************\
* PUBDOC START
* HDMGDATA EXPENTRY DdeAddData(hDmgData, pSrc, cb, cbOff)
* HDMGDATA hDmgData;    // data handle to add data to
* PBYTE pSrc;           // pointer to data to add (if NULL, no data copying)
* ULONG cb;             // size of data to add
* ULONG cbOff;          // offset within hData to place data
*
* This routine allows an application to add data to a hDmgData it had
* previously created using DdePutData().  The handle will be
* grown as necessary to support the data addition.  Data may be added
* to a data handle multiple times and in any order of data locations.
*
* Once a data handle is given to the DLL via return from the callback
* function or via a DLL API, it becomes readonly.  Further attempts to
* add data to the hData by any process will fail.
*
* This call will return 0 if it failed to allocate enough memory or if
* the data handle is readonly.  On success, the returned hDmgData should 
* replace the given hDmgData.  On failure, the hDmgData given is left
* in the state it was initially.
*
* NOTE: For huge segments, or segments expected to grow to greater than 
*    64K, it is best if DdePutData() be called with a cbOff + cb as 
*    large as maximally expected so as not to force reallocation from a 
*    normal to a huge segment in 16bit applications.  This also avoids 
*    the need of replaceing the hDmgData with the output each time
*    incase a new selector was needed to be allocated and copied to.  
* 
* PUBDOC END
*
* History:
*   Created     9/17/89    Sanfords
\***************************************************************************/
HDMGDATA EXPENTRY DdeAddData(hDmgData, pSrc, cb, cbOff)
HDMGDATA hDmgData;
PBYTE pSrc;
ULONG cb;
ULONG cbOff;
{
#define pmyddes ((PMYDDES)hDmgData)
#define selIn (SELECTOROF(hDmgData))
    
    PAPPINFO pai;
    SEL sel;
    ULONG cbOffAbs;

    if (!(pai = GetCurrentAppInfo(FALSE)))
        return(0L);;

    if (!CheckSel(selIn) ||
            pmyddes->offszItemName != sizeof(MYDDES) ||
            pmyddes->magic != MYDDESMAGIC ||
            pmyddes->fs & HDATA_READONLY) {
        pai->LastError = DMGERR_INVALID_HDMGDATA;
        return(0L);
    }
    
    cbOffAbs = pmyddes->offabData + cbOff;
    if (pmyddes->cbData + pmyddes->offabData < cb + cbOffAbs) {
        /*
         * need to grow...
         */
        if (cbOffAbs + cb > 0xFFFFL) {
            /*
             * going to be huge...
             */
            if ((pmyddes->offabData + pmyddes->cbData < 0xFFFFL) ||
                    DosReallocHuge(HIUSHORT(cb + cbOffAbs),
                    LOUSHORT(cb + cbOffAbs), selIn)) {
                /*
                 * Either we can't grow a huge seg or we need to make one.
                 */
                if (DosAllocHuge(HIUSHORT(cb + cbOffAbs),
                        LOUSHORT(cb + cbOffAbs), &sel, 0, SEG_GIVEABLE)) {
                    pai->LastError = DMGERR_MEMORY_ERROR;
                    return(0);
                }
                CopyHugeBlock(hDmgData, MAKEP(sel, 0), pmyddes->cbData +
                        sizeof(MYDDES) + 1);
                FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&hDmgData,
                        FPI_DELETE);
                DosFreeSeg(selIn);
                hDmgData = MAKEP(sel, 0);
                AddPileItem(pai->pHDataPile, (PBYTE)&hDmgData, NULL);
            }
        } else {
            /*
             * not going to be huge
             */
            if (DosReallocSeg((USHORT)(cb + cbOffAbs), selIn)) { 
                pai->LastError = DMGERR_MEMORY_ERROR;
                return(0L);
            }
        }
        pmyddes->cbData = cbOff + cb;
    }
    if (pSrc)
        CopyHugeBlock(pSrc, HugeOffset((PBYTE)hDmgData, cbOffAbs), cb);
    return(hDmgData);
#undef selIn    
#undef pmyddes    
}


/***************************** Public  Function ****************************\
* PUBDOC START
* ULONG EXPENTRY DdeGetData(hDmgData, pDst, cbMax, cbOff)
* HDMGDATA hDmgData;    // data handle to extract data from
* PBYTE pDst;           // destination for extracted data
* ULONG cbMax;          // destination buffer size
* ULONG cbOff;          // offset into data to start extraction
*
* This copies up to cbMax bytes of data contained in the hDmgData data handle
* at offset cbOff into application memory pointed to by pDst.
* If pDst == NULL, no copying is performed.
*
* returns the size of the data contained in the data handle remaining after
* cbOff or 0 if hDmgData is invalid or cbOff is too large.
*
* This API supports HUGE segments in 16 bit applications.
*
* PUBDOC END
*
* History:
*   Created     12/14/88    Sanfords
\***************************************************************************/
ULONG EXPENTRY DdeGetData(hDmgData, pDst, cbMax, cbOff)
HDMGDATA hDmgData;
PBYTE pDst;
ULONG cbMax;
ULONG cbOff;
{
    PAPPINFO pai;

    if ((pai = GetCurrentAppInfo(FALSE)) == NULL)
        return(0L);

    if (!CheckSel(SELECTOROF(hDmgData))) {
        pai->LastError = DMGERR_INVALID_HDMGDATA;
        return(0L);
    }
    if (cbOff >= ((PMYDDES)hDmgData)->cbData) {
        pai->LastError = DMGERR_INVALIDPARAMETER;
        return(0L);
    }
    cbMax = min(cbMax, ((PMYDDES)hDmgData)->cbData - cbOff);
    if (pDst == NULL) 
        return(((PMYDDES)hDmgData)->cbData - cbOff); 
    CopyHugeBlock(HugeOffset(DDES_PABDATA(hDmgData), cbOff), pDst, cbMax);
    return(cbMax);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* PBYTE EXPENTRY DdeAccessData(hDmgData)
* HDMGDATA hDmgData;    // data handle to access
*
* This API returns a pointer to the data referenced by the data handle.
* The pointer returned becomes invalid once the data handle is freed
* or given to the DLL via callback return or API call.
*
* NOTE: applications MUST take care not to access beyond the limits of
* the data handle.  Only hDmgData's created by the application via
* a DdePutData() call may write to this memory prior to passing on
* to any DLL API or returning from the callback function.  Any hDmgData
* received from the DLL should be considered shared-readonly data and
* should be treated as such.  This applies whether the application owns
* the data handle or not.
*
* 0L is returned on error.
*
* PUBDOC END
*
* History:
*   Created     8/24/88    Sanfords
\***************************************************************************/
PBYTE EXPENTRY DdeAccessData(hDmgData)
HDMGDATA hDmgData;
{
    PAPPINFO pai;
    
    if ((pai = GetCurrentAppInfo(FALSE)) == NULL)
        return(0L);
    if (CheckSel(SELECTOROF(hDmgData))) {
        return(DDES_PABDATA(hDmgData));
    }
    pai->LastError = DMGERR_ACCESS_DENIED;
    return(0L);
}




/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeFreeData(hDmgData)
* HDMGDATA hDmgData;        // data handle to destroy
*
* This routine should be called by an application wishing to release
* custody of an hDmgData it owns.
* An application owns any hDmgData
* it created with the HDATA_APPOWNED flag or an hDmgData given to
* it via DdeClientXfer(), DdeCheckQueue(), DdeAppNameServer(),
* DdeCreateInitPkt() or DdeProcessPkt().
*
* Returns fSuccess.  This function will fail if the hDmgData is not
* owned by the calling app.
*
* PUBDOC END
* History:
*   Created     12/14/88    Sanfords
*   6/12/90 sanfords    Fixed to work with non-DLL generated selectors
\***************************************************************************/
BOOL EXPENTRY DdeFreeData(hDmgData)
HDMGDATA hDmgData;
{
    PAPPINFO pai;
    USHORT cbSel;
    TID tid;

    if ((pai = GetCurrentAppInfo(FALSE)) == NULL)
        return(FALSE);
        
    cbSel = CheckSel(SELECTOROF(hDmgData));
    if (!cbSel) {
        pai->LastError = DMGERR_INVALID_HDMGDATA;
        return(FALSE);
    }
    SemEnter();
    /*
     * Apps can only free handles the DLL does not own or handles from external
     * non-DLL DDE apps.
     */
    if (!FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&hDmgData, FPI_DELETE)) {
        pai->LastError = DMGERR_INVALID_HDMGDATA;
        SemLeave();
        return(FALSE);
    }    
            
    tid = pai->tid;
    do {
        if (FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&hDmgData, FPI_COUNT)) {
            SemLeave();
            return(TRUE);
        }
        pai = pai->nextThread;
    } while (pai && pai->tid != tid);
    DosFreeSeg(SELECTOROF(hDmgData));
        
    SemLeave();
    return(TRUE);    
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeCopyBlock(pSrc, pDst, cb)
* PBYTE pSrc;   // source of copy
* PBYTE pDst;   // destination of copy
* ULONG cb;     // size in bytes of copy
*
* This copy utility can handle HUGE segments and can be used by any
* application as a copy utility.  This does not support overlapping huge
* copies.
*
* Returns fSuccess.
*
* PUBDOC END
* History:      1/1/89  created         sanfords
\***************************************************************************/
BOOL EXPENTRY DdeCopyBlock(pSrc, pDst, cb)
PBYTE pSrc;
PBYTE pDst;
ULONG cb;
{
    return(CopyHugeBlock(pSrc, pDst, cb));
}




/***************************************************************************\
* PUBDOC START
* HSZ management notes:
*
*   HSZs are used in this DLL to simplify string handling for applications
*   and for inter-process communication.  Since many applications use a
*   fixed set of Application/Topic/Item names, it is convenient to convert
*   them to HSZs and allow quick comparisons for lookups.  This also frees
*   the DLL up from having to constantly provide string buffers for copying
*   strings between itself and its clients.
*
*   HSZs are the same as atoms except they have no restrictions on length or
*   number and are 32 bit values.  They are case preserving and can be
*   compared directly for case sensitive comparisons or via DdeCmpHsz()
*   for case insensitive comparisons.
*
*   When an application creates an HSZ via DdeGetHsz() or increments its
*   count via DdeIncHszCount() it is essentially claiming the HSZ for
*   its own use.  On the other hand, when an application is given an
*   HSZ from the DLL via a callback, it is using another application's HSZ
*   and should not free that HSZ via DdeFreeHsz().
*
*   The DLL insures that during the callback any HSZs given will remain
*   valid for the duration of the callback.
*
*   If an application wishes to keep that HSZ to use for itself as a
*   standard for future comparisons, it should increment its count so that,
*   should the owning application free it, the HSZ will not become invalid.
*   This also prevents an HSZ from changing its value.  (ie, app A frees it
*   and then app B creates a new one that happens to use the same HSZ code,
*   then app C, which had the HSZ stored all along (but forgot to increment
*   its count) now is holding a handle to a different string.)
*
*   Applications may free HSZs they have created or incremented at any time
*   by calling DdeFreeHsz().
*
*   The DLL internally increments HSZ counts while in use so that they will
*   not be destroyed until both the DLL and all applications concerned are
*   through with them.
*
*   IT IS THE APPLICATIONS RESPONSIBILITY TO PROPERLY CREATE AND FREE HSZs!!
*
* PUBDOC END
\***************************************************************************/


/***************************** Public  Function ****************************\
* PUBDOC START
* HSZ EXPENTRY DdeGetHsz(psz, country, codepage)
* PSZ psz;          // string to HSZize.
* USHORT country;   // country ID to use in comparisons.
* USHORT codepage;  // codepage to use in comparisons.
*
* This routine returns a string handle to the psz passed in.
* 0 is returned on failure or for NULL strings.
*
* If country is 0, the default system country code is used.
* If codepage is 0, the default system codepage code is used.
*
* String handles are similar to atoms but without the 255
* character limit on strings.  String handles are case preserving.
* see DdeCmpHsz().
*
* String handles are consistant across all processes using the DLL.
*
* PUBDOC END
*
* History:      1/1/89 created         sanfords
\***************************************************************************/
HSZ EXPENTRY DdeGetHsz(psz, country, codepage)
PSZ psz;
USHORT country, codepage;
{
    PAPPINFO pai;

    if ((pai = GetCurrentAppInfo(FALSE)) == NULL)
        return(0);

    return(GetHsz(psz, country, codepage, TRUE));
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeFreeHsz(hsz)
* HSZ hsz;      // string handle to free
*
* This function decrements the usage count for the HSZ given and frees
* it if the count becomes 0.
*
* PUBDOC END
*
* History:      1/1/89 created         sanfords
\***************************************************************************/
BOOL EXPENTRY DdeFreeHsz(hsz)
HSZ hsz;
{
    PAPPINFO pai;

    if ((pai = GetCurrentAppInfo(FALSE)) == NULL)
        return(0);
    return(FreeHsz(hsz));
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeIncHszCount(hsz)
* HSZ hsz;  // string handle to increment.
*
* This function increments the usage count for the HSZ given.  This is
* useful when an application wishes to keep an HSZ given to it in its
* callback.
*
* PUBDOC END
*
* History:      1/1/89 created         sanfords
\***************************************************************************/
BOOL EXPENTRY DdeIncHszCount(hsz)
HSZ hsz;
{
    PAPPINFO pai;

    if ((pai = GetCurrentAppInfo(FALSE)) == NULL)
        return(0);

    return(IncHszCount(hsz));
}



/***************************** Public  Function ****************************\
* PUBDOC START
* USHORT EXPENTRY DdeGetHszString(hsz, psz, cchMax)
* HSZ hsz;      // string handle to extract string from
* PSZ psz;      // buffer for case-sensitive string
* ULONG cchMax; // buffer size.
*
* This API is the inverse of DdeGetHsz().  The actual length of the
* string (without NULL terminator) referenced by hsz is returned.
*
* if psz is NULL, no string is returned in psz.
* 0 is returned if hsz does not exist or is wild.
* If hsz is wild, psz will be set to a 0 length string.
*
* PUBDOC END
* History:      Created 5/10/89         sanfords
\***************************************************************************/
USHORT EXPENTRY DdeGetHszString(hsz, psz, cchMax)
HSZ hsz;
PSZ psz;
ULONG cchMax;
{
    if (psz) {
        if (hsz) {
            return(QueryHszName(hsz, psz, (USHORT)cchMax));
        } else {
            *psz = '\0';
            return(0);
        }
    } else if (hsz) {
        return(QueryHszLength(hsz));
    } else {
        return(0);
    }
}


/***************************** Public  Function ****************************\
* PUBDOC START
* SHORT EXPENTRY DdeCmpHsz(hsz1, hsz2)
* HSZ hsz1, hsz2;   // string handles to compare.
*
* This routine returns:
*   0  if hsz1 is of equal  rank to   hsz2
*   -2 if hsz1 is invalid.
*   -1 if hsz1 is of lower  rank than hsz2
*   1  if hsz1 is of higher rank than hsz2.
*   2  if hsz2 is invalid.
*
* Note that direct comparison of hszs (ie (hsz1 == hsz2)) is a case sensitive
* comparison.  This function performs a case insensitive comparison.  Thus
* different valued hszs may actually be equal.
* A ranking is provided for binary searching.
*
* PUBDOC END
\***************************************************************************/
SHORT EXPENTRY DdeCmpHsz(hsz1, hsz2)
HSZ hsz1, hsz2;
{
    return(CmpHsz(hsz1, hsz2));
}


/***************************** Public  Function ****************************\
* PUBDOC START
* ULONG EXPENTRY DdeCheckQueue(hConv, phDmgData, idXfer, afCmd)
* HCONV hConv;          // related convesation handle
* PHDMGDATA phDmgData;  // OUTPUT: for resultant data handle, or NULL
* ULONG idXfer;         // transaction ID or QID_NEWEST or QID_OLDEST
* ULONG afCmd;          // queue operation code.
*
*       This routine checks a client conversation's assynchronous 
*       transaction queue for queued transaction status.  This allows a 
*       client to extract data or monitor the status of any transaction 
*       previously started asynchronously.  (TIMEOUT_ASSYNC) hConv is 
*       the client conversation who's queue is being checked.  
*
*       If phDmgData is not NULL and the referenced item has data to 
*       return to the client, phDmgData is filled.  Returned hDmgDatas 
*       must be freed by the application.  phDmgData will be filled with 
*       0L if no return data is applicable or if the transaction is not 
*       complete.  
*
*       If the queue is not periodicly flushed by an application 
*       issueing asynchronous transactions the queue is automaticly
*       flushed as needed.  Oldest transactions are flushed first.  
*       DdeProcessPkt() and DdeDisconnect() and DdeEndEnumServers
*       remove items from this queue.  
*
*       idXfer is the transaction id returned by an asynchronous call to 
*       DdeClientXfer().
* 
* afCmd = CQ_FLUSH - remove all items in the queue - return is fSuccess.
* afCmd = CQ_REMOVE - the item referenced is removed from the queue.
* afCmd = CQ_NEXT - references the idXfer AFTER (more recent than) the id 
*       given. 0 is returned if the ID given was the newest in the 
*       queue otherwise the next ID is returned.
* afCmd = CQ_PREV - references the idXfer BEFORE (less recent than) the id 
*       given. 0 is returned if the ID given was the oldest in the 
*       queue otherwise the previous ID is returned.
* afCmd = CQ_COUNT    - returns the number of entries in the queue.
*
*       By ORing in one of the following flags, the above flags can be 
*       made to reference the apropriate subset of queue entries: 
*
* afCmd = CQ_ACTIVEONLY - incomplete active transactions only.
* afCmd = CQ_COMPLETEDONLY - completed transactions only.
* afCmd = CQ_FAILEDONLY - transactions which had protocol violations or
*                         communication failures.
* afCmd = CQ_INACTIVEONLY - The complement of CQ_ACTIVEONLY which is the
*                           union of CQ_COMPLETEDONLY and CQ_FAILEDONLY.
* 
* if phdmgdata = NULL, no hdmgdata is returned.
* if idXfer == QID_NEWEST, the top-most (most recent) entry in the 
*       queue is referenced.
* if idXfer == QID_OLDEST, the bottom-most (oldest) entry ID is 
*       referenced.
*
*       returns the ID of the item processed if it applies, or the count 
*       of items or fSuccess.  
*
*
* Standard usage examples:
*
*   To get the state of the oldest transaction:
*   id = DdeCheckQueue(hConv, &hDmgData, QID_OLDEST, 0)
*
*   To get and flush the next completed transaction data, if there is 
*       any:
*   id = DdeCheckQueue(hConv, &hDmgData, QID_OLDEST, CQ_REMOVE | 
*       CQ_INACTIVEONLY)
*
*   To flush all successfully completed transactions:
*   DdeCheckQueue(hConv, NULL, QID_OLDEST, CQ_FLUSH | CQ_COMPLETEDONLY)
*
*   To see if a specific transaction is complete, and if so, to get the
*   information and remove the transaction from the queue:
*   if (DdeCheckQueue(hConv, &hDmgData, id, CQ_REMOVE | CQ_INACTIVEONLY))
*       ProcessAndFreeData(hDmgData);
*       
* PUBDOC END
* History:      Created 6/6/89         sanfords
\***************************************************************************/
ULONG EXPENTRY DdeCheckQueue(hConv, phDmgData, idXfer, afCmd)
HCONV hConv;
PHDMGDATA phDmgData;
ULONG idXfer;
ULONG afCmd;
{
    PAPPINFO pai;
    PCLIENTINFO pci;
    PCQDATA pcqd, pcqdT, pcqdEnd;
    USHORT err;
    ULONG retVal = TRUE;
    int i;

    if ((pai = GetCurrentAppInfo(TRUE)) == 0)
        return(0);

    SemCheckOut();
    SemEnter();
    
    err = DMGERR_INVALIDPARAMETER;
    if (!WinIsWindow(DMGHAB, hConv) ||
            !(BOOL)WinSendMsg(hConv, UM_QUERY, (MPARAM)Q_CLIENT, 0L) ||
            idXfer == QID_SYNC) {
        goto failExit;
    }
    
    if (!(pci = (PCLIENTINFO)WinSendMsg(hConv, UM_QUERY, (MPARAM)Q_ALL, 0L))) 
        goto failExit;
    
    err = DMGERR_UNFOUND_QUEUE_ID;
    if ((pcqd = (PCQDATA)Findqi(pci->pQ, idXfer)) == NULL)
        goto failExit;

    /*
     * if referencing an end item, make sure it fits any subset flags.
     * If it doesn't we alter afCmd to force us to the first qualifying
     * entry in the correct direction.
     */
    if (!fInSubset(pcqd, afCmd)) {
        if (idXfer & (QID_OLDEST))
            afCmd |= CQ_NEXT;
        else if (idXfer & (QID_NEWEST))
            afCmd |= CQ_PREV;
        else if (!(afCmd & (CQ_NEXT | CQ_PREV | CQ_COUNT | CQ_FLUSH)))
            goto failExit;
    }
    
    if (afCmd & CQ_NEXT) {
        pcqdEnd = (PCQDATA)pci->pQ->pqiHead->next;
        if ((pcqd = (PCQDATA)pcqd->next) == pcqdEnd)
            goto failExit;
        while (!fInSubset(pcqd, afCmd)) {
            if ((pcqd = (PCQDATA)pcqd->next) == pcqdEnd)
                goto failExit;
        }
    }
            
    if (afCmd & CQ_PREV) {
        pcqdEnd = (PCQDATA)pci->pQ->pqiHead;
        if ((pcqd = (PCQDATA)pcqd->prev) == pcqdEnd)
            goto failExit;
        while (!fInSubset(pcqd, afCmd)) {
            if ((pcqd = (PCQDATA)pcqd->prev) == pcqdEnd)
                goto failExit;
        }
    }
    
    /*
     * pcqd now points to the apropriate entry
     */

    if (afCmd & CQ_COUNT) 
        retVal = 0;
    else
        retVal = MAKEID(pcqd);

    /*
     * Fill phDmgData if specified.
     */
    if (phDmgData != NULL)
        if ((pcqd->xad.state == CONVST_CONNECTED) &&
                CheckSel(SELECTOROF(pcqd->xad.pddes))) {
            *phDmgData = pcqd->xad.pddes;
            AddPileItem(pai->pHDataPile, (PBYTE)phDmgData, CmpULONG);
            if (((PMYDDES)*phDmgData)->magic == MYDDESMAGIC) {
                ((PMYDDES)*phDmgData)->fs |= HDATA_READONLY;
            }
        } else
            *phDmgData = NULL;

    /*
     * remove pcqd if apropriate.
     */   
    if (afCmd & (CQ_REMOVE | CQ_FLUSH)) {
        if (!FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&pcqd->xad.pddes,
                0))
            FreeData((PMYDDES)pcqd->xad.pddes, pai);
        /*
         * Decrement the use count we incremented when the client started
         * this transaction.
         */
        FreeHsz(pcqd->XferInfo.hszItem);
        Deleteqi(pci->pQ, MAKEID(pcqd));
    }

    /*
     * go through entire list and flush or count if specified.
     */
    if (afCmd & (CQ_FLUSH | CQ_COUNT)) {
        pcqd = (PCQDATA)pci->pQ->pqiHead;
        for (i = pci->pQ->cItems; i; i--) {
            if (fInSubset(pcqd, afCmd)) {
                if (afCmd & CQ_COUNT) 
                    retVal++;
                if (afCmd & CQ_FLUSH) {
                    pcqdT = (PCQDATA)pcqd->next;
                    /*
                     * Only free the data if not logged - ie the user never got a
                     * copy.
                     */
                    if (!FindPileItem(pci->ci.pai->pHDataPile, CmpULONG,
                            (PBYTE)&pcqd->xad.pddes, 0))
                        FreeData((PMYDDES)pcqd->xad.pddes, pai);
                    /*
                     * Decrement the use count we incremented when the client started
                     * this transaction.
                     */
                    FreeHsz(pcqd->XferInfo.hszItem);
                    Deleteqi(pci->pQ, MAKEID(pcqd));
                    pcqd = pcqdT;
                    continue;
                }
            }
            pcqd = (PCQDATA)pcqd->next;
        }
    }
        
    SemLeave();
    SemCheckOut();
    return(retVal);
    
failExit:
    pai->LastError = err;
    SemLeave();
    SemCheckOut();
    return(0);
}



BOOL fInSubset(pcqd, afCmd)
PCQDATA pcqd;
ULONG afCmd;
{
    if (afCmd & CQ_ACTIVEONLY && (pcqd->xad.state <= CONVST_INIT1))
        return(FALSE);

    if (afCmd & CQ_INACTIVEONLY && (pcqd->xad.state > CONVST_INIT1))
        return(FALSE);

    if (afCmd & CQ_COMPLETEDONLY && (pcqd->xad.state != CONVST_CONNECTED))
        return(FALSE);
                            
    if (afCmd & CQ_FAILEDONLY && (pcqd->xad.state > CONVST_TERMINATED))
        return(FALSE);
    return(TRUE);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeEnableCallback(hConv, fEnable)
* HCONV hConv;      // server conversation handle to enable/disable or NULL 
* BOOL fEnable;     // TRUE enables, FALSE disables callbacks.
*
* This routine is used by an application that does not want to be interrupted
* by DLL calls to its Callback function.  When callbacks are disabled,
* all non critical attempts to call the Callback function instead result in
* the call being placed on a server transaction queue until callbacks are
* reenabled.  An application should reenable callbacks in a timely manner
* to avoid causing synchronous clients to time out.
*
* note that DdeProcessPkt() has the side effect of unblocking and removing
* items from the server transaction queue when processing return packets.
* DdeDisconnect() and DdeEndEnumServers() have the side effect of removing
* any transactions for the disconnected conversation.
*
* If hConv is non-NULL, only the hConv conversation is affected.
* If hConv is NULL, all conversations are affected.
*
* Callbacks can also be disabled on return from the callback function by
* returning the constant CBR_BLOCK.  This has the same effect as
* calling DdeEnableCallback(hConv, FALSE) except that the callback
* returned from is placed back on the callback queue for later re-submission
* to the callback function when the conversations callbacks are reenabled.
* This allows a server that needs a long time to process a request to delay
* returning the result without blocking within the callback function.
* 
* No callbacks are made within this function.
* 
* Note:  Callback transactions that have XTYPF_NOBLOCK set in their usType
*       parameter cannot be blocked by CBR_BLOCK.
*       These callbacks are issued to the server regardless of the state of
*       callback enableing.
*
* fSuccess is returned.
*
* PUBDOC END
* History:      Created 6/6/89         sanfords
\***************************************************************************/
BOOL EXPENTRY DdeEnableCallback(hConv, fEnable)
HCONV hConv;
BOOL fEnable;
{
    PAPPINFO pai;

    if ((pai = GetCurrentAppInfo(TRUE)) == 0)
        return(FALSE);

    SemCheckOut();
    SemEnter();
    if (hConv == NULL) {
        pai->fEnableCB = fEnable = fEnable ? TRUE : FALSE;
        FlushLst(pai->plstCBExceptions);
    } else if (pai->fEnableCB != fEnable) {
        if (pai->plstCBExceptions == NULL) 
            pai->plstCBExceptions = CreateLst(pai->hheapApp, sizeof(HWNDLI));
        AddHwndList((HWND)hConv, pai->plstCBExceptions);
    }
    SemLeave();
    if (fEnable)
        WinPostMsg(pai->hwndDmg, UM_CHECKCBQ, (MPARAM)pai, 0L);
    return(TRUE);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* HDMGDATA EXPENTRY DdeAppNameServer(hszApp, afCmd);
* HSZ hszApp;       // referenced app name
* USHORT afCmd;     // action code.
*
* Updates or queries the DDE DLL application name server.  The DDE name
* server acts as a filter for initiate messages on behalf of registered
* applications and serves as a way for an application to determine what
* other applications are available without the need for wild initiates.
* Any change in the name server results in XTYP_REGISTER or XTYP_UNREGISTER
* callbacks to all other applications.  Agents will not be allowed to
* know about any other agent registrations.
*
* afCmd:
*
*   ANS_REGISTER
*       Adds hszApp to the name server.  hszApp cannot be NULL.
*       All other applications will receive a registration notification
*       if their callback filters allow it.
*       fSuccess is returned.
*
*   ANS_UNREGISTER
*       Remove hszApp from the name server.  If hszApp==NULL the name server
*       is cleared for the calling application.
*       All other applications will receive a deregistration notification
*       if their callback filters allow it.
*       fSuccess is returned.
*
*   ANS_QUERYALLBUTME
*       Returns a zero terminated array of hszApps/hApp pairs registered with
*       the name server by all other applications.
*       (Agent applications do not see other agent registered names.)
*       If hszApp is set, only names matching* hszApp are placed into the list.
*       0 is returned if no applicable names were found.
*
*   ANS_QUERYMINE
*       Returns a zero terminated array of hszApps registered with the name
*       server by the calling application.  If hszApp is set, only names
*       matching hszApp are placed into the list.  0 is returned if no
*       applicable names were found.
*
*   The following flags may be ORed in with one of the above flags:
*
*   ANS_FILTERON
*       Turns on dde initiate callback filtering.  Only wild app names or
*       those matching a registered name are given to the application for
*       initiate requests or registration notifications.
*
*   ANS_FILTEROFF
*       Turns off dde initiate callback filtering.  All initiate requests
*       and registration notifications are passed onto the application.
*
* A 0 is returned on error.
* 
* PUBDOC END
*
* History:
\***************************************************************************/
HDMGDATA EXPENTRY DdeAppNameServer(hszApp, afCmd)
HSZ hszApp;
USHORT afCmd;
{
    PAPPINFO pai, paiT;
    BOOL fAgent;
    HDMGDATA hData;
    HSZ hsz;
    
    if ((pai = GetCurrentAppInfo(TRUE)) == 0)
        return(FALSE);

    if (afCmd & ANS_FILTERON)
        pai->afCmd |= DMGCMD_FILTERINITS;

    if (afCmd & ANS_FILTEROFF) 
        pai->afCmd &= ~DMGCMD_FILTERINITS;

    
    if (afCmd & (ANS_REGISTER | ANS_UNREGISTER)) {
        
        if (pai->afCmd & DMGCMD_CLIENTONLY) {
            pai->LastError = DMGERR_DLL_USAGE;
            return(FALSE);
        }
    
        fAgent = pai->afCmd & DMGCMD_AGENT ? TRUE : FALSE;
        
        if (hszApp == NULL) {
            if (afCmd & ANS_REGISTER) {
                /*
                 * registering NULL is not allowed!
                 */
                pai->LastError = DMGERR_INVALIDPARAMETER;
                return(FALSE);
            }
            /*
             * unregistering NULL is just like unregistering each
             * registered name.
             */
            while (PopPileSubitem(pai->pAppNamePile, (PBYTE)&hsz)) {
                for (paiT = pAppInfoList; paiT; paiT = paiT->next) {
                    if (pai == paiT || (fAgent && (paiT->afCmd & DMGCMD_AGENT))) 
                        continue;
                    WinPostMsg(paiT->hwndFrame, UM_UNREGISTER, (MPARAM)hsz,
                            (MPARAM)pai->hwndFrame);
                }
            }
            return(TRUE);
        }
            
        if (afCmd & ANS_REGISTER) {
            if (pai->pAppNamePile == NULL) 
                pai->pAppNamePile = CreatePile(hheapDmg, sizeof(HSZ), 8);
            AddPileItem(pai->pAppNamePile, (PBYTE)&hszApp, NULL);
        } else
            FindPileItem(pai->pAppNamePile, CmpULONG, (PBYTE)&hszApp,
                    FPI_DELETE);

        for (paiT = pAppInfoList; paiT; paiT = paiT->next) {
            if (pai == paiT ||
                    (fAgent && (paiT->afCmd & DMGCMD_AGENT))) {
                continue;
            }
            WinPostMsg(paiT->hwndFrame,
                    afCmd & ANS_REGISTER ? UM_REGISTER : UM_UNREGISTER,
                    (MPARAM)hszApp, (MPARAM)pai->hwndFrame);
        }
        return(TRUE);
    }

    if (afCmd & ANS_QUERYMINE) {
        hData = PutData(NULL, 10L * sizeof(HSZ),
                0L, 0L, 0, HDATA_APPOWNED | HDATA_APPFREEABLE, pai);
        if (QueryAppNames(pai, hData, hszApp, 0L)) {
            return(hData);
        } else {
            DdeFreeData(hData);
            return(0L);
        }
    }

    if (afCmd & ANS_QUERYALLBUTME) {
        ULONG offAdd, offAddNew;
        
        hData = PutData(NULL, 10L * sizeof(HSZ),
                0L, 0L, 0, HDATA_APPOWNED | HDATA_APPFREEABLE, pai);
        *((PHSZ)DDES_PABDATA((PDDESTRUCT)hData)) = 0L;
        SemEnter();
        paiT = pAppInfoList;
        offAdd = 0L;
        while (paiT) {
            if (paiT != pai &&
                    !(fAgent && (paiT->afCmd & DMGCMD_AGENT))) {
                offAddNew = QueryAppNames(paiT, hData, hszApp, offAdd);
                if (offAddNew == 0 && offAddNew < offAdd) {
                    /*
                     * memory error most likely.
                     */
                    SemLeave();
                    DdeFreeData(hData);
                    return(0L);
                }
                offAdd = offAddNew;
            }
            paiT = paiT->next;
        }
        SemLeave();
        return(hData);
    }

    return(0L);
}



/***************************** Public  Function ****************************\
* PUBDOC START
* HDMGDATA EXPENTRY DdeCreateInitPkt(
* HSZ hszApp,         // initiate app string
* HSZ hszTopic,       // initiate topic string
* HDMGDATA hDmgData)  // packet data containing language information.
*
* This routine is called by agent applications from within their callback
* functions which need to store transaction initiate data into a 
* network-portable packet.  On return, HDMGDATA contains the packeted 
* data.  The calling application must free this data handle when 
* through.  Agents are given access to the app string so that they
* can modify it if needed for their net protocol.
*
* PUBDOC END
\***************************************************************************/
HDMGDATA EXPENTRY DdeCreateInitPkt(hszApp, hszTopic, hDmgData)
HSZ hszApp;
HSZ hszTopic;
HDMGDATA hDmgData;
{
    PAPPINFO pai;

    hszApp;
    hszTopic;
    hDmgData;

    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(0);
    /*
     * Add hDataRet to thread list
     */
    pai->LastError = DMGERR_NOT_IMPLEMENTED;
}


/***************************** Public  Function ****************************\
* PUBDOC START
* BOOL EXPENTRY DdeProcessPkt(
* HDMGDATA hPkt,        // packet from net to process
* ULONG hAgentFrom);    // foreign agent handle associated with this packet.
*
* This routine is called by agent applications which have received a packet
* from another agent.  This call performs what actions are requested by the
* hPkt.  If the particular transaction can be done synchronously, a return
* packet is returned.  If not, an XTYP_RTNPKT callback will eventually be
* sent to the agent and 0 is returned.  0 is also returned on error.
*
* hAgentFrom identifies the agent the packet came from.
*
* This call should NOT be made from within a callback.
*
* PUBDOC END
\***************************************************************************/
HDMGDATA EXPENTRY DdeProcessPkt(hPkt, hAgentFrom)
HDMGDATA hPkt;
ULONG hAgentFrom;
{
    PAPPINFO pai;

    hPkt;
    hAgentFrom;
    
    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(0);
    pai->LastError = DMGERR_NOT_IMPLEMENTED;
}



/***************************** Public  Function ****************************\
* PUBDOC START
* ULONG EXPENTRY DdeQueryVersion()
*
* Returns the DLL version number: 0xjjmmuuppL
* jj = major release;
* mm = minor release;
* uu = update number;
* pp = platform ID; 1=OS/2, 2=DOS, 3=WINDOWS, 4=UNIX, 5=MAC, 6=SUN
* 
*
* PUBDOC END
\***************************************************************************/
ULONG EXPENTRY DdeQueryVersion()
{
    return(MAKEULONG(MAKESHORT(1, rup),MAKESHORT(rmm, rmj)));
}



/* PUBDOC START
 * ----------------------- Platform specific APIs -----------------------
 * PUBDOC END
 */

/***************************** Public  Function ****************************\
* PUBDOC START
* HCONV EXPENTRY DdeConverseWithWindow(
* HWND hwnd,            // server window to converse with
* HSZ hszApp,           // app name to converse on
* HSZ hszTopic,         // topic name to converse on
* PCONVCONTEXT pCC)     // language information to converse on
* 
* This creates a pre-initiated client conversation with the given hwnd. 
* The conversation is assumed to be on the given app, topic and context.
* This is useful for implementing such things as drag and drop DDE 
* protocols.  hszApp and hszTopic cannot be NULL.  See
* DdeCreateServerWindow().
*  
* PUBDOC END
\***************************************************************************/
HCONV EXPENTRY DdeConverseWithWindow(hwnd, hszApp, hszTopic, pCC)
HWND hwnd;     
HSZ hszApp, hszTopic;  
PCONVCONTEXT pCC;
{
    PAPPINFO pai;

    UNUSED hwnd;
    UNUSED hszApp;
    UNUSED hszTopic;
    UNUSED pCC;
    
    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(0);

    if (QuerylatomLength((LATOM)hszApp) == 0 ||
            QuerylatomLength((LATOM)hszTopic) == 0) {
        pai->LastError = DMGERR_INVALIDPARAMETER;
        return(0);
    }
    
}


/***************************** Public  Function ****************************\
* PUBDOC START
* HWND DdeCreateServerWindow(hszApp, hszTopic, pCC)
* HSZ hszApp,       // app name to accept conversations on.
* HSZ hszTopic;     // topic name to accept conversations on.
* PCONVCONTEXT pCC; // language information to accept or NULL for sys default.
*    
* This creates a pre_initiated server DDE window for the caller.  The 
* server window does not know what client window it will be talking to.  As 
* soon as any DDE message is received by the DDE server window, its 
* companion client hwnd is remembered and the conversation is 'locked in'.  
* The hwnd of the server window is returned.  hszApp and hszTopic cannot 
* be NULL.  
*    
* Typically, the server app would call CreateServerWindow() and pass the 
* hwnd to some prospective client via some other transport mechanism.  The 
* client would then call ConverseWithWindow() with the hwnd it received 
* from the server and then begin DDE transactions using the HCONV returned.  
*    
* A server may pass this hwnd to many potential clients.  Only the first 
* one that responds would get 'locked in'.  The rest would be out of luck 
* and their API calls would fail.  The server window ignores messages that
* to not fit the app and topic given.
*  
* PUBDOC END
\****************************************************************************/
HWND EXPENTRY DdeCreateServerWindow(hszApp, hszTopic, pCC)
HSZ hszApp, hszTopic;
PCONVCONTEXT pCC;
{
    PAPPINFO pai;
    INITINFO ii;
    HWND hwndServer;
    
    hszApp;
    hszTopic;
    pCC;
    
    if ((pai = GetCurrentAppInfo(TRUE)) == NULL)
        return(0);
        
    if (QuerylatomLength((LATOM)hszApp) == 0 ||
            QuerylatomLength((LATOM)hszTopic) == 0) {
        pai->LastError = DMGERR_INVALIDPARAMETER;
        return(0);
    }
    
    if ((hwndServer = CreateServerWindow(pai, hszTopic)) == NULL)
        return(0);

    ii.hszAppName = hszApp;
    ii.hszTopic = hszTopic;
    ii.hwndSend =
    ii.hwndFrame = NULL;
    ii.pCC = pCC;
    WinSendMsg(hwndServer, UMSR_INITIATE, 0L, 0L);
}
