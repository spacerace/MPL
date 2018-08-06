/***************************************************************************\
*
* Module Name: OS2DOS.H
*
* OS/2 Base Include File
*
* Copyright (c) 1987  Microsoft Corporation
* Copyright (c) 1987  IBM Corporation
*
*****************************************************************************
*
* Subcomponents marked with "+" are partially included by default
*
*   #define:                To include:
*
* + INCL_DOSPROCESS         Process and thread support
*   INCL_DOSINFOSEG         InfoSeg support
* + INCL_DOSFILEMGR         File Management
* + INCL_DOSMEMMGR          Memory Management
* + INCL_DOSSEMAPHORES      Semaphore support
* + INCL_DOSDATETIME        Date/Time and Timer support
*   INCL_DOSMODULEMGR       Module manager
* + INCL_DOSRESOURCES       Resource support
*   INCL_DOSNLS             National Language Support
*   INCL_DOSSIGNALS         Signals
*   INCL_DOSMONITORS        Monitors
*   INCL_DOSQUEUES          Queues
*   INCL_DOSSESMGR          Session Manager Support
*   INCL_DOSDEVICES         Device specific, ring 2 support
*   INCL_DOSPROFILE         DosProfile API
*
\***************************************************************************/

#define INCL_DOSINCLUDED

#ifdef INCL_DOS

#define INCL_DOSPROCESS
#define INCL_DOSINFOSEG
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSDATETIME
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSNLS
#define INCL_DOSSIGNALS
#define INCL_DOSMONITORS
#define INCL_DOSQUEUES
#define INCL_DOSSESMGR
#define INCL_DOSDEVICES
#define INCL_DOSPROFILE

#endif /* INCL_DOS */

#ifdef INCL_ERRORS
#define INCL_DOSERRORS
#endif /* INCL_ERRORS */

#ifdef INCL_DOSPROCESS

/*** General services */

USHORT APIENTRY DosBeep(USHORT, USHORT);

/*** Process and Thread support */

VOID APIENTRY DosExit(USHORT, USHORT);

/* DosExit codes */

#define EXIT_THREAD         0
#define EXIT_PROCESS        1

#endif /* common INCL_DOSPROCESS stuff */

#ifdef INCL_DOSPROCESS

typedef struct _PIDINFO {         /* pidi */
    PID pid;
    TID tid;
    PID pidParent;
} PIDINFO;
typedef PIDINFO FAR *PPIDINFO;

typedef VOID (PASCAL FAR *PFNTHREAD)(/*VOID*/);

USHORT APIENTRY DosCreateThread(PFNTHREAD, PTID, PBYTE);
USHORT APIENTRY DosResumeThread(TID);
USHORT APIENTRY DosSuspendThread(TID);

/* Action code values */

#define DCWA_PROCESS        0
#define DCWA_PROCESSTREE    1

/* Wait option values */

#define DCWW_WAIT   0
#define DCWW_NOWAIT 1

typedef struct _RESULTCODES {     /* resc */
    USHORT codeTerminate;
    USHORT codeResult;
} RESULTCODES;
typedef RESULTCODES FAR *PRESULTCODES;

USHORT APIENTRY DosCwait(USHORT, USHORT, PRESULTCODES, PPID, PID);
USHORT APIENTRY DosSleep(ULONG);

/* codeTerminate values (also passed to ExitList routines) */

#define TC_EXIT          0
#define TC_HARDERROR     1
#define TC_TRAP          2
#define TC_KILLPROCESS   3

typedef VOID (PASCAL FAR *PFNEXITLIST)(/*USHORT*/);

VOID   APIENTRY DosEnterCritSec(VOID);
VOID   APIENTRY DosExitCritSec(VOID);
USHORT APIENTRY DosExitList(USHORT, PFNEXITLIST);

/* DosExitList functions */

#define EXLST_ADD       1
#define EXLST_REMOVE    2
#define EXLST_EXIT      3

USHORT APIENTRY DosExecPgm(PCHAR, SHORT, USHORT, PSZ, PSZ, PRESULTCODES, PSZ);

/* DosExecPgm functions */

#define EXEC_SYNC           0
#define EXEC_ASYNC          1
#define EXEC_ASYNCRESULT    2
#define EXEC_TRACE          3
#define EXEC_BACKGROUND     4
#define EXEC_LOAD           5

