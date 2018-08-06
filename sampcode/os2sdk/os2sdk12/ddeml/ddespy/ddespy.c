/***************************** Module Header ******************************\
* Module Name: ddespy
*
* This is a small DDE ddespyr which lets me see whats going on.
*
* Created:      sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "ddespy.h"

/*********** declares *********/

HWND hwndExec = NULL;

SWP gswp[IW_LAST - IW_FIRST + 1];
PSZ apszTitles[] = {
        "Applications", "Topics", "System Items", "Data"
                   };


PSZ apszType[] = {
                        "0-Type"        ,
                        "ACK"           ,
                        "ADVDATA"       ,   
                        "ADVREQ"        ,    
                        "ADVSTART"      ,  
                        "ADVSTOP"       ,   
                        "EXEC"          ,      
                        "INIT"          ,
                        "INITCONF"      ,
                        "MONITOR"       ,
                        "PKT"           ,
                        "POKE"          ,
                        "REGISTER"      ,  
                        "REQUEST"       ,
                        "RTNPKT"        ,
                        "TERM"          ,      
                        "UNREGISTER"    ,
                        "WILDINIT"      ,
                        "XFERCOMP"      ,
                        "19"            ,
                        "20"            ,
                        "21"            ,
                        "22"            ,
                        "23"            ,
                        "24"            ,
                        "25"            ,
                        "26"            ,
                        "27"            ,
                        "28"            ,
                        "29"            ,
                        "30"            ,
                        "31"            ,
               };

PSZ apszState[] = {         /* corresponds to convinfo.usStatus */
                        "DISCONNECTED"      ,
                        "CONNECTED"         ,
                        "NOADVISE"          ,
                        "ADVISE"            ,
                  };

PSZ apszStatus[] = {        /* corresponds to convionfi.usConvst */
                        "NOACTIVITY"    ,     
                        "INCOMPLETE"    ,    
                        "TERMINATED"    ,    
                        "CONNECTED"     ,
                        "INITIATING"    ,         
                        "REQSENT"       ,       
                        "DATARCVD"      ,      
                        "POKESENT"      ,      
                        "POKEACKRCVD"   ,   
                        "EXECSENT"      ,      
                        "EXECACKRCVD"   ,   
                        "ADVSENT"       ,       
                        "UNADVSENT"     ,     
                        "ADVACKRCVD"    ,    
                        "UNADVACKRCVD"  ,  
                        "ADVDATASENT"   ,   
                        "ADVDATAACKRCVD",
                   };


#define GetLBCount(hw) (SHORT)WinSendMsg((hw), LM_QUERYITEMCOUNT, 0L, 0L)
#define GetLBSelectedItem(hw) \
            (SHORT)WinSendMsg((hw), LM_QUERYSELECTION, (MPARAM)LIT_FIRST, 0L)
#define GetLBItemHandle(hw, i) \
            (ULONG)WinSendMsg((hw), LM_QUERYITEMHANDLE, (MPARAM)(i), 0L)
#define GetLBItemText(hw, i, cch, psz) \
            (ULONG)WinSendMsg((hw), LM_QUERYITEMTEXT, MPFROM2SHORT((i),(cch)), (MPARAM)(psz))
#define DeleteLBItem(hw, i) WinSendMsg(hw, LM_DELETEITEM, MPFROMSHORT(i), 0L)
#define NotifyOwner(hwnd, msg, mp1, mp2) \
            (WinSendMsg(WinQueryWindow(hwnd, QW_OWNER, FALSE), msg, mp1, mp2))


