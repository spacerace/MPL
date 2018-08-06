/***************************************************************************\
* options.c - Spy code for several options type dialogs
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define	INCL_WINBUTTONS
#define	INCL_WINDIALOGS
#define	INCL_WINHEAP		/* needed for spy.h */
#define	INCL_WINLISTBOXES
#define	INCL_WINMESSAGEMGR
#define	INCL_WINPOINTERS	/* needed for spy.h */
#include <os2.h>
#include "spyhook.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "spy.h"

/* Prototypes */
MRESULT EXPENTRY SpyOutputsDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY SpySaveListDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AboutWndProc(HWND, USHORT, MPARAM, MPARAM);

/***************************************************************************\
* MRESULT EXPENTRY SpyOutputsDlgProc (hwnd, msg, mp1, mp2)
\***************************************************************************/
MRESULT EXPENTRY SpyOutputsDlgProc(hwnd, msg, mp1, mp2)
HWND            hwnd;
USHORT          msg;
MPARAM          mp1;
MPARAM          mp2;
{
    USHORT      wAction;

    switch (msg) {

    case WM_INITDLG:
        /*
         * Now initialize the output options in the dialog
         */
        WinSendDlgItemMsg(hwnd, DID_WINDOW, BM_SETCHECK,
            (MPARAM)spyopt.fWindow, 0L);
        WinSendDlgItemMsg(hwnd, DID_FILE, BM_SETCHECK,
            (MPARAM)spyopt.fFile, 0L);

        WinSetDlgItemShort(hwnd, DID_WINDOWLINES, spyopt.cWindowLines, FALSE);
        WinSetDlgItemText(hwnd, DID_FILENAME, spystr.szFileName);


        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:

            /*
             * Now retrieve the output options from the
             * dialog
             */
            spyopt.fWindow = (BOOL)SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                DID_WINDOW, BM_QUERYCHECK, 0L, 0L));
            spyopt.fFile = (BOOL)SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                DID_FILE, BM_QUERYCHECK, 0L, 0L));

            WinQueryDlgItemShort(hwnd, DID_WINDOWLINES, &spyopt.cWindowLines, FALSE);
            WinQueryDlgItemText(hwnd, DID_FILENAME,
                sizeof(spystr.szFileName), spystr.szFileName);

            /*
             * Now take care of file operations
             * If a file is already active, we will continue to use it, and
             * ignore the case where the user may have changed file names
             * Will truncate any file.
             */
            if (spyopt.fFile) {
                 if (spyopt.hfileSpy == NULL)
                    if (DosOpen((PSZ)spystr.szFileName, &spyopt.hfileSpy,
                            (USHORT far *)&wAction, 0L, 0,
                            0x0012, 0x00C1, 0L) != 0)
                        spyopt.hfileSpy = NULL; /* Failed on open */
            } else {
                if (spyopt.hfileSpy != NULL) {
                    /* file open, not outputing, close it now */
                    DosClose (spyopt.hfileSpy);
                    spyopt.hfileSpy = NULL;
                }
            }
            /* Fall through to DID_CANCEL */
        case DID_CANCEL:
            /* Now dismiss the dialog */
            WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            break;
        }
        break;

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return 0L;
}




/***************************************************************************\
* MRESULT EXPENTRY SpySaveListDlgProc(hwnd, msg, mp1, mp2)
*
* The Spy Windows Dialog procedure
\***************************************************************************/
MRESULT EXPENTRY SpySaveListDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    HFILE   hfileOut;
    char    szTemp[100];
    char    szTime[10];
    char    szDate[10];
    SHORT   cItems;
    SHORT   iItem;
    USHORT  cch;
    USHORT  cchWritten;
    USHORT  wAction;
    ULONG   lTemp;

    switch (msg) {

    case WM_INITDLG:
        WinSendDlgItemMsg(hwnd, DID_APPEND, BM_SETCHECK,
            (MPARAM)spyopt.fAppend, 0L);
        /* Initialize the dialog items */
        WinSetDlgItemText(hwnd, DID_FILENAME, spystr.szSaveFileName);
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:
            /*
             * Get the file name, and try to open the file,
             * Then loop through and dump the listbox contents to the
             * file.
             */
            spyopt.fAppend = (BOOL)SHORT1FROMMR(WinSendDlgItemMsg(hwnd,
                DID_APPEND, BM_QUERYCHECK, 0L, 0L));

            WinQueryDlgItemText(hwnd, DID_FILENAME,
                sizeof(spystr.szSaveFileName), spystr.szSaveFileName);

            if (DosOpen((PSZ)spystr.szSaveFileName, (HFILE far *)&hfileOut,
                    (USHORT far *)&wAction, 0L, 0,
                    spyopt.fAppend? 0x0011 : 0x0012, 0x00C1, 0L) == 0) {

                /* If append, get to the end of the file */
                if (spyopt.fAppend)
                    DosChgFilePtr(hfileOut, 0L, 2, (PULONG)&lTemp);

                /* Get count of items */
                cItems = SHORT1FROMMR(WinSendMsg(hwndSpyList, LM_QUERYITEMCOUNT,
                            0L, 0L));

                /* Write out a title block to the file */
                _strdate(szDate);
                _strtime(szTime);
                DosWrite(hfileOut,
                        (PSZ)"***************************************\r\n",
                                41, (PUSHORT)&cchWritten);
                cch = sprintf(szTemp, "* Spy: %-10s %-10s          *\r\n",
                    szDate, szTime);
                DosWrite(hfileOut, (PSZ)szTemp, cch, (PUSHORT)&cchWritten);

                DosWrite(hfileOut,
                        (PSZ)"***************************************\r\n",
                                41, (PUSHORT)&cchWritten);


                        /* Now output the list to the file */
                for (iItem = 0; iItem < cItems; iItem++) {
                    cch = SHORT1FROMMR(WinSendMsg(hwndSpyList, LM_QUERYITEMTEXT,
                            MPFROM2SHORT(iItem, sizeof(szTemp)),
                            (MPARAM)(PSZ)szTemp));
                    /* Add Newline at end of string */
                    szTemp[cch++] = '\r';
                    szTemp[cch++] = '\n';
                    szTemp[cch] = '\0';
                    DosWrite(hfileOut, (PSZ)szTemp, cch,
                            (PUSHORT)&cchWritten);
                }
                DosClose(hfileOut);
            }

        case DID_CANCEL:
            /* Now dismiss the dialog */
            WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            break;
        break;
    }

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
    }
    return 0L;
}




/**************************************************************************\
* AboutWndProc(HWND hwnd, USHORT message, MPARAM mp1, MPARAM mp2)
*
* About Spy... Dialog procedure
\***************************************************************************/
MRESULT EXPENTRY AboutWndProc(hwnd, message, mp1, mp2)
HWND    hwnd;
USHORT  message;
MPARAM   mp1;
MPARAM   mp2;
{
    switch (message) {
        case WM_COMMAND:
            WinDismissDlg(hwnd, TRUE);
            break;
        default:
            return(WinDefDlgProc(hwnd, message, mp1, mp2));
            break;
    }
    return 0L;

} /* end aboutwndproc */

