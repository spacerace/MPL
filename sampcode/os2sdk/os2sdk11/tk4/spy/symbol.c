/***************************************************************************\

Module Name: SYMBOL.C

This module contains the code for displaying symbols for a specified address.

\***************************************************************************/

#define INCL_DOSPROCESS
#define	INCL_GPILCIDS
#define	INCL_WINDIALOGS
#define	INCL_WINFRAMEMGR
#define	INCL_WINHEAP			/* needed for spy.h */
#define	INCL_WININPUT
#define	INCL_WINLISTBOXES
#define	INCL_WINMENUS
#define	INCL_WINMESSAGEMGR
#define	INCL_WINPOINTERS		/* needed for spy.h */
#define	INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "spy.h"
#include <spyhook.h>
#include "symbol.h"

WHOISINFO whoIs;
MAPDEF mapdef;
MAPEND mapend;
SEGDEF segdef;
SYMDEF symdef;

extern VOID lstrcat(PSZ, PSZ);
extern void OutputString(char [], SHORT);       /* Output string to output devices */
extern  BOOL FValidPointer (CHAR FAR *pVoid, SHORT cbStruct);
extern  BOOL FGuessValidPointer (CHAR FAR *pVoid, SHORT cbStruct);


extern HWND     hwndSpyFrame;
extern HAB      hab;

/* Function prototypes */
HWND HwndFrameFromPID(PID);
VOID GetSymbolInformation(PID, char far *, char *);
VOID NEAR ProcessLNOkButton(HWND);
MRESULT NEAR FormatLNDialog(HWND, PSWP, PRECTL);
VOID NEAR EndLNDialog(HWND);
MRESULT CALLBACK ListNearDlgProc(HWND, USHORT, MPARAM, MPARAM);


/***************************************************************************\
* hwnd HwndFrameFromPID(pid)
*
* This function attempts to locate a frame window that was
* created by the specified PID. It returns NULL if none are found.
\***************************************************************************/
HWND HwndFrameFromPID(pid)
PID             pid;
{
    HENUM       henum;
    HWND        hwnd;
    PID         pidWindow;
    TID         tidWindow;
    char        szClassName[50];    /* Class name of window */
    CLASSINFO   classinfo;          /* Information about class */

    henum = WinBeginEnumWindows(HWND_DESKTOP);
    if (henum == NULL)
        return (NULL);

    while ((hwnd = WinGetNextWindow(henum)) != NULL) {
        WinQueryWindowProcess(hwnd, &pidWindow, &tidWindow);
        WinLockWindow(hwnd, FALSE);
        if (pid == pidWindow) {
            /* See if a frame window */
            WinQueryClassName(hwnd, sizeof(szClassName),
                (PSZ)szClassName);
            if (WinQueryClassInfo(hab, (PSZ)szClassName, &classinfo) &&
                    (classinfo.flClassStyle & CS_FRAME))
                break;  /* We have our frame */
        }
    }

    WinEndEnumWindows(henum);
    return (hwnd);
}