void cdecl main(int argc, char **argv);
void ResizeChildren(USHORT cxNew, USHORT cyNew);
HDMGDATA EXPENTRY dataxfer(HCONV hConv, HSZ hszTopic, HSZ hszItem,
        USHORT usFmt, USHORT usType, HDMGDATA hDmgData);
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY GetTimeoutDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY EnhancedEFWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ExecDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
void Refresh(void);
SHORT InsertLBItem(HWND hwndLB, PSZ psz, ULONG ulHandle);
void SetLBEntries(HCONV hConv, HSZ hszItem, USHORT iwlb);
void UpdateQueue(HWND hwndCB, HCONV hConv);
void ConvInfoToString(PCONVINFO pci, PSZ psz, USHORT cbMax);
PSZ mylstrcat(PSZ psz1, PSZ psz2, PSZ pszLast);
int lstrlen(PSZ psz);

/************* GLOBAL VARIABLES  ************/

HAB hab;
HMQ hmq;
HHEAP hheap;
HWND hwndFrame;
HWND hwndClient;
USHORT cyTitles;
USHORT cxBorder;
HCONVLIST hConvListMain;
HSZ hszSysTopic;
HSZ hszSysItemTopics;
HSZ hszSysItemSysItems;
ULONG gTimeout = 1000L;
PFNWP lpfnSysEFWndProc;

void cdecl main(argc, argv)
int argc;
char **argv;
{
    USHORT err;
    QMSG qmsg;

    argc; argv;
    
    hab = WinInitialize(0);
    hmq = WinCreateMsgQueue(hab, 0);
    hheap = WinCreateHeap(0, 0, 0, 0, 0, HM_MOVEABLE);

    if (!WinRegisterClass(hab, "DDESpyr Class", ClientWndProc, CS_SIZEREDRAW, 0))
        goto abort;

    cyTitles = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
    cxBorder = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE, NULL,
            (PSZ)"DDESpyr Class",
            (PSZ)"", WS_VISIBLE, (HMODULE)NULL, IDR_MAIN,
            &hwndClient);

    WinSetWindowText(hwndFrame, "DDE Spy");
    if (err = DdeInitialize((PFNCALLBACK)dataxfer, 0L, 0L)) {
        DdePostError(err);
        goto abort;
    }

    hszSysTopic = DdeGetHsz((PSZ)SZDDESYS_TOPIC, 0, 0);
    hszSysItemTopics = DdeGetHsz((PSZ)SZDDESYS_ITEM_TOPICS, 0, 0);
    hszSysItemSysItems = DdeGetHsz((PSZ)SZDDESYS_ITEM_SYSITEMS, 0, 0);

    Refresh();

    while (WinGetMsg(hab, &qmsg, 0, 0, 0)) {
        WinDispatchMsg(hab, &qmsg);
    }

    DdeUninitialize();
abort:
    if (hwndFrame)
        WinDestroyWindow(hwndFrame);
    if (hheap) {
        WinDestroyHeap(hheap);
    }
    WinTerminate(hab);
    DosExit(TRUE, 0);
}


