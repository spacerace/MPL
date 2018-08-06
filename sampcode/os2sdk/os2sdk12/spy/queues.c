/***************************************************************************\
* queues.c - Spy code to manager the queue list dialog
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define	INCL_WINDIALOGS
#define	INCL_WINHEAP
#define	INCL_WININPUT
#define	INCL_WINLISTBOXES
#define	INCL_WINMESSAGEMGR
#define	INCL_WINPOINTERS		/* needed for spy.h */
#define	INCL_WINWINDOWMGR
#include <os2.h>
#include "spyhook.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "spy.h"

#define SIZEOFWND   34
#define MAXMSGBYTES 100

/* File Global Variables */
HMQ      hmqDlgDisp;
SHORT   chmqWatched = 0;
SHORT   chmqInList = 0;



/* Function Prototypes */
MRESULT EXPENTRY SpyQueuesDlgProc(HWND, USHORT, MPARAM, MPARAM);

void    InitQueueList(VOID);
void    DisplayQueueInfoForWindow(HWND, HWND);
void    DisplayQueueInfo(HWND, HMQ);
void    SelectQueueFromText(HWND hwndDlg);
void    BuildQueueWatchList(void);



/***************************************************************************\
* MRESULT EXPENTRY SpyQueuesDlgProc(hwnd, msg, mp1, mp2)
*
* The Spy Windows Dialog procedure
\***************************************************************************/
MRESULT EXPENTRY SpyQueuesDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL        fSelect = TRUE;
    SHORT       cQueues;
    HWND        hwndPoint;
    HMQ         hmqDisp;
    HMQ         hmqItem;   /* from handle of list item */
    USHORT      iItemFocus; /* Index to item that has the focus */

    switch (msg) {

    case WM_INITDLG:
        /* Initialize the dialog items */
        hwndWindowLB = WinWindowFromID(hwnd, DID_WINDOWLIST);
        InitQueueList();
        hmqDlgDisp = NULL;
        fTrackingListBox = TRUE;
        break;

    case WM_CHAR:
        /*
         * Handle VK_ENTER and VK_NEWLINE if our Edit control has
         * the focus and it is a keydown
         */
        if (!(SHORT1FROMMP(mp1) & KC_KEYUP) &&
                (SHORT1FROMMP(mp1) & KC_VIRTUALKEY) &&
                ( (SHORT2FROMMP(mp2) == VK_ENTER) ||
                  (SHORT2FROMMP(mp2) == VK_NEWLINE) )) {


            if (WinQueryFocus(HWND_DESKTOP, FALSE) ==
                    WinWindowFromID(hwnd, DID_WHANDLE)) {
                SelectQueueFromText(hwnd);
                break;
            }
        }

        /* Normaly pass to dialog procedure to handle message */
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:
            BuildQueueWatchList();

        case DID_CANCEL:
            /* Now dismiss the dialog */
            WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            break;
        case DID_WUNSELALL:
            fSelect = FALSE;
        case DID_WSELALL:
            cQueues = SHORT1FROMMR( WinSendMsg(hwndWindowLB, LM_QUERYITEMCOUNT,
                0L, 0L));

            fTrackingListBox = FALSE;
            while (cQueues) {
                /* Loop through all windows, selecting or unselcting all */
                WinSendMsg(hwndWindowLB, LM_SELECTITEM, (MPARAM)--cQueues,
                    (MPARAM)fSelect);
            }
            fTrackingListBox = TRUE;
            break;

        case DID_WSELMOUSE:
            /* Call function to track mouse, returns window handle */
            hwndPoint = HwndSelWinWithMouse(hwnd, DisplayQueueInfoForWindow);
            if (hwndPoint == NULL)
                break;    /* No window to process */

            /*
             * Now find the window in the list, Make the item visible
             * and set the item as selected.
             */
            hmqDisp = (HMQ)WinQueryWindowULong(hwndPoint, QWL_HMQ);
            cQueues = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYITEMCOUNT,
                    0L, 0L));

            while (cQueues) {
                /*
                 * Loop through all windows until we wind the right
                 * one with the correct window handle
                 */
                hmqItem = (HMQ)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                        (MPARAM)--cQueues, 0L);

                if (hmqItem == hmqDisp) {
                    /* found the right item, move it to top */
                    WinSendMsg(hwndWindowLB, LM_SETTOPINDEX, (MPARAM)cQueues, 0L);
                    WinSendMsg(hwndWindowLB, LM_SELECTITEM, (MPARAM)cQueues,
                            (MPARAM)TRUE);
                    break;
                }
            }
            break;
        }
        break;


    default:
        /*
         * Default is to see if the listbox has changed its focus
         * item number.  If it has, then we want to display the information
         * about the window that the listbox cursor is over.  There is no
         * legal way to do this, One approach appears to temporary set the
         * listbox to be a single selection listbox, then query its selection
         * and set it back into multiple selection mode.
         */
        if (fTrackingListBox && hwndWindowLB != NULL) {

            WinSetWindowBits(hwndWindowLB, QWL_STYLE, 0L, LS_MULTIPLESEL);
            iItemFocus = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYSELECTION,
                    (MPARAM)LIT_FIRST, 0L));
            WinSetWindowBits(hwndWindowLB, QWL_STYLE, LS_MULTIPLESEL,
                    LS_MULTIPLESEL);

            if (iItemFocus != iCurItemFocus) {
                iCurItemFocus = iItemFocus;
                if (iItemFocus != (USHORT)-1) {

                    hmqDisp = (HMQ)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                            (MPARAM)iItemFocus, 0L);
                    DisplayQueueInfo(hwnd, hmqDisp);
                }
            }
        }
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
    }
    return 0L;
}




