/***************************************************************************\
* message.c - procecures to process the message dialog
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define	INCL_WINBUTTONS
#define	INCL_WINDIALOGS
#define	INCL_WINFRAMEMGR
#define	INCL_WINHEAP			/* needed for spy.h */
#define	INCL_WININPUT
#define	INCL_WINLISTBOXES
#define	INCL_WINMESSAGEMGR
#define	INCL_WINMENUS
#define	INCL_WINPOINTERS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "spy.h"
#include "spyhook.h"

#define SIZEOFWND   34
#define MAXMSGBYTES 100

/* Procedure prototypes */
void    UpdMsgsLBSels (USHORT, BOOL);
void    UpdMsgTblFromLB (HWND);
void    ProcessQueueMsg(QMSGSPY *);
void    SelOrDeselWithMouse(BOOL);
void    UpdateMsgBoxCurMsgText(HWND);
void    SelectMessageFromText(HWND);
MRESULT	EXPENTRY SpyMsgDlgProc(HWND, USHORT, MPARAM, MPARAM);

/***************************************************************************\
* MRESULT EXPENTRY SpyMsgDlgProc (hwnd, msg, mp1, mp2)
*
* Message List dialog procedure
\***************************************************************************/
MRESULT EXPENTRY SpyMsgDlgProc(hwnd, msg, mp1, mp2)
HWND            hwnd;
USHORT          msg;
MPARAM          mp1;
MPARAM          mp2;
{
    SHORT       i;
    MSGI        *pmsgi;
    USHORT      item;
    SHORT       bHooksNew;
    USHORT      iItemFocus; /* Index to item that has the focus */

    switch (msg) {

    case WM_INITDLG:
        /*
         * Initialize the list box with the list of messages that are
         * defined in our message table
         */
        iCurItemFocus = -1;
        pmsgi = rgmsgi; /* Point to start of list */
        hwndMessageLB = WinWindowFromID(hwnd, DID_OMSGLIST);
        for (i = 0; i < cmsgi; i++) {
            pmsgi->iListBox = item = SHORT1FROMMR(WinSendMsg(hwndMessageLB,
                    LM_INSERTITEM,
                    (MPARAM)(spyopt.fAlphaSortMsgList? LIT_SORTASCENDING : LIT_END),
                    (MPARAM)(PSZ)pmsgi->szMsg));
            WinSendMsg(hwndMessageLB, LM_SETITEMHANDLE, (MPARAM)item,
                (MPARAM)i);
            if (pmsgi->wOptions & MSGI_ENABLED) {
                WinSendMsg(hwndMessageLB, LM_SELECTITEM, (MPARAM)item,
                (MPARAM)TRUE);
            }
            pmsgi++;
        }

        /* Initialize the Hook type record */
        WinSendDlgItemMsg(hwnd, DID_OINPUT, BM_SETCHECK,
                (MPARAM)(spyopt.bHooks & SPYH_INPUT)?1 : 0, 0L);

        WinSendDlgItemMsg(hwnd, DID_OSENDMSG, BM_SETCHECK,
                (MPARAM)(spyopt.bHooks & SPYH_SENDMSG)?1 : 0, 0L);

        WinSendDlgItemMsg(hwnd, DID_OTHERMSGS, BM_SETCHECK,
                (MPARAM)(spyopt.fDispOtherMsgs), 0L);

        WinSetFocus(HWND_DESKTOP, hwndMessageLB);
        fTrackingListBox = TRUE;
        return ((MRESULT)TRUE);  /* We set the focus */

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
                    WinWindowFromID(hwnd, DID_MSGEDIT)) {
                SelectMessageFromText(hwnd);
                break;
            }
        }

        /* Normaly pass to dialog procedure to handle message */
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:

            /*
             * Call to update the Message table select bits
             */

            UpdMsgTblFromLB (hwnd);

            /* Setup new hook options */
            bHooksNew = 0;

            if ((BOOL)(SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                    DID_OINPUT, BM_QUERYCHECK, 0L, 0L))))
                bHooksNew = SPYH_INPUT;

            if ((BOOL)(SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                    DID_OSENDMSG, BM_QUERYCHECK, 0L, 0L))))
                bHooksNew |= SPYH_SENDMSG;

            if (bHooksNew != (SHORT) spyopt.bHooks) {
                SpyReleaseHook (FALSE);     /* Dont clear queue */
                spyopt.bHooks = bHooksNew;
                SpyInstallHook(hab, hmqSpy, spyopt.bHooks); /* Install hook again */
                WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                    MM_SETITEMATTR, MPFROM2SHORT(CMD_INPUTHOOK, TRUE),
                    MPFROM2SHORT(MIA_CHECKED,
                         (spyopt.bHooks & SPYH_INPUT) ? MIA_CHECKED : 0));
                WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                    MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDMSGHOOK, TRUE),
                    MPFROM2SHORT(MIA_CHECKED,
                         (spyopt.bHooks & SPYH_SENDMSG) ? MIA_CHECKED : 0));
            }

            /* Fall through to DID_CANCEL */
        case DID_CANCEL:
            /* Now dismiss the dialog */
            hwndMessageLB = NULL;   /* Not here anymore to process */
            fTrackingListBox = FALSE;
            WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            break;

        /*
         * These case simply update the listbox with which messages are
         * enabled
         */
        case DID_MALL:
            UpdMsgsLBSels (0, TRUE);
            break;
        case DID_MNONE:
            UpdMsgsLBSels (0, FALSE);
            break;
        case DID_MCON:
            UpdMsgsLBSels (MSGI_KEY, TRUE);
            break;
        case DID_MCOFF:
            UpdMsgsLBSels (MSGI_KEY, FALSE);
            break;
        case DID_MMON:
            UpdMsgsLBSels (MSGI_MOUSE, TRUE);
            break;
        case DID_MMOFF:
            UpdMsgsLBSels (MSGI_MOUSE, FALSE);
            break;
        case DID_MFON:
            UpdMsgsLBSels (MSGI_FREQ, TRUE);
            break;
        case DID_MFOFF:
            UpdMsgsLBSels (MSGI_FREQ, FALSE);
            break;
        }
        break;

    default:
        /*
         * Default is to see if the listbox has changed its focus
         * item number.  If it has, then we want to display the information
         * about the window that the listbox cursor is over.  There is no
         * legal way to do this, except to temporarily put the listbox into
         * single selection mode and query the selection.
         */
        if (fTrackingListBox && (hwndMessageLB != NULL)) {
            WinSetWindowBits(hwndMessageLB, QWL_STYLE, 0L, LS_MULTIPLESEL);
            iItemFocus = SHORT1FROMMR(WinSendMsg(hwndMessageLB, LM_QUERYSELECTION,
                    (MPARAM)LIT_FIRST, 0L));
            WinSetWindowBits(hwndMessageLB, QWL_STYLE, LS_MULTIPLESEL,
                    LS_MULTIPLESEL);
            if (iItemFocus != iCurItemFocus) {
                iCurItemFocus = iItemFocus;
                UpdateMsgBoxCurMsgText(hwnd);
            }
        }
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return 0L;
}