USHORT APIENTRY DosGetPid(PPIDINFO);
USHORT APIENTRY DosGetPPid(USHORT, PUSHORT);

USHORT APIENTRY DosGetPrty(USHORT, PUSHORT, USHORT);
USHORT APIENTRY DosSetPrty(USHORT, USHORT, SHORT, USHORT);

/* Priority scopes */

#define PRTYS_PROCESS       0
#define PRTYS_PROCESSTREE   1
#define PRTYS_THREAD        2

/* Priority classes */

#define PRTYC_NOCHANGE      0
#define PRTYC_IDLETIME      1
#define PRTYC_REGULAR       2
#define PRTYC_TIMECRITICAL  3

USHORT APIENTRY DosKillProcess(USHORT, PID);

#define DKP_PROCESS         0
#define DKP_PROCESSTREE     1

#endif /* INCL_DOSPROCESS */


/*** InfoSeg support */

#ifdef INCL_DOSINFOSEG

/* Global Info Seg */

typedef struct _GINFOSEG {      /* gis */
    ULONG   time;
    ULONG   msecs;
    UCHAR   hour;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    USHORT  timezone;
    USHORT  cusecTimerInterval;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    UCHAR   weekday;
    UCHAR   uchMajorVersion;
    UCHAR   uchMinorVersion;
    UCHAR   chRevisionLetter;
    UCHAR   sgCurrent;
    UCHAR   sgMax;
    UCHAR   cHugeShift;
    UCHAR   fProtectModeOnly;
    USHORT  pidForeground;
    UCHAR   fDynamicSched;
    UCHAR   csecMaxWait;
    USHORT  cmsecMinSlice;
    USHORT  cmsecMaxSlice;
    USHORT  bootdrive;
    UCHAR   amecRAS[32];
    UCHAR   csgWindowableVioMax;
    UCHAR   csgPMMax;
} GINFOSEG;
typedef GINFOSEG FAR *PGINFOSEG;

/* Local Info Seg */

typedef struct _LINFOSEG {      /* lis */
    PID     pidCurrent;
    PID     pidParent;
    USHORT  prtyCurrent;
    TID     tidCurrent;
    USHORT  sgCurrent;
    UCHAR   rfProcStatus;
    UCHAR   dummy1;
    BOOL    fForeground;
    UCHAR   typeProcess;
    UCHAR   dummy2;
    SEL     selEnvironment;
    USHORT  offCmdLine;
    USHORT  cbDataSegment;
    USHORT  cbStack;
    USHORT  cbHeap;
    HMODULE hmod;
    SEL     selDS;
} LINFOSEG;
typedef LINFOSEG FAR *PLINFOSEG;

/* Process Type codes (local info seg typeProcess field) */

#define PT_FULLSCREEN       0       /* Full screen app. */
#define PT_REALMODE         1       /* Real mode process */
#define PT_WINDOWABLEVIO    2       /* VIO windowable app. */
#define PT_PM               3       /* Presentation Manager app. */
#define PT_DETACHED         4       /* Detached app. */

/* Process Status Flag definitions (local info seg rfProcStatus field) */

#define PS_EXITLIST         1       /* Thread is in exitlist routine */


USHORT APIENTRY DosGetInfoSeg(PSEL, PSEL);

/* Helper macros used to convert selector to PINFOSEG or LINFOSEG */

#define MAKEPGINFOSEG(sel)  ((PGINFOSEG)MAKEP(sel, 0))
#define MAKEPLINFOSEG(sel)  ((PLINFOSEG)MAKEP(sel, 0))

#endif /* INCL_DOSINFOSEG */

#ifdef INCL_DOSFILEMGR

/*** File manager */

typedef SHANDLE HFILE;     /* hf */
typedef HFILE FAR *PHFILE;

USHORT APIENTRY DosOpen(PSZ, PHFILE, PUSHORT, ULONG, USHORT, USHORT, USHORT, ULONG);
USHORT APIENTRY DosClose(HFILE);
USHORT APIENTRY DosRead(HFILE, PVOID, USHORT, PUSHORT);
USHORT APIENTRY DosWrite(HFILE, PVOID, USHORT, PUSHORT);

/* File time and date types */

