/***************************************************************************\
* window.c - Spy Window dialog functions
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define	INCL_WINDIALOGS
#define	INCL_WINHEAP			/* needed for spy.h */
#define	INCL_WININPUT
#define	INCL_WINLISTBOXES
#define	INCL_WINMESSAGEMGR
#define	INCL_WINPOINTERS		/* needed for spy.h */
#define	INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "spy.h"
#include "spyhook.h"
#include <time.h>
#include <stdlib.h>


/* Forward Declarations */
void    InitWindowList(HWND, HWND, int);
void    BuildWindowWatchList(void);
void    DisplayWindowInfo(HWND, HWND);
void    SelectWindowFromText(HWND);
SHORT   DumpWindowInfo(HWND, SHORT);
int cdecl    CompareHwnds(const void *, const void *);
MRESULT	EXPENTRY SpyWindowsDlgProc(HWND, USHORT, MPARAM, MPARAM);




/***************************************************************************\
* MRESULT EXPENTRY SpyWindowsDlgProc(hwnd, msg, mp1, mp2)
*
* The Spy Windows Dialog procedure
\***************************************************************************/
MRESULT EXPENTRY SpyWindowsDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    BOOL        fSelect = TRUE;
    SHORT       cWindows;
    HWND        hwndPoint;
    HWND        hwndItem;   /* from handle of list item */
    USHORT      iItemFocus; /* Index to item that has the focus */

    switch (msg) {

    case WM_INITDLG:
        /* Initialize the dialog items */
        hwndWindowLB = WinWindowFromID(hwnd, DID_WINDOWLIST);
        InitWindowList(hwnd, HWND_DESKTOP, 0);
        InitWindowList(hwnd, HWND_OBJECT, -10);
        hwndWinDlgDisp = NULL;
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
                SelectWindowFromText(hwnd);
                break;
            }
        }

        /* Normaly pass to dialog procedure to handle message */
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:
            BuildWindowWatchList();
        case DID_CANCEL:
            /* Now dismiss the dialog */
            WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            break;
        case DID_WUNSELALL:
            fSelect = FALSE;
        case DID_WSELALL:
            cWindows = SHORT1FROMMR( WinSendMsg(hwndWindowLB, LM_QUERYITEMCOUNT,
                0L, 0L));

            fTrackingListBox = FALSE;
            while (cWindows) {
                /* Loop through all windows, selecting or unselcting all */
                WinSendMsg(hwndWindowLB, LM_SELECTITEM, (MPARAM)--cWindows,
                    (MPARAM)fSelect);
            }
            fTrackingListBox = TRUE;
            break;

        case DID_WSELMOUSE:
            /* Call function to track mouse, returns window handle */
            hwndPoint = HwndSelWinWithMouse(hwnd, DisplayWindowInfo);
            if (hwndPoint == NULL)
        break;    /* No window to process */

            /*
             * Now find the window in the list, Make the item visible
             * and set the item as selected.
             */
            cWindows = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYITEMCOUNT,
                    0L, 0L));

            while (cWindows) {
                /*
                 * Loop through all windows until we wind the right
                 * one with the correct window handle
                 */
                hwndItem = (HWND)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                        (MPARAM)--cWindows, 0L);

                if (hwndItem == hwndPoint) {
                    /* found the right item, move it to top */
                    WinSendMsg(hwndWindowLB, LM_SETTOPINDEX, (MPARAM)cWindows, 0L);
                    WinSendMsg(hwndWindowLB, LM_SELECTITEM, (MPARAM)cWindows,
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

                    hwndItem = (HWND)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                            (MPARAM)iItemFocus, 0L);
                    DisplayWindowInfo(hwnd, hwndItem);
                }
            }
        }
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
    }
    return 0L;
}




