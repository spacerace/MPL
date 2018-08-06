/****************************** Module Header ******************************\
* Module Name: MSNGR.H
*
* Created:  12/20/88    sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOSNLS
#include "os2.h"
#include "ddeml.h"

/****** Resource IDs *****/
#define IDR_MSNGR1         1
#define IDR_MSNGR2         2
#define IDR_SELBMAP        3

/****** Control IDs *****/
/* for IDR_MAIN */
#define IDM_SEND           200
#define IDM_DOSENDTEXT     201
#define IDM_DOSENDBITMAP   202
#define IDM_DOSENDSONG     203
#define IDM_INITLINK       204
#define IDM_REGUSER        205

#include "dialog.h"

/****** string constants *****/

#define MAX_NAMESTR         12
#define MAX_TITLEPREFIXSTR  20
#define MAX_TITLESTR        (MAX_NAMESTR + MAX_TITLEPREFIXSTR)
#define MAX_MSGSTR          128
#define MAX_LINKSTR         256
#define MAX_CAPTIONSTR      32
#define MAX_EXECSTR         32

#define SZAPPCLASS         "Messenger"
#define SZAPPTITLEPREFIX   "Messenger for "
#define SZDUPTITLE         "Duplicate Name..."
#define SZMAKESELECTION    "You must select someone."
#define SZCANTCONNECT      "Error: I can't connect you."
#define SZEMPTYMSG         "Can't send an empty Message."
#define SZSENDFAILED       "Send Operation failed."
#define SZMSGIN            "Message received from "
#define SZNOTIFYCAPTION    "___Messenger Notification___"
#define SZBADCOMPARAMS     "Command parameters not recognized."
#define SZMEMORYERROR      "A memory error occurred."
#define SZALREADYLINKED    "You are already linked"
#define SZLINKED           "-- Linked --"
#define SZCANTLINK         "Link attempt failed."
#define SZINTERNALERROR    "Internal Error"
#define SZMSGLINKWITH      "'s Link With "
#define SZLINKBROKEN       "The link has been externally broken."
#define SZTOOBIG           "The selected area is too big."

/* DDE topics and items for this application. */
/*  Note that system topic strings are in ddeml.h. */
#define     SZDDEAPPNAME   "Messenger Application"
#define     SZDDEMSGTOPIC  "Messages Topic"
#define     SZUSERNAME     "User Name"
#define     SZMESSAGEDATA  "Message Data"
#define     SZLINKDATA     "Link Data"
#define     SZBMAPDATA     "Bitmap Data"

/* used for execute DDE message */
#define SZNAMEINSTRUCT     "Email name setup"

/* class names for this app */
#define SZENHANCEDEFCLASS  "EnhancedEFClass"
#define SZHOLDINGCLASS     "HoldingClass"

/* topicList index constants */
#define ITL_SYS     0
#define ITL_MSG     1
#define ITL_LAST    1

/* itemList lengths and constants */
#define IIL_SYSLAST     4
#define IIL_MSGUSERNAME 0
#define IIL_MSGXFER     1
#define IIL_BMPXFER     2
#define IIL_MSGLAST     2

/* window ids */
#define WID_NOTIFY1 41
#define WID_NOTIFY2 42
#define WID_LISTBOX 43

#define DEFTIMEOUT  2000L

/****** Private window messages *****/
#define UM_MSG_NOTIFY   WM_USER
#define UM_DESTRUCT     (WM_USER + 1)
#define UM_LINKEVENT    (WM_USER + 2)
#define UM_LINK1        (WM_USER + 3)
#define UM_DOADVSTART   (WM_USER + 4)
#define UM_ADVRCVD      (WM_USER + 5)
#define UM_LINKDATAIN     (WM_USER + 6)
#define UM_BREAKLINK  (WM_USER + 7)
#define UM_LINKDATAOUT  (WM_USER + 8)
#define ENHAN_ENTER     (WM_USER + 9)

/****** link states ******/
#define LNKST_USERSTART     0
#define LNKST_DMGSTART      1
#define LNKST_UNLINKED      2
#define LNKST_LINKREQ       3
#define LNKST_LINKED        4
#define LNKST_UNLNKREQ      5