typedef struct _FILESTATUS {    /* fsts */
    FDATE  fdateCreation;
    FTIME  ftimeCreation;
    FDATE  fdateLastAccess;
    FTIME  ftimeLastAccess;
    FDATE  fdateLastWrite;
    FTIME  ftimeLastWrite;
    ULONG  cbFile;
    ULONG  cbFileAlloc;
    USHORT attrFile;
} FILESTATUS;
typedef FILESTATUS FAR *PFILESTATUS;

typedef struct _FSALLOCATE {    /* fsalloc */
    ULONG  idFileSystem;
    ULONG  cSectorUnit;
    ULONG  cUnit;
    ULONG  cUnitAvail;
    USHORT cbSector;
} FSALLOCATE;
typedef FSALLOCATE FAR *PFSALLOCATE;

typedef SHANDLE HDIR;        /* hdir */
typedef HDIR FAR *PHDIR;

USHORT APIENTRY DosDelete(PSZ, ULONG);
USHORT APIENTRY DosDupHandle(HFILE, PHFILE);

USHORT APIENTRY DosQFHandState(HFILE, PUSHORT);
USHORT APIENTRY DosSetFHandState(HFILE, USHORT);
USHORT APIENTRY DosQHandType(HFILE, PUSHORT, PUSHORT);

USHORT APIENTRY DosReadAsync (HFILE, PULONG, PUSHORT, PVOID, USHORT, PUSHORT);
USHORT APIENTRY DosWriteAsync(HFILE, PULONG, PUSHORT, PVOID, USHORT, PUSHORT);

USHORT APIENTRY DosFindFirst(PSZ, PHDIR, USHORT, PFILEFINDBUF, USHORT, PUSHORT, ULONG);
USHORT APIENTRY DosFindNext(HDIR, PFILEFINDBUF, USHORT, PUSHORT);
USHORT APIENTRY DosFindClose(HDIR);

USHORT APIENTRY DosNewSize(HFILE, ULONG);
USHORT APIENTRY DosBufReset(HFILE);

USHORT APIENTRY DosChgFilePtr(HFILE, LONG, USHORT, PULONG);

USHORT APIENTRY DosFileLocks(HFILE, PLONG, PLONG);

USHORT APIENTRY DosMove(PSZ, PSZ, ULONG);
USHORT APIENTRY DosMkDir(PSZ, ULONG);
USHORT APIENTRY DosRmDir(PSZ, ULONG);
USHORT APIENTRY DosSelectDisk(USHORT);
USHORT APIENTRY DosQCurDisk(PUSHORT, PULONG);

USHORT APIENTRY DosChDir(PSZ, ULONG);
USHORT APIENTRY DosQCurDir(USHORT, PBYTE, PUSHORT);

USHORT APIENTRY DosQFSInfo(USHORT, USHORT, PBYTE, USHORT);
USHORT APIENTRY DosSetFSInfo(USHORT, USHORT, PBYTE, USHORT);
USHORT APIENTRY DosQVerify(PUSHORT);
USHORT APIENTRY DosSetVerify(USHORT);
USHORT APIENTRY DosSetMaxFH(USHORT);

USHORT APIENTRY DosQFileInfo(HFILE, USHORT, PBYTE, USHORT);
USHORT APIENTRY DosSetFileInfo(HFILE, USHORT, PBYTE, USHORT);

USHORT APIENTRY DosQFileMode(PSZ, PUSHORT, ULONG);
USHORT APIENTRY DosSetFileMode(PSZ, USHORT, ULONG);

/*
 *
 *  Consistent names for file functions
 *
 *  DosQueryFileState       DosQFHandState
 *  DosSetFileState         DosSetFHandState
 *  DosQueryHandleType      DosQHandType
 *  DosResetBuffer          DosBufReset
 *  DosSetFilePtr           DosChgFilePtr
 *  DosMoveFile             DosMove
 *  DosMakeDirectory        DosMkdir
 *  DosRemoveDirectory      DosRmdir
 *  DosSetCurrentDisk       DosSelectDisk
 *  DosQueryCurrentDisk     DosQCurDisk
 *  DosSetCurrentDirectory  DosChdir
 *  DosQueryCurrentDirectory DosQCurDir
 *  DosQueryFileSystemInfo  DosQFSInfo
 *  DosSetFileSystemInfo    DosSetFSInfo
 *  DosQueryWriteVerify     DosQVerify
 *  DosQueryFileMode        DosQFileMode
 *  DosSetWriteVerify       DosSetVerify
 *  DosSetMaxFileHandles    DosSetMaxFH
 *  DosQueryFileInfo        DosQFileInfo
 *
 */