/***************************************************************************\
* void UpdateMsgBoxCurMsgText()
*
*   Updates the text that is displayed in the message text line
\***************************************************************************/
void UpdateMsgBoxCurMsgText(hwndDlg)
HWND    hwndDlg;
{
    SHORT   sMsgI;
    char    szTemp[80];


    if (iCurItemFocus >= 0) {
        /* Get the messge index */
        sMsgI = SHORT1FROMMR(WinSendMsg(hwndMessageLB, LM_QUERYITEMHANDLE,
                (MPARAM)iCurItemFocus, 0L));
        sprintf(szTemp, "0x%04x  - %s", rgmsgi[sMsgI].msg,
                rgmsgi[sMsgI].szMsg);
        WinSetDlgItemText(hwndDlg, DID_MSGEDIT, (PSZ)szTemp);
    } else {
        WinSetDlgItemText(hwndDlg, DID_MSGEDIT, (PSZ)"");
    }

}




/***************************************************************************\
* void SelectMessageFromText(hwndDlg)
*
*   Updates the text that is displayed in the message text line
\***************************************************************************/
void SelectMessageFromText(hwndDlg)
HWND    hwndDlg;
{
    char    szTemp[80];
    USHORT  msg;
    MSGI    *pmsgi;
    int     i;


    /* First get the edit text from the string */
    WinQueryDlgItemText(hwndDlg, DID_MSGEDIT, sizeof(szTemp),
            (PSZ)szTemp);

    if ((msg = UConvertStringToNum(szTemp)) != 0xffff) {

        /* We have a number, now try to find message in message table */
        pmsgi = PmsgiFromMsg(msg);

    } else {
        /* Assume String, Try to locate string in our table */
        pmsgi = rgmsgi; /* Start at beginning of table */
        /*
         * This does simple string compares, it does not map case, nor
         * does it trim the string.
         */
        for (i=0; i < cmsgi; i++) {
            if (strcmpi(pmsgi->szMsg, szTemp) == 0)
                break;
            pmsgi++;
        };

        if (i >= cmsgi)
            pmsgi = NULL;
    }

    /*
     * Have a pointer to MSGI of message, or NULL if not in list.
     */
    if(pmsgi != NULL) {
        /* First make sure it is visible */
        WinSendMsg(hwndMessageLB, LM_SETTOPINDEX,
                MPFROMSHORT(pmsgi->iListBox),  (MPARAM)0L);

        /* Always set it on */
        WinSendMsg(hwndMessageLB, LM_SELECTITEM,
                MPFROMSHORT(pmsgi->iListBox), MPFROMSHORT(TRUE));

    } else {
        WinAlarm(HWND_DESKTOP, WA_WARNING);
        WinSetDlgItemText(hwndDlg, DID_MSGEDIT, (PSZ)"");
    }
}





