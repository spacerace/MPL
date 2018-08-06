/*-------------------------------------------------------------------
  INIT.C -- PBX initialization related routines

  Description:

  These various routines are used during PBX initialization to
  allocate memory, initialize memory, and to create the child
  threads.

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
#define   INCL_NETWKSTA
#define   INCL_NETERRORS
#include  <lan.h>

#include  <process.h>
#include  <stddef.h>
#include  <stdio.h>
#include  <string.h>

#include  "pbxsrv.h"


/* AllocatePipeMgrMem -----------------------------------------------
  Description:  Allocate and initialize memory structures used
                by the PipeMgr thread
  Input      :  None
  Output     :  None, globals modified
-------------------------------------------------------------------*/
void AllocatePipeMgrMem(void)
{
  USHORT  usRetCode;                      // Return code

  // Allocate buffer for PipeMgr thread
  usRetCode = DosAllocSeg(pbPBXMem->usLineBufSize,    // Size
                          &(pbPBXMem->selBuf),        // Selector
                          SEG_GETTABLE);              // Shared
  if (usRetCode) {
    ERRRPT("Unable to allocate PipeMgr buffer, DosAllocSeg",
           usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                  SERVICE_UIC_M_MEMORY);
    ExitHandler();
  }
  pbPBXMem->pbBuf = MAKEP(pbPBXMem->selBuf, 0);

  // Allocate PIPESEMSTATE array (for the PipeMgr thread)
  // The maximum number of PIPESEMSTATE records ever returned by
  // the DosQNmPipeSemState call (see PIPEMGR.C) is three times the
  // number of pipes plus one extra for the NPSS_EOI record.
  pbPBXMem->usPSemStateSize = ((3*pbPBXMem->usLines)+1) *
                                         sizeof(PIPESEMSTATE);
  usRetCode = DosAllocSeg(pbPBXMem->usPSemStateSize,  // Size
                          &(pbPBXMem->selPSemState),  // Selector
                          SEG_GETTABLE);              // Shared
  if (usRetCode) {
    ERRRPT(
      "Unable to allocate PipeMgr PIPESEMSTATE array, DosAllocSeg",
      usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                  SERVICE_UIC_M_MEMORY);
    ExitHandler();
  }
  pbPBXMem->aPSemState = MAKEP(pbPBXMem->selPSemState, 0);

  return;
}