#endif /* common INCL_DOSFILEMAN */

#ifdef INCL_DOSMEMMGR
/*** Memory management */

USHORT APIENTRY DosAllocSeg(USHORT, PSEL, USHORT);
USHORT APIENTRY DosReallocSeg(USHORT, SEL);
USHORT APIENTRY DosFreeSeg(SEL);
USHORT APIENTRY DosGiveSeg(SEL, PID, PSEL);
USHORT APIENTRY DosGetSeg(SEL);
USHORT APIENTRY DosSizeSeg(SEL, PULONG);

/* Segment attribute flags (used with DosAllocSeg) */

#define SEG_GIVEABLE        0x0001
#define SEG_GETTABLE        0x0002
#define SEG_DISCARDABLE     0x0004

#endif /* common INCL_DOSMEMMGR */

#ifdef INCL_DOSMEMMGR

USHORT APIENTRY DosAllocHuge(USHORT, USHORT, PSEL, USHORT, USHORT);
USHORT APIENTRY DosReallocHuge(USHORT, USHORT, SEL);
USHORT APIENTRY DosGetHugeShift(PUSHORT);

USHORT APIENTRY DosAllocShrSeg(USHORT, PSZ, PSEL);

USHORT APIENTRY DosLockSeg(SEL);
USHORT APIENTRY DosUnlockSeg(SEL);

USHORT APIENTRY DosGetShrSeg(PSZ, PSEL);

USHORT APIENTRY DosMemAvail(PULONG);
USHORT APIENTRY DosCreateCSAlias(SEL, PSEL);

USHORT APIENTRY DosSubAlloc(SEL, PUSHORT, USHORT);
USHORT APIENTRY DosSubFree(SEL, USHORT, USHORT);
USHORT APIENTRY DosSubSet(SEL, USHORT, USHORT);

#endif /* INCL_DOSMEMMGR */

#ifdef INCL_DOSSEMAPHORES

/*** Semaphore support */

USHORT APIENTRY DosSemClear(HSEM);
USHORT APIENTRY DosSemSet(HSEM);
USHORT APIENTRY DosSemWait(HSEM, LONG);
USHORT APIENTRY DosSemSetWait(HSEM, LONG);
USHORT APIENTRY DosSemRequest(HSEM, LONG);

#endif /* common INCL_DOSSEMAPHORES */

#ifdef INCL_DOSSEMAPHORES

typedef LHANDLE HSYSSEM;          /* hssm */
typedef HSYSSEM FAR *PHSYSSEM;

USHORT APIENTRY DosCreateSem(USHORT, PHSYSSEM, PSZ);

#define CSEM_PRIVATE    0
#define CSEM_PUBLIC     1

USHORT APIENTRY DosOpenSem(PHSEM, PSZ);
USHORT APIENTRY DosCloseSem(HSEM);

typedef struct _MUXSEM {        /* mxs */
    USHORT zero;
    HSEM   hsem;
} MUXSEM;
typedef MUXSEM FAR *PMUXSEM;

typedef struct _MUXSEMLIST {    /* mxsl */
    USHORT  cmxs;
    MUXSEM  amxs[16];
} MUXSEMLIST;
typedef MUXSEMLIST FAR *PMUXSEMLIST;

/*
 * Since a MUXSEMLIST structure is actually a variable length
 * structure, the following macro may be used to define a MUXSEMLIST
 * structure having size elements, named "name".
 */
#define DEFINEMUXSEMLIST(name, size) \
    struct {                         \
        USHORT cmxs;                 \
        MUXSEM amxs[size];           \
    } name;

/*
 * This function actually takes a far pointer to a MUXSEMLIST structure
 * as its second parameter, but in order to allow its use with the
 * DEFINEMUXSEMLIST macro, it is declared here as PVOID.
 */
USHORT APIENTRY DosMuxSemWait(PUSHORT, PVOID, LONG);


/*** Fast safe ram semaphores */

typedef struct _DOSFSRSEM {        /* dosfsrs */
    USHORT cb;
    PID    pid;
    TID    tid;
    USHORT cUsage;
    USHORT client;
    ULONG  sem;
} DOSFSRSEM;
typedef DOSFSRSEM FAR *PDOSFSRSEM;

