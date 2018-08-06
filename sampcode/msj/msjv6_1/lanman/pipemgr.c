/*-------------------------------------------------------------------
  PIPEMGR.C -- PBX Pipe Manager routine

  Description:

  This thread manages pipes until they are connected, at which
  point control is transferred to the next available Router thread.
  The only data recognized by this thread are PBX directed packets
  (i.e., Register, Connect, and ListQuery requests).

  Author:  Brendan Dixon
           Microsoft, inc.
           LAN Manager Developer Support

  This code example is provided for demonstration purposes only.
  Microsoft makes no warranty, either express or implied,
  as to its usability in any given situation.
-------------------------------------------------------------------*/

// Includes ---------------------------------------------------------
#define   INCL_DOS
#define   INCL_DOSERRORS
#include  <os2.h>

#define   INCL_NETAUDIT
#define   INCL_NETSERVICE
#define   INCL_NETERRORS
#include  <lan.h>

#include  <errno.h>
#include  <memory.h>
#include  <process.h>
#include  <stddef.h>
#include  <stdio.h>
#include  <string.h>

#include  "pbxsrv.h"

// Function Prototypes (for functions not in PBXSRV.H) --------------
void      Register   (PBXPKT        *pbPkt,
                      ROUTEENT _far *pbREnt);
BOOL      Connect    (PBXPKT        *pbPkt,
                      ROUTEENT _far *pbREnt);
void      ListQuery  (PBXPKT        *pbPkt,
                      ROUTEENT _far *pbREnt);
USHORT    TblIndex   (PBXNAME       *pPBXName);


/* ReadPKT ----------------------------------------------------------
  Description:  Read data from a pipe
  Input      :  pbBuf   ----- Pointer to buffer
                usSize  ----- Number of bytes to read
                pbREnt  ----- Pointer to local routing table entry
  Output     :  True if the request succeeds, False otherwise
-------------------------------------------------------------------*/
BOOL ReadPKT(PCH           pbBuf,
             USHORT        usSize,
             ROUTEENT _far *pbREnt)
{
  BOOL    fState = TRUE;                  // Success/Failure flag
  CHAR    pszMsg[80];                     // Error message buffer
  USHORT  usByteCnt;                      // DosRead bytes read
  USHORT  usRetCode;                      // Return code

  // Read the incoming data
  usRetCode = DosRead(pbREnt->hLine,  // Line
                      pbBuf,          // Data buffer
                      usSize,         // Amount to read
                      &usByteCnt);    // Amount read

  // Was the read successful?
  if (usRetCode                    &&
      usRetCode != ERROR_MORE_DATA &&
      usRetCode != ERROR_NO_DATA    ) {
    if (usRetCode != ERROR_BROKEN_PIPE)
      ERRRPT("DosRead", usRetCode, Warning);
    fState = FALSE;
  }

  // Were all the bytes read?
  else if (usByteCnt != usSize) {
    sprintf(pszMsg,
            "Read unexpected length:  Expected(%u), Actual(%u)",
            usSize, usByteCnt);
    ERRRPT(pszMsg, 0, Warning);
    fState = FALSE;
  }

  // Decrement number of bytes available by number of bytes read
  pbREnt->usRData -= usByteCnt;

  return fState;
}