/***************************************************************************\
* void UpdMsgsLBSels (USHORT uMask, fOnOrOff);
*
* Will update the selected items in the message listbox, that is
*   displayed in the options dialog
\***************************************************************************/
void UpdMsgsLBSels (uMask, fOnOrOff)
USHORT      uMask;
BOOL        fOnOrOff;
{

    SHORT       i;
    MSGI        *pmsgi;

    /*
     * Loop through all of the items in our list, if the mask is 0 or
     * the bit is on in the item, then update the select state in listbox
     * defined in our message table
     */
    fTrackingListBox = FALSE;
    pmsgi = rgmsgi; /* Point to start of list */
    for (i = 0; i < cmsgi; i++) {
        if ((uMask == 0) || (pmsgi->wOptions & uMask)) {
            WinSendMsg(hwndMessageLB, LM_SELECTITEM,
        (MPARAM)pmsgi->iListBox,  (MPARAM)fOnOrOff);
        }
        pmsgi++;
    }
    fTrackingListBox = TRUE;
}



/***************************************************************************\
* void UpdMsgTblFromLB (HWND hwndDialog);
*
* Will update the selected items in the message listbox, that is
*   displayed in the options dialog
\***************************************************************************/
void UpdMsgTblFromLB (hwndDialog)
HWND            hwndDialog;
{

    USHORT       i;
    register MSGI *pmsgi;
    USHORT      itemSel;

    /*
     * Loop through all of the items in the list and update the selection
     * status depending of if the item is selected in the list box or
     * not.
     */
    /* First simply turn off all of the bits */
    pmsgi = rgmsgi;
    for (i = 0; i < (USHORT) cmsgi; i++) {
        pmsgi->wOptions &= ~MSGI_ENABLED;
        pmsgi++;
    }

    /* Then turn on all of the selected items */
    itemSel = (USHORT)LIT_FIRST;

    while ((itemSel = SHORT1FROMMR(WinSendMsg(hwndMessageLB, LM_QUERYSELECTION,
            (MPARAM)itemSel, 0L))) != (USHORT)LIT_NONE) {

        /* The item handle contains index in our array */
        i = SHORT1FROMMR(WinSendMsg(hwndMessageLB, LM_QUERYITEMHANDLE,
            (MPARAM)itemSel, 0L));

        rgmsgi[i].wOptions |= MSGI_ENABLED;
    }

    /* Get the Other message option from checkmark */
    spyopt.fDispOtherMsgs = ((BOOL)SHORT1FROMMR(WinSendDlgItemMsg(hwndDialog, DID_OTHERMSGS,
            BM_QUERYCHECK, 0L, 0L)));

    /* Now call function to update the hooks message list */
    UpdateHooksMsgTable();
}