/***************************************************************************\
* void SelectWindowFromText(hwndDlg)
*
*   Updates the text that is displayed in the message text line
\***************************************************************************/
void SelectWindowFromText(hwndDlg)
HWND    hwndDlg;
{
    char    szTemp[80];
    HWND    hwndSelect;
    SHORT   cItems;
    SHORT   i;


    /* First get the edit text from the string */
    WinQueryDlgItemText(hwndDlg, DID_WHANDLE, sizeof(szTemp),
            (PSZ)szTemp);

    hwndSelect = (HWND)UConvertStringToNum(szTemp);

    cItems =SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYITEMCOUNT,
            0L, 0L));

    for (i=0; i < cItems; i++) {
        if ((HWND)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                (MPARAM)i, 0L) == hwndSelect)
            break;  /* found it */
    }

    if (i < cItems) {
        /*
         * found the hwnd, bring to top, and select it
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


/****************************************************************************\
* InitWindowList (hwndDialog, hwnd, level)
*
* Builds the list of windows displayed in the windows dialog
\***************************************************************************/
void InitWindowList(hwndDialog, hwnd, level)
HWND    hwndDialog;
HWND    hwnd;
int     level;
{
    char    szTemp[30];
    char    szId[20];
    HWND    hwndT;
    USHORT  item;
    USHORT  id;
    int     i;

    /*
     * We will first add this item to our list of
     * items in the listbox, If the item is in our list of hwnds,
     * set the item selected. To keep from getting into endless loops
     * will not add spywindow client, and descendants.
     */
    if (hwnd != hwndSpy) {
        id = (USHORT)WinQueryWindowUShort(hwnd, QWS_ID);
        sprintf(szId, "ID: %x", id);

        for (i = 0; i < cToName; i++) {
            if (id == rgidtoname[i].id) {
                strcpy (szId, rgidtoname[i].szIdName);
                break;
            }
        }

        sprintf(szTemp, "%04x(%d) - %s", (USHORT)(ULONG)hwnd, level, szId);
        item = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_INSERTITEM,
                (MPARAM)LIT_END, (MPARAM)(PSZ)szTemp));

        /* Set the item handle to the handle of the window */
        WinSendMsg(hwndWindowLB, LM_SETITEMHANDLE, (MPARAM)item,
                (MPARAM)hwnd);

        if (SpyFWindowInList(hwnd, TRUE))
            WinSendMsg(hwndWindowLB, LM_SELECTITEM, (MPARAM)item,
                (MPARAM)TRUE);

        /*
         * Then we recurse with all of our children
         */
        if ((hwndT = WinQueryWindow(hwnd, QW_TOP, FALSE)) != NULL)
            InitWindowList(hwndDialog, hwndT, level+1);
    }

    /*
     * Then go to our next sibling
     */
    if ((hwndT = WinQueryWindow(hwnd, QW_NEXT, FALSE)) != NULL)
        InitWindowList(hwndDialog, hwndT, level);
}



/***************************************************************************\
* BuildWindowWatchList()
*
* Updates the list of windows to be watched from the listbox
\***************************************************************************/
void BuildWindowWatchList(void)
{

    USHORT  itemPrevious;
    USHORT  item;
    HWND    hwnd;

    SHORT   chwnd;
    HWND    rghwnd[MAXHWNDS];

    /*
     * Simply loop through asking for the next selected item in the
     * list.  Make sure not to overrun our list.
     */
    itemPrevious = (USHORT)LIT_FIRST;
    chwnd = 0;

    while ((item = SHORT1FROMMR(WinSendMsg(hwndWindowLB, LM_QUERYSELECTION,
            (MPARAM)itemPrevious, 0L))) != (USHORT)LIT_NONE) {
        /*
         * Get the items handle, which has the value of the window handle
         */
        hwnd = (HWND)WinSendMsg(hwndWindowLB, LM_QUERYITEMHANDLE,
                (MPARAM)item, 0L);

        rghwnd[chwnd++] = hwnd;
        if (chwnd >= MAXHWNDS)
            break;  /* Dont overflow array */
        itemPrevious = item;    /* Where to cointinue the search */
    }

    SpySetWindowList (chwnd, rghwnd);
}


