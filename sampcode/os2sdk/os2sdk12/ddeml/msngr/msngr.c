/****************************** MODULE Header ******************************\
* Module Name:  msngr.c - Messenger application
*
* Created:  1/1/89  sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#include "msngr.h"
#include "string.h"

/************* FILE GLOBALS  ************/

char szMessengerClass[] = SZAPPCLASS;
char szTitlePrefix[] = SZAPPTITLEPREFIX;
char szTitle[MAX_TITLESTR + 1]="";
char szEmailName[MAX_NAMESTR + 1] = "";

HCONVLIST hConvUserList = 0;    /* holds all client conversations */
HAB  hab;
HMQ  hmq;
HWND hwndMsngrFrame;
HWND hwndMsngr;
HWND hwndHolder;
HWND hwndLB;
HHEAP hheap;
HPOINTER hptrLink = 0;
PFNWP lpfnSysEFWndProc;         /* holds the system edit control proc */
NPUSERLIST gnpUL = 0;   /* relates hConvs to hszs. */
SHORT cyText;
ULONG ulTimeout = 500;

/*
 * The following arrays define which interface functions to use for a given
 * topic and item.
 * Hszize() fills in the hsz portions of this array.
 * Note: the order of some of these must corespond to the the index constants
 *        in msngr.h.
 */

HSZ hszEmailName = 0;
HSZ hszAppName = 0;

/*                               HSZ    PROCEDURE       PSZ        */

ITEMLIST sysTopicItemList[] = {
                                { 0, TopicListXfer,  SZDDESYS_ITEM_TOPICS   },
                                { 0, ItemListXfer,   SZDDESYS_ITEM_SYSITEMS },
                                { 0, NotSupported,   SZDDESYS_ITEM_RTNMSG   },
                                { 0, NotSupported,   SZDDESYS_ITEM_STATUS   },
                                { 0, sysFormatsXfer, SZDDESYS_ITEM_FORMATS  },
                              };

ITEMLIST msgTopicItemList[] = {
                                { 0, msgUserNameXfer, SZUSERNAME},
                                { 0, msgMessageXfer, SZMESSAGEDATA},
                                { 0, bmpXfer, SZBMAPDATA}, 
                              };


/*                           HSZ   PROCEDURE       #ofITEMS     PSZ     */
   
TOPICLIST topicList[] = {
    /* The system topic is always assumed to be first. */
                            { 0, sysTopicItemList, IIL_SYSLAST, SZDDESYS_TOPIC},
                            { 0, msgTopicItemList, IIL_MSGLAST, SZDDEMSGTOPIC},
                        };


/***************************** Private Function ****************************\
* DESCRIPTION:
*   Main dude.
* History:      1/1/89  Created         sanfords
\***************************************************************************/
void cdecl main(argc, argv)
int argc;
char *argv[];
{
    USHORT err;
    HPS hps;
    QMSG qmsg;
    ULONG createFlags;

    /*
     * check command line args
     *  User can specify a email name up to MAX_NAMESTR
     *  ! turns on a debug break
     */
    if (argc > 1) {
        if (strlen(argv[1]) > MAX_NAMESTR)
            NotifyUser(SZBADCOMPARAMS);
        else if (*argv[1] != '!')
            strcpy(szEmailName, argv[1]);
    }
    
    hab = WinInitialize(0);
    hheap = WinCreateHeap(0, 0, 0, 0, 0, HM_MOVEABLE | HM_VALIDSIZE);
    hmq = WinCreateMsgQueue(hab, 100);  /* Note the larger than normal Q ! */

    WinRegisterClass(hab, SZHOLDINGCLASS, WinDefWindowProc, 0L, 0L);
    WinRegisterClass(hab, SZAPPCLASS, MainWndProc, CS_SIZEREDRAW, 4L);
    
    /*
     * set up cyText for later
     */
    {
        FONTMETRICS fm;

        hps = WinGetPS(HWND_DESKTOP);
        GpiQueryFontMetrics(hps, (LONG)sizeof(FONTMETRICS), &fm);
        WinReleasePS(hps);
        cyText = (SHORT)fm.lMaxBaselineExt;
    }

    InitBmapModule();

    /*
     * force the user to set up his email name.
     */
    if (szEmailName[0] == '\0')
        WinDlgBox(HWND_DESKTOP, NULL, (PFNWP)GetNameDlgProc, (HMODULE)NULL,
                IDD_GETNAME, (PVOID)szEmailName);
                
    /*
     * Initialize the DDE manager
     */
    if ((err = DdeInitialize((PFNCALLBACK)Callback, 0L, 0L)) != NULL) {
        DdePostError(err);
        goto abort;
    }

    /*
     * Create all HSZs we expect to need
     */
    Hszize();

    /*
     * Create our main window
     */
    createFlags = FCF_STANDARD;
    hwndMsngrFrame = WinCreateStdWindow(HWND_DESKTOP, 0L, NULL,
        SZAPPCLASS, "", 0L, (HMODULE)NULL, IDR_MSNGR1, &hwndMsngr);
        
    WinSetWindowPos(hwndMsngrFrame, HWND_TOP, 0, 0,
        250, 150, SWP_SHOW | SWP_ZORDER | SWP_SIZE);

    /*
     * set up the initial title string.
     */
    SetEmailName(szEmailName);

    /*
     * process the main window till dismissed.
     */
    while (WinGetMsg(hab, &qmsg, NULL, 0, 0))
        WinDispatchMsg(hab, &qmsg);

abort:
    if (hptrLink != 0)
        WinDestroyPointer(hptrLink);

    WinDestroyWindow(hwndMsngrFrame);
    CloseBmapModule();
    DdeAppNameServer(hszAppName, ANS_UNREGISTER | ANS_FILTERON);
    UnHszize();
    DdeUninitialize();
    WinTerminate(hab);
    DosExit(EXIT_PROCESS, 0);
}