/***************************************************************************\
* void UpdateHooksMsgTable(void)
*
* Send the message bitmask to the hook, for interesting messages.
\***************************************************************************/
void  UpdateHooksMsgTable()
{
    MSGI            *pmsgi;
    UCHAR           rgb[MAXMSGFILTERBYTES];
    int             i;
    UCHAR           *prgb;
    unsigned char   mask;

    /*
     * First zero the bitmask
     */
    memset(rgb,'\0', MAXMSGFILTERBYTES);
    mask = 1;
    prgb = rgb;

    /*
     * Loop to set the bits
     */
    pmsgi = rgmsgi;
    for (i = 0; i <= MAXMSGFILTER; i++) {
        /* If enabled, set bit in bit table */

        if ((SHORT) pmsgi->msg == i) {
            if (pmsgi->wOptions & MSGI_ENABLED)
                *prgb =(UCHAR)(pmsgi->bMPTypes | MP_ENABLED);

            pmsgi++;
        } else {
            /* Hole in range, set it true */
            *prgb |= mask;
        }

        prgb++;
    }

    /* Now call the hook function with the new mask */
    SpySetMessageList((char far *)rgb, spyopt.fDispOtherMsgs);
}




/***************************************************************************\
* EnableOrDisableMsg(BOOL fEnable)
*
* Fast way to enable or disable a particular message code.  The one
*           that is currently selected in the output listbox.
\***************************************************************************/

void EnableOrDisableMsg(fEnable)
BOOL            fEnable;
{
    USHORT      itemSel;
    char        szTemp[100];
    char        *psz;
    MSGI        *pmsgi;
    SHORT       i;

    itemSel = SHORT1FROMMR(WinSendMsg(hwndSpyList, LM_QUERYSELECTION,
            (MPARAM)LIT_FIRST, 0L));
    if (itemSel == (USHORT)LIT_NONE)
        return;    /* None to process */

    /* Get the message text */
    WinSendMsg(hwndSpyList, LM_QUERYITEMTEXT,
            MPFROM2SHORT(itemSel, sizeof(szTemp)), (MPARAM)(PSZ)szTemp);

    /* Now lets extract the messgae string from the line */
    psz = &szTemp[3];
    while (*psz != ' ')
        psz++;    /* locate first blank */
    *psz = '\0';    /* Zero terminate string */


    /*
     * Loop through all of the items in our list, until we find a
     * string that matches our string.
     */
    pmsgi = rgmsgi; /* Point to start of list */
    for (i = 0; i < cmsgi; i++) {
        if (strcmpi(&szTemp[2], pmsgi->szMsg) == 0) {
            /*
             * Found our message, update the bit of the message, and
             * call the function to let the hook know the new results
             */
            if (fEnable)
                pmsgi->wOptions |= MSGI_ENABLED;
            else
                pmsgi->wOptions &= ~MSGI_ENABLED;

               UpdateHooksMsgTable();      /* Set Spy's Msg table */
            return;
        }
        pmsgi++;
    }
}