/***************************************************************************\
* HWND HwndSelWinWithMouse(HWND hwnd, void (*pfnDisplayInfo)(HWND, HWND))
*
* 	    This function is used to allow the user to select a window with
*           the mouse.  If fDisplayInfo is TRUE, it will update the
*           information in the dialog box, about the window that the
*           mouse is currently over.
\***************************************************************************/
HWND HwndSelWinWithMouse(hwnd, pfnDisplayInfo)
HWND hwnd;
void (*pfnDisplayInfo)(HWND, HWND);
{

    QMSG        qmsg;
    HWND        hwndPoint;
    char        szClassName[50];    /* Class name of window */
    CLASSINFO   classinfo;          /* Information about class */


    /*
     * First set the capture to the specified window
     */
    WinSetCapture(HWND_DESKTOP, hwnd);
    WinSetPointer (HWND_DESKTOP, hptrSelWin);

    /*
     * Now loop through all of the messages that are sent, until
     * we get our mouse 1 down message.  We will also filter out
     * the WM_MOVE message, else we will dispatch the messages.
     */
    while (WinGetMsg(hab, &qmsg, NULL, 0, 0)) {
        if (qmsg.msg == WM_MOUSEMOVE) {
            if (pfnDisplayInfo != NULL) {
                hwndPoint = WinWindowFromPoint(HWND_DESKTOP,
                    &qmsg.ptl, TRUE, FALSE);
                (*pfnDisplayInfo)(hwnd, hwndPoint);
            }
        }
        else if (qmsg.msg == WM_BUTTON1DOWN)
            break;
        else
            WinDispatchMsg(hab, &qmsg);
    }

    WinSetPointer (HWND_DESKTOP, hptrArrow);
    WinSetCapture(HWND_DESKTOP, NULL);


    /*
     * Map the point to the window,  If the CTRL-Key is down,
     * we will go up through the parent chain until we get to
     * a frame window or desktop.  Dont let hwndSpy through!!!
     */
    hwndPoint = WinWindowFromPoint(HWND_DESKTOP,
        &qmsg.ptl, TRUE, FALSE);
    if (WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000) {
        /* Asked for frame window */
        for (;;) {
            if (hwndPoint == NULL)
                return (NULL);         /* No frames available */
            /* See if frame class */
            WinQueryClassName(hwndPoint, sizeof(szClassName),
                (PSZ)szClassName);
            if (WinQueryClassInfo(hab, (PSZ)szClassName, &classinfo) &&
                    (classinfo.flClassStyle & CS_FRAME))
                break;  /* We have our frame */

            /* Not frame, go back to parent */
            hwndPoint = WinQueryWindow(hwndPoint, QW_PARENT, FALSE);
        }
    }

    if (pfnDisplayInfo != NULL)
        (*pfnDisplayInfo)(hwnd, hwndPoint);

    if (WinIsChild(hwndPoint, hwndSpy))
        return (NULL);    /* Dont want to get in endless loops */

    return (hwndPoint);
}



