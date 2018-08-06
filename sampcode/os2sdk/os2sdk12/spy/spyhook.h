#include <selector.h>

/* Maximum HWNDS, we will watch */
#define MAXHWNDS    100
#define MAXMSGCNT   50  /* size of Message queue */
#define MAXSTRACE   5   /* Max number of levels we will traceback */
#define CALLSTOSKIP 3   /* How many levels of calls to skip */

/*
 * Define message structure for spy to hold
 */
typedef struct _QMSGSPY {
    USHORT      fs;         /* Flags */
    QMSG        qmsg;       /* The message */
    SHORT       cbDataMP1;  /* Num extra bytes of information for MP1 */
    SHORT       cbDataMP2;  /* Num extra bytes of information for MP2 */
    PID         pidSend;    /* Process ID of the sender */
    TID         tidSend;    /* Thread ID of the sender */
    PVOID       pvoidStack[MAXSTRACE];  /* Stack trace for send messages */
    UCHAR       *rgData;    /* Pointer to additional information for message */
    UCHAR       bMPType;    /* Type of message information retrieved */
} QMSGSPY;

typedef QMSGSPY FAR *PQMSGSPY;


/* Define the HOOKS options word bits */
#define SPYH_INPUT      0x0001
#define SPYH_SENDMSG    0x0002
#define SPYH_JOURNALR   0x0004

/*
 * Define the types of data that the hook will return for message types
 * Note: We have a max of 8 types each for MP1 and MP2 and that currently
 * can be defined with a Byte for each message
 */
#define MPT_NORMAL      0x00
#define MPT_SWP         0x01
#define MPT_RECTL       0x02
#define MPT_WNDPRMS     0x03
#define MPT_QMSG        0x04
#define MP_ENABLED      0x80

#define MP_MASK         0x07

#define MPTS(mp1,mp2)   ((UCHAR)mp2 << 3 | (UCHAR)mp1)

#define MAXMSGFILTER    	0x0300	/* User message */
#define MAXMSGFILTERBYTES	0x0300

/* SPYHOOK library API functions */
BOOL FAR PASCAL SpyInstallHook( HAB, HMQ, USHORT);
BOOL FAR PASCAL SpyReleaseHook( BOOL );
BOOL FAR PASCAL SpyHookOnOrOff( BOOL);
BOOL FAR PASCAL SpySetWindowList(SHORT, HWND FAR *);
SHORT FAR PASCAL SpyGetWindowList(SHORT, HWND FAR *);
BOOL FAR PASCAL SpySetQueueList(SHORT, HMQ FAR *);
SHORT FAR PASCAL SpyGetQueueList(SHORT, HMQ FAR *);
BOOL FAR PASCAL SpySetMessageList(UCHAR FAR *, BOOL);
BOOL FAR PASCAL SpyFWindowInList(HWND, BOOL);
BOOL FAR PASCAL SpySetAllFrameOpt(BOOL);
BOOL FAR PASCAL SpySetAllWindowOpt (BOOL);

BOOL FAR PASCAL SpyGetNextMessage(PQMSGSPY, PSZ, SHORT, LONG);

/* API's For trying to get info symbol information from other PID */
BOOL FAR PASCAL SpySetLNSymbolPID(PID, USHORT);
int  FAR PASCAL SpyGetLNSymbolSelector(PWHOISINFO);