/***************************************************************************\
* int IdentifyCodeSegment(lpSegInfo, pid, selector)
*
* 	   Identifies the code selector for the specified process.
*          NOTE: The process is currently ignored, but later will somehow
*          return information in the context of the specified process ID.
\***************************************************************************/
int
pascal near IdentifyCodeSegment( pSegInfo, pid, selector )
PSEGINFO   pSegInfo;
PID         pid;
USHORT      selector;
{
    char far *lp;
    char    *np;
    int     n;
    BYTE    c;
    USHORT  segnum;
    USHORT  seg_ptr;
    USHORT  uTemp;
    int     rc;
    PIDINFO pidinfo;
    HWND    hwndPIDFrame;

    /* If new selector matches the current one, then nothing to do */
    if ((pSegInfo->pid == pid) && (pSegInfo->selector == selector))
        return 0;

    /*
     * Try to identify the code selector.  We will first try in the context
     * of our process.  If this fails, or if we find that the segment is
     * associated with our program and it is not our PID, we will attempt
     * to get the symbol in the specified PID context, with the help of
     * our hooks.
     */
    DosGetPID(&pidinfo);
    rc = IdentifyCodeSelector( selector, (PWHOISINFO)&whoIs );
    if ((pid != pidinfo.pid) && (rc == 0)) {
        /*
         * User asked for a different context, and symbol was found.
         * If the symbol was in our .exe we will ignore what we found,
         * and try again.
         */
        /* First get past the module name */
        np = whoIs.names;
        while (*np++)
            ;
        /* Now go through the pathname */
        while (*np) {
            if (*np++ == '.') {
                if (strcmp(np,"EXE") == 0) {
                    rc = 1;     /* It is in our exe, so set errr condition */
                    break;  /* no need to go on */
                }
            }
        }
    }

    if (rc) {
        /*
         * We need to try in other PID context
         */
        if ((pid == pidinfo.pid) ||
                ((hwndPIDFrame = HwndFrameFromPID(pid)) == NULL))
            return (rc);    /* segment not found or no way to find segment */

        /* Lets use the hook */
        SpySetLNSymbolPID(pid, selector);

        /* HACK: Send message to frame known to send another message */
        WinSendMsg(hwndPIDFrame, WM_QUERYTASKFOCUS, 0L, 0L);

        if (SpyGetLNSymbolSelector(&whoIs) != 0)
            return (1);     /* Still did not find the symbol */

    }


    /* It is a valid code selector, make it the new current selector */
    pSegInfo->selector = selector;
    pSegInfo->pid = pid;

    /* Is the new selector in the same module? */
    if (pSegInfo->lpSegName && pSegInfo->whoIs.mte == whoIs.mte)
        pSegInfo->whoIs.segNum = whoIs.segNum;     /* Yes, just copy seg# */
    else {

        /* New module, get segment #, mte handle, module name and pathstring */
        pSegInfo->whoIs    = whoIs;

        /* Generate separate pointers to module name and pathstring */
        lp = &pSegInfo->whoIs.names[0];
        pSegInfo->lpModName = lp;
        while (*lp++)
            ;
        pSegInfo->lpPathName = lp;

        /* Change pathstring extension to be .SYM */
        while (*lp)
            if (*lp++ == '.') {
                *lp++ = 'S';
                *lp++ = 'Y';
                *lp++ = 'M';
                }
        lp++;

        /*
         * No current segment name or symbol name, so make them point to
         * null string
         */
        *lp = 0;
        pSegInfo->lpSegName = lp;
        pSegInfo->lpSymName = 0L;

        /* Close any currently open .SYM file and then open the new .SYM file */
        if (pSegInfo->symfh != -1)
            pSegInfo->symfh = DosClose((HFILE)pSegInfo->symfh );

        if (DosOpen(pSegInfo->lpPathName, (PHFILE)&pSegInfo->symfh,
                (PUSHORT)&uTemp,
                0L, 0, 1, 0x00c0, 0L) == 0) {

            /* Read in .SYM file header and save segment count and pointer */

            DebugFileRead( pSegInfo->symfh, (PSZ)&mapdef, sizeof( mapdef ) );
            pSegInfo->segcnt = mapdef.seg_cnt;
            pSegInfo->segptr = mapdef.seg_ptr;

            /*	Read version of MAPSYM that produced this file.
		If not a version 4 file then ignore the file */

            DebugFileSeek( pSegInfo->symfh, (long)-sizeof( mapend ), 2 );
            DebugFileRead( pSegInfo->symfh, (PSZ)&mapend, sizeof( mapend ) );
            if (mapend.ver != 4)
                pSegInfo->symfh = DosClose( (HFILE)pSegInfo->symfh );
            }
        }

/* Point to segment name buffer and make it a null string for now */

    lp = pSegInfo->lpSegName;
    *lp = 0;

/* If we have an open .SYM file, search for segment definition record */

    if (pSegInfo->symfh != -1 && pSegInfo->whoIs.segNum < pSegInfo->segcnt) {

    /* Start with first segment definition and read segNum definitions */

        seg_ptr = pSegInfo->segptr;
        n = pSegInfo->whoIs.segNum;
        while (n--) {
            DebugFileSeek( pSegInfo->symfh, (long)seg_ptr, 4 );
            DebugFileRead( pSegInfo->symfh, (PSZ)&segdef, sizeof( segdef ) );
            seg_ptr = (USHORT)segdef.nxt_seg;
        }

    /* Now read in the segment name string associated with this segment
       and make it a null terminated string */

        DebugFileRead( pSegInfo->symfh, (PSZ)(lp),
                       (int)((BYTE)segdef.nam_len) );
        lp += segdef.nam_len;
        *lp++ = 0;

    /* Remember the count of symbols for this segment */

        pSegInfo->symcnt = segdef.sym_cnt;


    /* Remember the file offset of the symbols for this segment, which is
       immediately after the segment definition record and name we just read */

        pSegInfo->symFPos = DebugFileSeek( pSegInfo->symfh, 0L, 1 );
        }
    else {
    /* If no .SYM file or segment number too big, then make the segnemt name
       be the hex representation of its number */

        pSegInfo->symcnt = 0;

        segnum = pSegInfo->whoIs.segNum;
        n = 4;
        lp += n;
        while (n--) {
            c = (char)(segnum & (USHORT)0xF);
            segnum >>= 4;
            if (c > 9)
                c += 'A' - 10;
            else
                c += '0';
            *--lp = c;
            *lp++ = 0;
        }

    /* Null terminate the segment name string */

        lp += 4;
        *lp++ = 0;
    }

/* Initialize the symbol name pointer to point to a null string, immediately
   after the segment name string just created above. */

    pSegInfo->lpSymName = lp;
    *lp = 0;

    return 0;
}