/***************************************************************************\
* DisplayWindowInfo(HWND hwndDialog, HWND hwnd)
*
* Displays the information about the selected window in the dialog
\***************************************************************************/
void DisplayWindowInfo(hwndDlg, hwnd)
HWND            hwndDlg;
HWND            hwnd;
{
    HWND        hwndT;
    HWND        hwndParent;
    char        szTemp[50];
    char        szTemp2[10];
    CLASSINFO   classinfo;
    RECTL       rcl;
    USHORT      id;
    ULONG       ul;
    USHORT      us1;
    USHORT      us2;
    USHORT      us3;
    USHORT      us4;
    PID         pidWindow;
    TID         tidWindow;


    if (hwnd != hwndWinDlgDisp)
    {
        hwndWinDlgDisp = hwnd;

        /* This could be table driven */
        sprintf(szTemp, "0x%04x", (SHORT)(ULONG) hwnd);
        WinSetDlgItemText(hwndDlg, DID_WHANDLE, (PSZ)szTemp);

        WinQueryClassName(hwnd, sizeof(szTemp), (PSZ)szTemp);
        if (!WinQueryClassInfo(hab, (PSZ)szTemp, &classinfo)) {
            classinfo.flClassStyle = -1;    /* Let know error conditon */
            classinfo.cbWindowData = 0;     /* Make sure we dont dump */
        }

        WinSetDlgItemText(hwndDlg, DID_WCLASS, (PSZ)szTemp);


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

        hwndParent = WinQueryWindow(hwnd, QW_PARENT, FALSE);
        sprintf(szTemp, "0x%04x", (SHORT)(LONG)hwndParent);
        WinSetDlgItemText(hwndDlg, DID_WPARENT, (PSZ)szTemp);

        hwndT = WinQueryWindow(hwnd, QW_TOP, FALSE);
        sprintf(szTemp, "0x%04x", (SHORT)(LONG) hwndT);
        WinSetDlgItemText(hwndDlg, DID_WCHILD, (PSZ)szTemp);

        hwndT = WinQueryWindow(hwnd, QW_OWNER, FALSE);
        sprintf(szTemp, "0x%04x", (SHORT)(LONG) hwndT);
        WinSetDlgItemText(hwndDlg, DID_WOWNER, (PSZ)szTemp);

        WinQueryWindowRect(hwnd, &rcl);
        WinMapWindowPoints(hwnd, hwndParent, (PPOINTL)&rcl, 2);
        sprintf(szTemp, "(%d, %d), (%d, %d)", (SHORT)rcl.xLeft,
            (SHORT)rcl.yBottom, (SHORT)rcl.xRight, (SHORT)rcl.yTop);
        WinSetDlgItemText(hwndDlg, DID_WRECT, (PSZ)szTemp);

        id = (USHORT)WinQueryWindowUShort(hwnd, QWS_ID);
        sprintf(szTemp, "0x%04x", id);
        WinSetDlgItemText(hwndDlg, DID_WID, (PSZ)szTemp);

        ul = (ULONG)WinQueryWindowULong(hwnd, QWL_STYLE);
        sprintf(szTemp, "0x%08lx", ul);
        WinSetDlgItemText(hwndDlg, DID_WSTYLE, (PSZ)szTemp);

        sprintf(szTemp, "0x%08lx", classinfo.flClassStyle);
        WinSetDlgItemText(hwndDlg, DID_WCSTYLE, (PSZ)szTemp);

        ul = (ULONG)WinQueryWindowULong(hwnd, QWP_PFNWP);
        sprintf(szTemp, "%p", ul);
        WinSetDlgItemText(hwndDlg, DID_WPFNWP, (PSZ)szTemp);

        ul = (ULONG)WinQueryWindowULong(hwnd, QWL_HMQ);
        sprintf(szTemp, "0x%04x", (SHORT)ul);
        WinSetDlgItemText(hwndDlg, DID_WHMQ, (PSZ)szTemp);

        WinQueryWindowProcess(hwnd, &pidWindow, &tidWindow);
        sprintf(szTemp, "%d", (SHORT)pidWindow);
        WinSetDlgItemText(hwndDlg, DID_WPID, (PSZ)szTemp);
        sprintf(szTemp, "%d", (SHORT)tidWindow);
        WinSetDlgItemText(hwndDlg, DID_WTID, (PSZ)szTemp);

        /*
         * We have four General purpose lines left, used only for
         * frames now
         */
        if ((classinfo.flClassStyle & CS_FRAME) &&
                    (classinfo.cbWindowData > QWL_HWNDFOCUSSAVE)) {
            ul = (ULONG)WinQueryWindowULong(hwnd, QWL_HWNDFOCUSSAVE);
            sprintf(szTemp, "Frame Focus: %p",ul);
            WinSetDlgItemText(hwndDlg, DID_WOLINE1, (PSZ)szTemp);

            us1 = (USHORT)WinQueryWindowUShort(hwnd, QWS_FLAGS);
            us2 = (USHORT)WinQueryWindowUShort(hwnd, QWS_RESULT);
            sprintf(szTemp, "Flags: 0x%04x Rslt: 0x%04x", us1, us2);
            WinSetDlgItemText(hwndDlg, DID_WOLINE2, (PSZ)szTemp);

            us1 = (USHORT)WinQueryWindowUShort(hwnd, QWS_XRESTORE);
            us2 = (USHORT)WinQueryWindowUShort(hwnd, QWS_YRESTORE);
            us3 = (USHORT)WinQueryWindowUShort(hwnd, QWS_CXRESTORE);
            us4 = (USHORT)WinQueryWindowUShort(hwnd, QWS_CYRESTORE);
            sprintf(szTemp, "Restore: (%d, %d, %d, %d)",us1, us2, us3, us4);
            WinSetDlgItemText(hwndDlg, DID_WOLINE3, (PSZ)szTemp);

            us1 = (USHORT)WinQueryWindowUShort(hwnd, QWS_XMINIMIZE);
            us2 = (USHORT)WinQueryWindowUShort(hwnd, QWS_YMINIMIZE);
            sprintf(szTemp, "Minimize: (%d, %d)",us1, us2);
            WinSetDlgItemText(hwndDlg, DID_WOLINE4, (PSZ)szTemp);
        } else {
            /*
             * Nothing special to output for this window type, so lets
             * dump the window extra words.
             * Note: This code is sh.ty
             */
            us1 = 0;    /* Word offset */
            for (id=DID_WOLINE1; id <= DID_WOLINE4; id++) {
                szTemp[0] = '\0';
                for (us2 = 0; us2 < 4; us2++) {
                    if (us1 >= classinfo.cbWindowData)
                        break;
                    us3 = (USHORT)WinQueryWindowUShort(hwnd, us1);
                    sprintf(szTemp2, "%04x ", us3);
                    strcat(szTemp, szTemp2);
                    us1 += 2;   /* Setup for next word */
                }

                /* output this line */
                WinSetDlgItemText(hwndDlg, id, (PSZ)szTemp);
            }
        }
    }
}