/********** Msngr Window Procedure **************/

MRESULT EXPENTRY MainWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    char szCaption[MAX_CAPTIONSTR + 1];
    NPUSERLIST pUserItem;

    switch (msg) {
    case WM_CREATE:
        /*
         * initialize globals
         */
        hwndMsngr = hwnd;
        hwndLB = WinCreateWindow(hwndMsngr, WC_LISTBOX, "",
                WS_VISIBLE | LS_NOADJUSTPOS, 0, 0, 0, 0, hwndMsngr,
                HWND_TOP, WID_LISTBOX, NULL, NULL);

        /*
         * create a holding window to be the owner of all the floating
         * dialogs which need to be destroyed on exit.
         */
        hwndHolder = WinCreateWindow(HWND_OBJECT, SZHOLDINGCLASS, "", 0L,
                0, -1, 0, 0, (HWND)NULL, HWND_TOP, 0, (PVOID)NULL, (PVOID)NULL);
        /*
         * we register our name here so keeping track of who is out there
         * is simplified.  The DLL will now let us know when other
         * instances of this app come and go.
         *
         * As the DLL lets other msngr apps know we are here, they
         * will connect with us, ask us our name, and poke to us their
         * names.  Each poke will cause their name to be added to our
         * user list.
         *
         * If our requested name happens to be the same as one of theirs,
         * they will ignore our registration and assume we will change
         * our name.
         *
         * If we find that a poke from them is the same as our name, we
         * know that we are a duplicate.  We will then alter the email
         * name to be unique and reregister ourselves with the AppNameServer.
         * This will restart the registration notification process as
         * described here.
         */    
        DdeAppNameServer(hszAppName, ANS_REGISTER | ANS_FILTERON);
        break;

    case WM_SIZE:
        WinSetWindowPos(hwndLB, HWND_TOP, -1, -1, SHORT1FROMMP(mp2) + 2,
                SHORT2FROMMP(mp2) + 2, SWP_SIZE | SWP_MOVE | SWP_SHOW);
        break;

    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case IDM_DOSENDTEXT:
            if (!(pUserItem = AccessSelection()))
                break;
            WinDlgBox(HWND_DESKTOP, hwndMsngr, SendTextDlgProc, 0L,
                    IDD_SENDTEXT, (PVOID)pUserItem);
            break;

        case IDM_DOSENDBITMAP:
            if (!(pUserItem = AccessSelection()))
                break;
            WinDlgBox(HWND_DESKTOP, hwndMsngr, SendBitmapDlgProc,
                    0L, IDD_SENDBITMAP, (PVOID)pUserItem);
            break;

        case IDM_DOSENDSONG:
            if (!(pUserItem = AccessSelection()))
                break;
            WinDlgBox(HWND_DESKTOP, hwndMsngr, SendSongDlgProc, 0L,
                    IDD_SENDSONG, (PVOID)pUserItem);
            break;

        case IDM_INITLINK:
            if (!(pUserItem = AccessSelection()))
                break;
            if (!WinIsWindow(hab, pUserItem->hwndLink)) {
                CreateLinkWindow(pUserItem, LNKST_USERSTART);
            } else {
                WinSetWindowPos(pUserItem->hwndLink, HWND_TOP,
                        0, 0, 0, 0,
                        SWP_SHOW | SWP_RESTORE | SWP_FOCUSACTIVATE);
            }
            break;
        }
        break;

    /*
     * we use this message so we can do asynchronous incomming message
     * notification.  This prevents us from eternal loops via the
     * callback function.
     */
    case UM_MSG_NOTIFY:
        lstrcat((PSZ)szCaption, (PSZ)SZMSGIN,
                (PSZ)&(((PSZ)mp1)[lstrlen((PSZ)mp1) + 1]));
        NotifyUser2(szCaption, (PSZ)mp1);
        WinFreeMem(hheap, (NPBYTE)(USHORT)mp1,
                MAX_MSGSTR + 1 + MAX_NAMESTR + 1);
        break;

    case WM_CLOSE:
        /*
         * destroy the holder window, and with it, shutdown any floating
         * dialogs.
         */
        WinDestroyWindow(hwndHolder);
        /* fall through */
    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}




