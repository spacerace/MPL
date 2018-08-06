/****************************** Module Header ******************************\
* Module Name: DDE.H
*
* Assumes pmwin.h is already included.
*
* Created: 12/15/88 Sanford Staab
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#ifdef INCL_32
#define DdeInitialize Dde32Initialize
#define DdeUninitialize Dde32Uninitialize
#define DdeBeginEnumServers Dde32BeginEnumServers
#define DdeGetNextServer Dde32GetNextServer
#define DdeEndEnumServers Dde32EndEnumServers
#define DdeConnect Dde32Connect
#define DdeDisconnect Dde32Disconnect
#define DdeQueryConvInfo Dde32QueryConvInfo
#define DdeCheckQueue Dde32CheckQueue
#define DdePostAdvise Dde32PostAdvise
#define DdeEnableCallback Dde32EnableCallback
#define DdeAppNameServer Dde32AppNameServer
#define DdeCreateInitPkt Dde32CreateInitPkt
#define DdeProcessPkt Dde32ProcessPkt
#define DdeClientXfer Dde32ClientXfer
#define DdePutData Dde32PutData
#define DdeAddData Dde32AddData
#define DdeGetData Dde32GetData
#define DdeAccessData Dde32AccessData
#define DdeFreeData Dde32FreeData
#define DdeGetLastError Dde32GetLastError
#define DdePostError Dde32PostError
#define DdeGetErrorString Dde32GetErrorString
#define DdeDebugBreak Dde32DebugBreak
#define DdeCopyBlock Dde32CopyBlock
#define DdeGetHsz Dde32GetHsz
#define DdeGetHszString Dde32GetHszString
#define DdeFreeHsz Dde32FreeHsz
#define DdeIncHszCount Dde32IncHszCount
#define DdeCmpHsz Dde32CmpHsz
#define DdeQueryVersion Dde32QueryVersion
#define DdeConverseWithWindow DdeMGr32ConverseWithWindow
#define DdeCreateServerWindow Dde32CreateServerWindow
#define DdeTest Dde32Test
#endif

/******** public types ********/

typedef HWND HCONVLIST;
typedef HWND HCONV;
typedef LHANDLE HAPP;
typedef LHANDLE HSZ;
typedef HSZ FAR *PHSZ;
typedef LHANDLE HDMGDATA;
typedef HDMGDATA FAR *PHDMGDATA;

#ifdef SLOOP
typedef struct _CONVCONTEXT { /* cctxt */
    USHORT cb;		/* sizeof(CONVCONTEXT) */
    USHORT fsContext;
    USHORT idCountry;
    USHORT usCodepage;
    USHORT usLangID;
    USHORT usSubLangID;
} CONVCONTEXT;
typedef CONVCONTEXT FAR *PCONVCONTEXT;
#define DDECTXT_CASESENSITIVE	 0x0001

typedef struct _NEWDDEINIT { /* ddei */
    USHORT  cb;		/* sizeof(DDEINIT) */
    PSZ	    pszAppName;
    PSZ	    pszTopic;
    USHORT  offConvContext;
} NEWDDEINIT;
typedef NEWDDEINIT FAR *PNEWDDEINIT;

#define DDEINIT NEWDDEINIT
#define PDDEINIT PNEWDDEINIT
#define DDEI_PCONVCONTEXT(pddei)	\
	((PCONVCONTEXT)((PBYTE)pddei + pddei->offConvContext))
    
#endif /* SLOOP */

/* PUBDOC START */

/* the following structure is for use with XTYP_WILDINIT processing. */

typedef struct _HSZPAIR {
    HSZ hszApp;
    HSZ hszTopic;
} HSZPAIR;
typedef HSZPAIR FAR *PHSZPAIR;

/* The following structure is provided for servicing DdeAppNameServer()
 * data handles.
 */

typedef struct _HSZHAPP {
    HSZ hsz;
    HAPP hApp;
} HSZHAPP;
typedef HSZHAPP FAR *PHSZHAPP;

/* The following structure is used by DdeQueryConvInfo(): */

/* PUBDOC START */

