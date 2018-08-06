/****************************** Module Header ******************************\
* Module Name: DMGP.H
*
* Private header for DDE manager DLL.
*
* Created:      12/16/88    by Sanford Staab
* 6/12/90 sanfords  Added definitions so it can compile on SLOOP environment.
*                   Added ST_BOTHDLLCLIENTS
*                   Added Dynamic DDE initiate APIs
*                   Reconciled PQUEUEITEM and QST structures
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#define INCL_DOSPROCESS
#define INCL_DOSMEMMGR
#define INCL_DOSSEMAPHORES
#define INCL_WIN
#define INCL_WINDDE
#define INCL_WINATOM
#define INCL_WINHEAP
#define INCL_DOSNLS
#include "os2.h"
#include "ddeml.h"

typedef HATOMTBL FAR *PHATOMTBL;
typedef ULONG LATOM;

/***** structures *****/



/*
 * generic list structures
 */
typedef struct _LITEM {         /* 4 */
    struct _LITEM FAR *next;    /* 0 */
} LITEM;
typedef LITEM FAR *PLITEM;

typedef struct _LST {           /* A */
    PLITEM pItemFirst;          /* 0 */
    HHEAP hheap;                /* 4 */
    USHORT cbItem;              /* 8 */
} LST;
typedef LST FAR *PLST;

#define ILST_LAST       0x0000
#define ILST_FIRST      0x0001
#define ILST_NOLINK     0x0002


/*
 * Specific list item structures
 */
typedef struct _CBLI {      /* 20 callback list item */
    PLITEM next;            /* 0 */
    HCONV hConv;            /* 4 perameters for callback */
    HSZ hszTopic;           /* 8 */
    HSZ hszItem;            /* C */
    USHORT usFmt;           /* 10 */
    USHORT usType;          /* 12 */
    HDMGDATA hDmgData;      /* 14 */
    USHORT msg;             /* 18 DDE message received that created this item */
    USHORT fsStatus;        /* 1A Status from DDE msg */
    HCONV hConvPartner;     /* 1C source window */
} CBLI;
typedef CBLI FAR *PCBLI;

typedef struct _HSZLI {     /* 8 */
    PLITEM next;            /* 0 */
    HSZ hsz;                /* 4 */
} HSZLI;
typedef HSZLI FAR *PHSZLI;

typedef struct _HWNDLI {    /* 8 */
    PLITEM next;            /* 0 */
    HWND   hwnd;            /* 4 */
} HWNDLI;
typedef HWNDLI FAR *PHWNDLI;

typedef struct _ACKHWNDLI { /* 10 */
    PLITEM next;            /* 0 */
    HWND   hwnd;            /* 4 same as HWNDLI to here */
    HSZ    hszApp;          /* 8 */
    HSZ    hszTopic;        /* C */
} ACKHWNDLI;
typedef ACKHWNDLI FAR *PACKHWNDLI;

typedef struct _HWNDHSZLI { /* C */
    PLITEM next;            /* 0 */
    HSZ    hsz;             /* 4 should be same as HSZLI to here */
    HWND   hwnd;            /* 8 */
} HWNDHSZLI;
typedef HWNDHSZLI FAR *PHWNDHSZLI;

typedef struct _ADVLI {     /* C */
    PLITEM  next;           /* 0 */
    HSZ     hszItem;        /* 4 should be same as HSZLI to here */
    USHORT  usFmt;          /* 8 */
    USHORT  fsStatus;       /* A used to remember NODATA and FACKREQ state */
} ADVLI;
typedef ADVLI FAR *PADVLI;


/*
 * generic pile structures - a pile is an unordered list with no key
 * duplicates.
 */
typedef struct _PILEB {         /* E */
    struct _PILEB FAR *next;    /* 0 same as LITEM structure */
    USHORT cItems;              /* 4 pile item additions */
    USHORT reserved;            /* 6 */
} PILEB;
typedef PILEB FAR *PPILEB;

typedef struct PILE {           /*  */
    PPILEB pBlockFirst;         /* 0 same as LST structure */
    HHEAP hheap;                /* 4 */
    USHORT cbBlock;             /* 8 */
    
    USHORT cSubItemsMax;        /* A additions for piles */
    USHORT cbSubItem;           /* C */
} PILE;
typedef PILE FAR *PPILE;
typedef BOOL (*NPFNCMP)(PBYTE, PBYTE);