/****************************************************************************\
* InitQueueList ()
*
* Builds the list of Queues displayed in the queues dialog
\***************************************************************************/
void InitQueueList()
{
    HENUM       henum;
    char        szClassName[50];    /* Class name of window */
    HWND        hwnd;
    ULONG       ulHMQ;
    PID         pidWindow;
    TID         tidWindow;
    SHORT       item;
    HMQ         *ahmqWatched;
    HMQ         *phmqT;
    SHORT       chmqAlloc;
    SHORT       i;

    /*
     * We will enumerate through all of the OBJECT windows looking for
     * windows that have the  WC_QUEUE type.
     */
    if (chmqWatched > 0) {
        ahmqWatched = (HMQ *)WinAllocMem(hHeap, sizeof(HMQ) * chmqWatched);
        chmqAlloc = chmqWatched;
        if (ahmqWatched != NULL)
            chmqWatched = SpyGetQueueList(chmqWatched, ahmqWatched);
    } else
        ahmqWatched = NULL;

    henum = WinBeginEnumWindows(HWND_OBJECT);
    if (henum == NULL)
        return;

    chmqInList = 0;
    while ((hwnd = WinGetNextWindow(henum)) != NULL) {
        WinLockWindow(hwnd, FALSE);
        WinQueryClassName(hwnd, sizeof(szClassName),
            (PSZ)szClassName);

        if (strcmp(szClassName, "#32767") == 0) {
            /*
             * We found an HMQ, add it to our list, Do not add our own
             * HMQ to the list!.
             */
            ulHMQ = (ULONG)WinQueryWindowULong(hwnd, QWL_HMQ);
            if ((HMQ)ulHMQ != hmqSpy) {
                WinQueryWindowProcess(hwnd, &pidWindow, &tidWindow);

                /* Reuse szClassName */
                sprintf(szClassName, "0x%04x (PID:%d TID:%d)",
                        (USHORT)ulHMQ, (SHORT)pidWindow, (SHORT)tidWindow);

                item = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_INSERTITEM,
                        (MPARAM)LIT_END, (MPARAM)(PSZ)szClassName));

                /* Set the item handle to the handle of the window */
                WinSendMsg(hwndWindowLB, LM_SETITEMHANDLE, (MPARAM)item,
                        (MPARAM)ulHMQ);
                chmqInList++;

                /*
                 * Now see if we should select the item
                 */
                if (phmqT = ahmqWatched) {
                    for (i = 0; i < chmqWatched; i++) {
                        if ((HMQ)ulHMQ == *phmqT++) {
                            WinSendMsg(hwndWindowLB, LM_SELECTITEM, (MPARAM)item,
                                    (MPARAM)TRUE);
                            break;
                        }
                    }
                }
            }
        }

    }

    WinEndEnumWindows(henum);

    if (ahmqWatched)
        WinFreeMem(hHeap, (char *)ahmqWatched, chmqAlloc * sizeof(HMQ));
}



/***************************************************************************\
* DisplayQueueInfoForWindow(HWND hwndDialog, HWND hwnd)
*
* Displays the information about the selected window in the dialog
\***************************************************************************/
void DisplayQueueInfoForWindow(hwndDlg, hwndDisp)
HWND            hwndDlg;
HWND            hwndDisp;
{
HMQ             hmqDisp;

    /* simply get the HMQ for the window and call display function */
    hmqDisp = (HMQ)WinQueryWindowULong(hwndDisp, QWL_HMQ);
    DisplayQueueInfo(hwndDlg, hmqDisp);
}