/* AllocateRouterMem ------------------------------------------------
  Description:  Allocate and initialize structures used by each
                Router thread
  Input      :  None
  Output     :  None, globals modified
-------------------------------------------------------------------*/
void AllocateRouterMem(void)
{
  USHORT  usPSemStateSize;                // PIPESEMSTATE array size
  USHORT  usRNum;                         // Structure number
  CHAR    pszSem[255];                    // Semaphore name
  USHORT  usRetCode;                      // Return code

  // Allocate Router thread structures
  usRetCode = DosAllocSeg(
                 pbPBXMem->usRThreadCnt * sizeof(RTHREAD), // Size
                 &(pbPBXMem->selRouters),              // Selector
                 SEG_GETTABLE);                        // Shared
  if (usRetCode) {
    ERRRPT("Unable to allocate Router structures, DosAllocSeg",
           usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                  SERVICE_UIC_M_MEMORY);
    ExitHandler();
  }
  pbPBXMem->abRouters = MAKEP(pbPBXMem->selRouters, 0);

  // Determine size of PIPESEMSTATE array for each thread
  // Again, allocate space for three records per each pipe (there
  // two pipes per connection) plus one extra for the NPSS_CLOSE
  // record
  usPSemStateSize = ((3*(pbPBXMem->usConnsPerThread*2))+1) *
                                           sizeof(PIPESEMSTATE);

  for (usRNum=0; usRNum < pbPBXMem->usRThreadCnt; usRNum++) {
    pbPBXMem->abRouters[usRNum].sRouterID    = 0;
    pbPBXMem->abRouters[usRNum].cConnections = 0;
    pbPBXMem->abRouters[usRNum].semRAccess   = 0;

    // Create the pipe semaphore for the thread
    sprintf(pszSem, ROUTERSEM, usRNum);
    usRetCode = DosCreateSem(
                   CSEM_PUBLIC,
                   &(pbPBXMem->abRouters[usRNum].hsemRouter),
                   pszSem);
    if (usRetCode) {
      ERRRPT("DosCreateSem", usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM,
                                    usRetCode);
      ExitHandler();
    }

    // Set the Router semaphore to capture the first pipe events
    DosSemSet(pbPBXMem->abRouters[usRNum].hsemRouter);

    // Allocate line buffer
    usRetCode = DosAllocSeg(
             pbPBXMem->usLineBufSize,                  // Size
             &(pbPBXMem->abRouters[usRNum].selLineBuf),// Selector
             SEG_GETTABLE);                            // Shared
    if (usRetCode) {
      ERRRPT("Unable to allocate Router line buffer, DosAllocSeg",
             usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                    SERVICE_UIC_M_MEMORY);
      ExitHandler();
    }
    pbPBXMem->abRouters[usRNum].pbLineBuf =
                   MAKEP(pbPBXMem->abRouters[usRNum].selLineBuf, 0);

    // Allocate NPSS_RDATA array
    usRetCode = DosAllocSeg(
           2*pbPBXMem->usConnsPerThread*sizeof(ROUTEENT),// Size
           &(pbPBXMem->abRouters[usRNum].selRData),      // Selector
           SEG_GETTABLE);                                // Shared
    if (usRetCode) {
      ERRRPT(
        "Unable to allocate Router NPSS_RDATA array, DosAllocSeg",
        usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                    SERVICE_UIC_M_MEMORY);
      ExitHandler();
    }
    pbPBXMem->abRouters[usRNum].aRData =
                     MAKEP(pbPBXMem->abRouters[usRNum].selRData, 0);

    // Allocate NPSS_CLOSE array
    usRetCode = DosAllocSeg(
           2*pbPBXMem->usConnsPerThread*sizeof(ROUTEENT),// Size
           &(pbPBXMem->abRouters[usRNum].selClose),      // Selector
           SEG_GETTABLE);                                // Shared
    if (usRetCode) {
      ERRRPT(
        "Unable to allocate Router NPSS_CLOSE array, DosAllocSeg",
        usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                    SERVICE_UIC_M_MEMORY);
      ExitHandler();
    }
    pbPBXMem->abRouters[usRNum].aClose =
                     MAKEP(pbPBXMem->abRouters[usRNum].selClose, 0);

    // Allocate PIPESEMSTATE array
    usRetCode = DosAllocSeg(
           usPSemStateSize,                            // Size
           &(pbPBXMem->abRouters[usRNum].selPSemState),// Selector
           SEG_GETTABLE);                              // Shared
    if (usRetCode) {
      ERRRPT(
       "Unable to allocate Router PIPESEMSTATE array, DosAllocSeg",
       usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                    SERVICE_UIC_M_MEMORY);
      ExitHandler();
    }
    pbPBXMem->abRouters[usRNum].aPSemState =
                MAKEP(pbPBXMem->abRouters[usRNum].selPSemState, 0);
    pbPBXMem->abRouters[usRNum].usPSemStateSize = usPSemStateSize;
  }

  return;
}


/* AllocateRoutingTable ---------------------------------------------
  Description:  Build and initialize the PBX routing table
  Input      :  None
  Output     :  None, globals modified
-------------------------------------------------------------------*/
void AllocateRoutingTable(void)
{
  USHORT  usLineNum;                      // Current line number
  USHORT  usRetCode;                      // Return code

  // Allocate Routing Table
  usRetCode = DosAllocSeg(
                  pbPBXMem->usLines * sizeof(ROUTEENT), // Size
                  &(pbPBXMem->selRTable),               // Selector
                  SEG_GETTABLE);                        // Shared
  if (usRetCode) {
    ERRRPT("Unable to allocate routing table, DosAllocSeg",
           usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                  SERVICE_UIC_M_MEMORY);
    ExitHandler();
  }
  pbPBXMem->abRTable = MAKEP(pbPBXMem->selRTable, 0);

  // Initialize Routing Table
  for (usLineNum = 0;
       usLineNum < pbPBXMem->usLines;
       usLineNum++) {

    // Initialize information fields
    pbPBXMem->abRTable[usLineNum].pszName[0]      = '\0';
    pbPBXMem->abRTable[usLineNum].usClientType    = 0;
    pbPBXMem->abRTable[usLineNum].fState          = Available;
    pbPBXMem->abRTable[usLineNum].usConnection    = usLineNum;
    pbPBXMem->abRTable[usLineNum].usWSpace =
                                      pbPBXMem->usLineBufSize;
    pbPBXMem->abRTable[usLineNum].usRData  = 0;
    pbPBXMem->abRTable[usLineNum].hLine    = 0;
  }

  return;
}