#define PTOPPILEITEM(p) ((PBYTE)p->pBlockFirst + sizeof(PILEB))

#define FPI_DELETE 0x1
#define FPI_COUNT  0x2

/*
 * These bits are used to keep track of advise loop states.
 */
#define ADVST_POSTED    0x0100  /* Appstatus bit - set if callback Q has ADVREQ */
#define ADVST_WAITING   0x0080  /* fReserved bit - set if still waiting for FACK */
#define ADVST_CHANGED   0x0040  /* fReserved bit - set if data changed while waiting */

/*
 * These may need to change if either the list items change or any of
 * their component parts change size.
 */
#define CmpHwnd CmpULONG


/* queue structures */

typedef struct _QUEUEITEM {         /* A */
    struct _QUEUEITEM FAR *next;    /* 0 */
    struct _QUEUEITEM FAR *prev;    /* 4 */
    USHORT   inst;                  /* 8 */
} QUEUEITEM;
typedef QUEUEITEM FAR *PQUEUEITEM;
                    
typedef struct _QST {               /* C */
    USHORT cItems;                  /* 0 */
    USHORT instLast;                /* 2 */
    USHORT cbItem;                  /* 4 */
    USHORT usRes;                   /* 6 */
    HHEAP hheap;                    /* 8 */
    PQUEUEITEM pqiHead;
} QST;
typedef QST FAR *PQST;

#define MAKEID(pqd) ((ULONG)(USHORT)(pqd) + ((ULONG)(((PQUEUEITEM)pqd)->inst) << 16))
#define PFROMID(pQ, id) ((PQUEUEITEM)MAKEP(HIUSHORT(pQ), LOUSHORT(id)))

typedef struct _XFERINFO { /* 1C */
    PULONG pidXfer;        /* 0 if of queued transaction on QID_SYNC */
    ULONG  ulTimeout;      /* 4 timeout allowed if QID_SYNC */
    USHORT usType;         /* 8 transaction type */
    USHORT usFmt;          /* A */
    HSZ    hszItem;        /* C */
    HCONV  hConv;          /* 10 client conversation handle */
    ULONG  cb;             /* 14 count of data to get from client */
    PBYTE  pData;          /* 18 location of data in client app */
} XFERINFO;
typedef XFERINFO FAR *PXFERINFO;

typedef struct _APPINFO {           /* 46 */
    struct  _APPINFO FAR *next;     /* 0 */
    struct  _APPINFO FAR *nextThread; /* 4 */ 
    PPILE   pAppNamePile;           /* 8 */ 
    PLST    plstCBExceptions;       /* C */ 
    ULONG   afCmd;                  /* 10 */
    HWND    hwndFrame;              /* 14 */
    HWND    hwndMonitor;            /* 18 */
    HWND    hwndDmg;                /* 1C */
    HWND    hwndTimer;              /* 20 */
    PID     pid;                    /* 24 */
    TID     tid;                    /* 28 */
    PFNCALLBACK pfnCallback;        /* 2C */
    PPILE   pHDataPile;             /* 30 */
    PLST    pSvrTopicList;          /* 34 */
    PLST    plstCB;                 /* 38 */
    HHEAP   hheapApp;               /* 3C */
    USHORT  cInCallback;            /* 40 */
    USHORT  LastError;              /* 42 */
    BOOL    fEnableCB;              /* 44 */
} APPINFO;
typedef APPINFO FAR *PAPPINFO;
typedef PAPPINFO FAR *PPAPPINFO;

typedef struct _MYDDES {    /* 20 */
    ULONG   cbData;         /* 0 same as DDESTRUCT */
    USHORT  fsStatus;       /* 4 */
    USHORT  usFormat;       /* 6 */
    USHORT  offszItemName;  /* 8 */
    USHORT  offabData;      /* A */
    ULONG   ulRes1;         /* C    handle control info */
    USHORT  magic;          /* 14 */
    USHORT  fs;             /* 16   HDATA_ flags */
    HSZ     hszItem;        /* 18 */
    PAPPINFO pai;           /* 1C */
} MYDDES;
typedef MYDDES FAR *PMYDDES;