/***************************** Private Function ****************************\
* DESCRIPTION:
* This dialog forces the user to enter a string.  It may be used for
* entering the user's initial email name or it can be used to change
* the current email name.
*
* It must be invoked with a psz specified for its cData which is where its
* result will be stored.  The psz can be no larger than MAX_NAMESTR.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
MRESULT EXPENTRY GetNameDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    PSZ pszName;

    switch (msg) {
    case WM_INITDLG: {
            HWND hwndEF;

            /*
             * store pszName into window words for later access and set up the
             * default name entry.
             * This assumes mp2=pszText for this dialog's entryfield.
             */
            WinSetWindowULong(hwnd, QWL_USER, (ULONG)mp2);
            if (*(PSZ)mp2 != '\0') {
                /*
                 * we are changing a name because of a duplicate, change
                 * the title apropriately.
                 */
                WinSetDlgItemText(hwnd, FID_TITLEBAR, SZDUPTITLE);
            }
            /*
             * set up our entryfield to be enhanced.
             */
            hwndEF = WinWindowFromID(hwnd, IDC_EF_NAME);
            lpfnSysEFWndProc = WinSubclassWindow(hwndEF, EnhancedEFWndProc);

            WinSetWindowText(hwndEF, (PSZ)mp2);
            WinSendMsg(hwndEF, EM_SETTEXTLIMIT, MPFROMSHORT(MAX_NAMESTR), 0L);
            return(0L);
        }
        break;

    case ENHAN_ENTER:
        /*
         * when the user hits the enter key, it will be passed from the
         * entryfield to here and we will use it as a signal to exit.
         */
        pszName = (PSZ)WinQueryWindowULong(hwnd, QWL_USER);
        WinQueryDlgItemText(hwnd, IDC_EF_NAME, MAX_NAMESTR + 1,
                pszName);
        if (*pszName != '\0') {
            WinDismissDlg(hwnd, 0);
        }
        break;

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}




/*****************************************************************************
*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*
*쿟his window proc, which is actually a subclass of the standard entryfield �*
*퀃ype of window, notifies its owner of more interesting things than the    �*
*쿽rigonal did.                                                             �*
*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*
*****************************************************************************/
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
            NOTIFYOWNER(hwnd, ENHAN_ENTER,
                    (MPARAM)WinQueryWindowUShort(hwnd, QWS_ID), 0L);
        }
        break;
    }
    return(lpfnSysEFWndProc(hwnd, msg, mp1, mp2));
}





