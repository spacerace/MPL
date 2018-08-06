/*-------------------------------------------------------------------
  PBXSRV.C -- PBX Packet Routing Service, main thread routine

  Description:

  PBX is general purpose, named-pipe packet router designed to
  support virtual, named-pipe, peer-to-peer connections.  These
  connections are virtual because all communications are through
  the PBX Service.  However, they appear as a peer-to-peer named-
  pipe connection to the clients (once a connection is established).
  PBX performs this using a quick, protocol-unaware method and may,
  therefore, be used by several clients each of which supports its
  own internal protocol.

  PBX employs several threads to create and manage the named-pipes:

    PBXSRV.C ----  This is the main PBX thread.  It creates the
                   child threads, processes incoming signals, and
                   responds to broadcasts looking for the PBX
                   service.

    MAKEPIPE.C --  This thread dynamically creates instances of the
                   named-pipe and attempts to ensure that the
                   minimum number of unowned pipes are always
                   available.

    PIPEMGR.C ---  This thread manages pipe traffic until it becomes
                   connected with another client.  For example,
                   client registration and connection requests are
                   processed by this thread.

    ROUTER.C ----  This thread manages pipes that are connected, its
                   sole purpose is to transfer packets between
                   connected clients.  The number of connections a
                   Router thread will manage (and thus the total
                   number of Router threads created) may be
                   controlled by the CONNSPERTHREAD keyword (see
                   below).

  PBX may configured via keywords supplied either in LANMAN.INI or
  explicitly when PBX is started.  In LANMAN.INI PBX looks for these
  keywords under the section named [PBX].  Listed below are the
  keywords, their meanings, and valid ranges:

    LINES         :  Total number of pipe lines (pipe instances) to
                     be created and managed.  The minimum value is
                     2 and the maximum is 1000, it defaults to 20.

    LINEBUFSIZE   :  Size (in bytes) of the buffers associated with
                     each pipe line (instance).  A buffer of this
                     size is created for each pipe instance, for
                     each Router thread, and for the PipeMgr thread.
                     The minimum value is 512 and the maximum is
                     65,535, it defaults to 4096.

    OPENLINES     :  The minimum number of non-connected pipe
                     instances that PBX will try to maintain.  Since
                     there are situations where the minimum cannot
                     be maintained (e.g., all lines are allocated
                     and used), this number is only a goal.  The
                     minimum value is 10 or the total number of
                     lines (whichever is smaller).  The maximum is
                     the total number of lines.  It defaults to the
                     minimum value.

    CONNSPERTHREAD:  Number of connections a single Router thread
                     should manage, a connection always involves two
                     pipes. Lower values should be used if heavy
                     traffic is expected from the clients.  The
                     minimum value is 1 and the maximum value is the
                     total number of lines created, rounded up to
                     the next even number, divided by two.  The
                     default is for a single Router thread to manage
                     all connections.

    AUDITING      :  Determines whether PBX writes audit records or
                     not.  Valid values are Yes or No.  The default
                     is Yes.
                     The data portion of the audit records written
                     by PBX is mapped by the PBXAUDIT structure in
                     PBXSRV.H.

  It is possible to specify combinations that cannot work on the
  local machine.  For example, requesting 1000 pipes (LINES=1000)
  each with a 64k buffer (LINEBUFSIZE=65535) would require over
  62 MB.  PBX does not check for these invalid combinations,
  instead other errors will be received (e.g., DosAllocSeg will
  fail).

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
#define   INCL_NETMAILSLOT
#define   INCL_NETSERVICE
#define   INCL_NETERRORS
#include  <lan.h>

#include  <ctype.h>
#include  <memory.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#define   INCL_GLOBAL
#include  "pbxsrv.h"

// Constants --------------------------------------------------------
const CHAR  pszLines[]          = LINES;
const CHAR  pszLineBufSize[]    = LINEBUFSIZE;
const CHAR  pszOpenLines[]      = OPENLINES;
const CHAR  pszConnsPerThread[] = CONNSPERTHREAD;
const CHAR  pszAuditing[]       = AUDITING;
const CHAR  pszYes[]            = "YES";
const CHAR  pszNo[]             = "NO";

// Function Prototypes (for functions not in PBXSRV.H) --------------
void  RedirectFiles (void);
void  SetParms      (int argc, char _far *argv[]);
void  Initialize    (int argc, char _far *argv[]);


/* RedirectFiles ----------------------------------------------------
  Description:  Redirect STDIN, STDOUT, and STDERR to the NUL device
  Input      :  None
  Output     :  None
-------------------------------------------------------------------*/
void RedirectFiles(void)
{
  HFILE   hFileNul;                       // Handle to NUL device
  HFILE   hNewHandle;                     // Temporary file handle
  USHORT  usAction;                       // Action taken by DosOpen
  USHORT  usRetCode;                      // Return code

  // First, open the NUL device
  usRetCode = DosOpen("NUL",                   // NUL device name
                      &hFileNul,               // File handle
                      &usAction,               // Action taken
                      0L,                      // File size
                      FILE_NORMAL,             // File attribute
                      FILE_CREATE,             // Open action
                      OPEN_ACCESS_READWRITE |
                      OPEN_SHARE_DENYNONE,     // Open mode
                      0L);
  if (usRetCode) {
    ERRRPT("DosOpen", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Then, redirect STDIN, STDOUT, and STDERR
  for (hNewHandle=0; hNewHandle < 3; hNewHandle++) {
    // If the handle is already directed to NUL, skip it
    if (hFileNul != hNewHandle) DosDupHandle(hFileNul, &hNewHandle);
  }

  // Lastly, close the extra handle to NUL
  if (hFileNul > 2) DosClose(hFileNul);

  return;
}


/* SetParms ---------------------------------------------------------
  Description:  Process PBX configuration parameters
                These will come from either the command line or
                predefined defaults.  Any keywords not entered by
                the user, but found in LANMAN.INI will be added to
                the command line by LAN Manager and passed to PBX.
                Therefore, the LANMAN.INI file does not need to be
                read.
  Input      :  argc --- Number of command line parameters passed
                         to PBX
                argv --- Array of command line parameters
  Output     :  None (globals are set)
-------------------------------------------------------------------*/
void SetParms(int       argc,
              char _far *argv[])
{
  BOOL      fLines,                       // LINES          supplied
            fLineBufSize,                 // LINEBUFSIZE    supplied
            fOpenLines,                   // OPENLINES      supplied
            fConnsPerThread,              // CONNSPERTHREAD supplied
            fAuditing;                    // AUDITING       supplied
  int       sArg;                         // Argument number
  CHAR _far *pszArg;                      // Argument pointer
  CHAR      pszKeyword[20];               // Keyword value
  CHAR      pszAudit[5];                  // Audit arg value
  CHAR _far *pszTemp;                     // Temporary pointer
  USHORT    usTemp;                       // Temporary variable
  CHAR      pszMsg[80];                   // Error message buffer

  // Assume no keywords have been supplied
  fLines          =
  fLineBufSize    =
  fOpenLines      =
  fConnsPerThread =
  fAuditing       = FALSE;

  // Scan the command lines arguments PBX keywords, unknown
  // keywords will be treated as an error
  // Note that LAN Manager will not pass LANMAN.INI keywords if the
  // user entered the full keyword name on the command line as an
  // override value.  But if the full keyword was not specified,
  // then the LANMAN.INI keyword will also be passed (but after the
  // values entered by the user).  So once a keyword is found, any
  // subsequent occurances should be ignored (i.e., duplicate
  // keywords will not be detected).
  for (sArg=1; sArg < argc; sArg++) {
    if ((pszArg=strpbrk(argv[sArg], "=")) != NULL) {
      // Point to the keyword value and mark its end
      pszArg++;
      if ((pszTemp=strpbrk(pszArg, " ")) != NULL) *pszTemp = '\0';

      // Fold line to upper case and extract the keyword
      strupr(argv[sArg]);
      usTemp = MIN(sizeof(pszKeyword)-1, pszArg-(argv[sArg]+1));
      strncpy(pszKeyword, argv[sArg], usTemp);
      pszKeyword[usTemp] = '\0';

      // Check for the LINES keyword
      if (strstr(pszLines, pszKeyword) == pszLines) {
        // All characters in LINES are required since anything
        // less is ambiguous with LINEBUFSIZE
        if (strlen(pszKeyword) < strlen(pszLines)) {
          strncpy(pszExitText, pszKeyword, sizeof(pszExitText)-1);
          pszExitText[sizeof(pszExitText)-1] = '\0';
          sprintf(pszMsg, "Ambiguous keyword %s", pszExitText);
          ERRRPT(pszMsg, 0, Error);
          ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_AMBIGPARM,
                                        SERVICE_UIC_M_NULL);
          ExitHandler();
        }
        else {
          fLines = TRUE;
          pbPBXMem->usLines = atoi(pszArg);
        }
      }

      // Check for LINEBUFSIZE
      else if (strstr(pszLineBufSize, pszKeyword) ==
                                                 pszLineBufSize) {
        fLineBufSize = TRUE;
        pbPBXMem->usLineBufSize = atoi(pszArg);
      }

      // Check for OPENLINES
      else if (strstr(pszOpenLines, pszKeyword) == pszOpenLines) {
        fOpenLines = TRUE;
        pbPBXMem->usOpenLines = atoi(pszArg);
      }

      // Check for CONNSPERTHREAD
      else if (strstr(pszConnsPerThread, pszKeyword) ==
                                              pszConnsPerThread) {
        fConnsPerThread = TRUE;
        pbPBXMem->usConnsPerThread = atoi(pszArg);
      }

      // Check for AUDITING
      else if (strstr(pszAuditing, pszKeyword) == pszAuditing) {
        fAuditing = TRUE;
        // Copy the argument to a buffer
        // One extra byte beyond the largest valid value is copied
        // so invalid values can be detected
        strncpy(pszAudit, pszArg, sizeof(pszAudit)-1);
        pszAudit[sizeof(pszAudit)-1] = '\0';
      }

      // The keyword is not valid for PBX, report the error
      else {
        strncpy(pszExitText, pszKeyword, sizeof(pszExitText)-1);
        pszExitText[sizeof(pszExitText)-1] = '\0';
        sprintf(pszMsg, "Unknown keyword %s", pszExitText);
        ERRRPT(pszMsg, 0, Error);
        ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_UNKPARM,
                                      SERVICE_UIC_M_NULL);
        ExitHandler();
      }
    }
  }

  // Validity check the number lines PBX should support
  if (fLines) {
    if (pbPBXMem->usLines > MAXLINES ||
        pbPBXMem->usLines < MINLINES   ) {
      sprintf(pszMsg, "Invalid value for %s", pszLines);
      ERRRPT(pszMsg, 0, Error);
      strcpy(pszExitText, pszLines);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_BADPARMVAL,
                                    SERVICE_UIC_M_NULL);
      ExitHandler();
    }
  }
  else
    // Take default value
    pbPBXMem->usLines = DEFLINES;

  // Validity check the buffer size PBX should use for each line
  if (fLineBufSize) {
    if (pbPBXMem->usLineBufSize > MAXLINEBUFSIZE ||
        pbPBXMem->usLineBufSize < MINLINEBUFSIZE  ) {
      sprintf(pszMsg, "Invalid value for %s", pszLineBufSize);
      ERRRPT(pszMsg, 0, Error);
      strcpy(pszExitText, pszLineBufSize);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_BADPARMVAL,
                                    SERVICE_UIC_M_NULL);
      ExitHandler();
    }
  }
  else
    // Take default value
    pbPBXMem->usLineBufSize = DEFLINEBUFSIZE;

  // Validity check the minimum number of available lines PBX
  // should try and maintain open
  if (fOpenLines) {
    if (pbPBXMem->usOpenLines > pbPBXMem->usLines            ||
        pbPBXMem->usOpenLines < (USHORT)MIN(MINOPENLINES,
                                           pbPBXMem->usLines) ) {
      sprintf(pszMsg, "Invalid value for %s", pszOpenLines);
      ERRRPT(pszMsg, 0, Error);
      strcpy(pszExitText, pszOpenLines);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_BADPARMVAL,
                                  SERVICE_UIC_M_NULL);
      ExitHandler();
    }
  }
  else
    // Take the minimum number, but do not set it higher then
    // the total number of lines available
    pbPBXMem->usOpenLines = (USHORT)MIN(MINOPENLINES,
                                        pbPBXMem->usLines);

  // Validity check the number of connections a Router thread
  // should manage
  if (fConnsPerThread) {
    if (pbPBXMem->usConnsPerThread > ((pbPBXMem->usLines+1)/2) ||
        pbPBXMem->usConnsPerThread < MINCONNSPERTHREAD          ) {
      sprintf(pszMsg, "Invalid value for %s", pszConnsPerThread);
      ERRRPT(pszMsg, 0, Error);
      strcpy(pszExitText, pszConnsPerThread);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_BADPARMVAL,
                                    SERVICE_UIC_M_NULL);
      ExitHandler();
    }
  }
  else
    // All connections will be managed by a single Router thread
    pbPBXMem->usConnsPerThread = (pbPBXMem->usLines+1) / 2;

  // Validity check the value supplied for the audit flag
  if (fAuditing) {
    if (strstr(pszYes, pszAudit) == pszYes)
      pbPBXMem->fAuditing = TRUE;
    else if (strstr(pszNo, pszAudit) == pszNo)
      pbPBXMem->fAuditing = FALSE;
    else {
      sprintf(pszMsg, "Invalid value for %s", pszAuditing);
      ERRRPT(pszMsg, 0, Error);
      strcpy(pszExitText, pszAuditing);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_BADPARMVAL,
                                    SERVICE_UIC_M_NULL);
      ExitHandler();
    }
  }
  else
    // Take default value
    pbPBXMem->fAuditing = TRUE;

  return;
}


