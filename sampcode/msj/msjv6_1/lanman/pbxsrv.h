/*-------------------------------------------------------------------
  PBXSRV.H -- Global data, structures, and prototypes used by PBX
              Note, this file assumes LAN.H has been included.

  Author:  Brendan Dixon
           Microsoft, inc.
           LAN Manager Developer Support

  This code example is provided for demonstration purposes only.
  Microsoft makes no warranty, either express or implied,
  as to its usability in any given situation.
-------------------------------------------------------------------*/

#ifndef _PBXSRVH
#define _PBXSRVH

#ifdef INCL_GLOBAL
#define GLOBAL
#else
#define GLOBAL  extern
#endif

#include  "pbxpkt.h"

// Defines ----------------------------------------------------------
// LANMAN.INI section/keyword names
#define PBXCOMPONENT      "PBX"           // Section name
#define LINES             "LINES"         // Lines number
#define LINEBUFSIZE       "LINEBUFSIZE"   // Line buffer size
#define OPENLINES         "OPENLINES"     // Number of open lines
#define CONNSPERTHREAD    "CONNSPERTHREAD"// Connections/Thread ratio
#define AUDITING          "AUDITING"      // Audit enable/disable

#define MINLINES          2               // Min number of pipe lines
#define MAXLINES          1000            // Max number of pipe lines
#define DEFLINES          20              // Def number of pipe lines

#define MINLINEBUFSIZE    512             // Min line buffer size
#define MAXLINEBUFSIZE    65535           // Max line buffer size
#define DEFLINEBUFSIZE    4096            // Def line buffer size

#define MINOPENLINES      10              // Min open lines value

#define MINCONNSPERTHREAD 1               // Min connections/thread

#define MAXDOMAINS        5               // Max number of domains
#define MINFHANDLES       50              // Min file handle guess
#define MAXROUTERTHREADS  (MAXLINES/MINCONNSPERTHREAD)// Max Routers
#define MPIPESTACKSIZE    4096            // MakePipe stack size
#define PIPEMGRSTACKSIZE  4096            // PipeMgr  stack size
#define ROUTERSTACKSIZE   4096            // Router   stack size
#define LINEWAITTIMEOUT   5000L           // DosWaitNmPipe time out
#define PBXINSTALLTIME    620L            // Est. PBX install time
#define PBXUNINSTALLTIME  200L            // Est. PBX uninstall time

#define PBXSHUTDOWNMSG    "PBX on %s is stopping"   // Shutdown msg

#define PBXMEMNAME        "\\SHAREMEM\\MSJ\\PBXMEM" // Memory name
#define PIPEMGRSEM        "\\SEM\\MSJ\\PBXPIPEMGR"  // PipeMgr sem
#define ROUTERSEM         "\\SEM\\MSJ\\PBXROUTER%u" // Router  sem

#define PBXAUDITEVENT     (0x8000)        // PBX audit event type

// Basic macros
#define ERRRPT(x,y,z)     { ErrorRpt(x, __FILE__, __LINE__, y, z); }
#define MIN(x,y)          (x < y ? x : y)

// Types ------------------------------------------------------------
// Error message severity
typedef enum {
  Error = 1,                              // Critical, PBX will stop
  Warning,                                // Error, PBX will continue
  Informational                           // No error
} ERRSEV;

// Structures -------------------------------------------------------
// Routing table entry
typedef struct _ROUTEENT {
  CHAR    pszName[NAMESIZE];              // Client name
  USHORT  usClientType;                   // Client-type identifier
                                          //   This value is client
                                          //   dependent (however,
                                          //   PBX reserves zero)
  enum {                                  // Line state
          Available = 1,                  //   Line is not created
          Open,                           //   Line created
          Owned,                          //   Owned by a client
          Busy                            //   Line is connected
  }       fState;
  USHORT  usConnection;                   // Connection table index
  USHORT  usWSpace;                       // Write space available
  USHORT  usRData;                        // Amount of data to read
  HPIPE   hLine;                          // Named-Pipe handle
} ROUTEENT, _far *PROUTEENT;

// Router thread structure
typedef struct _RTHREAD {
  SHORT             sRouterID;            // Router thread ID
  USHORT            cConnections;         // Number of connections
  ULONG             semRAccess;           // Router access semaphore
  HSEM              hsemRouter;           // Router semaphore
  SEL               selLineBuf;           // Line buffer selector
  CHAR         _far *pbLineBuf;           // Line buffer pointer
  SEL               selRData;             // NPSS_RDATA selector
  PROUTEENT    _far *aRData;              // NPSS_RDATA entry array
  SEL               selClose;             // NPSS_CLOSE selector
  PROUTEENT    _far *aClose;              // NPSS_CLOSE entry array
  USHORT            usPSemStateSize;      // PIPESEMSTATE array size
  SEL               selPSemState;         // PIPESEMSTATE selector
  PIPESEMSTATE _far *aPSemState;          // PIPESEMSTATE array
} RTHREAD;