MRESULT EXPENTRY SendTextDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    switch (msg) {
    case WM_INITDLG:
        /*
         * mp2 should be set to the pUserItem of the person we are sending to.
         */
        WinSendDlgItemMsg(hwnd, IDC_EF_TEXT, EM_SETTEXTLIMIT,
                MPFROMSHORT(MAX_MSGSTR + 1), 0L);
                
        if (((NPUSERLIST)(SHORT)mp2)->hConvMsg == 0) {
            NotifyUser(SZCANTCONNECT);
            WinDismissDlg(hwnd, 0);
            return(0);
        }
        WinSetWindowULong(hwnd, QWL_USER, (ULONG)((NPUSERLIST)(SHORT)mp2)->hConvMsg);
        return(0);
        break;

    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case IDC_SENDTEXT:
            {
                char szMsg[MAX_MSGSTR + 1 + MAX_NAMESTR + 1];

                WinQueryDlgItemText(hwnd, IDC_EF_TEXT, MAX_MSGSTR + 1,
                        (PSZ)szMsg);
                if (szMsg[0] == '\0') {
                    NotifyUser(SZEMPTYMSG);
                    return(0);
                }
                lstrpak((PSZ)szMsg, (PSZ)szMsg, (PSZ)szEmailName);
                if (!((USHORT)DdeClientXfer((PBYTE)szMsg,
                        (ULONG)(lstrlen((PSZ)szMsg) + lstrlen((PSZ)szEmailName) + 2),
                        (HCONV)WinQueryWindowULong(hwnd, QWL_USER),
                        msgTopicItemList[IIL_MSGXFER].hszItem, DDEFMT_TEXT,
                        XTYP_POKE, ulTimeout, NULL) & DDE_FACK)) {
                    NotifyUser(SZSENDFAILED);
                    MyPostError(DdeGetLastError());
                    return(0);
                }
            }
            WinDismissDlg(hwnd, 0);
            break;

        default:
            /*
             * This would handle the DID_OK exit.
             */
            return(WinDefDlgProc(hwnd, msg, mp1, mp2));
            break;
        }
        break;

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}





MRESULT EXPENTRY SendSongDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    switch (msg) {
    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}





/***************************** Public  Function ****************************\
*
* This function is called by the DDE manager DLL and passes control onto
* the apropriate function pointed to by the global topic and item arrays.
* It handles all DDE interaction generated by external events.
*
* History:  1/1/89  created     sanfords
\***************************************************************************/
HDMGDATA EXPENTRY Callback(hConv, hszTopic, hszItem, usFmt, usType,
        hDmgData)
HCONV hConv;
HSZ hszTopic;
HSZ hszItem;
USHORT usFmt;
USHORT usType;
HDMGDATA hDmgData;
{
    SHORT i, j;
    register ITEMLIST *pItemList;
    USHORT iItemLast;
    HDMGDATA hDmgDataRet;

    UNUSED usFmt;
    UNUSED hConv;

    if (usType == XTYP_REGISTER || usType == XTYP_UNREGISTER) {
        if (hszItem == hszAppName) 
            /*
             * we are being notified that another msngr app is starting up
             * or going away.
             * hDmgData should be the App handle to use for initiateing.
             */
            if (usType == XTYP_REGISTER) {
                RegisterUser(hDmgData, NULL, TRUE);
            } else {
                UnregisterUser(hDmgData);
            }
        return(0);
    }


    if (usType == XTYP_WILDINIT) {
        /*
         * He wants a hsz list of all our available app/topic pairs
         * that convorm to hszTopic and hszItem(App).
         */
        HSZ ahsz[(ITL_LAST + 3) * 2];

        if (hszItem != hszAppName && hszItem != NULL) 
            return(0);

        j = 0;
        for (i = 0; i <= ITL_LAST; i++) {
            if (hszTopic == NULL || hszTopic == topicList[i].hszTopic) {
                ahsz[j++] = hszAppName;
                ahsz[j++] = topicList[i].hszTopic;    
            }
        }
        
        if (hszTopic == NULL || hszTopic == hszEmailName) {
            ahsz[j++] = hszAppName;
            ahsz[j++] = hszEmailName;
        }
            
        ahsz[j++] = ahsz[j++] = 0L;
        return(DdePutData((PBYTE)&ahsz[0], (ULONG)sizeof(HSZ) * j, 0L,
                (HSZ)0L, 0, 0));
    }


    /*
     * Check for Topic/Item convention for Linking.  Remember that ADVDATA
     * is client intended so its backwards.
     */
    if ((hszTopic == hszEmailName) ||
            (usType == XTYP_ADVDATA && hszItem == hszEmailName)) {
        return(LinkXfer((PXFERINFO)&hDmgData, hszTopic == hszEmailName ?
                hszItem : hszTopic));
    }

    /*
     * Check our hsz tables and send to the apropriate proc.
     * We use DdeCmpHsz() so hsz comparisons are case insensitive.
     */
    for (i = 0; i <= ITL_LAST; i++) {
        if (DdeCmpHsz(topicList[i].hszTopic, hszTopic) == 0) {
            if (usType == XTYP_INIT) {
                /*
                 * it must be a request to start another server conversation.
                 */
                return(TRUE);
            }
            pItemList = topicList[i].pItemList;
            iItemLast = topicList[i].iItemLast;
            for (j = 0; j <= iItemLast; j++) {
                if (DdeCmpHsz(pItemList[j].hszItem, hszItem) == 0) {
                    hDmgDataRet = (*pItemList[j].npfnCallback)
                            ((PXFERINFO)&hDmgData);
                    /*
                     * The table functions return a boolean or data.
                     * It gets translated here.
                     */
                    switch (usType & XCLASS_MASK) {
                    case XCLASS_DATA:
                        return(hDmgDataRet);
                        break;
                    case XCLASS_FLAGS:
                        return(hDmgDataRet ? DDE_FACK : DDE_NOTPROCESSED);
                        break;
                    case XCLASS_BOOL:
                        return(TRUE);
                    default:
                        return(0);
                        break;
                    }
                    break;
                }
            }
            break;
        }
    }
    return(0);
}