typedef struct _CONVINFO {
    ULONG   cb;          /* sizeof(CONVINFO) */
    HCONV   hConvPartner;/* hConv on other end or 0 if non-ddeml partner */
    HSZ     hszAppPartner; /* app name of partner if obtainable */
    HSZ     hszAppName;  /* AppName for conversation */
    HSZ     hszTopic;    /* Topic name for conversation */
    HSZ     hszItem;     /* transaction item name or NULL if quiescent */
    ULONG   hAgent;      /* foreign agent handle associated with this conv */
    USHORT  usFmt;       /* transaction format or NULL if quiescent */
    USHORT  usType;      /* XTYP_ for current transaction */
    USHORT  usStatus;    /* ST_ constant for current conversation */
    USHORT  usConvst;    /* CONVST_ constant for current transaction */
    USHORT  LastError;   /* last transaction error. */
    USHORT  fsContext;   /* conversation context flags */
    USHORT  usCodepage;  /* conversation context codepage */
    USHORT  idCountry;   /* conversation context countrycode */
    HAPP    hApp;        /* application handle associated with this hConv */
} CONVINFO;
typedef CONVINFO FAR *PCONVINFO;

/***** conversation states (usConvst) *****/

#define     CONVST_NULL              0  /* quiescent states */
#define     CONVST_INCOMPLETE        1
#define     CONVST_TERMINATED        2
#define     CONVST_CONNECTED         3
#define     CONVST_INIT1             4  /* mid-initiation state */
#define     CONVST_REQSENT           5  /* active conversation states */
#define     CONVST_DATARCVD          6
#define     CONVST_POKESENT          7
#define     CONVST_POKEACKRCVD       8
#define     CONVST_EXECSENT          9
#define     CONVST_EXECACKRCVD      10
#define     CONVST_ADVSENT          11
#define     CONVST_UNADVSENT        12
#define     CONVST_ADVACKRCVD       13
#define     CONVST_UNADVACKRCVD     14
#define     CONVST_ADVDATASENT      15
#define     CONVST_ADVDATAACKRCVD   16

/***** conversation status bits (fsStatus) *****/

#define     ST_CONNECTED        0x0001
#define     ST_ADVISE           0x0002
#define     ST_INTRADLL         0x0004    

/* PUBDOC END */

/****** constants *******/

#define     MAX_ERRSTR      50
#define     MAX_MONITORSTR  511

/***** modal loop timer IDs *****/

#define     TID_TIMEOUT             1
#define     TID_ABORT               2
#define     TID_SELFDESTRUCT        3

/***** message filter hook types *****/

#define     MSGF_DDE             0x8001

/***** transaction types *****/

#define     XTYPF_NOBLOCK            0x0002  /* CBR_BLOCK will not work */
#define     XTYPF_NODATA             0x0004  /* DDE_FNODATA */
#define     XTYPF_ACKREQ             0x0008  /* DDE_FACKREQ */
                                    
#define     XCLASS_MASK              0xF000
#define     XCLASS_BOOL              0x1000
#define     XCLASS_DATA              0x2000
#define     XCLASS_FLAGS             0x4000
#define     XCLASS_NOTIFICATION      0x8000
#define     XCLASS_DATAIN            0x0800
                                           
#define     XTYP_ACK                (0x0010 | XCLASS_NOTIFICATION)
#define     XTYP_ADVDATA            (0x0020 | XCLASS_FLAGS | XCLASS_DATAIN )
#define     XTYP_ADVREQ             (0x0030 | XCLASS_DATA          )
#define     XTYP_ADVSTART           (0x0040 | XCLASS_BOOL          )
#define     XTYP_ADVSTOP            (0x0050 | XCLASS_NOTIFICATION)
#define     XTYP_EXEC               (0x0060 | XCLASS_FLAGS | XCLASS_DATAIN )
#define     XTYP_INIT               (0x0070 | XCLASS_BOOL | XTYPF_NOBLOCK)
#define     XTYP_INIT_CONFIRM       (0x0080 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK)
#define     XTYP_MONITOR            (0x0090 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK | XCLASS_DATAIN)
#define     XTYP_PKT                (0x00A0 | XCLASS_DATA          )
#define     XTYP_POKE               (0x00B0 | XCLASS_FLAGS | XCLASS_DATAIN )
#define     XTYP_REGISTER           (0x00C0 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK)
#define     XTYP_REQUEST            (0x00D0 | XCLASS_DATA          )
#define     XTYP_RTNPKT             (0x00E0 | XCLASS_NOTIFICATION | XCLASS_DATAIN )
#define     XTYP_TERM               (0x00F0 | XCLASS_NOTIFICATION  )
#define     XTYP_UNREGISTER         (0x0100 | XCLASS_NOTIFICATION | XTYPF_NOBLOCK)
#define     XTYP_WILDINIT           (0x0110 | XCLASS_DATA | XTYPF_NOBLOCK )
#define     XTYP_XFERCOMPLETE       (0x0120 | XCLASS_NOTIFICATION  )