/* BuildPBXMsg ------------------------------------------------------
  Description:  Build an announcement message consisting of the
                computer name on which PBX is executing prepended
                with two back-slashes (as in a UNC name)
  Input      :  None
  Output     :  None, globals modified
-------------------------------------------------------------------*/
void BuildPBXMsg(void)
{
  USHORT                    usBufSize;       // Buffer size in bytes
  SEL                       selBuf;          // Buffer selector
  struct wksta_info_10 _far *pbWkstaInfo10;  // Wksta structure
  USHORT                    usRetCode;       // Return code

  // Make one call to get the size of buffer required
  usRetCode = NetWkstaGetInfo(NULL,     // Local
                              10,       // Basic info
                              NULL,     // Get buffer size
                              0,        // No buffer
                              &usBufSize); // Return size needed
  if (usRetCode && usRetCode != NERR_BufTooSmall) {
    ERRRPT("NetWkstaGetInfo", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Allocate the needed buffer
  usRetCode     = DosAllocSeg(usBufSize, &selBuf, SEG_NONSHARED);
  if (usRetCode) {
    ERRRPT("DosAllocSeg", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                  SERVICE_UIC_M_MEMORY);
    ExitHandler();
  }
  pbWkstaInfo10 = MAKEP(selBuf, 0);

  // Get the workstation information
  usRetCode = NetWkstaGetInfo(
                      NULL,                       // Local
                      10,                         // Basic info
                      (CHAR _far *)pbWkstaInfo10, // Buffer addr
                      usBufSize,                  // Buffer size
                      &usBufSize);                // Bytes avail
  if (usRetCode) {
    ERRRPT("NetWkstaGetInfo", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Build announcement message using the current computer name
  else {
    strcpy(pbPBXMem->pszPBXMsg, "\\\\");
    strcat(pbPBXMem->pszPBXMsg,
           pbWkstaInfo10->wki10_computername);
    pbPBXMem->usPBXMsgSize = strlen(pbPBXMem->pszPBXMsg) + 1;
  }

  // Free allocated memory
  if (usRetCode = DosFreeSeg(selBuf))
    ERRRPT("DosFreeSeg", usRetCode, Warning);

  return;
}


/* CreateThreads ----------------------------------------------------
  Description:  Create the MakePipe, PipeMgr, and Router threads
  Input      :  None
  Output     :  None
-------------------------------------------------------------------*/
void CreateThreads(void)
{
  CHAR    pszMsg[80];                     // Error message buffer
  USHORT  usRetCode;                      // Return code

  // Because pipes are created by the MakePipe thread, transferred
  // to the PipeMgr thread, and finally to a Router thread; Router
  // threads are created first, then the PipeMgr thread, and finally
  // the MakePipe thread.  This ensures that all threads will be
  // fully ready to handle pipes as they become available.

  // Create Router threads
  { USHORT       usRNum;                  // Current Router thread
    SHORT        sThreadID;               // Thread ID

    for (usRNum=0; usRNum < pbPBXMem->usRThreadCnt; usRNum++) {
      // Create the thread
      sThreadID = _beginthread(
                      (void (_cdecl _far *)(void _far *))Router,
                      NULL,
                      ROUTERSTACKSIZE,
                      (void *)usRNum);
      if (sThreadID < 0) {
        sprintf(pszMsg, "Unable to start Router thread %u", usRNum);
        ERRRPT(pszMsg, 0, Error);
        ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                      SERVICE_UIC_M_THREADS);
        ExitHandler();
      }

      // Save the thread number
      pbPBXMem->abRouters[usRNum].sRouterID = sThreadID;
    }
  }

  // Create the PipeMgr thread
  { SHORT sPipeMgrID;

    // Create the pipe semaphore
    usRetCode = DosCreateSem(CSEM_PUBLIC,
                             &(pbPBXMem->hsemPipeMgr),
                             PIPEMGRSEM);
    if (usRetCode) {
      ERRRPT("DosCreateSem", usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
      ExitHandler();
    }

    // Set the semaphore to capture the first pipe events
    DosSemSet(pbPBXMem->hsemPipeMgr);

    // Create the thread
    sPipeMgrID = _beginthread(
                       (void (_cdecl _far *)(void _far *))PipeMgr,
                       NULL,
                       PIPEMGRSTACKSIZE,
                       NULL);
    if (sPipeMgrID < 0) {
      ERRRPT("Unable to start PipeMgr thread", 0, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                    SERVICE_UIC_M_THREADS);
      ExitHandler();
    }
  }

  // Create the MakePipe thread
  { SHORT sMakePipeID;

    sMakePipeID = _beginthread(
                        (void (_cdecl _far *)(void _far *))MakePipe,
                        NULL,
                        MPIPESTACKSIZE,
                        NULL);
    if (sMakePipeID < 0) {
      ERRRPT("Unable to start MakePipe thread", 0, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                    SERVICE_UIC_M_THREADS);
      ExitHandler();
    }
  }

  return;
}