/***************************** Private Function ****************************\
* This passes out a standard tab-delimited list of topic names for this
* application.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
* History:  1/3/89  created     sanfords
\***************************************************************************/
HDMGDATA TopicListXfer(pXferInfo)
PXFERINFO pXferInfo;
{
    USHORT cb, cbAlloc, iTopic, cch;
    PSZ pszTopicList;
    HDMGDATA hDmgData;
    char szT[MAX_NAMESTR + 1];
    SHORT lit;

    if (pXferInfo->usFmt != DDEFMT_TEXT ||
            (pXferInfo->usType != XTYP_REQUEST &&
            pXferInfo->usType != XTYP_ADVREQ))
        return(0);
    /*
     * construct the list of topics we have
     */
    cbAlloc = 0;
    for (iTopic = 0; iTopic <= ITL_LAST; iTopic++)
        cbAlloc += lstrlen(topicList[iTopic].pszTopic) + 1;
    lit = 0;
    while (WinSendMsg(hwndLB, LM_QUERYITEMTEXT,
            MPFROM2SHORT(lit, MAX_NAMESTR + 1), (MPARAM)szT)) {
        cbAlloc += lstrlen(szT) + 1;
        lit++;
    }
    pszTopicList = FarAllocMem(hheap, cbAlloc);
    if (!LOUSHORT(pszTopicList)) 
        return(0);
    cb = 0;
    for (iTopic = 0; iTopic <= ITL_LAST; iTopic++) {
        cch = lstrlen(topicList[iTopic].pszTopic) + 1;
        DdeCopyBlock(topicList[iTopic].pszTopic, pszTopicList + cb, (ULONG)cch);
        cb += cch;
        pszTopicList[cb - 1] = '\t';
    }
    lit = 0;
    while (WinSendMsg(hwndLB, LM_QUERYITEMTEXT,
            MPFROM2SHORT(lit, MAX_NAMESTR + 1), (MPARAM)szT)) {
        cch = lstrlen(szT) + 1;
        DdeCopyBlock(szT, pszTopicList + cb, (ULONG)cch);
        cb += cch;
        pszTopicList[cb - 1] = '\t';
        lit++;
    }
    pszTopicList[cb - 1] = '\0';
    hDmgData = DdePutData(pszTopicList, (ULONG)cb, 0L, pXferInfo->hszItem,
            DDEFMT_TEXT, 0);
    WinFreeMem(hheap, (NPBYTE)(SHORT)pszTopicList, cbAlloc);
    return(hDmgData);
}