/****** Structrues ******/

typedef struct _XFERINFO {
    HDMGDATA hDmgData;
    USHORT usType;
    USHORT usFmt;
    HSZ hszItem;
    HSZ hszTopic;
    HCONV hConv;
} XFERINFO;
typedef XFERINFO FAR *PXFERINFO;

typedef struct _USERLIST {
    struct _USERLIST        *next;
    HCONV                   hConvMsg;
    HCONV                   hConvLink;
    HSZ                     hsz;
    HWND                    hwndLink;
    HAPP                    hApp;
} USERLIST;
typedef USERLIST *NPUSERLIST;
typedef NPUSERLIST FAR *PNPUSERLIST;

typedef struct _ITEMLIST {
    HSZ hszItem;
    HDMGDATA (*npfnCallback)(PXFERINFO);
    PSZ pszItem;
} ITEMLIST;

typedef struct _TOPICLIST {
    HSZ hszTopic;
    ITEMLIST *pItemList;
    USHORT iItemLast;
    PSZ pszTopic;
} TOPICLIST;



/****** globals ******/

extern HHEAP hheap;


/****** MACROS *****/

#define min(a,b)    (a > b ? b : a)
#define NOTIFYOWNER(hwnd, msg, mp1, mp2) \
            (WinSendMsg(WinQueryWindow(hwnd, QW_OWNER, FALSE), msg, mp1, mp2))
#define UNUSED


/************* PROCEDURE DECLARATIONS   *************/
/* from msngr.c */

void cdecl main(int argc, char *argv[]);
MRESULT EXPENTRY MainWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY GetNameDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY EnhancedEFWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SendTextDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SendBitmapDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SendSongDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
HDMGDATA EXPENTRY Callback(HCONV hConv, HSZ hszTopic, HSZ hszItem,
        USHORT usFmt, USHORT usType, HDMGDATA hDmgData);
HDMGDATA TopicListXfer(PXFERINFO pXferInfo);
HDMGDATA ItemListXfer(PXFERINFO pXferInfo);
HDMGDATA NotSupported(PXFERINFO pXferInfo);
HDMGDATA sysFormatsXfer(PXFERINFO pXferInfo);
HDMGDATA msgUserNameXfer(PXFERINFO pXferInfo);
HDMGDATA msgMessageXfer(PXFERINFO pXferInfo);
void CheckForDups(HWND hwndLB);
void Hszize(void);
void UnHszize(void);
NPUSERLIST AccessSelection(void);
void MyPostErr(USHORT err);
BOOL RegisterUser(HAPP hApp, HDMGDATA hData, BOOL fRegister);
BOOL UnregisterUser(HAPP);
VOID SetEmailName(PSZ psz);
void NotifyUser(PSZ psz);
void NotifyUser2(PSZ pszCaption, PSZ pszText);

extern ULONG ulTimeout;

/* from link.c */
BOOL CreateLinkWindow(NPUSERLIST pUserItem, USHORT state);
MRESULT EXPENTRY LinkDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
HDMGDATA LinkXfer(PXFERINFO pXferInfo, HSZ hszPartner);

/* from util.c */
void lstrcat(PSZ pszDest, PSZ psz1, PSZ psz2);
void lstrcpy(PSZ pszDst, PSZ pszSrc);
void lstrpak(PSZ pszDest, PSZ psz1, PSZ psz2);
int lstrlen(PSZ psz);
void DestroyUser(NPUSERLIST npUL);
VOID AddUser(HCONV hConvMsg, HSZ hsz, HAPP hApp);
HCONV HszTohConvMsg(NPUSERLIST pList, HSZ hsz);
NPUSERLIST FindUser(NPUSERLIST pList, HSZ hsz);
void freeUserList(PNPUSERLIST ppList);
PVOID FarAllocMem(HHEAP hheap, USHORT cb);
void MyPostError(USHORT err);

/* from bmap.c */
void InitBmapModule(void);
void CloseBmapModule(void);
HDMGDATA bmpXfer(PXFERINFO pXferInfo);
MRESULT EXPENTRY BmpDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SendBitmapDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);