#define MYDDESMAGIC     0x5353
 
/*
 * private afCmd values
 */
#define     DMGCMD_32BIT    0x8000L

typedef struct _XADATA {    /* C */
    USHORT     state;       /* 0  state of this transaction (CONVST_) */     
    USHORT     LastError;   /* 2  last error logged in this transaction */
    PXFERINFO  pXferInfo;   /* 4  transaction info */
    PDDESTRUCT pddes;       /* 8  data for the client from the server */
} XADATA;
typedef XADATA FAR *PXADATA;    

typedef struct _COMMONINFO {  /* 38 */
    PAPPINFO   pai;           /* 0 */ 
    HSZ        hszServerApp;  /* 4 */  
    HSZ        hszTopic;      /* 8 */ 
    ULONG      hAgent;        /* C  agent handle, 0 for local conversations */
    HWND       hwndPartner;   /* 10 */
    PLST       pAdviseList;   /* 14 */
    XADATA     xad;           /* 18 */
    CONVCONTEXT cc;           /* 24 */
    SHORT      fs;            /* 30 */
    USHORT     reserverd;     /* 32 for DWORD alignment */  
    HWND       hwndFrame;     /* 34 */
} COMMONINFO;

typedef struct _CLIENTINFO {        /* 3C */
    COMMONINFO ci;                  /* 0 */
    PQST       pQ;                  /* 38 */
} CLIENTINFO;
typedef CLIENTINFO FAR *PCLIENTINFO;

typedef struct _SERVERINFO {
    COMMONINFO ci;
} SERVERINFO;
typedef SERVERINFO FAR *PSERVERINFO;

typedef struct _INITINFO {          /* 18 */
    HSZ        hszTopic;            /* 0 */
    HSZ        hszAppName;          /* 4 */
    HWND       hwndSend;            /* C */
    HWND       hwndFrame;           /* 10 */
    PCONVCONTEXT pCC;               /* 14 */
} INITINFO;
typedef INITINFO FAR *PINITINFO;

/* specific queue structures */

typedef struct _CQDATA {            /* 32 */
    PQUEUEITEM FAR *next;           /* 0 */
    PQUEUEITEM FAR *prev;           /* 4 */
    USHORT   inst;                  /* 8 */ /* same as QUEUEITEM */
    XADATA   xad;                   /* a */
    XFERINFO XferInfo;              /* 14 */
} CQDATA;
typedef CQDATA FAR *PCQDATA;



/****** strings ******/
#define SZDMGSEM "\\sem\\ddeml.lck"
#define SZDMGCLASS "DMGClass"
#define SZCLIENTCLASS "DMGClientClass"
#define SZSERVERCLASS "DMGServerClass"
#define SZMONITORCLASS "DMGMonitorClass"
#define SZDEFCLASS "DMGHoldingClass"
#define SZERRCAPTION "DDE Manager DLL error:"
#define SZSYSTEMTOPIC "System"
#define SZSYSTOPICS "Topics"

/***** private window messages and constants ******/

#define     HDATA_READONLY          0x8000
#define     HDATA_APPFREEABLE       0x4000
#define     HDATA_PSZITEMSET        0x2000

#define     UMCL_INITIATE           WM_USER
#define     UMCL_TERMINATE          (WM_USER + 2)

#define     UMCL_XFER               (WM_USER + 4)

#define     UMSR_INITIATE           (WM_USER + 100)
#define     UMSR_TERMINATE          (WM_USER + 102)
#define     UMSR_RESET              (WM_USER + 103)
#define     UMSR_POSTADVISE         (WM_USER + 104)
#define     UMSR_QREPLY             (WM_USER + 105)

#define     UM_REGISTER             (WM_USER + 200)
#define     UM_UNREGISTER           (WM_USER + 201)
#define     UM_MONITOR              (WM_USER + 202)
#define     UM_QUERY                (WM_USER + 203)
#define         Q_APPNAME             1
#define         Q_STATUS              2
#define         Q_CLIENT              3
#define         Q_APPINFO             4
#define         Q_TOPIC               5
#define         Q_ALL                10
#define     UM_CHECKCBQ             (WM_USER + 204)