/***************************** Private Function ****************************\
* This passes out a standard tab-delimited list of item names for the
* specified topic.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
* History:  1/3/89  created     sanfords
\***************************************************************************/
HDMGDATA ItemListXfer(pXferInfo)
PXFERINFO pXferInfo;
{
    USHORT cb, cbAlloc, iTopic, iItem, iLast, cch;
    ITEMLIST *pItemList = 0;
    PSZ pszItemList;
    HDMGDATA hDmgData;

    if (pXferInfo->usFmt != DDEFMT_TEXT ||
                (pXferInfo->usType != XTYP_REQUEST &&
                pXferInfo->usType != XTYP_ADVREQ))
        return(0);
    /*
     * construct the list of items we support for this topic.
     */
    for (iTopic = 0; iTopic < ITL_LAST; iTopic++) {
        if (pXferInfo->hszTopic == topicList[iTopic].hszTopic) {
            pItemList = topicList[iTopic].pItemList;
            iLast = topicList[iTopic].iItemLast;
            break;
        }
    }
    if (pItemList == 0)
        return(0);
    cbAlloc = 0;
    for (iItem = 0; iItem < iLast; iItem++)
        cbAlloc += lstrlen(pItemList[iItem].pszItem) + 1;
    pszItemList = FarAllocMem(hheap, cbAlloc);
    if (!LOUSHORT(pszItemList))
        return(0);
    cb = 0;
    for (iItem = 0; iItem < iLast; iItem++) {
        cch = lstrlen(pItemList[iItem].pszItem) + 1;
        DdeCopyBlock(pItemList[iItem].pszItem, pszItemList + cb, (ULONG)cch);
        cb += cch;
        pszItemList[cb - 1] = '\t';
    }
    pszItemList[cb - 1] = '\0';
    hDmgData = DdePutData(pszItemList, (ULONG)cb, 0L,
            pXferInfo->hszItem, DDEFMT_TEXT, 0);
    WinFreeMem(hheap, (NPBYTE)(USHORT)pszItemList, cbAlloc);
    return(hDmgData);
}




/***************************** Private Function ****************************\
* Used for unsupported transfers.
*
* History:  1/1/89  created     sanfords
\***************************************************************************/
HDMGDATA NotSupported(pXferInfo)
PXFERINFO pXferInfo;
{
    pXferInfo;
    
    return(0);
}





/***************************** Private Function ****************************\
* Gives out a 0 terminated array of dde format numbers supported by this app.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
HDMGDATA sysFormatsXfer(pXferInfo)
PXFERINFO pXferInfo;
{
    USHORT Formats[2];

    if (pXferInfo->usFmt != DDEFMT_TEXT ||
            (pXferInfo->usType != XTYP_REQUEST &&
            pXferInfo->usType != XTYP_ADVREQ))
        return(0);

    Formats[0] = DDEFMT_TEXT;
    Formats[1] = 0;
    return(DdePutData((PBYTE)Formats, 4L, 0L, pXferInfo->hszItem,
            DDEFMT_TEXT, 0));
}



/***************************** Private Function ****************************\
* Gives out our szEmailName or registers someone elses.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
HDMGDATA msgUserNameXfer(pXferInfo)
PXFERINFO pXferInfo;
{
    CONVINFO ci;
    
    if (pXferInfo->usFmt != DDEFMT_TEXT)
        return(0);

    switch (pXferInfo->usType) {
    case XTYP_REQUEST:
        /*
         * someone wants to know our name...cool.
         */
        return(DdePutData(szEmailName, (ULONG)(lstrlen(szEmailName) + 1),
                0L, pXferInfo->hszItem, DDEFMT_TEXT, 0));
        break;

    case XTYP_POKE:
        /*
         * Someone is responding to our initial registration.  Add them
         * to our list.  If their name is the same as ours, WE are at
         * fault and must change our email name and reregister.
         */
        DdeQueryConvInfo(pXferInfo->hConv, &ci, QID_SYNC);
        RegisterUser(ci.hApp, pXferInfo->hDmgData, FALSE); 
        return(1);
        break;
    }
    return(0);
}





/*
 * This is used for receiving text messages.
 */