USHORT APIENTRY DosFSRamSemRequest(PDOSFSRSEM, LONG);
USHORT APIENTRY DosFSRamSemClear(PDOSFSRSEM);

#endif /* INCL_DOSSEMAPHORES */

#ifdef INCL_DOSDATETIME

/*** Time support */

typedef struct _DATETIME {    /* date */
    UCHAR   hours;
    UCHAR   minutes;
    UCHAR   seconds;
    UCHAR   hundredths;
    UCHAR   day;
    UCHAR   month;
    USHORT  year;
    SHORT   timezone;
    UCHAR   weekday;
} DATETIME;
typedef DATETIME FAR *PDATETIME;

USHORT APIENTRY DosGetDateTime(PDATETIME);
USHORT APIENTRY DosSetDateTime(PDATETIME);

#endif /* common INCL_DOSDATETIME */

#ifdef INCL_DOSDATETIME

typedef SHANDLE HTIMER;
typedef HTIMER FAR *PHTIMER;

USHORT APIENTRY DosGetTimerInt(PUSHORT);

USHORT APIENTRY DosTimerAsync(ULONG, HSEM, PHTIMER);
USHORT APIENTRY DosTimerStart(ULONG, HSEM, PHTIMER);
USHORT APIENTRY DosTimerStop(HTIMER);

#endif /* INCL_DOSDATETIME */


/*** Module manager */

#ifdef INCL_DOSMODULEMGR

USHORT APIENTRY DosLoadModule(PSZ, USHORT, PSZ, PHMODULE);
USHORT APIENTRY DosFreeModule(HMODULE);
USHORT APIENTRY DosGetProcAddr(HMODULE, PSZ, PFN FAR *);
USHORT APIENTRY DosGetModHandle(PSZ, PHMODULE);
USHORT APIENTRY DosGetModName(HMODULE, USHORT, PCHAR);

#endif /* INCL_DOSMODULEMGR */

#ifdef INCL_DOSRESOURCES

/*** Resource support */

/* Predefined resource types */

#define RT_POINTER      1   /* mouse pointer shape */
#define RT_BITMAP       2   /* bitmap */
#define RT_MENU         3   /* menu template */
#define RT_DIALOG       4   /* dialog template */
#define RT_STRING       5   /* string tables */
#define RT_FONTDIR      6   /* font directory */
#define RT_FONT         7   /* font */
#define RT_ACCELTABLE   8   /* accelerator tables */
#define RT_RCDATA       9   /* binary data */
#define RT_MESSAGE      10  /* error msg     tables */
#define RT_DLGINCLUDE   11  /* dialog include file name */
#define RT_VKEYTBL      12  /* key to vkey tables */
#define RT_KEYTBL       13  /* key to UGL tables */
#define RT_CHARTBL      14  /* glyph to character tables */

#endif /* common INCL_DOSRESOURCES */

#ifdef INCL_DOSRESOURCES

USHORT APIENTRY DosGetResource(HMODULE, USHORT, USHORT, PSEL);

#endif /* INCL_DOSRESOURCES */


/*** NLS Support */

#ifdef INCL_DOSNLS

typedef struct _COUNTRYCODE { /* ctryc */
    USHORT country;
    USHORT codepage;
} COUNTRYCODE;
typedef COUNTRYCODE FAR *PCOUNTRYCODE;

typedef struct _COUNTRYINFO { /* ctryi */
    USHORT country;
    USHORT codepage;
    USHORT fsDateFmt;
    CHAR   szCurrency[5];
    CHAR   szThousandsSeparator[2];
    CHAR   szDecimal[2];
    CHAR   szDateSeparator[2];
    CHAR   szTimeSeparator[2];
    UCHAR  fsCurrencyFmt;
    UCHAR  cDecimalPlace;
    UCHAR  fsTimeFmt;
    USHORT abReserved1[2];
    CHAR   szDataSeparator[2];
    USHORT abReserved2[5];
} COUNTRYINFO;
typedef COUNTRYINFO FAR *PCOUNTRYINFO;