MRESULT EXPENTRY ClientWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1, mp2;
{
    USHORT i;

    switch (msg) {
    case WM_CREATE:
        /*
         * initialize globals
         */
        hwndClient = hwnd;
        for (i = IW_APPSLBOX; i <= IW_ITEMSLBOX; i++) {
            gswp[i].hwnd = WinCreateWindow(hwndClient, WC_LISTBOX, "",
                    WS_VISIBLE | LS_NOADJUSTPOS, 0, 0, 0, 0, hwndClient,
                    HWND_TOP, i, NULL, NULL);
        }
        for (i = IW_APPSTITLE; i <= IW_DATATITLE; i++) {
            gswp[i].hwnd = WinCreateWindow(hwndClient, WC_STATIC,
                    apszTitles[i - IW_APPSTITLE],
                    WS_VISIBLE | SS_TEXT | DT_CENTER | DT_BOTTOM,
                    0, 0, 0, 0, hwndClient,
                    HWND_TOP, i, NULL, NULL);
        }
        gswp[IW_DATATEXT].hwnd = WinCreateWindow(hwndClient, WC_STATIC, "",
                WS_VISIBLE | SS_TEXT | DT_WORDBREAK,
                0, 0, 0, 0, hwndClient,
                HWND_TOP, i, NULL, NULL);

        break;

    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case IDM_REFRESH:
            Refresh();
            break;

        case IDM_SETTIMEOUT:
            gTimeout = (USHORT)WinDlgBox(hwndFrame, NULL,
                    (PFNWP)GetTimeoutDlgProc,
                    (HMODULE)NULL, IDD_GETTIMEOUT, NULL);
            break;

        case IDM_EXEC:
            WinDlgBox(HWND_DESKTOP, hwnd, ExecDlgProc, NULL, IDD_EXEC, NULL);
            break;
        }
        break;

    case WM_CONTROL:
        switch (LOUSHORT(mp1)) {
        case IW_APPSLBOX:
            switch (HIUSHORT(mp1)) {
            case LN_SELECT:
                hwnd = gswp[IW_APPSLBOX].hwnd;
                SetLBEntries((HCONV)GetLBItemHandle(hwnd,
                        GetLBSelectedItem(hwnd)),
                        hszSysItemTopics, IW_TOPICSLBOX);
                SetLBEntries((HCONV)GetLBItemHandle(hwnd,
                        GetLBSelectedItem(hwnd)),
                        hszSysItemSysItems, IW_ITEMSLBOX);
                break;
            }
            break;

        case IW_TOPICSLBOX:
            break;

        case IW_ITEMSLBOX:
            break;

        default:
            goto DoDefAction;
        }
        break;

    case WM_SIZE:
        /*
         * resize children
         */
        ResizeChildren(SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
        break;

    case WM_ERASEBACKGROUND:
        return(TRUE);
        break;

    default:
DoDefAction:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
    }
    return(0L);
}



void ResizeChildren(cxNew, cyNew)
USHORT cxNew, cyNew;
{
    USHORT i;

    for (i = IW_FIRST; i <= IW_LAST; i++) {
        gswp[i].fs = SWP_SIZE | SWP_MOVE | SWP_SHOW;
        gswp[i].hwndInsertBehind = HWND_TOP;
    }

    for (i = IW_APPSTITLE; i <= IW_ITEMSTITLE; i++) {
        gswp[i].x =
        gswp[i - IW_APPSTITLE + IW_APPSLBOX].x = i == IW_APPSTITLE ? -cxBorder :
                gswp[i - 1].x + gswp[i - 1].cx - cxBorder;

        gswp[i].y = cyNew - cyTitles;
        gswp[i - IW_APPSTITLE + IW_APPSLBOX].y = cyNew / 2;

        gswp[i].cx =
        gswp[i - IW_APPSTITLE + IW_APPSLBOX].cx = cxNew / 3;

        gswp[i].cy = cyTitles;
        gswp[i - IW_APPSTITLE + IW_APPSLBOX].cy = (cyNew / 2) - cyTitles;
    }

    gswp[IW_ITEMSLBOX].cx = cxNew - gswp[IW_ITEMSLBOX].x + cxBorder;

    gswp[IW_DATATITLE].cy = cyTitles;
    gswp[IW_DATATITLE].y =
    gswp[IW_DATATEXT].cy = cyNew / 2 - cyTitles;

    gswp[IW_DATATITLE].cx =
    gswp[IW_DATATEXT].cx = cxNew;

    WinSetMultWindowPos(hab, gswp, IW_DATATEXT - IW_APPSTITLE + 1);
}