/***** Timeout constants *****/

#define     TIMEOUT_ASYNC           -1L

/***** error codes *****/

#define     DMGERR_NO_ERROR                    0       /* must be 0 */
#define     DMGERR_FIRST                       0x4000
#define     DMGERR_POSTMSG_FAILED              0x4000
#define     DMGERR_DLL_NOT_INITIALIZED         0x4001
#define     DMGERR_PMWIN_ERROR                 0x4002
#define     DMGERR_NO_CONV_ESTABLISHED         0x4003
#define     DMGERR_TIMEOUT                     0x4004
#define     DMGERR_REENTRANCY                  0x4005
#define     DMGERR_DLL_USAGE                   0x4006
#define     DMGERR_BAD_APP_NAME                0x4007
#define     DMGERR_DOS_ERROR                   0x4008
#define     DMGERR_SYSTOPIC_NOT_SUPPORTED      0x4009
#define     DMGERR_INVALID_HDMGDATA            0x400A
#define     DMGERR_BUSY                        0x400B
#define     DMGERR_NOTPROCESSED                0x400C
#define     DMGERR_INVALIDPARAMETER            0x400D
#define     DMGERR_MEMORY_ERROR                0x400E
#define     DMGERR_POKEACKTIMEOUT              0x400F
#define     DMGERR_EXECACKTIMEOUT              0x4010
#define     DMGERR_DATAACKTIMEOUT              0x4011
#define     DMGERR_SERVER_DIED                 0x4012
#define     DMGERR_ADVACKTIMEOUT               0x4013
#define     DMGERR_UNADVACKTIMEOUT             0x4014
#define     DMGERR_ACCESS_DENIED               0x4015
#define     DMGERR_UNFOUND_QUEUE_ID            0x4016
#define     DMGERR_NOT_IMPLEMENTED             0x4017 
#define     DMGERR_LAST                        0x4017


/****** public strings used in DDE ******/

#define SZDDESYS_TOPIC "System"
#define SZDDESYS_ITEM_TOPICS "Topics"
#define SZDDESYS_ITEM_SYSITEMS "SysItems"
#define SZDDESYS_ITEM_RTNMSG "ReturnMessage"
#define SZDDESYS_ITEM_STATUS "Status"
#define SZDDESYS_ITEM_FORMATS "Formats"


/****** API entry points ******/

typedef HDMGDATA EXPENTRY FNCALLBACK(HCONV hConv, HSZ hszTopic, HSZ hszItem,
            USHORT usFmt, USHORT usType, HDMGDATA hDmgData);
typedef HDMGDATA (EXPENTRY FAR *PFNCALLBACK)(HCONV hConv, HSZ hszTopic,
            HSZ hszItem, USHORT usFmt, USHORT usType, HDMGDATA hDmgData);
            
#define     CBR_BLOCK                0xffffffffL

/* DLL registration functions */

USHORT EXPENTRY DdeInitialize(PFNCALLBACK pfnCallback,
        ULONG afCmd, ULONG ulRes);

#define     DMGCMD_MONITOR          0x0001L
#define     DMGCMD_CLIENTONLY       0x0002L
#define     DMGCMD_CASEINSENSITIVE  0x0008L
#define     DMGCMD_AGENT            0x0010L
#define     DMGCMD_FILTERINITS      0x0020L

BOOL EXPENTRY DdeUninitialize(void);

/* conversation enumeration functions */

HCONVLIST EXPENTRY DdeBeginEnumServers(HSZ hszAppName, HSZ hszTopic,
        HCONV hConvList, PCONVCONTEXT pCC, HAPP hApp);
HCONV EXPENTRY DdeGetNextServer(HCONVLIST hConvList, HCONV hConvPrev);
BOOL EXPENTRY DdeEndEnumServers(HCONVLIST hConvList);

/* conversation control functions */