HDMGDATA msgMessageXfer(pXferInfo)
PXFERINFO pXferInfo;
{
    PSZ pszMsg;

    if (pXferInfo->usFmt != DDEFMT_TEXT)
        return(DDE_NOTPROCESSED);

    switch (pXferInfo->usType) {
    case XTYP_POKE:
        /*
         * we have the message text followed by the users name.
         */
        pszMsg = FarAllocMem(hheap, MAX_MSGSTR + 1 + MAX_NAMESTR + 1);
        DdeGetData(pXferInfo->hDmgData, (PBYTE)pszMsg,
                (ULONG)(MAX_MSGSTR + 1 + MAX_NAMESTR + 1), 0L);
        DdeFreeData(pXferInfo->hDmgData);
        /*
         * We must do an assynchronous user notification so we don't
         * clog up the DLL in our callback.
         */
        WinPostMsg(hwndMsngr, UM_MSG_NOTIFY, pszMsg, 0L);
        return(1);
        break;

    default:
        break;
    }
    return(0);
}





/***************************** Private Function ****************************\
*  This creates often used global hszs from standard global strings.
*  It also fills the hsz fields of the topic and item tables.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
void Hszize()
{
    register ITEMLIST *pItemList;
    USHORT iTopic, iItem;

    hszAppName = DdeGetHsz((PSZ)SZDDEAPPNAME, 0, 0);

    for (iTopic = 0; iTopic <= ITL_LAST; iTopic++) {
        topicList[iTopic].hszTopic = DdeGetHsz(topicList[iTopic].pszTopic, 0, 0);
        pItemList = topicList[iTopic].pItemList;
        for (iItem = 0; iItem <= topicList[iTopic].iItemLast; iItem++) {
            pItemList[iItem].hszItem = DdeGetHsz(pItemList[iItem].pszItem, 0, 0);
        }
    }
}





/***************************** Private Function ****************************\
*  This destroys often used global hszs from standard global strings.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
void UnHszize()
{
    register ITEMLIST *pItemList;
    USHORT iTopic, iItem;

    DdeFreeHsz(hszAppName);
    DdeFreeHsz(hszEmailName);

    for (iTopic = 0; iTopic <= ITL_LAST; iTopic++) {
        DdeFreeHsz(topicList[iTopic].hszTopic);
        pItemList = topicList[iTopic].pItemList;
        for (iItem = 0; iItem <= topicList[iTopic].iItemLast; iItem++) {
            DdeFreeHsz(pItemList[iItem].hszItem);
        }
    }
}







/***************************** Private Function ****************************\
* DESCRIPTION:
*   This routine returns the userlist item pointed to by the listbox selection.
*   It notifies the user of any errors and returns 0 in that case.
*
* History:      1/17/89     Created         sanfords
\***************************************************************************/
NPUSERLIST AccessSelection()
{
    NPUSERLIST pUserItem;
    SHORT lit;

    lit = (SHORT)WinSendMsg(hwndLB, LM_QUERYSELECTION, 0L, 0L);
    if (lit == LIT_NONE) {
        NotifyUser(SZMAKESELECTION);
        return(0);
    }
    pUserItem = (NPUSERLIST)(SHORT)WinSendMsg(hwndLB, LM_QUERYITEMHANDLE,
            (MPARAM)lit, 0L);
    return(pUserItem);
}



/*
 * if fRegister : (We got the name via a request)
 * This routine creates a connection with hApp on the msgTopic and
 * gets the user's name from the connection.  It then looks for the
 * user's name in our current list.  If the name is already there
 * we fail - he is a fault, otherwise we add the user name to our
 * list of users. We then poke our name to the server we connected
 * with so he knows who we are.
 *
 * else :   (We got the name via a poke)
 * hData contains a valid user name which we will add.
 * If it is a dup of ours, we must change our name and reregister.  The
 * data handle is freed on return.
 */