void Refresh()
{
    HCONV hConv;
    register NPSZ npszAppName;
    CONVINFO ci;
    USHORT cb;

    WinLockWindowUpdate(HWND_DESKTOP, gswp[IW_APPSLBOX].hwnd);
    WinSendMsg(gswp[IW_APPSLBOX].hwnd, LM_DELETEALL, 0L, 0L);
    WinSendMsg(gswp[IW_TOPICSLBOX].hwnd, LM_DELETEALL, 0L, 0L);
    WinSendMsg(gswp[IW_ITEMSLBOX].hwnd, LM_DELETEALL, 0L, 0L);
    
    hConvListMain = DdeBeginEnumServers(0L, hszSysTopic, hConvListMain, NULL, NULL);
    if (hConvListMain) {
        hConv = 0;
        while (hConv = DdeGetNextServer(hConvListMain, hConv)) {
            DdeQueryConvInfo(hConv, &ci, QID_SYNC);
            if (ci.hszAppPartner != 0) {
                cb = DdeGetHszString(ci.hszAppPartner, NULL, 0L) + 1;
                npszAppName = WinAllocMem(hheap, cb);
                DdeGetHszString(ci.hszAppPartner, (PSZ)npszAppName, (ULONG)cb);
                InsertLBItem(gswp[IW_APPSLBOX].hwnd, (PSZ)npszAppName,
                        (ULONG)hConv);
                WinFreeMem(hheap, npszAppName, cb);
            } else {
                InsertLBItem(gswp[IW_APPSLBOX].hwnd, SZINDETERMINATE,
                        (ULONG)hConv);
            }
        }
    }
    WinLockWindowUpdate(HWND_DESKTOP, NULL);
}


SHORT InsertLBItem(hwndLB, psz, ulHandle)
HWND hwndLB;
PSZ psz;
ULONG ulHandle;
{
    SHORT ili;

    ili = (SHORT)WinSendMsg(hwndLB, LM_INSERTITEM, (MPARAM)LIT_SORTASCENDING,
            (MPARAM)psz);
    WinSendMsg(hwndLB, LM_SETITEMHANDLE, MPFROMSHORT(ili), (MPARAM)ulHandle);
    return(ili);
}


void SetLBEntries(hConv, hszItem, iwlb)
HCONV hConv;
HSZ hszItem;
USHORT iwlb;
{
    NPSZ npsz, npszT1, npszT2;
    BOOL fDone = 0;
    ULONG cb;
    HDMGDATA hDmgData;

    hDmgData = DdeClientXfer(0L, 0L, hConv, hszItem, DDEFMT_TEXT,
        XTYP_REQUEST, gTimeout, NULL);

    if (hDmgData == 0) {
        DdePostError(DdeGetLastError());
        return;
    }

    cb = DdeGetData(hDmgData, NULL, 0L, 0L);
    /*
     * BUG - may later want to handle the case for cb > 0xFFFF
     */
    npsz = WinAllocMem(hheap, (USHORT)cb);
    if (npsz == NULL) {
        DdePostError(DMGERR_MEMORY_ERROR);
        return;
    }
    if (DdeGetData(hDmgData, (PBYTE)npsz, cb, 0L) == 0) {
        DdePostError(DdeGetLastError());
        goto Exit;
    }
    npszT1 = npszT2 = npsz;
    WinLockWindowUpdate(HWND_DESKTOP, gswp[iwlb].hwnd);
    WinSendMsg(gswp[iwlb].hwnd, LM_DELETEALL, 0L, 0L);
    while (!fDone) {
        while (*npszT2 != '\t' && *npszT2 != '\0')
            npszT2++;
        if (*npszT2 == '\t') {
            *npszT2 = '\0';
            npszT2++;
        } else
            fDone = TRUE;
        InsertLBItem(gswp[iwlb].hwnd, (PSZ)npszT1,
                (ULONG)DdeGetHsz(npszT1, 0, 0));
        npszT1 = npszT2;
    }
    WinLockWindowUpdate(HWND_DESKTOP, NULL);

Exit:
    WinFreeMem(hheap, (NPBYTE)npsz, (USHORT)cb);
    return;
}



HDMGDATA EXPENTRY dataxfer(hConv, hszTopic, hszItem, usFmt, usType,
        hDmgData)