/* SendPKT ----------------------------------------------------------
  Description:  Send data to a pipe
  Input      :  pbBuf   ----- Pointer to buffer
                usSize  ----- Number of bytes to send
                pbREnt  ----- Pointer to local routing table entry
  Output     :  True if the request succeeds, False otherwise
-------------------------------------------------------------------*/
BOOL SendPKT(PCH           pbBuf,
             USHORT        usSize,
             ROUTEENT _far *pbREnt)
{
  BOOL    fState = TRUE;                  // Success/Failure flag
  CHAR    pszMsg[80];                     // Error message buffer
  USHORT  usByteCnt;                      // Number of bytes written
  USHORT  usRetCode;                      // Return code

  // Write the outgoing data
  usRetCode = DosWrite(pbREnt->hLine,   // Line
                       pbBuf,           // Data buffer
                       usSize,          // Amount to write
                       &usByteCnt);     // Amount written

  // Was the write successful?
  if (usRetCode) {
    if (usRetCode != ERROR_BROKEN_PIPE)
      ERRRPT("DosWrite", usRetCode, Warning);
    fState = FALSE;
  }

  // Were all the bytes written?
  else if (usByteCnt != usSize) {
    sprintf(pszMsg,
            "Write unexpected length:  Expected(%u), Actual(%u)",
            usSize, usByteCnt);
    ERRRPT(pszMsg, 0, Warning);
    fState = FALSE;
  }

  // Reduce the write space available by the number of bytes written
  pbREnt->usWSpace -= usByteCnt;

  return fState;
}


/* TblIndex ---------------------------------------------------------
  Description:  Locate a client name in the routing table and return
                its index
  Input      :  pPBXName --- Pointer to a PBXNAME structure
  Output     :  Table index of client name if found
                pbPBXMem->usLines if the client name is not found
-------------------------------------------------------------------*/
USHORT TblIndex(PBXNAME *pbPBXName)
{
  register USHORT  usLines;               // Total number of lines
  register USHORT  usIndex;               // Index into Routing Table

  // Check all table entries for one which is owned and whose client
  // name matches the one supplied by the caller
  for (usIndex = 0, usLines = pbPBXMem->usLines;
       usIndex < usLines;
       usIndex++) {
    if (pbPBXMem->abRTable[usIndex].fState > Open                &&
        pbPBXMem->abRTable[usIndex].usClientType ==
                                         pbPBXName->usClientType &&
        !strcmp(pbPBXMem->abRTable[usIndex].pszName,
                                         pbPBXName->pszName)      )
      break;
  }

  return usIndex;
}


/* Register ---------------------------------------------------------
  Description:  Set client name and type in the routing table
                Additional Register requests can be used to change a
                client name and/or type
  Input      :  pbPkt   ----- Pointer to PBX Packet
                pbREnt  ----- Pointer to local routing table entry
  Output     :  None
-------------------------------------------------------------------*/
void Register (PBXPKT        *pbPkt,
               ROUTEENT _far *pbREnt)
{
  PBXAUDIT  bPBXAudit;                    // Audit record buffer
  CHAR      pszMsg[80];                   // Message buffer
  USHORT    usTemp;                       // Temporary variable
  USHORT    usRetCode;                    // Return code

  // Inform the MakePipe thread that more pipes may be needed
  DosSemClear(&(pbPBXMem->semMakePipe));

  // Prepare client ACK packet
  pbPkt->usPktID |= ACK;

  // Did the client supply too many names?
  if (pbPkt->usNameCnt > 1) {
    pbPkt->usRetCode = R_TOOMANYNAMES;
    // Read extraneous information from the pipe
    for (usTemp = pbPkt->usNameCnt - 1;
         usTemp;
         usTemp--)
      ReadPKT((PCH)pbPBXMem->pbBuf, sizeof(PBXNAME), pbREnt);
  }

  // Did the client supply a name entry?
  else if (pbPkt->usNameCnt               < 1   ||
           *(pbPkt->aPBXName[0].pszName) == '\0' ) {
    pbPkt->usRetCode = R_NONAME;
  }

  // Was a valid client type supplied?
  else if (pbPkt->aPBXName[0].usClientType == 0) {
      pbPkt->usRetCode = R_INVALIDCLIENTTYPE;
  }

  // Is another entry in the routing table using the name?
  else {
    usTemp = TblIndex(&(pbPkt->aPBXName[0]));
    if (usTemp <  pbPBXMem->usLines                     &&
        usTemp != (USHORT)(pbREnt-(pbPBXMem->abRTable))  ) {
      pbPkt->usRetCode = R_DUPLICATENAME;
    }

    // Everything is Ok, assign current entry to the client
    else {
      strcpy(pbREnt->pszName, pbPkt->aPBXName[0].pszName);
      pbREnt->usClientType = pbPkt->aPBXName[0].usClientType;
      pbREnt->fState       = Owned;
      pbPkt->usRetCode     = NO_ERROR;
      DosSemRequest(&(pbPBXMem->semPBXMem), SEM_INDEFINITE_WAIT);
      pbPBXMem->usLinesOwned++;
      DosSemClear(&(pbPBXMem->semPBXMem));

      // If auditing has been enabled, write a RegUser audit record
      if (pbPBXMem->fAuditing) {
        memset(&bPBXAudit, '\0', sizeof(PBXAUDIT));
        bPBXAudit.fType = RegUser;
        strcpy(bPBXAudit.pbRegister.pszName, pbREnt->pszName);

        usRetCode = NetAuditWrite(
                            PBXAUDITEVENT,            // Record type
                            (char _far *)&bPBXAudit,  // Buffer
                            sizeof(PBXAUDIT),         // Buffer size
                            NULL, NULL);              // Required
        if (usRetCode                     &&
            usRetCode != NERR_LogOverflow  ) {
          ERRRPT("NetAuditWrite", usRetCode, Warning);
        }
      }
    }
  }

  // Send client an acknowledgement and return
  if (!SendPKT((PCH)pbPkt, sizeof(PBXPKT), pbREnt)) {
    sprintf(pszMsg, "Unable to send Register ACK to %s",
            pbREnt->pszName);
    ERRRPT(pszMsg, 0, Warning);
  }
  return;
}