/***************************************************************************\
* DisplayQueueInfo(HWND hwndDialog, HWND hwnd)
*
* Displays the information about the selected window in the dialog
\***************************************************************************/
void DisplayQueueInfo(hwndDlg, hmqDisp)
HWND            hwndDlg;
HMQ             hmqDisp;
{
    HWND        hwnd;
    HMQ         hmqWindow;
    HENUM       henum;
    char        szTemp[50];
    PID         pidWindow;
    TID         tidWindow;


    if (hmqDisp != hmqDlgDisp)
    {
        hmqDlgDisp = hmqDisp;

        /*
         * See if we can find a top level window for this HMQ
         */
        henum = WinBeginEnumWindows(HWND_DESKTOP);
        if (henum == NULL)
            return;

        while ((hwnd = WinGetNextWindow(henum)) != NULL) {
            WinLockWindow(hwnd, FALSE);
            hmqWindow = (HMQ)WinQueryWindowULong(hwnd, QWL_HMQ);

            if (hmqWindow == hmqDisp)
                break;
        }
        WinEndEnumWindows(henum);


        if (hwnd == NULL) {
            /*
             * No top level windows found look for object window.
             */
            henum = WinBeginEnumWindows(HWND_OBJECT);
            if (henum == NULL)
                return;

            while ((hwnd = WinGetNextWindow(henum)) != NULL) {
                WinLockWindow(hwnd, FALSE);
                hmqWindow = (HMQ)WinQueryWindowULong(hwnd, QWL_HMQ);

                if (hmqWindow == hmqDisp)
                    break;
            }
            WinEndEnumWindows(henum);
            if (hwnd == NULL)
                return;
        }

        /* output information to window now */
        sprintf(szTemp, "0x%04x", (SHORT)(LONG)hmqDisp);
        WinSetDlgItemText(hwndDlg, DID_WHMQ, (PSZ)szTemp);

        sprintf(szTemp, "0x%04x", (SHORT)(LONG)hwnd);
        WinSetDlgItemText(hwndDlg, DID_WHANDLE, (PSZ)szTemp);

        /*
         * Warning, we only query the text if the window is not an object
         * window.  If it is an object window, the message queue may not
         * be processing messages, which could hang us
         */
        if (WinIsChild(hwnd, HWND_OBJECT))
            szTemp[0] = '\0';   /* No text available */
        else
            WinQueryWindowText(hwnd, sizeof(szTemp), (PSZ)szTemp);
        WinSetDlgItemText(hwndDlg, DID_WTEXT, (PSZ)szTemp);

        WinQueryWindowProcess(hwnd, &pidWindow, &tidWindow);
        sprintf(szTemp, "%d", (SHORT)pidWindow);
        WinSetDlgItemText(hwndDlg, DID_WPID, (PSZ)szTemp);
        sprintf(szTemp, "%d", (SHORT)tidWindow);
        WinSetDlgItemText(hwndDlg, DID_WTID, (PSZ)szTemp);
    }
}



/***************************************************************************\
* void SelectQueueFromText(hwndDlg)
*
*   Updates the text that is displayed in the message text line
\***************************************************************************/
void SelectQueueFromText(hwndDlg)
HWND    hwndDlg;
{
    char    szTemp[80];
    HMQ     hmqSelect;
    SHORT   cItems;
    SHORT   i;


    /* First get the edit text from the string */
    WinQueryDlgItemText(hwndDlg, DID_WHANDLE, sizeof(szTemp),
            (PSZ)szTemp);

    hmqSelect = (HMQ)UConvertStringToNum(szTemp);

    cItems = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYITEMCOUNT,
            0L, 0L));

    for (i=0; i < cItems; i++) {
        if ((HMQ)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                (MPARAM)i, 0L) == hmqSelect)
            break;  /* found it */
    }

    if (i < cItems) {
        /*
         * found the hmq, bring to top, and select it
         */
        WinSendMsg(hwndWindowLB, LM_SETTOPINDEX,
                MPFROMSHORT(i),  (MPARAM)0L);

        /* Always set it on */
        WinSendMsg(hwndWindowLB, LM_SELECTITEM,
                MPFROMSHORT(i), MPFROMSHORT(TRUE));

    } else {
        WinAlarm(HWND_DESKTOP, WA_WARNING);
        WinSetDlgItemText(hwndDlg, DID_WHANDLE, (PSZ)"");
    }

}




/***************************************************************************\
* BuildQueueWatchList()
*
* Updates the list of windows to be watched from the listbox
\***************************************************************************/
void BuildQueueWatchList(void)
{

    USHORT  itemPrevious;
    USHORT  item;

    HMQ     *ahmqList;
    HMQ     *phmqT;


    ahmqList = (HMQ *)WinAllocMem(hHeap, sizeof(HMQ) * chmqInList);
    if ((phmqT = ahmqList) == NULL)
        return;     /* Error */


    /*
     * Simply loop through asking for the next selected item in the
     * list.  Make sure not to overrun our list.
     */
    itemPrevious = (USHORT)LIT_FIRST;
    chmqWatched = 0;

    while ((item = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYSELECTION,
            (MPARAM)itemPrevious, 0L))) != (USHORT)LIT_NONE) {
        /*
         * Get the items handle, which has the value of the window handle
         */
        *phmqT++ = (HMQ)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                (MPARAM)item, 0L);
        chmqWatched++;

        itemPrevious = item;    /* Where to cointinue the search */
    }

    SpySetQueueList (chmqWatched, ahmqList);

    WinFreeMem(hHeap, (char *)ahmqList, sizeof(HMQ) * chmqInList);
}