HCONV hConv;
HSZ hszTopic;
HSZ hszItem;
USHORT usFmt;
USHORT usType;
HDMGDATA hDmgData;
{
    hConv; hszTopic; hszItem; usFmt; usType; hDmgData;

    switch (usType) {
    case XTYP_XFERCOMPLETE:
        if (hwndExec) {
            if (DdeCheckQueue(hConv, &hDmgData, (ULONG)hDmgData, 0L)) {
                PSZ psz;
                if (psz = (PSZ)DdeAccessData(hDmgData)) {
                    WinSetDlgItemText(hwndExec, IDEF_DATA, psz);
                    /*
                     * Free this data here.
                     */
                    DdeFreeData(hDmgData);
                }
            }
        }
        break;
    }
    return(0);
}


MRESULT EXPENTRY GetTimeoutDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    USHORT usValue;

    switch (msg) {
    case WM_INITDLG: 
        /*
         * set up our entryfield to be enhanced.
         */
        WinSetDlgItemShort(hwnd, IDC_EF, (USHORT)gTimeout, FALSE);
        lpfnSysEFWndProc = WinSubclassWindow(WinWindowFromID(hwnd, IDC_EF),
                EnhancedEFWndProc);
        break;

    case ENHAN_ENTER:
        /*
         * when the user hits the enter key, it will be passed from the
         * entryfield to here and we will use it as a signal to exit.
         */
        WinQueryDlgItemShort(hwnd, IDC_EF, &usValue, FALSE);
        WinDismissDlg(hwnd, usValue);
        break;

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}

MRESULT EXPENTRY EnhancedEFWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    switch (msg) {
    case WM_CHAR:
        if (LOUSHORT(mp1) & KC_SCANCODE &&
                LOUSHORT(mp1) & KC_KEYUP &&
                /*---HACK ALERT!---*/
                LOBYTE(LOUSHORT(mp2)) == 0x0d) {
            NotifyOwner(hwnd, ENHAN_ENTER,
                    (MPARAM)WinQueryWindowUShort(hwnd, QWS_ID), 0L);
        }
        break;
    }
    return(lpfnSysEFWndProc(hwnd, msg, mp1, mp2));
}