/***************************************************************************\
* DumpOneWindowInfo()
*
* Dump the information about one window to the current outputs
\***************************************************************************/
void    DumpOneWindowInfo()
{
    HWND        hwndPoint;
    HWND        hwndT;
    SHORT       wLevel;

    hwndPoint = HwndSelWinWithMouse(hwndSpy, NULL);
    if (hwndPoint == NULL)
        return;    /* No window selected */

    /* Now see what level the window is at */
    wLevel = 0;
    hwndT = hwndPoint;
    while (hwndT != NULL) {
        wLevel++;
        hwndT = WinQueryWindow(hwndT, QW_PARENT, FALSE);
    };


    DumpWindowInfo(hwndPoint, wLevel);
}


/****************************************************************************\
* DumpAllWIndowsInfo (HWND hwnd, WORD wLevel)
*
* Dumps the complet window list out to the current output units.
\***************************************************************************/
SHORT DumpAllWindowsInfo(hwnd, wLevel)
HWND    hwnd;
SHORT   wLevel;
{
    HWND    hwndT;
    SPWD    *pspwdT;
    SHORT   cWindowBytes;

    pspwdT = pspwd + wDumpCount;

    cWindowBytes = DumpWindowInfo(hwnd, wLevel);

    pspwdT->hwnd = hwnd;
    pspwdT->index = wDumpCount;


    /*
     * Then we recurse with all of our children
     */
    if ((hwndT = WinQueryWindow(hwnd, QW_TOP, FALSE)) != NULL)
        cWindowBytes += DumpAllWindowsInfo(hwndT, wLevel+1);

    /*
     * Then go to our next sibling
     */
    if ((hwndT = WinQueryWindow(hwnd, QW_NEXT, FALSE)) != NULL)
        cWindowBytes += DumpAllWindowsInfo(hwndT, wLevel);

    return (cWindowBytes);
}



/****************************************************************************\
* DumpWindowIndex (void)
*
* Dump a sorted list of Hwnds and index into other list
\***************************************************************************/
void    DumpWindowIndex(cBytes)
SHORT   cBytes;
{
    SHORT   cch;
    char    szTemp[20];
    char    szOutput[100];
    SPWD    *pspwdT;
    SHORT   i;

    /* Sort the hwnds first */
    qsort((void *)pspwd, wDumpCount, sizeof(SPWD), CompareHwnds);
    pspwdT = pspwd;

    strcpy (szOutput, "Index of Window Handles");
    cch = strlen(szOutput);
    for (i=0; i< wDumpCount; i++) {
        if ((i & 3) == 0) {
            /* 4 per row */
            OutputString(szOutput, cch);
            szOutput[0] = '\0';
            cch = 0;
        }

        cch += sprintf(szTemp, "%3d-%04x ",
                pspwdT->index,    (USHORT)(ULONG) pspwdT->hwnd);
        strcat (szOutput, szTemp);
        pspwdT++;
    }

    OutputString(szOutput, cch);

    cch = sprintf(szOutput, "Number of Windows: %d, Approx heap size: %d",
            wDumpCount, cBytes);
    OutputString(szOutput, cch);


}


/****************************************************************************\
* int  CompareHwnds(SPWD *pspwd1, SPWD *pspwd2)
*
* Compares two window handles
\***************************************************************************/
int  cdecl CompareHwnds(pspwd1, pspwd2)
const void    *pspwd1;
const void    *pspwd2;
{
    return (((SPWD *)pspwd1)->hwnd < ((SPWD *)pspwd2)->hwnd)? -1 : 1;
}