/* Deregister -------------------------------------------------------
  Description:  Remove a line from the routing table
  Input      :  pbREnt  ----- Pointer to local routing table entry
  Output     :  None
-------------------------------------------------------------------*/
void Deregister (ROUTEENT _far *pbREnt)
{
  PBXAUDIT  bPBXAudit;                    // Audit record buffer
  USHORT    usCIndex;                     // Client table index
  USHORT    usTIndex;                     // Target table index
  USHORT    usRetCode;                    // Return code

  // If a connection exists between this client and another,
  // then also close the target pipe
  usCIndex = pbREnt - pbPBXMem->abRTable;
  usTIndex = pbREnt->usConnection;
  if (pbREnt->fState                            == Busy     &&
      pbPBXMem->abRTable[usTIndex].fState       == Busy     &&
      pbPBXMem->abRTable[usTIndex].usConnection == usCIndex  ) {
    // Shut down the pipe instance
    DosDisConnectNmPipe(pbPBXMem->abRTable[usTIndex].hLine);
    DosClose(pbPBXMem->abRTable[usTIndex].hLine);

    // If auditing has been enabled, write a DeregUser audit record
    if (pbPBXMem->fAuditing) {
      memset(&bPBXAudit, '\0', sizeof(PBXAUDIT));
      bPBXAudit.fType = DeregUser;
      strcpy(bPBXAudit.pbDeregister.pszName, pbREnt->pszName);

      usRetCode = NetAuditWrite(
                          PBXAUDITEVENT,            // Record type
                          (char _far *)&bPBXAudit,  // Buffer
                          sizeof(PBXAUDIT),         // Buffer size
                          NULL, NULL);              // Required
      if (usRetCode                     &&
          usRetCode != NERR_LogOverflow  ) {
        ERRRPT("NetAuditWrite", usRetCode, Warning);
      }
    }

    // Update PBX counts
    DosSemRequest(&(pbPBXMem->semPBXMem), SEM_INDEFINITE_WAIT);
    pbPBXMem->usLinesCreated--;
    pbPBXMem->usLinesOwned--;
    DosSemClear(&(pbPBXMem->semPBXMem));
    pbPBXMem->abRTable[usTIndex].fState = Available;
  }

  // Close the pipe owned by the client
  DosDisConnectNmPipe(pbREnt->hLine);
  DosClose(pbREnt->hLine);

  // If auditing has been enabled, write a DeregUser audit record
  if (pbPBXMem->fAuditing) {
    memset(&bPBXAudit, '\0', sizeof(PBXAUDIT));
    bPBXAudit.fType = DeregUser;
    strcpy(bPBXAudit.pbDeregister.pszName, pbREnt->pszName);

    usRetCode = NetAuditWrite(
                        PBXAUDITEVENT,            // Record type
                        (char _far *)&bPBXAudit,  // Buffer
                        sizeof(PBXAUDIT),         // Buffer size
                        NULL, NULL);              // Required
    if (usRetCode                     &&
        usRetCode != NERR_LogOverflow  ) {
      ERRRPT("NetAuditWrite", usRetCode, Warning);
    }
  }

  // Update PBX counts
  DosSemRequest(&(pbPBXMem->semPBXMem), SEM_INDEFINITE_WAIT);
  pbPBXMem->usLinesCreated--;
  if (pbREnt->fState > Open)
    pbPBXMem->usLinesOwned--;
  DosSemClear(&(pbPBXMem->semPBXMem));
  pbREnt->fState = Available;

  // Inform the MakePipe thread that more pipes may be needed
  DosSemClear(&(pbPBXMem->semMakePipe));

  return;
}