// PBX shared memory structure
typedef struct _PBXMEM {
  ULONG             semPBXMem;            // Shared memory semaphore
  ULONG             semPause;             // Pause service semaphore
  ULONG             semExit;              // Exit  service semaphore
  ULONG             semMakePipe;          // MakePipe semaphore
  HSEM              hsemPipeMgr;          // PipeMgr  semaphore
  USHORT            hMailslot;            // Mailslot handle
  CHAR              pbMSlotBuf[255];      // Mailslot buffer
  CHAR              pszPBXMsg[PBXMSGSIZE];// PBX message
  USHORT            usPBXMsgSize;         // PBX message size
  USHORT            usLines;              // Number of lines
  USHORT            usLinesCreated;       // Number of created lines
  USHORT            usLinesOwned;         // Number of owned lines
  USHORT            usLineBufSize;        // Line buffer size
  USHORT            usOpenLines;          // Open lines goal
  USHORT            usConnsPerThread;     // Conns/Router ratio
  BOOL              fAuditing;            // Audit or not flag
  USHORT            usRThreadCnt;         // Number of Router threads
  SEL               selBuf;               // PipeMgr buffer selector
  CHAR         _far *pbBuf;               // PipeMgr work buffer
  USHORT            usPSemStateSize;      // PIPESEMSTATE array size
  SEL               selPSemState;         // PIPESEMSTATE selector
  PIPESEMSTATE _far *aPSemState;          // PIPESEMSTATE array
  SEL               selRTable;            // Routing table selector
  ROUTEENT     _far *abRTable;            // Routing table array
  SEL               selRouters;           // Router struct selector
  RTHREAD      _far *abRouters;           // Router struct array
} PBXMEM;

// PBX LAN Manager audit record structure
// This structure maps the data portion of a PBX audit record
// (the portion of the record not mapped by LAN Manager)
typedef struct _PBXAUDIT {
  enum {
    Start = 1,                            // PBX starting event
    RegUser,                              // User registered event
    ConnectUsers,                         // Connection made event
    DeregUser                             // User deregistered event
  } fType;                                // Type of audit record

  // The following union contains the various structures used
  // for the different events, only one structure at a time will
  // be valid
  union {
    // Start event data
    struct {
      unsigned  usLines;                  // Number of lines
      unsigned  usLineBufSize;            // Line buffer size
      unsigned  usOpenLines;              // Open lines goal
      unsigned  usConnsPerThread;         // Conns/Router thread
    } pbStart;                            // Start data

    // Register event data
    struct {
      char  pszName[NAMESIZE];            // User name
    } pbRegister;

    // Connect event data
    struct {
      char  pszSource[NAMESIZE];          // Source user name
      char  pszTarget[NAMESIZE];          // Target user name
    } pbConnect;

    // Deregister event data
    struct {
      char  pszName[NAMESIZE];            // User name
    } pbDeregister;
  };
} PBXAUDIT;

// Globals ----------------------------------------------------------
GLOBAL SEL          selPBXMem;            // Shared memory selector
GLOBAL PBXMEM _far  *pbPBXMem;            // Shared memory pointer
GLOBAL struct service_status ssStatus;    // LAN Man. Service struct
GLOBAL ULONG        ulExitCode;             // LAN Man. Exit Code
GLOBAL CHAR         pszExitText[STXTLEN+1]; // LAN Man. Exit Text

// Function Prototypes ----------------------------------------------
// PBX threads
void _cdecl main            (int argc, char *argv[]);// Main thread
void _cdecl MakePipe        (void);               // MakePipe thread
void _cdecl PipeMgr         (void);               // PipeMgr  thread
void _cdecl Router          (USHORT usThreadNum); // Router   thread

// Initialization routines
void  AllocatePipeMgrMem  (void);              // Get PipeMgr memory
void  AllocateRouterMem   (void);              // Get Router memory
void  AllocateRoutingTable(void);              // Get Routing table
void  BuildPBXMsg         (void);              // Build PBX message
void  CreateThreads       (void);              // Create children

// Internal work routines
void  Deregister      (ROUTEENT _far *pbREnt); // Remove entry
void  ErrorRpt        (PSZ    pszMsg,          // Error function
                       PSZ    pszFile,
                       USHORT usLine,
                       USHORT usRC,
                       ERRSEV esSev);
void  ExitHandler     (void);                  // PBX Exit routine
void  InstallSignals  (void);                  // Install signals
BOOL  ReadPKT         (PCH    pbBuf,           // Read from a pipe
                       USHORT usSize,
                       ROUTEENT _far *pbREnt);
BOOL  SendPKT         (PCH    pbBuf,           // Write to a pipe
                       USHORT usSize,
                       ROUTEENT _far *pbREnt);

// PBX signal handler
void _far _pascal SignalHandler (USHORT usSigArg,
                                 USHORT usSigNum);

#endif
