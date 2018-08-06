/*-------------------------------------------------------------------
  SIGNALS.C -- PBX signal related routines

  Description:

  This file contains the routines used by PBX to install an OS/2
  signal handler and to process incoming signals.

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

#define   INCL_NETHANDLE
#define   INCL_NETMESSAGE
#define   INCL_NETSERVICE
#define   INCL_NETERRORS
#include  <lan.h>

typedef struct handle_info_2 HDLI2;   // NetHandleGetInfo structure

#include  <stdio.h>
#include  <string.h>

#include  "pbxsrv.h"


/* ExitHandler ------------------------------------------------------
  Description:  Perform clean-up processing and shut down the PBX
  Input      :  None, Exit codes and text are taken from global
                variables
  Ouput      :  Control is returned to OS/2
-------------------------------------------------------------------*/
void ExitHandler(void)
{
  CHAR    pszBuf[sizeof(HDLI2)+UNCLEN+1]; // Buffer
  HDLI2   *phdli2 = (HDLI2 *)pszBuf;      // NetHandleGetInfo pointer
  USHORT  usByteCnt;                      // Bytes returned
  USHORT  usCIndex;                       // Routing table index
  CHAR    pszMsg[255];                    // Shutdown message buffer
  USHORT  usWCnt;                         // Uninstall wait counter
  USHORT  usRetCode;                      // Return code

  // Inform LAN Manager that this service is being uninstalled
  usWCnt = 0;
  ssStatus.svcs_status = SERVICE_UNINSTALL_PENDING;
  ssStatus.svcs_code   = SERVICE_CCP_CODE(PBXUNINSTALLTIME,
                                          usWCnt++);
  NetServiceStatus((char _far *)&ssStatus, sizeof(ssStatus));

  // Inform registered clients that PBX is shutting down
  if (pbPBXMem           != NULL &&       // PBX memory obtained
      pbPBXMem->abRTable != NULL  ) {     // Routing table obtained
    // Build shut down message
    sprintf(pszMsg, PBXSHUTDOWNMSG, pbPBXMem->pszPBXMsg);

    for (usCIndex=0; usCIndex < pbPBXMem->usLines; usCIndex++) {
      if (pbPBXMem->abRTable[usCIndex].fState > Open) {
        // Get client computer name
        usRetCode =
            NetHandleGetInfo(
                  pbPBXMem->abRTable[usCIndex].hLine, // Pipe handle
                  2,                                  // Info level
                  pszBuf,                             // Buffer
                  sizeof(pszBuf),                     // Size
                  &usByteCnt);                     // Bytes returned

        // If computer name was returned, send client a message
        if (usRetCode == NERR_Success) {
          NetMessageBufferSend(NULL,                   // Local
                               phdli2->hdli2_username, // Target
                               pszMsg,                 // Message
                               strlen(pszMsg)+1);      // Size
        }

        // Provide LAN Manager with a status hint
        ssStatus.svcs_status = SERVICE_UNINSTALL_PENDING;
        ssStatus.svcs_code   = SERVICE_CCP_CODE(PBXINSTALLTIME,
                                                usWCnt++);
        NetServiceStatus((char _far *)&ssStatus, sizeof(ssStatus));
      }
    }
  }

  // Inform LAN Manager that this service has been uninstalled
  ssStatus.svcs_status = SERVICE_UNINSTALLED;
  ssStatus.svcs_code   = ulExitCode;
  strcpy(ssStatus.svcs_text, pszExitText);
  NetServiceStatus((char _far *)&ssStatus, sizeof(ssStatus));

  // Return control to OS/2
  DosExit(EXIT_PROCESS, 0);

  return;
}