#define     AI_TIMEOUT          0x0001

#define     DMGHAB                  0
#define     MAX_MONITORS            8
#define     MAX_RECURSE             3


/* these constants help identify windows during debugging. */

#define     WID_APPROOT         0x100
#define     WID_CLROOT          0x300
#define     WID_SVRTOPIC        0x201
#define     WID_SERVER          0x202
#define     WID_CLIENT          0x302
#define     WID_DMGROOT         0x400
#define     WID_MONITORROOT     0x500
#define     WID_MONITOR         0x502


/***** Queue control constants *****/

#define QST_OVERFLOW    0x1
#define QST_CREATED     0x2

#define MDPM_FREEHDATA  1

/****** private procedures ******/

/* from dmgstrt.asm */

void LoadProc(void);
void FillBlock(PBYTE pDst, USHORT cb, USHORT b);
BOOL CopyBlock(PBYTE pSrc, PBYTE pDst, USHORT cb);
PBYTE HugeOffset(PBYTE pSrc, ULONG cb);


/* dmg.c entrypoints are exported by ddeml.h */

/* from ddeml.c */

void SpawnExtraAckClients(HCONV hConv, HCONVLIST hConvList, PCONVCONTEXT pCC);
USHORT EXPENTRY Register(PFNCALLBACK pfnCallback, ULONG afCmd, ULONG ulRes,
        BOOL f32bit);

/* from dmgwndp.c */

MRESULT EXPENTRY DmgWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ServerWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
HWND CreateServerWindow(PAPPINFO pai, HSZ hszTopic);
HWND FindFrame(HWND hwnd);
void QReply(PCBLI pcbi, PDDESTRUCT pddes);
MRESULT EXPENTRY subframeWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
HDMGDATA DoCallback(PAPPINFO pai, HCONV hConv, HSZ hszTopic, HSZ hszItem,
    USHORT usFmt, USHORT usType, HDMGDATA hDmgData);

/* from dmgdde.c */

BOOL    timeout(PAPPINFO pai, ULONG ulTimeout, HWND hwndTimeout);
BOOL    ClientInitiate(HWND hwnd, PINITINFO pinitInfo, PCLIENTINFO pci);
BOOL    SendDDEInit(HWND hwndFrom, HWND hwndTo, PCLIENTINFO pci);
PDDESTRUCT AllocDDESel(USHORT fsStatus, USHORT usFmt, HSZ hszItem,
        ULONG cbData, PAPPINFO pai);
PDDESTRUCT getServerData(HCONV hConv, HSZ hszTopic, HSZ hszItem,
        USHORT usFmt, USHORT usType, PFNCALLBACK pfnDataFromApp,
        PAPPINFO pai);
HCONV   GetDDEClientWindow(HCONVLIST hConvList, HWND hwndFrame, HWND hwndSend,
        HSZ hszApp, HSZ hszTopic, PCONVCONTEXT pCC);
BOOL    MakeCallback(PAPPINFO pai, HCONV hConv, HSZ hszTopic, HSZ hszItem,
        USHORT usFmt, USHORT usType, HDMGDATA hDmgData, USHORT msg,
        USHORT fsStatus, HCONV hConvClient);
BOOL MyDdePostMsg(HWND hwndTo, HWND hwndFrom, USHORT msg, PMYDDES pmyddes,
        PAPPINFO paiFrom, USHORT afCmd);
        
/* from dmgmon.c */

BOOL EXPENTRY DdePostHookProc(HAB hab, PQMSG pqmsg, USHORT msgf);
BOOL EXPENTRY DdeSendHookProc(HAB hab, PSMHSTRUCT psmh, BOOL fInterTask);
void    MonitorBroadcast(PSZ psz);
BOOL    allocMonStr(PSZ FAR *ppsz, PSZ FAR *ppszLast);
MRESULT EXPENTRY MonitorWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);

/* from dmghsz.c */