USHORT
pascal near FindSymbol( pSegInfo, offset )
PSEGINFO pSegInfo;
USHORT offset;
{
    USHORT i;
    long symPos;
    char far *lp;

/* If no .SYM file then return failure */

    if (pSegInfo->symfh == -1)
        return 0xFFFF;

/* No symbol found yet, so make it a null string */

    lp = pSegInfo->lpSymName;
    *lp = 0;

/* Now do a linear search of the symbol definitions for this segment, looking
   for the closest match.  The definitions are stored in sorted order */

    symPos = DebugFileSeek( pSegInfo->symfh, (long)pSegInfo->symFPos, 0 );
    i = (USHORT)pSegInfo->symcnt;
    while (i--) {
        DebugFileRead( pSegInfo->symfh, (PSZ)&symdef, sizeof( symdef ) );

    /* If this is not our symbol then just remember its offset */

        if ((USHORT)symdef.sym_val <= offset) {
            symPos = DebugFileSeek( pSegInfo->symfh, 0L, 1 ) -
                     sizeof( symdef );

        /* If exact match, exit this loop */

            if ((USHORT)symdef.sym_val == offset)
                break;
            }
        else
        /* If we went past then exit this loop */

            break;

    /* Still looking, so seek past this symbols name string to next symbol
       definition */

        DebugFileSeek( pSegInfo->symfh, (long)symdef.nam_len, 1 );
        }

/* Seek to found symdebol definition and read it in, along with the name */

    DebugFileSeek( pSegInfo->symfh, (long)symPos, 0 );
    DebugFileRead( pSegInfo->symfh, (PSZ)&symdef,
                   sizeof( symdef ) );
    DebugFileRead( pSegInfo->symfh, (PSZ)lp,
                   (int)((BYTE)symdef.nam_len) );

/* Make the name a null terminated string */

    lp += symdef.nam_len;
    *lp++ = 0;

/* Return the distance of the passed offset from the value of the symbol */

    return offset - symdef.sym_val;
}



/*************************************************************************
 *
 * GetSymbolInformation(pid, pvoidInfo, pszInfo)
 *
 * Purpose: To get information about the supplied address
 */
VOID GetSymbolInformation(pid, pvoidInfo, pszInfo)
PID             pid;
char far        *pvoidInfo;
char            *pszInfo;
{
    SEGINFO     seginfo;
    char        szTemp[256];
    USHORT      uOffsetFromSym;


    /*
     * First try to find the segment in the segment table.
     */
    if (IdentifyCodeSegment(&seginfo, pid, SELECTOROF(pvoidInfo)) != 0) {

        /*
         * It failed to find the segment, ie it was invalid, so
         * Simply return INVALID to caller
         */
        sprintf(pszInfo,"PID:%d %04x:%04x - INVALID", (SHORT)pid,
                SELECTOROF(pvoidInfo), OFFSETOF(pvoidInfo));
        return;
    }


    sprintf(pszInfo,"PID:%d %04x:%04x - %Fs ! ", (SHORT)pid,
            SELECTOROF(pvoidInfo), OFFSETOF(pvoidInfo),
            seginfo.lpModName);

    if (seginfo.symfh == -1) {
        sprintf(szTemp,"%04x", seginfo.whoIs.segNum);
        strcat(pszInfo, szTemp);
    } else {
        lstrcat((PSZ)pszInfo, seginfo.lpSegName);
    }

    uOffsetFromSym = FindSymbol(&seginfo, OFFSETOF(pvoidInfo));
    if (uOffsetFromSym != 0xffff) {
        strcat(pszInfo, ":");

        lstrcat((PSZ)pszInfo, seginfo.lpSymName);
        if (uOffsetFromSym != 0) {
            sprintf(szTemp, "+%x", uOffsetFromSym);
            strcat(pszInfo, szTemp);
        }
    }
}