/* Connect ----------------------------------------------------------
  Description:  Establish a connection between two lines
                Once a connection is made, control of both lines
                will be transferred to a Router thread
  Input      :  pbPkt   ----- Pointer to PBX Packet
                pbREnt  ----- Pointer to local routing table entry
  Output     :  TRUE if connection completes, FALSE otherwise
-------------------------------------------------------------------*/
BOOL Connect (PBXPKT        *pbPkt,
              ROUTEENT _far *pbREnt)
{
  PBXAUDIT      bPBXAudit;                // Audit record buffer
  PBXPKT        bPkt;                     // Local PBX packet buffer
  ROUTEENT _far *pbRTargetEnt;            // Target table entry
  USHORT        usCIndex;                 // Client table index
  USHORT        usTIndex;                 // Target table index
  BOOL          fRAssigned = FALSE;       // Router assigned flag
  USHORT        usRNum;                   // Router number
  CHAR          pszMsg[80];               // Error message buffer
  USHORT        usTemp;                   // Temporary variable
  USHORT        usRetCode;                // Return code

  // Prepare client ACK packet
  pbPkt->usPktID |= ACK;

  // Did the client supply too many names?
  if (pbPkt->usNameCnt > 1) {
    pbPkt->usRetCode = C_TOOMANYNAMES;
    // Read extraneous information from the pipe
    for (usTemp = pbPkt->usNameCnt - 1;
         usTemp;
         usTemp--)
      ReadPKT((PCH)pbPBXMem->pbBuf, sizeof(PBXNAME), pbREnt);
  }

  // Did client supply target name?
  else if (pbPkt->usNameCnt               < 1   ||
           *(pbPkt->aPBXName[0].pszName) == '\0' ) {
    pbPkt->usRetCode = C_NONAME;
  }

  // Does the target client name exist in the routing table?
  else if ((usTIndex=TblIndex(&(pbPkt->aPBXName[0]))) >=
                                            pbPBXMem->usLines) {
    pbPkt->usRetCode = C_INVALIDNAME;
  }

  // The client supplied valid information, check target information
  else {
    // Get pointer of target entry and determine client offset
    pbRTargetEnt = pbPBXMem->abRTable + usTIndex;
    usCIndex     = pbREnt - pbPBXMem->abRTable;

    // Is the target client not Busy?
    if (pbRTargetEnt->fState != Owned) {
      pbPkt->usRetCode = C_TARGETBUSY;
    }

    // Both clients are open for a connection,
    // complete the connection
    else {
      // Build connection packet to inform the target of
      // the connection
      bPkt.usPktID   = CONNECT;
      bPkt.usNameCnt = 1;
      strcpy(bPkt.aPBXName[0].pszName, pbREnt->pszName);
      bPkt.aPBXName[0].usClientType = pbREnt->usClientType;

      // Send packet to target and complete the connection
      if (SendPKT((PCH)&bPkt, sizeof(PBXPKT), pbRTargetEnt)) {

        // Determine which Router thread will handle the connection
        for (usRNum=0;
             usRNum < (pbPBXMem->usRThreadCnt-1);
             usRNum++) {
          if (pbPBXMem->abRouters[usRNum].cConnections <
                                        pbPBXMem->usConnsPerThread)
            break;
        }

        // Update the routing table and send client the ACK
        pbREnt->fState             = Busy;
        pbRTargetEnt->fState       = Busy;
        pbREnt->usConnection       = usTIndex;
        pbRTargetEnt->usConnection = usCIndex;
        pbPkt->usRetCode           = NO_ERROR;
        if (!SendPKT((PCH)pbPkt, sizeof(PBXPKT), pbREnt)) {
          sprintf(pszMsg, "Unable to send Connect ACK to %s",
                  pbREnt->pszName);
          ERRRPT(pszMsg, 0, Warning);
        }

        // Increment the number of connections Router is managing
        DosSemRequest(&(pbPBXMem->abRouters[usRNum].semRAccess),
                      SEM_INDEFINITE_WAIT);
        pbPBXMem->abRouters[usRNum].cConnections++;
        DosSemClear(&(pbPBXMem->abRouters[usRNum].semRAccess));

        // If auditing has been enabled, write a ConnectUsers
        // audit record
        if (pbPBXMem->fAuditing) {
          memset(&bPBXAudit, '\0', sizeof(PBXAUDIT));
          bPBXAudit.fType = ConnectUsers;
          strcpy(bPBXAudit.pbConnect.pszSource,
                 pbREnt->pszName);
          strcpy(bPBXAudit.pbConnect.pszTarget,
                 pbRTargetEnt->pszName);

          usRetCode = NetAuditWrite(
                              PBXAUDITEVENT,          // Record type
                              (char _far *)&bPBXAudit,// Buffer
                              sizeof(PBXAUDIT),       // Buffer size
                              NULL, NULL);            // Required
          if (usRetCode                     &&
              usRetCode != NERR_LogOverflow  ) {
            ERRRPT("NetAuditWrite", usRetCode, Warning);
          }
        }

        // Transfer control of both lines to the Router thread
        DosSetNmPipeSem(pbREnt->hLine,
                        pbPBXMem->abRouters[usRNum].hsemRouter,
                        usCIndex);
        DosSetNmPipeSem(pbRTargetEnt->hLine,
                        pbPBXMem->abRouters[usRNum].hsemRouter,
                        usTIndex);

        // Start the Router thread (in case additional data is in
        // the pipe)
        DosSemClear(pbPBXMem->abRouters[usRNum].hsemRouter);

        fRAssigned = TRUE;
      }

      // An error occurred while informing the target client, do not
      // complete the connection
      else {
        pbPkt->usRetCode = C_TARGETERR;
      }
    }
  }

  // If the connection was not fully completed, then send the
  // acknowledgement from here (the ACK for a completed connection
  // is sent earlier)
  if (!fRAssigned                                 &&
      !SendPKT((PCH)pbPkt, sizeof(PBXPKT), pbREnt) ) {
    sprintf(pszMsg, "Unable to send Connect ACK to %s",
            pbREnt->pszName);
    ERRRPT(pszMsg, 0, Warning);
  }

  return fRAssigned;
}