USHORT APIENTRY DosGetCtryInfo(USHORT, PCOUNTRYCODE, PCOUNTRYINFO, PUSHORT);
USHORT APIENTRY DosGetDBCSEv(USHORT, PCOUNTRYCODE, PCHAR);
USHORT APIENTRY DosCaseMap(USHORT, PCOUNTRYCODE, PCHAR);
USHORT APIENTRY DosGetCollate(USHORT, PCOUNTRYCODE, PCHAR, PUSHORT);
USHORT APIENTRY DosGetCp(USHORT, PUSHORT, PUSHORT);
USHORT APIENTRY DosSetCp(USHORT, USHORT);

#endif /* INCL_DOSNLS */


/*** Signal support */

#ifdef INCL_DOSSIGNALS

/* Signal Numbers for DosSetSigHandler  */

#define SIG_CTRLC           1       /* Control C            */
#define SIG_BROKENPIPE      2       /* Broken Pipe          */
#define SIG_KILLPROCESS     3       /* Program Termination  */
#define SIG_CTRLBREAK       4       /* Control Break        */
#define SIG_PFLG_A          5       /* Process Flag A       */
#define SIG_PFLG_B          6       /* Process Flag B       */
#define SIG_PFLG_C          7       /* Process Flag C       */
#define SIG_CSIGNALS        8       /* number of signals plus one */

/* Flag Numbers for DosFlagProcess      */

#define PFLG_A              0       /* Process Flag A       */
#define PFLG_B              1       /* Process Flag B       */
#define PFLG_C              2       /* Process Flag C       */

/* Signal actions */

#define SIGA_KILL           0
#define SIGA_IGNORE         1
#define SIGA_ACCEPT         2
#define SIGA_ERROR          3
#define SIGA_ACKNOWLEDGE    4

/* DosHoldSignal constants */

#define HLDSIG_ENABLE       0
#define HLDSIG_DISABLE      1

/* DosFlagProcess codes */

#define FLGP_SUBTREE        0
#define FLGP_PID            1

typedef VOID (FAR PASCAL *PFNSIGHANDLER)(USHORT, USHORT);

USHORT APIENTRY DosSetSigHandler(PFNSIGHANDLER, PFNSIGHANDLER FAR *, PUSHORT, USHORT, USHORT);
USHORT APIENTRY DosFlagProcess(PID, USHORT, USHORT, USHORT);
USHORT APIENTRY DosHoldSignal(USHORT);
USHORT APIENTRY DosSendSignal(USHORT, USHORT);

#endif /* INCL_DOSSIGNALS */


/*** Monitor support */

#ifdef INCL_DOSMONITORS

typedef SHANDLE HMONITOR;    /* hmon */
typedef HMONITOR FAR *PHMONITOR;

USHORT APIENTRY DosMonOpen(PSZ, PHMONITOR);
USHORT APIENTRY DosMonClose(HMONITOR);
USHORT APIENTRY DosMonReg(HMONITOR, PBYTE, PBYTE, USHORT, USHORT);
USHORT APIENTRY DosMonRead(PBYTE, UCHAR, PBYTE, PUSHORT);
USHORT APIENTRY DosMonWrite(PBYTE, PBYTE, USHORT);

#endif /* INCL_DOSMONITORS */


/*** Pipe and queue support */

#ifdef INCL_DOSQUEUES

typedef SHANDLE HQUEUE;  /* hq */
typedef HQUEUE FAR *PHQUEUE;

USHORT APIENTRY DosMakePipe(PHFILE, PHFILE, USHORT);
USHORT APIENTRY DosCloseQueue(HQUEUE);
USHORT APIENTRY DosCreateQueue(PHQUEUE, USHORT, PSZ);
USHORT APIENTRY DosOpenQueue(PUSHORT, PHQUEUE, PSZ);
USHORT APIENTRY DosPeekQueue(HQUEUE, PULONG, PUSHORT, PULONG, PUSHORT, UCHAR, PBYTE, ULONG);
USHORT APIENTRY DosPurgeQueue(HQUEUE);
USHORT APIENTRY DosQueryQueue(HQUEUE, PUSHORT);
USHORT APIENTRY DosReadQueue(HQUEUE, PULONG, PUSHORT, PULONG, USHORT, UCHAR, PBYTE, HSEM);
USHORT APIENTRY DosWriteQueue(HQUEUE, USHORT, USHORT, PBYTE, UCHAR);