/***************************************************************************\
* VOID NEAR ProcessLNOkButton(hwndDlg)
*
* Process the Ok Button for the List Near Dlg proc
\***************************************************************************/
VOID NEAR ProcessLNOkButton(hwndDlg)
HWND hwndDlg;
{
    char        szTemp[100];
    char far    *pVoid;
    PID         pid;
    SHORT       item;

    /*
     * For now try using sscanf to convert the string into a
     * pointer.
     */
    WinQueryDlgItemShort(hwndDlg, DID_PID, &pid, FALSE);
    WinQueryDlgItemText(hwndDlg, DID_ADDR, sizeof(szTemp), szTemp);
    if ((strchr(szTemp,':') != NULL) && (sscanf(szTemp,"%p", &pVoid)) > 0) {
        /* It translated, so try to get info about pointer */
        GetSymbolInformation(pid, pVoid, szTemp);

        /* For now simply output to output areas */
        item = (SHORT)WinSendDlgItemMsg(hwndDlg, DID_SYMLIST,
                LM_INSERTITEM, (MPARAM)LIT_END, (MPARAM)(PSZ)szTemp);

        WinSendDlgItemMsg(hwndDlg, DID_SYMLIST, LM_SETTOPINDEX,
                (MPARAM)item, 0L);
    }

    /*
     * Now clear the text out of the ADDR field, and set the focus
     * back to the window
     */
    WinSetDlgItemText(hwndDlg, DID_ADDR, "");
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwndDlg, DID_ADDR));
}

/***************************************************************************\
* MRESULT NEAR FormatLNDialog(hwndDlg, paswp, prectl)
*
* Process the WM_SIZE message in the List Near Dialog
\***************************************************************************/
MRESULT NEAR FormatLNDialog(hwndDlg, paswp, prclClientX)
HWND                hwndDlg;
PSWP                paswp;
PRECTL              prclClientX;
{
    USHORT          cy;
    USHORT          cswp;
    HPS             hps;
    SHORT           dy;
    register HWND   hwndT;
    FONTMETRICS     fm;
    PSWP            pswpT;
    RECTL           rclClient;
    SWP             swpOk;
    SHORT           y;

    /*
     * First let the default frame processing set up the position of
     * all of the standard controls.  Then we will position the list box
     * and the two entry fields to fill in the remainder of the space.
     */

#define PSWPLN(x) (paswp + cswp + x - DID_SYMLIST)


    cswp = (USHORT)WinDefDlgProc(hwndDlg, WM_FORMATFRAME, (MPARAM)paswp,
            (MPARAM)(PRECTL)&rclClient);
    if (prclClientX != NULL)
        *prclClientX = rclClient;   /* Give caller info */


    /*
     * Now Add our controls to the SWP list - Verify that the first one
     * exists. If not, we probably are initializing the dialog, so
     * Simply return what the default Dlg Proc returned.
     */
    hwndT = WinWindowFromID(hwndDlg, DID_OK);
    if (hwndT == NULL)
        return (cswp);

    /*
     * We need to get a PS, such that we can find out the size of the
     * font, to adjust our edit fields such that they wont keep moving
     * in.
     */
    hps = WinGetPS(hwndDlg);
    GpiQueryFontMetrics(hps, (LONG)sizeof(fm), &fm);
    WinReleasePS(hps);

    WinQueryWindowPos(hwndT, &swpOk);

    pswpT=PSWPLN(DID_SYMLIST);
    WinQueryWindowPos(hwndT = WinWindowFromID(hwndDlg, DID_SYMLIST), pswpT);
    dy = pswpT->y - (swpOk.y + swpOk.cy);
    pswpT->fs = SWP_SIZE;
    pswpT->cx = (SHORT)rclClient.xRight - (SHORT)rclClient.xLeft - pswpT->x;

    /* First position the top line, just below titlebar */
    pswpT=PSWPLN(DID_PIDLABEL);
    WinQueryWindowPos(hwndT = WinWindowFromID(hwndDlg, DID_PIDLABEL), pswpT);
    pswpT->fs = SWP_MOVE;
    cy = pswpT->cy;
    y = pswpT->y = (SHORT)rclClient.yTop - (SHORT)rclClient.yBottom -
            cy - dy;

    pswpT=PSWPLN(DID_PID);
    WinQueryWindowPos(hwndT = WinWindowFromID(hwndDlg, DID_PID), pswpT);
    pswpT->fs = SWP_MOVE;
    pswpT->x += (SHORT)fm.lAveCharWidth / 2; /* Take care of margin */
    pswpT->y = y;

    /* Since Edit control is bigger Setup Start Y for the next row */
    y += (cy - pswpT->cy);


    /* Now position Addr line below PID line */
    pswpT=PSWPLN(DID_ADDRLABEL);
    WinQueryWindowPos(hwndT = WinWindowFromID(hwndDlg, DID_ADDRLABEL), pswpT);
    pswpT->fs = SWP_MOVE;
    y = (pswpT->y = y - pswpT->cy - dy);

    pswpT=PSWPLN(DID_ADDR);
    WinQueryWindowPos(hwndT = WinWindowFromID(hwndDlg, DID_ADDR), pswpT);
    pswpT->fs = SWP_MOVE;
    pswpT->x += (SHORT)fm.lAveCharWidth / 2; /* Take care of margin */
    pswpT->y = y;

    /* Now set size of listbox */
    y += (cy - pswpT->cy);

    pswpT=PSWPLN(DID_SYMLIST);
    pswpT->cy = y - dy - pswpT->y;


    /*
     * Return the count of swps
     */
    return ((MRESULT)(cswp + (DID_PID-DID_SYMLIST+1)));
}