/* ListQuery --------------------------------------------------------
  Description:  Send a list of all registered clients
  Input      :  pbPkt   ----- Pointer to PBX Packet
                pbREnt  ----- Pointer to local routing table entry
  Output     :  None
-------------------------------------------------------------------*/
void ListQuery (PBXPKT        *pbPkt,
                ROUTEENT _far *pbREnt)
{
  PBXPKT  _far  *pbPBXPkt;                // PBXPKT  pointer
  PBXNAME _far  *aPBXName;                // PBXNAME array
  USHORT        usIndex;                  // Routing table index
  USHORT        usMaxCnt;                 // Max names to return
  USHORT        usNameCnt;                // Returned name count
  USHORT        usOwnedCnt;               // Count of owned lines
  CHAR          pszMsg[80];               // Error message buffer

  // Determine if the user is asking for more names than exist
  // If they are, immediately return the request with no names
  if (pbPkt->aLQNames.usFirstName > pbPBXMem->usLinesOwned) {
    pbPkt->usPktID            |= ACK;
    pbPkt->usPktSize           = sizeof(PBXPKT);
    pbPkt->usRetCode           = NO_ERROR;
    pbPkt->aLQNames.usNameCnt  = 0;
    pbPkt->aLQNames.fMoreNames = FALSE;
    if (!SendPKT((PCH)pbPkt, sizeof(PBXPKT), pbREnt)) {
      sprintf(pszMsg, "Unable to send ListQuery ACK to %s",
              pbREnt->pszName);
      ERRRPT(pszMsg, 0, Warning);
    }
  }

  // Otherwise, return as many names as possible to the client
  else {
    // First determine how many names can be returned
    // It will either be the total number of names in the table
    // or the number of names that will fit in the available buffer
    // space
    usMaxCnt = MIN(pbPBXMem->usLinesOwned,
                   ((pbREnt->usWSpace-sizeof(PBXPKT))/
                                        sizeof(PBXNAME))+1);

    // Locate the first name to return
    for (usIndex=0, usOwnedCnt=0;
         usIndex    < pbPBXMem->usLines           &&
         usOwnedCnt < pbPBXMem->usLinesOwned      &&
         usOwnedCnt < pbPkt->aLQNames.usFirstName  ;
         usIndex++)
      if (pbPBXMem->abRTable[usIndex].fState > Open) usOwnedCnt++;

    // Move the names into the PipeMgr buffer (leaving space for
    // the PBXPKT)
    aPBXName = MAKEP(SELECTOROF(pbPBXMem->pbBuf),
                     FIELDOFFSET(PBXPKT, aPBXName));
    for (usNameCnt=0;
         usIndex    < pbPBXMem->usLines      &&
         usOwnedCnt < pbPBXMem->usLinesOwned &&
         usNameCnt  < usMaxCnt                ;
         usIndex++) {
      if (pbPBXMem->abRTable[usIndex].fState > Open) {
        usOwnedCnt++;
        strcpy(aPBXName[usNameCnt].pszName,
               pbPBXMem->abRTable[usIndex].pszName);
        aPBXName[usNameCnt].usClientType =
               pbPBXMem->abRTable[usIndex].usClientType;
        usNameCnt++;
      }
    }

    // Complete the build of the packet and send it to the client
    pbPBXPkt = (PBXPKT _far *)(pbPBXMem->pbBuf);
    pbPBXPkt->usPktID             = ACK | LISTQUERY;
    pbPBXPkt->usPktSize           = sizeof(PBXPKT) +
                                    (sizeof(PBXNAME)*(usNameCnt-1));
    pbPBXPkt->usRetCode           = NO_ERROR;
    pbPBXPkt->aLQNames.usNameCnt  = usNameCnt;
    pbPBXPkt->aLQNames.fMoreNames =
                             (usOwnedCnt < pbPBXMem->usLinesOwned);
    if (!SendPKT((PCH)pbPBXMem->pbBuf,
                 pbPBXPkt->usPktSize, pbREnt)) {
      sprintf(pszMsg, "Unable to send ListQuery ACK to %s",
              pbREnt->pszName);
      ERRRPT(pszMsg, 0, Warning);
    }
  }

  return;
}