USHORT APIENTRY DosError(USHORT);
USHORT APIENTRY DosSetVec(USHORT, PFN, PFN);
USHORT APIENTRY DosGetMessage(PCHAR FAR *, USHORT, PCHAR, USHORT, USHORT, PSZ, PUSHORT);
USHORT APIENTRY DosErrClass(USHORT, PUSHORT, PUSHORT, PUSHORT);
USHORT APIENTRY DosInsMessage(PCHAR FAR *, USHORT, PSZ, USHORT, PCHAR, USHORT, PUSHORT);
USHORT APIENTRY DosPutMessage(USHORT, USHORT, PCHAR);
USHORT APIENTRY DosSysTrace(USHORT, USHORT, USHORT, PCHAR);
USHORT APIENTRY DosGetEnv(PUSHORT, PUSHORT);
USHORT APIENTRY DosScanEnv(PSZ, PSZ  FAR *);
USHORT APIENTRY DosSearchPath(USHORT, PSZ, PSZ, PBYTE, USHORT);
USHORT APIENTRY DosGetVersion(PUSHORT);
USHORT APIENTRY DosGetMachineMode(PBYTE);

#endif /* INCL_DOSQUEUES */


/*** Session manager support */

#ifdef INCL_DOSSESMGR

typedef struct _STARTDATA {   /* stdata */
    USHORT cb;
    USHORT Related;
    USHORT FgBg;
    USHORT TraceOpt;
    PSZ    PgmTitle;
    PSZ    PgmName;
    PBYTE  PgmInputs;
    PBYTE  TermQ;
    PBYTE  Environment;
    USHORT InheritOpt;
    USHORT SessionType;
    PSZ    IconFile;
    ULONG  PgmHandle;
    USHORT PgmControl;
    USHORT InitXPos;
    USHORT InitYPos;
    USHORT InitXSize;
    USHORT InitYSize;
} STARTDATA;
typedef STARTDATA FAR *PSTARTDATA;

typedef struct _STATUSDATA { /* stsdata */
    USHORT cb;
    USHORT SelectInd;
    USHORT BindInd;
} STATUSDATA;
typedef STATUSDATA FAR *PSTATUSDATA;

USHORT APIENTRY DosStartSession(PSTARTDATA, PUSHORT, PUSHORT);
USHORT APIENTRY DosSetSession(USHORT, PSTATUSDATA);
USHORT APIENTRY DosSelectSession(USHORT, ULONG);
USHORT APIENTRY DosStopSession(USHORT, USHORT, ULONG);
USHORT APIENTRY DosQAppType(PSZ, PUSHORT);
USHORT APIENTRY DosSGNum(PUSHORT);
USHORT APIENTRY DosSGSwitch(USHORT);
USHORT APIENTRY DosSGSwitchMe(USHORT);

#endif /* INCL_DOSSESMGR */


/*** Device support */

#ifdef INCL_DOSDEVICES

USHORT APIENTRY DosDevConfig(PVOID, USHORT, USHORT);
USHORT APIENTRY DosDevIOCtl(PVOID, PVOID, USHORT, USHORT, HFILE);

USHORT APIENTRY DosCLIAccess(VOID);
USHORT APIENTRY DosIOAccess(USHORT, USHORT, USHORT, USHORT);
USHORT APIENTRY DosPortAccess(USHORT, USHORT, USHORT, USHORT);
USHORT APIENTRY DosPhysicalDisk(USHORT, PBYTE, USHORT, PBYTE, USHORT);

USHORT APIENTRY DosR2StackRealloc(USHORT);
VOID   APIENTRY DosCallback(PFN);

#endif /* INCL_DOSDEVICES */


/*** DosProfile API support */

#ifdef INCL_DOSPROFILE

/* DosProfile ordinal number */

#define PROF_ORDINAL     133

/* DosProfile usType */

#define PROF_SYSTEM      0
#define PROF_USER        1
#define PROF_USEDD       2
#define PROF_KERNEL      4
#define PROF_VERBOSE     8
#define PROF_ENABLE     16

/* DosProfile usFunc */

#define PROF_ALLOC       0
#define PROF_CLEAR       1
#define PROF_ON          2
#define PROF_OFF         3
#define PROF_DUMP        4
#define PROF_FREE        5

/* DosProfile tic count granularity (DWORD) */

#define PROF_SHIFT       2

/* DosProfile module name string length */

#define PROF_MOD_NAME_SIZE   10

/* DosProfile error code for end of data */

#define PROF_END_OF_DATA     13

#endif /* INCL_DOSPROFILE */