/***************************************************************************\
* DumpWindowInfo(HWND hwnd, SHORT wLevel)
*
* Displays the information about the selected window in the dialog
\***************************************************************************/
SHORT DumpWindowInfo(hwnd, wLevel)
HWND        hwnd;
SHORT       wLevel;
{
    HWND    hwndParent;
    HWND    hwndChild;
    HWND    hwndOwner;

    char    szTemp[100];
    char    szTemp2[20];
    SHORT   cch;
    char    szClass[30];
    RECTL   rcl;
    USHORT  id;
    ULONG   ulStyle;
    ULONG   ulPFNWP;
    ULONG   ulHMQ;
    SHORT   wOffsetClassData;
    SHORT   wWindowWord;
    PID     pidWindow;
    TID     tidWindow;

    CLASSINFO   classinfo;

    hwndParent = WinQueryWindow(hwnd, QW_PARENT, FALSE);
    hwndChild = WinQueryWindow(hwnd, QW_TOP, FALSE);
    hwndOwner = WinQueryWindow(hwnd, QW_OWNER, FALSE);
    id = (USHORT)WinQueryWindowUShort(hwnd, QWS_ID);
    ulHMQ = (ULONG)WinQueryWindowULong(hwnd, QWL_HMQ);
    WinQueryWindowRect(hwnd, &rcl);
    WinMapWindowPoints(hwnd, hwndParent, (PPOINTL)&rcl, 2);

    cch = sprintf(szTemp,
        "%d-H:%04x(%d) P:%04x C:%04x O:%04x ID:%04x MQ:%04x (%d, %d) (%d, %d)",
        ++wDumpCount, (SHORT)(LONG)hwnd, wLevel, (SHORT)(LONG)hwndParent,
        (SHORT)(LONG)hwndChild, (SHORT)(LONG)hwndOwner, id, (SHORT)ulHMQ,
        (SHORT)rcl.xLeft, (SHORT)rcl.yBottom,
        (SHORT)rcl.xRight, (SHORT)rcl.yTop);

    OutputString(szTemp, cch);

    ulStyle = (ULONG)WinQueryWindowULong(hwnd, QWL_STYLE);
    ulPFNWP = (ULONG)WinQueryWindowULong(hwnd, QWP_PFNWP);
    WinQueryClassName(hwnd, sizeof(szClass), (PSZ)szClass);
    if (!WinQueryClassInfo(hab, (PSZ)szClass, &classinfo)) {
        classinfo.flClassStyle = -1;    /* Let know error conditon */
        classinfo.cbWindowData = 0;     /* Make sure we dont dump */
    }

    WinQueryWindowProcess(hwnd, &pidWindow, &tidWindow);

    cch = sprintf(szTemp,
        "          St:%08lx  PID:%d  TID:%d  Pfn:%p  Cl:%s",
        ulStyle, pidWindow, tidWindow, ulPFNWP, szClass);
    OutputString(szTemp, cch);

    /*
     * Dump the window extra words out also.
     */
    strcpy (szTemp, "          ");
    id = 8;
    for (wOffsetClassData = 0; wOffsetClassData < (SHORT) classinfo.cbWindowData;
            wOffsetClassData += 2) {

        wWindowWord = (USHORT)WinQueryWindowUShort(hwnd, wOffsetClassData);
        sprintf(szTemp2, "%04x ", wWindowWord);
        strcat(szTemp, szTemp2);
        if (--id == 0) {
            /* line full is full */
            OutputString(szTemp, strlen(szTemp));
            szTemp[10] = '\0';
            id = 8;
        }
    }

    if (id != 8)
        OutputString(szTemp, strlen(szTemp));

    /* Return the number of bytes associated with the window */
    return ((SIZEOFWND + classinfo.cbWindowData + 3) & 0xfffc);
}




/***************************************************************************\
* SelOrDeselWithMouse(BOOL fSelect)
*
* Fastway to add/or remove window from watch list
\***************************************************************************/

void SelOrDeselWithMouse(fSelect)
BOOL    fSelect;
{
    HWND    rghwnd[MAXHWNDS];
    HWND    hwndPoint;
    SHORT    chwnd;
    BOOL    fWinCurInList;
    SHORT    i;

    /* First get the window of interest */
    hwndPoint = HwndSelWinWithMouse(hwndSpy, NULL);
    if (hwndPoint == NULL)
        return;    /* No window selected */
    fWinCurInList = SpyFWindowInList(hwndPoint, TRUE);

    if ((fWinCurInList && fSelect)
            || (!fWinCurInList && !fSelect))
        return;        /* Alredy right state */

    chwnd = SpyGetWindowList(MAXHWNDS, (HWND FAR *)rghwnd);

    if (fSelect) {
        /* Add window to end of list */
        rghwnd[chwnd++] = hwndPoint;
    } else {
        /* find it in the list, and delete it out */
        for (i=0; rghwnd[i] != hwndPoint; i++)
            ;

        /* Now copy rest of them down */
        chwnd--;    /* One less item */
        for (;i < chwnd; i++ ) {
            rghwnd[i] = rghwnd[i+1];
        }
    }

    /* Now call to update the list */
    SpySetWindowList(chwnd, (HWND FAR *)rghwnd);
}