PSZ pszFromHsz(HSZ hsz, USHORT FAR *pcch);
HSZ GetHsz(PSZ psz, USHORT cc, USHORT cp, BOOL fAdd);
BOOL FreeHsz(HSZ hsz);
BOOL IncHszCount(HSZ hsz);
BOOL AddAtomTable(BOOL fInit);
USHORT QueryHszLength(HSZ hsz);
USHORT QueryHszName(HSZ hsz, PSZ psz, USHORT cchMax);
SHORT CmpHsz(HSZ hsz1, HSZ hsz2);
USHORT QuerylatomLength(LATOM latom);
BOOL Freelatom(LATOM latom);
BOOL InclatomCount(LATOM latom);
LATOM Decode(PBYTE pb);
PBYTE Encode(ULONG latom, PBYTE pb);
USHORT QuerylatomName(LATOM latom, PSZ psz, USHORT cchMax);
LATOM FindAddlatom(PSZ psz, BOOL fAdd);
LATOM FindAddlatomHelper(PSZ psz, BOOL fAdd);
HSZ GetHszItem(PMYDDES pmyddes, PCONVCONTEXT pCC, BOOL fAdd);

/* from dmgstrt.asm */
BOOL CheckSel(SEL sel);

/* from dmgdb.c */

PAPPINFO GetCurrentAppInfo(BOOL fChkCallback);
void UnlinkAppInfo(PAPPINFO pai);
BOOL CmpAdv(PBYTE pb1, PBYTE pb2);

PLST CreateLst(HHEAP hheap, USHORT cbItem);
void FlushLst(PLST pLst);
void DestroyLst(PLST pLst);
PLITEM FindLstItem(PLST pLst, NPFNCMP npfnCmp, PLITEM piSearch);
BOOL CmpULONG(PBYTE pb1, PBYTE pb2);
BOOL CmppHsz(PBYTE pb1, PBYTE pb2);
PLITEM NewLstItem(PLST pLst, USHORT afCmd);
BOOL RemoveLstItem(PLST pLst, PLITEM pi);
BOOL InsertLstItem(PLST pLst, PLITEM pi, USHORT afCmd);

PPILE CreatePile(HHEAP hheap, USHORT cbItem, USHORT cItemsPerBlock);
PPILE DestroyPile(PPILE pPile);
void FlushPile(PPILE pPile);
USHORT QPileItemCount(PPILE pPile);
BOOL CopyPileItems(PPILE pPile, PBYTE pDst);
PBYTE FindPileItem(PPILE pPile, NPFNCMP npfnCmp, PBYTE pbSearch, USHORT afCmd);
BOOL AddPileItem(PPILE pPile, PBYTE pb, NPFNCMP npfncmp);
BOOL PopPileSubitem(PPILE pPile, PBYTE pb);

BOOL fSearchHwndList(PLST pLst, HWND hwnd);
void AddHwndList(HWND hwnd, PLST pLst);
void AddAckHwndList(HWND hwnd, HSZ hszApp, HSZ hszTopic, PLST pLst);
         
void AddHwndHszList(HSZ hsz, HWND hwnd, PLST pLst);
void DestroyHwndHszList(PLST pLst);
HWND HwndFromHsz(HSZ hsz, PLST pLst);
BOOL AddAdvList(PLST pLst, HSZ hszItem, USHORT fsStatus, USHORT usFormat);
BOOL DeleteAdvList(PLST pLst, HSZ hszItem, USHORT usFmt);
PADVLI FindAdvList(PLST pLst, HSZ hszItem, USHORT usFmt);
PADVLI FindNextAdv(PADVLI padvli, HSZ hszItem);

void SemInit(void);
void SemCheckIn(void);
void SemCheckOut(void);
void SemEnter(void);
void SemLeave(void);

void EXPENTRY ExlstAbort(USHORT usTermCode);
BOOL CopyHugeBlock(PBYTE pSrc, PBYTE pDst, ULONG cb);
BOOL DestroyWindow(HWND hwnd);
HCONV IsDdeWindow(HWND hwnd); 
void FreeData(PMYDDES pmyddes, PAPPINFO pai);
HDMGDATA PutData(PBYTE pSrc, ULONG cb, ULONG cbOff, HSZ hszItem, USHORT usFmt,
        USHORT afCmd, PAPPINFO pai);
ULONG QueryAppNames(PAPPINFO pai, HDMGDATA hDataAdd, HSZ hszApp, ULONG offAdd);