/* Initialize -------------------------------------------------------
  Description:  Prepare PBX for execution by allocating all global
                storage and creating the child threads
  Input      :  argc ---- Count of input parameters passed to PBX
                argv ---- Input parameters passed to PBX
  Output     :  None
-------------------------------------------------------------------*/
void Initialize(int       argc,
                char _far *argv[])
{
  CHAR    pszMsg[80];                     // Error message buffer
  USHORT  usWCnt;                         // Uninstall wait counter
  USHORT  usRetCode;                      // Return code

  // Inform LAN Manager that this service is being installed
  usWCnt = 2;
  ssStatus.svcs_status = SERVICE_INSTALL_PENDING;
  ssStatus.svcs_code   = SERVICE_CCP_CODE(PBXINSTALLTIME, usWCnt++);
  usRetCode = NetServiceStatus((char _far *)&ssStatus,
                               sizeof(ssStatus));
  if (usRetCode) {
    ERRRPT("NetServiceStatus", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Allocate shared memory segment
  selPBXMem = 0;
  pbPBXMem  = NULL;
  usRetCode = DosAllocShrSeg(sizeof(PBXMEM),  // Segment size
                             PBXMEMNAME,      // Segment name
                             &selPBXMem);     // Selector
  if (usRetCode) {
    sprintf(pszMsg, "Unable to allocate %s, DosAllocShrSeg",
            PBXMEMNAME);
    ERRRPT(pszMsg, usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_RESOURCE,
                                  SERVICE_UIC_M_MEMORY);
    ExitHandler();
  }

  // Make shared memory segment pointer and initialize the segment
  pbPBXMem = MAKEP(selPBXMem, 0);
  memset(pbPBXMem, '\0', sizeof(PBXMEM));

  // Process configuration parameters
  SetParms(argc, argv);

  // Provide LAN Manager with a status hint
  ssStatus.svcs_status = SERVICE_INSTALL_PENDING;
  ssStatus.svcs_code   = SERVICE_CCP_CODE(PBXINSTALLTIME,
                                          usWCnt++);
  usRetCode = NetServiceStatus((char _far *)&ssStatus,
                               sizeof(ssStatus));
  if (usRetCode) {
    ERRRPT("NetServiceStatus", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Ensure that enough pipe handles exist
  // Note, MINFHANDLES is a guess at how many handles are already in
  // use.  Alternatively, a loop could be performed to determine the
  // actual number of files handles in use.
  usRetCode = DosSetMaxFH((pbPBXMem->usLines) + MINFHANDLES);
  if (usRetCode) {
    ERRRPT("DosSetMaxFH", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Allocate and initialize PipeMgr thread memory
  AllocatePipeMgrMem();

  // Build Routing table
  AllocateRoutingTable();

  // Set number of Router threads required to handle requested lines
  pbPBXMem->usRThreadCnt = (pbPBXMem->usLines+1) /
                                   (2 * pbPBXMem->usConnsPerThread);

  // Allocate and initialize Router thread structures
  AllocateRouterMem();

  // Build the PBX message (used in broadcast messages)
  BuildPBXMsg();

  // Create child threads
  CreateThreads();

  return;
}


/* main -------------------------------------------------------------
  Description:  See file header
  Input      :  None (directly)
  Output     :  None
-------------------------------------------------------------------*/
void _cdecl main(int argc, char *argv[])
{
  PIDINFO   pidInfo;                      // OS/2 PID info struct
  PBXAUDIT  bPBXAudit;                    // PBX audit record
  USHORT    usByteCnt;                    // Mailslot bytes read
  USHORT    usNextCnt;                    // Mailslot next msg size
  USHORT    usNextPriority;               // Mailslot next priority
  USHORT    usRetCode;                    // Return Code

  // Initialize global variables
  selPBXMem  = 0;
  pbPBXMem   = NULL;
  ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_NORMAL,
                                SERVICE_UIC_M_NULL);

  // Obtain the process ID of this process
  usRetCode = DosGetPID(&pidInfo);
  if (usRetCode) {
    ERRRPT("DosGetPID", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Inform LAN Manager that this service is in installation
  // Because the signal handler is not yet installed, the service
  // cannot be uninstalled or paused
  ssStatus.svcs_pid    = pidInfo.pid;
  ssStatus.svcs_status = SERVICE_INSTALL_PENDING   |
                         SERVICE_NOT_UNINSTALLABLE |
                         SERVICE_NOT_PAUSABLE;
  ssStatus.svcs_code   = SERVICE_CCP_CODE(PBXINSTALLTIME, 0);
  usRetCode = NetServiceStatus((char _far *)&ssStatus,
                               sizeof(ssStatus));
  if (usRetCode != NERR_Success) {
    ERRRPT("NetServiceStatus", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Install the signal handler
  InstallSignals();

  // Indicate that the signal handler is now installed, inform LAN
  // Manager that the service may be uninstalled
  ssStatus.svcs_status = SERVICE_INSTALL_PENDING |
                         SERVICE_UNINSTALLABLE   |
                         SERVICE_NOT_PAUSABLE;
  ssStatus.svcs_code   = SERVICE_CCP_CODE(PBXINSTALLTIME, 1);
  usRetCode = NetServiceStatus((char _far *)&ssStatus,
                               sizeof(ssStatus));
  if (usRetCode) {
    ERRRPT("NetServiceStatus", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Verify that the LAN Manager Server service is started
  { struct service_info_1 si1;            // Level 1 info structure
    USHORT  usByteCnt;                    // Byte count

    // Obtain Server service status from LAN Manager
    usRetCode = NetServiceGetInfo(
                          NULL,             // Local
                          SERVICE_SERVER,   // Server service
                          1,                // Level 1 info
                          (char _far *)&si1,// Buffer
                          sizeof(si1),      // Buffer size
                          &usByteCnt);      // Bytes available
    if (usRetCode                            &&
        usRetCode != NERR_ServiceNotInstalled ) {
      ERRRPT("NetServiceStatus", usRetCode, Error);
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
      ExitHandler();
    }

    // Is the Server running?  If not, exit with an error
    if (usRetCode                 == NERR_ServiceNotInstalled ||
        (si1.svci1_status & 0x03) != SERVICE_INSTALLED         ) {
      ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_CONFIG,
                                    SERVICE_UIC_M_SERVER);
      ExitHandler();
    }
  }

  // Redirect STDIN, STDOUT, and STDERR to the NUL device (since no
  // console I/O is allowed from a LAN Manager service)
  RedirectFiles();

  // Perform PBX specific initialization
  Initialize(argc, argv);

  // Open the mailslot that clients can use to detect the presence
  // of a PBX
  usRetCode = DosMakeMailslot(ANNOUNCELINE,    // Mailslot name
                              PBXMSGSIZE,      // Max message size
                              0,               // Default size
                              &(pbPBXMem->hMailslot)); // Handle
  if (usRetCode) {
    ERRRPT("NetServiceStatus", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // If auditing has been enabled, write a start-up audit record
  if (pbPBXMem->fAuditing) {
    memset(&bPBXAudit, '\0', sizeof(PBXAUDIT));
    bPBXAudit.fType = Start;
    bPBXAudit.pbStart.usLines          = pbPBXMem->usLines;
    bPBXAudit.pbStart.usLineBufSize    = pbPBXMem->usLineBufSize;
    bPBXAudit.pbStart.usOpenLines      = pbPBXMem->usOpenLines;
    bPBXAudit.pbStart.usConnsPerThread = pbPBXMem->usConnsPerThread;

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

  // Inform LAN Manager that this service is now installed
  ssStatus.svcs_status = SERVICE_INSTALLED     |
                         SERVICE_UNINSTALLABLE |
                         SERVICE_PAUSABLE;
  ssStatus.svcs_code   = SERVICE_UIC_NORMAL;
  usRetCode = NetServiceStatus((char _far *)&ssStatus,
                               sizeof(ssStatus));
  if (usRetCode) {
    ERRRPT("NetServiceStatus", usRetCode, Error);
    ulExitCode = SERVICE_UIC_CODE(SERVICE_UIC_SYSTEM, usRetCode);
    ExitHandler();
  }

  // Run until PBX is shutdown
  do {
    // Wait until a signal is received or a message arrives on the
    // mailslot (which means a client is looking for the PBX)
    usRetCode = DosReadMailslot(
                       pbPBXMem->hMailslot,   // Handle
                       pbPBXMem->pbMSlotBuf,  // Buffer
                       &usByteCnt,            // Bytes read
                       &usNextCnt,            // Next msg size
                       &usNextPriority,       // Next msg priority
                       MAILSLOT_NO_TIMEOUT);  // Wait forever

    // If control has been returned because a client sent a message,
    // return to the client the computer name of the machine on
    // which PBX is executing
    if (usRetCode == NERR_Success &&
        usByteCnt != 0             ) {
      strcat(pbPBXMem->pbMSlotBuf, ANNOUNCELINE);
      usRetCode = DosWriteMailslot(
                          pbPBXMem->pbMSlotBuf,    // Mailslot
                          pbPBXMem->pszPBXMsg,     // Message
                          pbPBXMem->usPBXMsgSize,  // Message size
                          9,                       // Priority
                          2,                       // 2nd class
                          0L);                     // No wait
    }

  } while (usRetCode == ERROR_INTERRUPT   ||
           usRetCode == ERROR_SEM_TIMEOUT  );

  // Control never arrives here, the ExitHandler always exits PBX
  return;
}