/* SignalHandler ----------------------------------------------------
  Description:  Process system signals
                Signals should come (primarily) from two sources:
                LAN Manager and the PBX Service itself (during error
                handling)
  Input      :  usSigArg --- Signal argument
                usSigNum --- Signal number
                             (should only be SERVICE_RCV_SIG_FLAG)
  Output     :  None
-------------------------------------------------------------------*/
void _far _pascal SignalHandler(USHORT usSigArg, USHORT usSigNum)
{
  UCHAR        fSigArg;                   // Signal argument
  USHORT       usRetCode;                 // Return code

  // Extract signal argument from the first byte
  fSigArg = (UCHAR)(usSigArg & 0x00FF);

  // And take the appropriate action
  switch (fSigArg) {
    // Uninstall PBXSRV.EXE
    case SERVICE_CTRL_UNINSTALL:
      DosSemSet(&(pbPBXMem->semExit));
      ExitHandler();
      break;

    // Pause all processing
    case SERVICE_CTRL_PAUSE:
      DosSemSet(&(pbPBXMem->semPause));
      ssStatus.svcs_status = SERVICE_INSTALLED     |
                             SERVICE_PAUSED        |
                             SERVICE_UNINSTALLABLE |
                             SERVICE_PAUSABLE;
      ssStatus.svcs_code   = SERVICE_UIC_NORMAL;
      usRetCode = NetServiceStatus((char _far *)&ssStatus,
                                   sizeof(ssStatus));
      if (usRetCode) {
        ERRRPT("NetServiceStatus", usRetCode, Error);
        ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM,
                                      usRetCode);
        ExitHandler();
      }
      break;

    // Continue (after a pause) processing
    case SERVICE_CTRL_CONTINUE:
      DosSemClear(&(pbPBXMem->semPause));
      ssStatus.svcs_status = SERVICE_INSTALLED     |
                             SERVICE_ACTIVE        |
                             SERVICE_UNINSTALLABLE |
                             SERVICE_PAUSABLE;
      ssStatus.svcs_code   = SERVICE_UIC_NORMAL;
      usRetCode = NetServiceStatus((char _far *)&ssStatus,
                                   sizeof(ssStatus));
      if (usRetCode) {
        ERRRPT("NetServiceStatus", usRetCode, Error);
        ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM,
                                      usRetCode);
        ExitHandler();
      }
      break;

    // Return service information
    case SERVICE_CTRL_INTERROGATE:
    default:
      usRetCode = NetServiceStatus((char _far *)&ssStatus,
                                   sizeof(ssStatus));
      if (usRetCode) {
        ERRRPT("NetServiceStatus", usRetCode, Error);
        ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM,
                                      usRetCode);
        ExitHandler();
      }
      break;
  }

  // Reset the signal handler to accept the next signal
  usRetCode = DosSetSigHandler(0,
                               0,
                               0,
                               SIGA_ACKNOWLEDGE,
                               usSigNum);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM,
                                  usRetCode);
    ExitHandler();
  }

  return;
}


/* InstallSignals ---------------------------------------------------
  Description:  Install signal handler for PBX
  Input      :  None
  Output     :  None
-------------------------------------------------------------------*/
void InstallSignals(void)
{
  // Values associated with the previous signal handler are not
  // needed after this routine and therefore may be held in local
  // variables
  PFNSIGHANDLER fnSigHandler;             // Previous signal handler
  USHORT        fSPrevAction;             // Previous signal action
  USHORT        usRetCode;                // Return Code

  // The handler will be called only for SERVICE_RCV_SIG_FLAG
  // signals (which are the same as SIG_PFLG_A).  This implies that
  // LAN Manager will cover all CTRL+C, BREAK, and KILL signals
  usRetCode = DosSetSigHandler(SignalHandler,
                               &fnSigHandler,
                               &fSPrevAction,
                               SIGA_ACCEPT,
                               SERVICE_RCV_SIG_FLAG);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Tell OS/2 that CTRL+C, BREAK, and KILL signals will not be
  // processed by PBXSRV.EXEs signal handler
  usRetCode = DosSetSigHandler(NULL,      // Ignore CTRL+C
                               &fnSigHandler,
                               &fSPrevAction,
                               SIGA_IGNORE,
                               SIG_CTRLC);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }
  usRetCode = DosSetSigHandler(NULL,      // Ignore BREAK
                               &fnSigHandler,
                               &fSPrevAction,
                               SIGA_IGNORE,
                               SIG_CTRLBREAK);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }
  usRetCode = DosSetSigHandler(NULL,      // Ignore KILL
                               &fnSigHandler,
                               &fSPrevAction,
                               SIGA_IGNORE,
                               SIG_KILLPROCESS);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // However, a SIG_PFLG_B and SIG_PFLG_C signal should cause errors
  // (since they shouldn't occur)
  usRetCode = DosSetSigHandler(NULL,      // SIG_PFLG_B is an error
                               &fnSigHandler,
                               &fSPrevAction,
                               SIGA_ERROR,
                               SIG_PFLG_B);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }
  usRetCode = DosSetSigHandler(NULL,      // SIG_PFLG_C is an error
                               &fnSigHandler,
                               &fSPrevAction,
                               SIGA_ERROR,
                               SIG_PFLG_C);
  if (usRetCode) {
    ERRRPT("DosSetSigHandler", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  return;
}