/* PipeMgr ----------------------------------------------------------
  Description:  See file header
  Input      :  None
  Output     :  None
-------------------------------------------------------------------*/
void _cdecl PipeMgr(void)
{
  PBXPKT        bPkt;                     // PBX packet buffer
  CHAR          pszMsg[80];               // Error message buffer
  ROUTEENT _far *pbREnt;                  // Routing entry pointer
  USHORT        usCIndex;                 // Client table index
  USHORT        usTemp;                   // Temporary variable
  USHORT        usRetCode;                // Return code

  // Process all incoming requests until told to exit or pause
  // The main thread will set semPause to pause and semExit to
  // indicate a shutdown should occur
  for (;;) {

    // Wait for input on one of the lines
    DosSemRequest(pbPBXMem->hsemPipeMgr, SEM_INDEFINITE_WAIT);

    // Requests to pause or exit should prevent the PipeMgr thread
    // from processing any addtional packets, so first check these
    // semaphores

    // Check for shutdown
    if (DosSemWait(&(pbPBXMem->semExit), SEM_IMMEDIATE_RETURN))
      break;

    // Check for pause
    DosSemWait(&(pbPBXMem->semPause), SEM_INDEFINITE_WAIT);

    // Read all of the PIPESEMSTATE records
    usRetCode = DosQNmPipeSemState(pbPBXMem->hsemPipeMgr,
                                   pbPBXMem->aPSemState,
                                   pbPBXMem->usPSemStateSize);
    if (usRetCode) {
      ERRRPT("DosQNmPipeSemState", usRetCode, Warning);
    }

    // Update the status of all managed pipes
    else  {
      for (usTemp=0;
           pbPBXMem->aPSemState[usTemp].fStatus != NPSS_EOI;
           usTemp++) {
        // Get the routing table index for the client from the
        // PIPESEMSTATE record and build a pointer into the
        // routing table
        usCIndex = pbPBXMem->aPSemState[usTemp].usKey;
        pbREnt   = pbPBXMem->abRTable + usCIndex;

        // Process the record
        switch (pbPBXMem->aPSemState[usTemp].fStatus) {

          // Read and immediately handle any incoming packets
          case NPSS_RDATA : {
            BOOL  fConnected = FALSE;

            // Process data in the pipe until it becomes connected
            // to another pipe (the Router thread will handle any
            // data from that point onwards)
            pbREnt->usRData = pbPBXMem->aPSemState[usTemp].usAvail;
            while (!fConnected && pbREnt->usRData) {
              ReadPKT((PCH)&bPkt, sizeof(PBXPKT), pbREnt);
              switch (bPkt.usPktID) {
                case CONNECT  : fConnected = Connect(&bPkt, pbREnt);
                                break;
                case REGISTER : Register (&bPkt, pbREnt); break;
                case LISTQUERY: ListQuery(&bPkt, pbREnt); break;
              }
            }
            break;
          }

          // Record amount of available buffer space
          case NPSS_WSPACE:
            pbREnt->usWSpace = pbPBXMem->aPSemState[usTemp].usAvail;
            break;

          // A client has broken their pipe connection, remove them
          // from the routing table
          case NPSS_CLOSE :
            Deregister(pbREnt);
            break;

          default         :
            sprintf(pszMsg,
                    "Unknown DosQNmPipeSemState record type of %u",
                    (USHORT)(pbPBXMem->aPSemState[usTemp].fStatus));
            ERRRPT(pszMsg, 0, Warning);
            break;
        }
      }
    }
  }

  // Terminate processing
  _endthread();
}