MRESULT EXPENTRY ExecDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    static UCHAR szT[255];
    static HCONV hConv = NULL;
    register USHORT i;
    USHORT cb;
    register USHORT xtyp;
    HDMGDATA hDmgData;
    HSZ hszApp, hszItem, hszTopic;
    BOOL fAssync;
    ULONG xid;

    switch (msg) {
    case WM_INITDLG:
        hwndExec = hwnd;
        WinSendDlgItemMsg(hwnd, IDEF_DATA, EM_SETTEXTLIMIT, (MPARAM)MAX_QSTRING, 0L);
        WinSendDlgItemMsg(hwnd, IDCB_QDATA, EM_SETTEXTLIMIT, (MPARAM)MAX_QSTRING, 0L);
        if ((i = GetLBSelectedItem(gswp[IW_APPSLBOX].hwnd)) != LIT_NONE) {
            GetLBItemText(gswp[IW_APPSLBOX].hwnd, i, 255, szT);
            WinSetDlgItemText(hwnd, IDEF_APP, szT);
        }
        if ((i = GetLBSelectedItem(gswp[IW_TOPICSLBOX].hwnd)) != LIT_NONE) {
            GetLBItemText(gswp[IW_TOPICSLBOX].hwnd, i, 255, szT);
            WinSetDlgItemText(hwnd, IDEF_TOPIC, szT);
        }
        if ((i = GetLBSelectedItem(gswp[IW_ITEMSLBOX].hwnd)) != LIT_NONE) {
            GetLBItemText(gswp[IW_ITEMSLBOX].hwnd, i, 255, szT);
            WinSetDlgItemText(hwnd, IDEF_ITEM, szT);
        }
        WinSendDlgItemMsg(hwnd, IDRB_REQUEST, BM_CLICK, 0L, 0L);
        break;

    case WM_CONTROL:
    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case MBID_CANCEL:
            if (hConv != NULL)
                DdeDisconnect(hConv);
            hConv = NULL;
            WinDismissDlg(hwnd, 0);
            break;
            
        case IDC_QFLUSH:
            DdeCheckQueue(hConv, NULL, QID_NEWEST, CQ_FLUSH);
            WinSetDlgItemText(hwnd, IDCB_QDATA, "");
            UpdateQueue(WinWindowFromID(hwnd, IDCB_QDATA), hConv);
            break;
            
        case IDC_QUPDATE:
            UpdateQueue(WinWindowFromID(hwnd, IDCB_QDATA), hConv);
            WinSendDlgItemMsg(hwnd, IDCB_QDATA, CBM_SHOWLIST, (MPARAM)TRUE, 0L);
            break;
            
        case IDC_DOIT:
            i = (USHORT)WinSendDlgItemMsg(hwnd, IDRB_ADVSTART,
                    BM_QUERYCHECKINDEX, 0L, 0L) + IDRB_ADVSTART;
            WinQueryDlgItemText(hwnd, IDEF_APP, 255, szT);
            hszApp = DdeGetHsz(szT, 0, 0);
            WinQueryDlgItemText(hwnd, IDEF_TOPIC, 255, szT);
            hszTopic = DdeGetHsz(szT, 0, 0);
            WinQueryDlgItemText(hwnd, IDEF_ITEM, 255, szT);
            hszItem = DdeGetHsz(szT, 0, 0);
            if (i != IDRB_REQUEST) {
                WinQueryDlgItemText(hwnd, IDEF_DATA, 255, szT);
                cb = WinQueryDlgItemTextLength(hwnd, IDEF_DATA);
            }
            if (hConv == NULL && !(hConv = DdeConnect(hszApp, hszTopic, NULL, 0L))) {
                DdePostError(DdeGetLastError());
                if (LOUSHORT(mp1) == MBID_OK)
                    WinDismissDlg(hwnd, 0);
                return(0);
            }
            switch (i) {
            case IDRB_REQUEST:
                xtyp = XTYP_REQUEST;
                goto XferOut;
                break;
            case IDRB_ADVSTART:
                xtyp = XTYP_ADVSTART;
                goto XferOut;
                break;
            case IDRB_ADVSTOP:
                xtyp = XTYP_ADVSTOP;
                goto XferOut;
                break;
            case IDRB_POKE:
                xtyp = XTYP_POKE;
                goto XferOut;
                break;
            case IDRB_EXECUTE:
                xtyp = XTYP_EXEC;
XferOut:
                fAssync = (BOOL)WinSendDlgItemMsg(hwnd, IDCBX_ASSYNC,
                        BM_QUERYCHECK, 0L, 0L);
                if (!(hDmgData = DdeClientXfer((PBYTE)szT, (ULONG)cb + 1,
                        hConv, hszItem, DDEFMT_TEXT, xtyp,
                        fAssync ? TIMEOUT_ASYNC : gTimeout, &xid)))
                    DdePostError(DdeGetLastError());
                    
                if (fAssync) {
                    UpdateQueue(WinWindowFromID(hwnd, IDCB_QDATA), hConv);
                } else {
                    if (i == IDRB_REQUEST) {
                        DdeGetData(hDmgData, szT, 255L, 0L);
                        DdeFreeData(hDmgData);
                        WinSetWindowText(gswp[IW_DATATEXT].hwnd, szT);
                        WinSetDlgItemText(hwnd, IDEF_DATA, szT);
                    }
                }
                
            }
            break;
        }
        break;

    case WM_DESTROY:
        hwndExec = NULL;
    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}