/* from mem.c */

PBYTE  FarAllocMem(HHEAP hheap, USHORT cb);
NPBYTE DbgAllocMem(HHEAP hheap, USHORT cb);
HHEAP DbgCreateHeap(USHORT sel, USHORT cb, USHORT cbGrow, USHORT cbMinDed,
    USHORT cbMaxDed, USHORT fs);
HHEAP DbgDestroyHeap(HHEAP hheap);
NPBYTE DbgFreeMem(HHEAP hheap, NPBYTE pb, USHORT cb);


/* from dmgstr.c */

int lstrlen(PSZ psz);
PSZ lstrcat(PSZ psz1, PSZ psz2, PSZ pszLast);
PSZ hwndToPsz(HWND hwnd, PSZ psz, PSZ pszLast);
PSZ pddesToPsz(USHORT msg, PDDESTRUCT pddes, PSZ psz, PSZ pszLast);
PSZ Status(USHORT fs, PSZ psz, PSZ pszLast);
PSZ Format(USHORT fmt, PSZ psz, PSZ pszLast);
PSZ ddeMsgToPsz(USHORT msg, PSZ psz, PSZ pszLast);
PSZ itoa(USHORT us, PSZ psz, PSZ pszLast);
PSZ ltoa(ULONG ul, PSZ psz, PSZ pszLast);
PSZ stoa(PSZ psz, USHORT us);
PSZ dtoa(PSZ psz, USHORT us, BOOL fRecurse);
PSZ timestamp(PSZ psz, PSZ pszLast);

/* from dmgq.c */

PQST CreateQ(USHORT cbItem);
BOOL DestroyQ(PQST pQ);
PQUEUEITEM Addqi(PQST pQ);
void Deleteqi(PQST pQ, ULONG id);
PQUEUEITEM Findqi(PQST pQ, ULONG id);

/* from bakthnk.asm */
HDMGDATA NEAR PASCAL ThkCallback(HCONV hConv, HSZ hszTopic, HSZ hszItem,
    USHORT usFmt, USHORT usType, HDMGDATA hDmgData, PFNCALLBACK pfn);

/* macros */

#define UNUSED
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define PSZAPP(p) (PSZ)(((ULONG)p & 0xFFFF0000L) + (USHORT)(((PDDEINIT)(p))->pszAppName))
#define PSZTOPIC(p) (PSZ)(((ULONG)p & 0xFFFF0000L) + (USHORT)(((PDDEINIT)(p))->pszTopic))

#ifdef DEBUG

#define HEAPFLAGS (HM_MOVEABLE | HM_VALIDSIZE)
#define MyAllocMem DbgAllocMem
#define MyFreeMem DbgFreeMem
#define MyCreateHeap DbgCreateHeap
#define MyDestroyHeap DbgDestroyHeap
void fAssert(BOOL f, PSZ psz, USHORT line, PSZ szFile);
#define AssertF(f, psz)	fAssert(f, psz, __LINE__, __FILE__);
#define DebugBreak() DdeDebugBreak()

#else

#define HEAPFLAGS 0
#define MyAllocMem WinAllocMem
#define MyFreeMem WinFreeMem
#define MyCreateHeap WinCreateHeap
#define MyDestroyHeap WinDestroyHeap
#define AssertF(f, psz)
#define DebugBreak()
#define SemCheckIn()
#define SemCheckOut()
#endif /* DEBUG */

#define FarFreeMem(hheap, pb, cb) (MyFreeMem(hheap, (NPBYTE)(SHORT)pb, cb))

/******* globals ******/
extern HMODULE hmodDmg;
extern HWND hwndDmgMonitor;
extern PFNWP lpfnFrameWndProc;
extern HHEAP hheapDmg;
extern USHORT cMonitor;
extern USHORT cAtoms;
extern HATOMTBL hatomtblDmg;
extern DOSFSRSEM FSRSemDmg;
extern PAPPINFO pAppInfoList;
extern USHORT usHugeShift;
extern USHORT usHugeAdd;
extern COUNTRYCODE syscc;
extern PHATOMTBL aAtbls;
extern USHORT cAtbls;
extern USHORT iAtblCurrent;
extern char szT[];