HCONV EXPENTRY DdeConnect(HSZ hszAppName, HSZ hszTopic, PCONVCONTEXT pCC,
        HAPP hApp);
BOOL EXPENTRY DdeDisconnect(HCONV hConv);

BOOL EXPENTRY DdeQueryConvInfo(HCONV hConv, PCONVINFO pConvInfo,
        ULONG idXfer);
        
ULONG EXPENTRY DdeCheckQueue(HCONV hConv, PHDMGDATA phDmgData,
        ULONG idXfer, ULONG afCmd);

#define     CQ_FLUSH                0x0001L
#define     CQ_NEXT                 0x0002L
#define     CQ_PREV                 0x0004L
#define     CQ_COUNT                0x0008L
#define     CQ_ACTIVEONLY           0x0010L
#define     CQ_INACTIVEONLY         0x0020L
#define     CQ_COMPLETEDONLY        0x0040L
#define     CQ_FAILEDONLY           0x0080L
#define     CQ_REMOVE               0x0100L

#define     QID_SYNC                 0L
#define     QID_NEWEST              -1L
#define     QID_OLDEST              -2L


/* app server interface functions */

BOOL EXPENTRY DdePostAdvise(HSZ hszTopic, HSZ hszItem);
BOOL EXPENTRY DdeEnableCallback(HCONV hConv, BOOL fEnable);

HDMGDATA EXPENTRY DdeAppNameServer(HSZ hszApp, USHORT afCmd);

#define ANS_REGISTER        0x0001
#define ANS_UNREGISTER      0x0002
#define ANS_QUERYALLBUTME   0x0004
#define ANS_QUERYMINE       0x0008
#define ANS_FILTERON        0x8000
#define ANS_FILTEROFF       0x4000

/* agent interface functions */

HDMGDATA EXPENTRY DdeCreateInitPkt(HSZ hszApp, HSZ hszTopic,
        HDMGDATA hDmgData);
HDMGDATA EXPENTRY DdeProcessPkt(HDMGDATA hPkt, ULONG hAgentFrom);

/* app client interface functions */

HDMGDATA EXPENTRY DdeClientXfer(PBYTE pSrc, ULONG cb, HCONV hConv,
        HSZ hszItem, USHORT usFmt, USHORT usType, ULONG ulTimeout,
        PULONG pidXfer);

/* data transfer functions */

HDMGDATA EXPENTRY DdePutData(PBYTE pSrc, ULONG cb, ULONG cbOff, HSZ hszItem,
        USHORT usFmt, USHORT afCmd);

#define     HDATA_APPOWNED          0x0001

HDMGDATA EXPENTRY DdeAddData(HDMGDATA hDmgData, PBYTE pSrc, ULONG cb,
        ULONG cbOff);
ULONG EXPENTRY DdeGetData(HDMGDATA hDmgData, PBYTE pDst, ULONG cbMax,
        ULONG cbOff);
PBYTE EXPENTRY DdeAccessData(HDMGDATA hDmgData);
BOOL EXPENTRY DdeFreeData(HDMGDATA hDmgData);

/* other exported functions */

USHORT EXPENTRY DdeGetLastError(void);
void EXPENTRY DdePostError(USHORT err);
USHORT EXPENTRY DdeGetErrorString(USHORT err, USHORT cbMax, PSZ psz);
void EXPENTRY DdeDebugBreak(void);
BOOL EXPENTRY DdeCopyBlock(PBYTE pSrc, PBYTE pDst, ULONG cb);
HSZ EXPENTRY DdeGetHsz(PSZ psz, USHORT country, USHORT codepage);
USHORT EXPENTRY DdeGetHszString(HSZ hsz, PSZ psz, ULONG cchMax);
BOOL EXPENTRY DdeFreeHsz(HSZ hsz);
BOOL EXPENTRY DdeIncHszCount(HSZ hsz);
SHORT EXPENTRY DdeCmpHsz(HSZ hsz1, HSZ hsz2);
ULONG EXPENTRY DdeQueryVersion(void);
HCONV EXPENTRY DdeConverseWithWindow(HWND hwnd, HSZ hszApp,
        HSZ hszTopic, PCONVCONTEXT pCC);
HWND EXPENTRY DdeCreateServerWindow(HSZ hszApp, HSZ hszTopic, PCONVCONTEXT pCC);
void EXPENTRY DdeTest(void);