void UpdateQueue(hwndCB, hConv)
HWND hwndCB;
HCONV hConv;
{
    SHORT lit;
    SHORT litSel = LIT_FIRST;
    CONVINFO ci;
    ULONG id, idSel;
    USHORT cItems;
    char szT[MAX_QSTRING];
    
    lit = (SHORT)WinSendMsg(hwndCB, LM_QUERYSELECTION,
            MPFROMSHORT(LIT_FIRST), 0L);
    idSel = (SHORT)WinSendMsg(hwndCB, LM_QUERYITEMHANDLE, MPFROMSHORT(lit), 0L);
    WinSendMsg(hwndCB, LM_DELETEALL, 0L, 0L);
    cItems = (USHORT)DdeCheckQueue(hConv, NULL, QID_NEWEST, CQ_COUNT);
    id = DdeCheckQueue(hConv, NULL, QID_NEWEST, 0L);
    while (cItems--) {
        DdeQueryConvInfo(hConv, &ci, id);
        ConvInfoToString(&ci, szT, MAX_QSTRING);
        lit = (SHORT)WinSendMsg(hwndCB, LM_INSERTITEM, MPFROMSHORT(LIT_END), szT);
        WinSendMsg(hwndCB, LM_SETITEMHANDLE, MPFROMSHORT(lit), (MPARAM)id);
        if (id == idSel) 
            litSel = lit;
        id = DdeCheckQueue(hConv, NULL, id, CQ_NEXT);
    }
    WinSendMsg(hwndCB, LM_SELECTITEM, MPFROMSHORT(litSel), (MPARAM)TRUE);
}



void ConvInfoToString(pci, psz, cbMax)
PCONVINFO pci;
PSZ psz;
USHORT cbMax;
{
    PSZ pszLast;
    char szT[100];
    
    pszLast = psz + cbMax - 1;
    *psz = '\0';
    psz = mylstrcat(psz, apszType[(pci->usType >> 4) && 0x1F], pszLast);
    psz = mylstrcat(psz, ": ", pszLast);
    psz = mylstrcat(psz, apszState[pci->usStatus & ST_CONNECTED ? 1 : 0], pszLast);
    psz = mylstrcat(psz, " ", pszLast);
    psz = mylstrcat(psz, apszState[pci->usStatus & ST_ADVISE ? 3 : 2], pszLast);
    psz = mylstrcat(psz, " ", pszLast);
    psz = mylstrcat(psz, apszStatus[pci->usConvst], pszLast);
    psz = mylstrcat(psz, " ", pszLast);
    if (pci->usFmt == DDEFMT_TEXT) {
        psz = mylstrcat(psz, "TEXT", pszLast);
    } else {   
        DdeGetHszString(pci->hszItem, szT, 100L);
        psz = mylstrcat(psz, szT, pszLast);
    }
    psz = mylstrcat(psz, " ", pszLast);
    DdeGetHszString(pci->hszItem, szT, 100L);
    psz = mylstrcat(psz, szT, pszLast);
    
    if (pci->LastError) {
        psz = mylstrcat(psz, " ", pszLast);
        DdeGetErrorString(pci->LastError, pszLast - psz, psz);
    }
    pszLast = '\0';
}


/***************************** Public  Function ****************************\
* Concatonates psz1 and psz2 into psz1.
* returns psz pointing to end of concatonated string.
* pszLast marks point at which copying must stop.  This makes this operation
* safe for limited buffer sizes.
*
* History:  1/1/89  created sanfords
\***************************************************************************/
PSZ mylstrcat(psz1, psz2, pszLast)
PSZ psz1, psz2, pszLast;
{
    psz1 += lstrlen(psz1);
    while (*psz2 != '\0' && psz1 < pszLast) {
        *psz1++ = *psz2++;
    }
    *psz1 = '\0';
    return(psz1);
}


/***************************** Private Function ****************************\
*
* returns string length not counting null terminator.
*
* History:  1/1/89  created     sanfords
\***************************************************************************/
int lstrlen(psz)
PSZ psz;
{
    int c = 0;

    while (*psz != 0) {
        psz++;
        c++;
    }
    return(c);
}