BOOL RegisterUser(
HAPP hApp,
HDMGDATA hData,
BOOL fRegister)
{
    HCONV hConv;
    HSZ hsz;
    BOOL fDup;

    hConv = DdeConnect(hszAppName, topicList[ITL_MSG].hszTopic, NULL, hApp);
    if (!hConv)  {
        return(FALSE);
    }
        
    if (fRegister) 
        hData = DdeClientXfer(NULL, 0L, hConv,
                msgTopicItemList[IIL_MSGUSERNAME].hszItem,
                DDEFMT_TEXT, XTYP_REQUEST, DEFTIMEOUT, NULL);
        
    if (!hData) 
        return(FALSE);
        
    hsz = DdeGetHsz(DdeAccessData(hData), 0, 0);
    DdeFreeData(hData);
    
    fDup = (hsz == hszEmailName) || FindUser(gnpUL, hsz);
    
    if (fDup && !fRegister) {
        /*
         * OOPS - we are a duplicate name! - Change our name and reregister!!!
         * (we must first stop callbacks so the dialog modal loop
         *  doesn't allow more messy callbacks to come in.)
         */
        DdeEnableCallback(NULL, FALSE);
        while (gnpUL) 
            DestroyUser(gnpUL);
        WinSendMsg(hwndLB, LM_DELETEALL, 0L, 0L);
        WinDlgBox(HWND_DESKTOP, NULL, (PFNWP)GetNameDlgProc, (HMODULE)NULL,
                IDD_GETNAME, (PVOID)szEmailName);
        SetEmailName(szEmailName);
        DdeEnableCallback(NULL, TRUE);
        /*
         * reregister with the name server so others know we changed our name.
         */
        DdeAppNameServer(hszAppName, ANS_UNREGISTER);
        DdeAppNameServer(hszAppName, ANS_REGISTER);
        DdeFreeHsz(hsz);
        DdeFreeData(hData);
        DdeDisconnect(hConv);
        return(FALSE);
    } 
    
    if (!fDup)
        AddUser(hConv, hsz, hApp);
    /*
     * Now poke our name to him (if he is registering), so he gets updated
     * or discovers he's a duplicate.
     */
    if (fRegister) {
        DdeClientXfer(szEmailName, lstrlen(szEmailName) + 1L,
                hConv, msgTopicItemList[IIL_MSGUSERNAME].hszItem,
                DDEFMT_TEXT, XTYP_POKE, DEFTIMEOUT, NULL);
    }
    return(TRUE);
}



/* 
 * We search for the hApp in our user list.  If its not there we fail.
 * Otherwise, we destroy any associated connections or windows and
 * remove the entry from our list.
 *
 * The case of duplicate name re-registration is covered here since
 * we ignore duplicate registrations and so we will not find hApp
 * if a duplicate name is being unregistered.
 */
BOOL UnregisterUser(
HAPP hApp)
{
    NPUSERLIST npUser;
    SHORT lit;

    if (hApp == 0) {
        return(FALSE);
    }
    npUser = gnpUL;
    while (npUser) {
        if (hApp == npUser->hApp)
            break;
        npUser = npUser->next;
    }
    
    if (!npUser) {
        return(FALSE);
    }
    
    /*
     * Remove item from listbox.
     */        
    if (lit = (SHORT)WinSendMsg(hwndLB, LM_QUERYITEMCOUNT, 0L, 0L)) {
        while (lit--) {
            if (npUser == (NPUSERLIST)(SHORT)WinSendMsg(hwndLB,
                    LM_QUERYITEMHANDLE, MPFROMSHORT(lit), 0L)) {
                WinSendMsg(hwndLB, LM_DELETEITEM, MPFROMSHORT(lit), 0L);   
                break;
            }
        }
    }
    DestroyUser(npUser);
}



VOID SetEmailName(psz)
PSZ psz;
{
    if (hszEmailName) 
        DdeFreeHsz(hszEmailName);
    hszEmailName = DdeGetHsz(psz, 0, 0);
    lstrcpy(szEmailName, psz);
    lstrcat(szTitle, szTitlePrefix, szEmailName);
    WinSetWindowText(hwndMsngrFrame, szTitle);
}




void NotifyUser(psz)
PSZ psz;
{
    WinMessageBox(HWND_DESKTOP, hwndMsngr, psz, SZNOTIFYCAPTION,
            WID_NOTIFY1,
            MB_OK | MB_DEFBUTTON1 | MB_ICONASTERISK | MB_MOVEABLE);
}




void NotifyUser2(pszCaption, pszText)
PSZ pszCaption;
PSZ pszText;
{
    WinMessageBox(HWND_DESKTOP, hwndMsngr, pszText, pszCaption, WID_NOTIFY2,
            MB_OK | MB_DEFBUTTON1 | MB_ICONASTERISK | MB_MOVEABLE);
}


