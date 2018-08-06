/*-------------------------------------------------------------------
  ROUTER.C -- PBX Packet transfer routine

  Description:

  This thread manages pipes that are connected.  After receiving a
  CONNECT request, the PipeMgr thread transfers control of the
  connected pipes to one of the Router threads (via
  DosSetNmPipeSem).  This thread rapidly moves data between the two
  pipes with no regard for the contents of the data.  A connection
  is broken by one of the clients closing their end of the pipe.

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

#define   INCL_NETSERVICE
#include  <lan.h>

#include  <stdio.h>
#include  <string.h>

#include  "pbxsrv.h"


/* Router -----------------------------------------------------------
  Description:  See file header
  Input      :  usThreadNum -- Router thread instance number
  Output     :  None
-------------------------------------------------------------------*/
void _cdecl Router(USHORT usThreadNum)
{
  ROUTEENT _far     *pbREnt;              // Client table entry
  ROUTEENT _far     *pbRTargetEnt;        // Target table entry
  RTHREAD  _far     *pbRThread;           // Router struct pointer
  USHORT            usNPSSRData;          // NPSS_RDATAs found
  USHORT            usNPSSClose;          // NPSS_CLOSEs found
  USHORT            usCnt;                // Loop counter
  USHORT            usByteCnt;            // Bytes to transfer
  USHORT            usRetCode;            // Return code

  // Establish a pointer to this threads memory structure
  pbRThread = pbPBXMem->abRouters + usThreadNum;

  // Loop forever processing requests
  for (;;) {

    // Wait for input on one of the lines
    DosSemRequest(pbRThread->hsemRouter, SEM_INDEFINITE_WAIT);

    // Read all of the PIPESEMSTATE records
    usRetCode = DosQNmPipeSemState(pbRThread->hsemRouter,
                                   pbRThread->aPSemState,
                                   pbRThread->usPSemStateSize);
    if (usRetCode) {
      ERRRPT("DosQNmPipeSemState", usRetCode, Warning);
    }

    // First, loop thru the PIPESEMSTATE records setting the amount
    // of write space available for each pipe and counting the
    // number of NPSS_RDATA and NPSS_CLOSE records encountered
    // Next, for each NPSS_RDATA and NPSS_CLOSE record found during
    // the first loop, transfer as much data as possible and close
    // those pipes which have been closed by their clients
    else  {
      // First, set amount of write space each pipe has and count
      // the number of NPSS_RDATA and NPSS_CLOSE records encountered
      // (save pointers into the routing table for each record
      // encountered)
      for (usCnt = usNPSSRData = usNPSSClose = 0;
           pbRThread->aPSemState[usCnt].fStatus != NPSS_EOI;
           usCnt++) {
        // Build a pointer into the routing table
        pbREnt = pbPBXMem->abRTable +
                          pbRThread->aPSemState[usCnt].usKey;

        // And process the record
        switch (pbRThread->aPSemState[usCnt].fStatus) {

          // Update the amount of data available to be read
          case NPSS_RDATA :
            pbREnt->usRData = pbRThread->aPSemState[usCnt].usAvail;
            if (pbREnt->usRData) {
              pbRThread->aRData[usNPSSRData] = pbREnt;
              usNPSSRData++;
            }
            break;

          // Update the amount of write space available
          case NPSS_WSPACE:
            pbREnt->usWSpace = pbRThread->aPSemState[usCnt].usAvail;
            break;

          // A client has broken their pipe connection
          case NPSS_CLOSE :
            pbRThread->aClose[usNPSSClose] = pbREnt;
            usNPSSClose++;
            break;

          default         : {
            CHAR  pszMsg[80];             // Error message buffer

            sprintf(pszMsg,
                    "Unknown DosQNmPipeSemState record type of %u",
                    (USHORT)(pbRThread->aPSemState[usCnt].fStatus));
            ERRRPT(pszMsg, 0, Warning);
            break;
          }
        }
      }

      // Transfer all data that can be transferred
      for (usCnt=0; usCnt < usNPSSRData; usCnt++) {
        pbREnt       = pbRThread->aRData[usCnt];
        pbRTargetEnt = pbPBXMem->abRTable + pbREnt->usConnection;

        // Determine the maximum amount of data that can be moved
        usByteCnt = MIN(pbREnt->usRData, pbRTargetEnt->usWSpace);

        // And transfer the data
        if (usByteCnt                                        &&
            ReadPKT(pbRThread->pbLineBuf, usByteCnt, pbREnt)  )
          SendPKT(pbRThread->pbLineBuf, usByteCnt, pbRTargetEnt);
      }

      // Close pipes for those clients which have closed their end
      for (usCnt=0; usCnt < usNPSSClose; usCnt++) {
        Deregister(pbRThread->aClose[usCnt]);

        // Decrement the number of connections managed by this pipe
        DosSemRequest(&(pbRThread->semRAccess), SEM_INDEFINITE_WAIT);
        pbRThread->cConnections--;
        DosSemClear(&(pbRThread->semRAccess));
      }
    }
  }

  // Control never arrives at this point, the thread is simply
  // terminated when PBX shuts down
  return;
}