/***************************************************************************\
* VOID NEAR EndLNDialog(hwndDlg)
*
* End processing for the List Near Dialog
\***************************************************************************/
VOID NEAR EndLNDialog(hwndDlg)
HWND hwndDlg;
{
    /* First Dismiss the dialog and destroy the windows */
    WinDismissDlg(hwndDlg, 0);
    WinDestroyWindow(hwndDlg);

    /* And re-enable the menu item */
    WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
        MM_SETITEMATTR, MPFROM2SHORT(CMD_LISTNEAR, TRUE),
        MPFROM2SHORT(MIA_DISABLED, 0));

}



/***************************************************************************\
* MRESULT CALLBACK ListNearDlgProc(hwnd, msg, mp1, mp2)
*
* Will locate the symbol near the specified address.
\***************************************************************************/
MRESULT CALLBACK ListNearDlgProc(hwnd, msg, mp1, mp2)
HWND            hwnd;
USHORT          msg;
MPARAM          mp1;
MPARAM          mp2;
{
    PIDINFO     pidinfo;

    switch (msg) {

    case WM_INITDLG:
        /* Initialize the dialog items */
        WinSetDlgItemText(hwnd, DID_ADDR, "");
        DosGetPID(&pidinfo);
        WinSetDlgItemShort(hwnd, DID_PID, (USHORT)pidinfo.pid, FALSE);
        break;

    case WM_SYSCOMMAND:
        if (SHORT1FROMMP(mp1) == SC_CLOSE) {
            /* End processing the dialog */
            EndLNDialog(hwnd);
            break;
        } else
            return(WinDefDlgProc(hwnd, msg, mp1, mp2));


    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:

            /*
             * Process The Ok Button, by caling ProcessLNOkButton, which
             * will add the information to the listbox in our dialog.
             */
            ProcessLNOkButton(hwnd);
            break;

        case DID_CANCEL:
            /* End processing the dialog */
            EndLNDialog(hwnd);
            break;
        }

        break;

    /*
     * We want to add our controls into the formating of the frame, so
     * we need to add some room onto the SWP list
     */
    case WM_QUERYFRAMECTLCOUNT:
        return((MRESULT)((SHORT)WinDefDlgProc(hwnd, msg, mp1, mp2) +
                (SHORT)(DID_PID-DID_SYMLIST+1)));
        break;

    /*
     * Call FormatLNDialog, to format the dialog
     */
    case WM_FORMATFRAME:
        return (FormatLNDialog(hwnd, (PSWP)mp1, (PRECTL)mp2));

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
    }

    return (0L);
}
