/***************************************************************************\
* spy.c - Spy application
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define INCL_DOSPROCESS
#define	INCL_WINDIALOGS
#define	INCL_WINFRAMEMGR
#define	INCL_WINHEAP
#define	INCL_WININPUT
#define	INCL_WINLISTBOXES
#define	INCL_WINMENUS
#define	INCL_WINMESSAGEMGR
#define	INCL_WINPOINTERS
#define	INCL_WINSHELLDATA
#define	INCL_WINSYS
#define	INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spyhook.h>
#include <time.h>
#include "spy.h"

/* File Local Variables */
HAB         hab;
HMQ         hmqSpy;
HWND        hwndSpy;
HWND        hwndSpyFrame;
HWND        hwndSpyList = NULL;
HWND        hwndWindowLB;
HWND        hwndMessageLB;
HHEAP       hHeap;
SHORT       cxBorder;
SHORT       cyBorder;

HPOINTER    hptrArrow;
HPOINTER    hptrSelWin;

USHORT      iCurItemFocus;              /* Index to item that has the focus */
BOOL        fSpyActive = MIA_CHECKED;   /* Any non-zero is true */
BOOL        fTrackingListBox = FALSE;   /* Tracking windows active ? */
BOOL        fAllFrames = 0;             /* Are we processing all frames ? */
BOOL        fAllWindows = 0;            /* Are we processing all windows ? */


HWND        hwndWinDlgDisp = NULL;      /* hwnds info in Window Dialog */

SHORT       wDumpCount = 0;             /* Count of which window is being dumped */
SPWD        *pspwd = NULL;


/* Define memory semaphore to have second thread sleep on */
ULONG       semThread = 0L;             /* Thread to wait on */
int         AboutCount = 0;
UCHAR       rgMsgData[MAXMSGBYTES];     /* Max bytes to extract per message */


char szSpyClass[] = "Spy";
char szTitle[] = "";

/* Function Prototypes */
int	cdecl main(int, char **);
void    FAR ProcHookThread(void);    /* will process the hook messages */
void    ProcessQueueMsg(QMSGSPY *);
void    UpdateMsgBoxCurMsgText(HWND);
void    InitializeOptions(int, char **);    /* initialize Spy initial state */
PSZ     DumpParam(PSZ prgData, MPARAM mp, SHORT cb, UCHAR bMPType);
MRESULT CALLBACK SpyWndProc(HWND, USHORT, MPARAM, MPARAM);
VOID	SpyPaint(VOID);


/***************************************************************************\
* int cdecl main (argc, argv)
*
* Spy Main function
\***************************************************************************/
int cdecl main(argc, argv)
int argc;
char **argv;
{
    ULONG   flCreateFlags;
    QMSG    qmsg;
    RECTL   rcl;
    TID     tid;
    char    *prgStack;


    hab = WinInitialize(0);

    hmqSpy = WinCreateMsgQueue(hab, 0);

    if (!WinRegisterClass((HAB)NULL, szSpyClass, (PFNWP)SpyWndProc,
            CS_SYNCPAINT, 0)) {
        WinAlarm(HWND_DESKTOP, 0xffff);
        return(0);
    }


    /*
     * Create a heap for the program
     */
    hHeap = WinCreateHeap(0, 0, 0, 0, 0, 0);

    /*
     * Create a stack for the thread - also initialize the stack by zeroing
     * the first 32 bytes, and filling the remainder with a known value
     */
    prgStack = WinAllocMem(hHeap, CBSTACK);
    if (prgStack == NULL)
        goto Abort;
    memset(prgStack, '\0', 32); /* Init first 32 bytes to zero */
    memset(prgStack+32,'\345', CBSTACK-32); /* Remainder to known value */

    hptrArrow = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, TRUE);
    cxBorder = (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
    cyBorder = (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);

    hptrSelWin = WinQuerySysPointer(HWND_DESKTOP, SPTR_MOVE, TRUE);

    SpyInstallHook(hab, hmqSpy, spyopt.bHooks);
    SpySetAllWindowOpt (fAllWindows);
    SpySetAllFrameOpt (fAllFrames);

    flCreateFlags = FCF_STANDARD;
    hwndSpyFrame = WinCreateStdWindow(HWND_DESKTOP,
            WS_VISIBLE,
            (VOID FAR *)&flCreateFlags,
            szSpyClass, szTitle,
            WS_VISIBLE,
            (HMODULE)NULL, IDR_SPY,
            (HWND FAR *)&hwndSpy);

    WinQueryWindowRect(hwndSpy, &rcl);
    hwndSpyList = WinCreateWindow (hwndSpy, WC_LISTBOX, "",
            WS_VISIBLE | LS_NOADJUSTPOS,
            -cxBorder, -cyBorder,
            (SHORT)(rcl.xRight - rcl.xLeft) + 2 * cxBorder,
            (SHORT)(rcl.yTop - rcl.yBottom) + 2 * cyBorder,
            hwndSpy, HWND_TOP, DID_SPYLIST, NULL, NULL);

    /*
     * Read the os2.ini information if it exists, and set the menu items
     * to correspond to the initial state read from OS2.INI.
     */
    InitializeOptions(argc, argv);    /* initialize Spy initial state */

    /*
     * Set the focus to the list box.  Note: Only call WinSetFocus if
     * our frame is the active window.  As we may have been started in
     * the background.  If this is the case, we want to set the frame's
     * focus save to the listbox, such that it will be the active window
     * when our frame is activated.
     */
    if (WinQueryWindow(HWND_DESKTOP, QW_TOP, FALSE) == hwndSpyFrame)
        WinSetFocus(HWND_DESKTOP, hwndSpyList);
    else
        WinSetWindowULong(hwndSpyFrame, QWL_HWNDFOCUSSAVE,
            (ULONG)hwndSpyList);



    /* Start the thread that will process the messages from the hook */
    DosCreateThread(ProcHookThread, (PTID)&tid,
        (PBYTE)(prgStack + CBSTACK - 1));

    UpdateHooksMsgTable();      /* Set Spy's Message Table */
    SpyHookOnOrOff (TRUE);      /* Turn the hook on */


   /*
    * Now process all of the messages
    */
    while (WinGetMsg(NULL, (PQMSG)&qmsg, NULL, 0, 0)) {
        WinDispatchMsg(NULL, (PQMSG)&qmsg);
    }

    SpyReleaseHook (TRUE);      /* Release input hook */

    WinDestroyWindow(hwndSpyFrame);

    WinDestroyPointer(hptrArrow);
    WinDestroyPointer(hptrSelWin);

Abort:
    WinDestroyMsgQueue(hmqSpy);
    WinTerminate(hab);

    /* If the spy output file is open, close it now */
    if (spyopt.hfileSpy != NULL)
        DosClose(spyopt.hfileSpy);


    DosExit(EXIT_PROCESS, 0);
}




/***************************************************************************\
* InitializeOptions(argc, argv)
*
* Initialize spy, first from the default options, second from
* OS2.INI file, and override from command switches.
\***************************************************************************/
VOID InitializeOptions(argc, argv)
int argc;
char **argv;
{
    USHORT      cch;
    USHORT      wAction;
    HWND        hwndMenu;


    /*
     * If the OS2.INI information exists, initialize our options to
     * the stored values.
     */
    if (WinQueryProfileSize (hab, "Spy", "Options", &cch) == 0) {
        cch = sizeof(SPYOPT);

        WinQueryProfileData(hab, "Spy", "Options", (PSZ)&spyopt,
                &cch);
        WinQueryProfileString(hab, "Spy", "FileName", "spy.out",
                (PSZ)spystr.szFileName, sizeof(spystr.szFileName));
        WinQueryProfileString(hab, "Spy", "SaveFileName", "spy.lis",
                (PSZ)spystr.szSaveFileName, sizeof(spystr.szSaveFileName));
    }

    /*
     * Then check for command line overrides
     */
    while  (argc > 1) {
        argv++; /* get beyond the program name */

        /* Test for send message hook flag */
        if (!strcmpi(*argv, "+s"))
            spyopt.bHooks |= SPYH_SENDMSG;
        if (!strcmpi(*argv, "-s"))
            spyopt.bHooks &= ~SPYH_SENDMSG;

        /* Test for input hook flag */
        if (!strcmpi(*argv, "+i"))
            spyopt.bHooks |= SPYH_INPUT;
        if (!strcmpi(*argv, "-i"))
            spyopt.bHooks &= ~SPYH_INPUT;

        argc--;
    }

    /*
     * Now we need to update the menu items to the final
     * state
     */
    hwndMenu = WinWindowFromID(hwndSpyFrame, FID_MENU);

    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_INPUTHOOK, TRUE),
            MPFROM2SHORT(MIA_CHECKED,
             (spyopt.bHooks & SPYH_INPUT) ? MIA_CHECKED : 0));
    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDMSGHOOK, TRUE),
             MPFROM2SHORT(MIA_CHECKED,
             (spyopt.bHooks & SPYH_SENDMSG) ? MIA_CHECKED : 0));

    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDEXTEND, TRUE),
            MPFROM2SHORT(MIA_CHECKED,
             spyopt.fSendExtend ? MIA_CHECKED : 0));

    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDSTACK, TRUE),
            MPFROM2SHORT(MIA_CHECKED,
            spyopt.fSendStack ? MIA_CHECKED : 0));
    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_OUTSCREEN, TRUE),
            MPFROM2SHORT(MIA_CHECKED, spyopt.fWindow ? MIA_CHECKED : 0));

    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_OUTFILE, TRUE),
            MPFROM2SHORT(MIA_CHECKED, spyopt.fFile ? MIA_CHECKED : 0));

    WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(CMD_ALPHASORT, TRUE),
            MPFROM2SHORT(MIA_CHECKED,
                    spyopt.fAlphaSortMsgList ? MIA_CHECKED : 0));

    /*
     * If the options specify output to file, open the file now
     */
    if (spyopt.fFile) {
        if (DosOpen((PSZ)spystr.szFileName, &spyopt.hfileSpy,
                (USHORT far *)&wAction, 0L, 0,
                0x0012, 0x00C1, 0L) != 0)
            spyopt.hfileSpy = NULL; /* Failed on open */
    }
}


/***************************************************************************\
* MRESULT CALLBACK SpyWndProc(hwnd, msg, mp1, mp2)
*
* Spy Client window procedure
\***************************************************************************/
MRESULT CALLBACK SpyWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    QMSGSPY qmsgspy;
    SHORT   cBytes;
    USHORT      wAction;

    switch (msg) {
    case WM_CREATE:
        /* Set up this global first thing in case we need it elsewhere */
        hwndSpy = hwnd;
        break;

    case WM_SEM2:
        /*
         * Other thread told use there are some messages out there.  Loop
         * through and process all of the pending messages, and output
         * the listbox position at the end.  Also make sure to flush
         * the file buffer before we go back to sleep.
         */
        while (SpyGetNextMessage(&qmsgspy, rgMsgData, sizeof(rgMsgData), 0L)) {
            ProcessQueueMsg(&qmsgspy);
        }

        if (spyopt.fFile)
            DosBufReset(spyopt.hfileSpy);

        DosSemClear((HSEM)(PULONG)&semThread);

        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case CMD_ACTIVE:

            /*
             * THe active menu item was selected, we will toggle the
             * the selection by setting active to 0 or MIA_CHECKED.
             * Call the hook, and then update the checkmark on the menu
             */
            fSpyActive ^= MIA_CHECKED;  /* Toggle on or off */
            SpyHookOnOrOff (fSpyActive);
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_ACTIVE, TRUE),
                MPFROM2SHORT(MIA_CHECKED, fSpyActive));

            break;
        case CMD_ABOUT:
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)AboutWndProc, NULL, AboutDlg, (PCH)NULL);
            break;

        case CMD_EXIT:
            WinPostMsg(NULL, WM_QUIT, 0L, 0L);
            break;

        case CMD_CLRWIN:
            /*
             * Delete all items in the list.  Simply do this
             * By deleting the first item, until the count goes to
             * zero
             */
            WinSendMsg(hwndSpyList, LM_DELETEALL, 0L, 0L);
            break;

        case CMD_SAVEWIN:
            SpyHookOnOrOff (FALSE);
            WinDlgBox(HWND_DESKTOP, hwndSpyFrame,
                (PFNWP)SpySaveListDlgProc, (HMODULE)NULL,
                SaveListDlg, (PCH)NULL);
            SpyHookOnOrOff (fSpyActive);
            break;

        /*
         * This command saves out the current options to OS2.ini
         */
        case CMD_SAVEOPT:
            WinWriteProfileData(hab, "Spy", "Options", (PSZ)&spyopt,
                    sizeof(SPYOPT));
            WinWriteProfileString(hab, "Spy", "FileName",
                    (PSZ)spystr.szFileName);
            WinWriteProfileString(hab, "Spy", "SaveFileName",
                    (PSZ)spystr.szSaveFileName);

            break;

        case CMD_LISTNEAR:
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_LISTNEAR, TRUE),
                MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            WinLoadDlg(HWND_DESKTOP, hwndSpyFrame,
                (PFNWP)ListNearDlgProc, (HMODULE)NULL,
                ListNearDlg, (PCH)NULL);
            break;

        case CMD_WINDOWS:
            SpyHookOnOrOff (FALSE);
            hwndWindowLB = NULL;
            iCurItemFocus = (USHORT)-1;
            WinDlgBox(HWND_DESKTOP, hwndSpyFrame,
                (PFNWP)SpyWindowsDlgProc, (HMODULE)NULL,
                WindowsDlg, (PCH)NULL);

            SpyHookOnOrOff (fSpyActive);
            break;

        case CMD_QUEUES:
            SpyHookOnOrOff (FALSE);
            hwndWindowLB = NULL;
            iCurItemFocus = (USHORT)-1;
            WinDlgBox(HWND_DESKTOP, hwndSpyFrame,
                (PFNWP)SpyQueuesDlgProc, (HMODULE)NULL,
                MsgQueueDlg, (PCH)NULL);

            SpyHookOnOrOff (fSpyActive);
            break;

        case CMD_WNMSSEL:
        case CMD_WNMSDSL:
            SpyHookOnOrOff (FALSE);
            SelOrDeselWithMouse(SHORT1FROMMP(mp1) == CMD_WNMSSEL);
            SpyHookOnOrOff (fSpyActive);
            break;

        case CMD_ALLWNDWS:

            /*
             * The user selected the ALLFRAMES, toggle the state, and
             * update the menu and the hook state.
             */
            fAllWindows ^= MIA_CHECKED;  /* Toggle on or off */
            SpySetAllWindowOpt (fAllWindows);
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_ALLWNDWS, TRUE),
                MPFROM2SHORT(MIA_CHECKED, fAllWindows));
            break;

        case CMD_ALLFRAMES:

            /*
             * The user selected the ALLFRAMES, toggle the state, and
             * update the menu and the hook state.
             */
            fAllFrames ^= MIA_CHECKED;  /* Toggle on or off */
            SpySetAllFrameOpt (fAllFrames);
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_ALLFRAMES, TRUE),
                MPFROM2SHORT(MIA_CHECKED, fAllFrames));
            break;

        case CMD_WNDPWIN:
            wDumpCount = 0;
            DumpOneWindowInfo();
            break;

        case CMD_WNDPALL:
            wDumpCount = 0;
            pspwd = (SPWD *)WinAllocMem(hHeap, sizeof(SPWD)* MAXSPYDUMP);
            cBytes = DumpAllWindowsInfo(HWND_DESKTOP, 0);
            cBytes += DumpAllWindowsInfo(HWND_OBJECT, -10);
            DumpWindowIndex(cBytes);
            WinFreeMem(hHeap, (char *)pspwd, sizeof(SPWD)* MAXSPYDUMP);
            break;

        case CMD_MESSAGES:
            SpyHookOnOrOff (FALSE);
            WinDlgBox(HWND_DESKTOP, hwndSpyFrame,
                (PFNWP)SpyMsgDlgProc, (HMODULE)NULL,
                MessagesDlg, (PCH)NULL);
            SpyHookOnOrOff (fSpyActive);
            break;

        case CMD_ALPHASORT:
            spyopt.fAlphaSortMsgList ^= TRUE;

            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                    MM_SETITEMATTR, MPFROM2SHORT(CMD_ALPHASORT, TRUE),
                    MPFROM2SHORT(MIA_CHECKED,
                            spyopt.fAlphaSortMsgList ? MIA_CHECKED : 0));
            break;

        /*
         * The command in this section are defined in the Hooks Menu.
         * All of these items toggle selections on or off.  The first two
         * items must be registered with the input hook.  The last two simply
         * retrict how much information is displayed for send messages.
         */

        case CMD_INPUTHOOK:
            spyopt.bHooks ^= SPYH_INPUT;
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_INPUTHOOK, TRUE),
                MPFROM2SHORT(MIA_CHECKED,
                     (spyopt.bHooks & SPYH_INPUT) ? MIA_CHECKED : 0));
                SpyReleaseHook (FALSE);     /* Dont clear queue */
                SpyInstallHook(hab, hmqSpy, spyopt.bHooks);
            break;

        case CMD_SENDMSGHOOK:
            spyopt.bHooks ^= SPYH_SENDMSG;
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDMSGHOOK, TRUE),
                MPFROM2SHORT(MIA_CHECKED,
                     (spyopt.bHooks & SPYH_SENDMSG) ? MIA_CHECKED : 0));
                SpyReleaseHook (FALSE);     /* Dont clear queue */
                SpyInstallHook(hab, hmqSpy, spyopt.bHooks);
            break;

        case CMD_SENDEXTEND:
            spyopt.fSendExtend ^= 1;  /* Toggle on or off */
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDEXTEND, TRUE),
                MPFROM2SHORT(MIA_CHECKED,
                     spyopt.fSendExtend ? MIA_CHECKED : 0));
            break;

        case CMD_SENDSTACK:
            spyopt.fSendStack ^= 1;  /* Toggle on or off */
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_SENDSTACK, TRUE),
                MPFROM2SHORT(MIA_CHECKED,
                    spyopt.fSendStack ? MIA_CHECKED : 0));
            break;

        /*
         * The commands in this section are defined in the Outputs Menu.
         * The first 3 items simply toggle outputs on or off, where the
         * last item allows the user to change all of the output options.
         */
        case CMD_OUTSCREEN:
            spyopt.fWindow ^= 1;  /* Toggle on or off */
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_OUTSCREEN, TRUE),
                MPFROM2SHORT(MIA_CHECKED, spyopt.fWindow ? MIA_CHECKED : 0));
            break;

        case CMD_OUTFILE:
            spyopt.fFile ^= 1;  /* Toggle on or off */
            WinSendMsg(WinWindowFromID(hwndSpyFrame, FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT(CMD_OUTFILE, TRUE),
                MPFROM2SHORT(MIA_CHECKED, spyopt.fFile ? MIA_CHECKED : 0));
            /*
             * Open or close the output file
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
            break;

        case CMD_OUTPUTS:
            SpyHookOnOrOff (FALSE);
            WinDlgBox(HWND_DESKTOP, hwndSpyFrame,
                (PFNWP)SpyOutputsDlgProc, (HMODULE)NULL,
                OutputsDlg, (PCH)NULL);
            SpyHookOnOrOff (fSpyActive);
            break;

        case CMD_MGDABLE:
        case CMD_MGEABLE:
            EnableOrDisableMsg(SHORT1FROMMP(mp1) == CMD_MGEABLE);
            break;
        }

        break;

    case WM_SIZE:
        /* We need to resize the listbox, if it exists */
        if (hwndSpyList != NULL) {
            WinSetWindowPos(hwndSpyList, HWND_TOP, -cxBorder, -cyBorder,
                SHORT1FROMMP(mp2) + 2 * cxBorder,
                SHORT2FROMMP(mp2) + 2 * cyBorder, SWP_MOVE | SWP_SIZE);
        }

        /* Now fall through to process the message */
    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0L);
}




/***************************************************************************\
* USHORT UConvertStringToNum(psz)
*
* Converts the passed string to a number 0xffff if not number
\***************************************************************************/
USHORT  UConvertStringToNum(psz)
register char   *psz;
{
    register USHORT uNum;

    /*
     * If the first few chars are 0x, we assume the user typed in a
     * HEX number, else if 0-9, we assume decimal, else we use the string.
     */
    /* First see if digit in first position */
    if ((*psz >= '0') && (*psz <= '9')) {
        /* Assume numbers now */
        if ((*psz == '0') && (*(psz+1) == 'x')) {

            /* We are in hex mode */
            psz += 2;
            uNum = 0;
            for (;;) {
                if ((*psz >= '0') && (*psz <= '9'))
                    uNum = uNum * 16 + (USHORT)(*psz - '0');
                else if ((*psz >= 'a') && (*psz <= 'f'))
                    uNum = uNum * 16 + (USHORT)(*psz - 'a');
                else if ((*psz >= 'F') && (*psz <= 'F'))
                    uNum = uNum * 16 + (USHORT)(*psz - 'A');
                else
                    break;
                psz++;
            }

        } else {
            /* Decimal mode */
            uNum = (USHORT)(*psz++ - '0');
            while ((*psz >= '0') && (*psz <= '9')) {
                uNum = uNum * 10 + (USHORT)(*psz++ - '0');
            }
        }

        return (uNum);
    } else
        /* Not num, return 0xffff */
        return (0xffff);
}





/***************************************************************************\
* void ProcHookThread()
*
*   This function will wait for the hook to have messages,  when it
*   does, it will set a memory semaphore, post a WM_SEM1 message to the other
*   thread, and wait for the other thread has processed all of the messages.
\***************************************************************************/

void FAR ProcHookThread()
{
    while (TRUE) {
        /*
         * Wait for a message to become available.
         */
        if (!SpyGetNextMessage(NULL, NULL, 0, -1L))
            break;

        /*
         * Now we have a message, set our semaphore, Post a WM_SEM2
         * message to the Client window, and wait for the client to
         * clear the semaphore.
         */
        DosSemSet((HSEM)(PULONG)&semThread);
        WinPostMsg(hwndSpy, WM_SEM2, (MPARAM)1, (MPARAM)1);
        DosSemWait((HSEM)(PULONG)&semThread, -1L);
    }

    DosExit(EXIT_THREAD, 0);
}




/***************************************************************************\
* void ProcessQueueMsg(pqmsg)
*
*   This function will process the hook, by calling the hooks which get
*   messages.  We will than post the message to the current output destinations.
\***************************************************************************/

void ProcessQueueMsg(pqmsgspy)
    QMSGSPY    *pqmsgspy;
{
    MSGI    *pmsgi;
    SHORT   item;
    CHAR    cSource;
    CHAR    cThread;
    char    szNextMessage[100];
    char    szTime[12];
    SHORT   cch;
    CHAR    bAscii;
    PSZ     prgData;



    /*
     * Now let's build the message to output
     */
    if (WinIsWindow(hab, pqmsgspy->qmsg.hwnd)) {
        if (WinIsChild(pqmsgspy->qmsg.hwnd, hwndSpy))
            return;     /* don't want endless loops */
    }

    cThread = ':';
    if (pqmsgspy->qmsg.time == (ULONG)-1) {
        /* Sent message */
        szTime[0] = '\0';
        cSource = 'S';
        if (pqmsgspy->fs)
            cThread = '*';
    } else {
        cSource = 'I';
        if (pqmsgspy->fs != PM_REMOVE)
            cThread = '-';  /* Show different for non-remove */

        sprintf (szTime, "%-08lx", pqmsgspy->qmsg.time);
    }

    if ((pmsgi = PmsgiFromMsg(pqmsgspy->qmsg.msg)) == NULL) {
        /*
         * Message not in list, use default
         */
        cch = sprintf(szNextMessage,
            "%c%cMSG:0x%04x            H:%04x 1:%08lx 2:%08lx T:%s",
            cSource, cThread, (SHORT)pqmsgspy->qmsg.msg, (SHORT)pqmsgspy->qmsg.hwnd,
            pqmsgspy->qmsg.mp1, pqmsgspy->qmsg.mp2, szTime);
    } else if (pmsgi->wOptions & MSGI_MOUSE) {
        /*
         * Mouse message, decode to mouse types
         */
        cch = sprintf(szNextMessage,
            "%c%c%-20s H:%04x X:%-4d Y:%-4d HT:%04x T:%s",
            cSource, cThread, pmsgi->szMsg, (SHORT)pqmsgspy->qmsg.hwnd,
            SHORT1FROMMP(pqmsgspy->qmsg.mp1), SHORT2FROMMP(pqmsgspy->qmsg.mp1),
            SHORT1FROMMP(pqmsgspy->qmsg.mp2), szTime);
    } else if (pmsgi->wOptions & MSGI_KEY) {
        /*
         * Key messages, output special
         */
        bAscii = (CHAR)SHORT1FROMMP(pqmsgspy->qmsg.mp2);
        if ((bAscii < ' ') || (bAscii > '~'))
            bAscii = ' ';

        cch = sprintf(szNextMessage,
            "%c%c%-20s H:%04x F:%04x R:%d S:%2x C:%04x(%c) V:%02x T:%s",
            cSource, cThread, pmsgi->szMsg, (SHORT)pqmsgspy->qmsg.hwnd,
            SHORT1FROMMP(pqmsgspy->qmsg.mp1),
            CHAR3FROMMP(pqmsgspy->qmsg.mp1),  CHAR4FROMMP(pqmsgspy->qmsg.mp1),
            SHORT1FROMMP(pqmsgspy->qmsg.mp2), bAscii,
            SHORT2FROMMP(pqmsgspy->qmsg.mp2), szTime);
    } else {
        /* No special format */
        cch = sprintf(szNextMessage,
            "%c%c%-20s H:%04x 1:%08lx 2:%08lx T:%s",
            cSource, cThread, pmsgi->szMsg, (SHORT)pqmsgspy->qmsg.hwnd,
            pqmsgspy->qmsg.mp1, pqmsgspy->qmsg.mp2, szTime);
    }

    OutputString(szNextMessage, cch);

    /*
     * Now dump out any additional information associated with the
     * message.  The processing depends of the type of message on
     * how we are going to process the data.
     */
    if (spyopt.fSendExtend) {
        prgData = DumpParam((PSZ)rgMsgData, pqmsgspy->qmsg.mp1,
                pqmsgspy->cbDataMP1, pqmsgspy->bMPType);
        DumpParam(prgData, pqmsgspy->qmsg.mp2, pqmsgspy->cbDataMP2,
                (UCHAR)((pqmsgspy->bMPType) >> 3));
    }

    /*
     * If this is a send message, also display the call stack information
     * of who called WinSendMsg
     */
    if (spyopt.fSendStack && (pqmsgspy->qmsg.time == (ULONG)-1)) {
        cch = sprintf(szNextMessage,
            "    PID: %-3d TID: %-2d   Stack:",
            pqmsgspy->pidSend, pqmsgspy->tidSend);

        /* Now loop and add the stack info */
        for (item=0; (item < MAXSTRACE) &&
                (pqmsgspy->pvoidStack[item] != NULL); item++) {
            cch += sprintf(szTime, " %p", pqmsgspy->pvoidStack[item]);
            strcat(szNextMessage, szTime);
        }

        OutputString(szNextMessage, cch);
    }

}




/***************************************************************************\
* PSZ DumpParam(PSZ prgData, MPARAM mp, SHORT cb, UCHAR bMPType)
*
* Dump the additional information that was captured for the message.
*   using the currently defined types.
*
* Returns: PSZ - Pointer to next available byte after process DATA
\***************************************************************************/
PSZ DumpParam(prgData, mp, cb, bMPType)
PSZ         prgData;
MPARAM      mp;
SHORT       cb;
UCHAR       bMPType;
{
    char    szNextMessage[100];
    SHORT   cch;


    if (FGuessValidPointer((PSZ)mp, cb)) {
        /* Process by type */
        switch (bMPType & 0x07) {
        case MPT_SWP:
            cch = sprintf(szNextMessage,
                "    SWP:               fs:%04x cx:%d cy:%d y:%d x:%d HB:%04x H:%04x",
                ((PSWP)prgData)->fs, ((PSWP)prgData)->cy, ((PSWP)prgData)->cx,
                ((PSWP)prgData)->y, ((PSWP)prgData)->x,
                (SHORT)((PSWP)prgData)->hwndInsertBehind,
                (SHORT)((PSWP)prgData)->hwnd);

            break;

        case MPT_RECTL:
            cch = sprintf(szNextMessage,
                "    RECTL:             xLeft:%d yBottom:%d xRight:%d yTop:%d",
                ((PRECTL)prgData)->xLeft, ((PRECTL)prgData)->yBottom,
                ((PRECTL)prgData)->xRight, ((PRECTL)prgData)->yTop);
            break;

        case MPT_QMSG:
            cch = sprintf(szNextMessage,
                "    QMSG: H:%04x       M:%04x M1:%08lx M2:%08lx T:%08lx (%d, %d)",
                (SHORT)((PQMSG)prgData)->hwnd, ((PQMSG)prgData)->msg,
                ((PQMSG)prgData)->mp1,((PQMSG)prgData)->mp2,
                ((PQMSG)prgData)->time,
                ((PQMSG)prgData)->ptl.x, ((PQMSG)prgData)->ptl.y);
            break;

        default:
            goto NoData;
        }

        OutputString(szNextMessage, cch);
    }
NoData:
    return (prgData + cb);
}




/***************************************************************************\
* void OutputString(char szOut, SHORT cch);
*
*   This function will output the specified string to the
*   destinations.
\***************************************************************************/

void OutputString(szOut, cch)
char        szOut[];
SHORT       cch;
{
    SHORT   item;
    char    *psz;
    USHORT  cchWritten;



    /* Now display the new line on the screen */
    if (spyopt.fWindow) {
        item = (SHORT)WinSendMsg(hwndSpyList, LM_INSERTITEM,
            (MPARAM)LIT_END, (MPARAM)(PSZ)szOut);

        WinSendMsg(hwndSpyList, LM_SETTOPINDEX, (MPARAM)item, 0L);

        /* See if we have too many lines now */
        while (item >= spyopt.cWindowLines)
            item = (SHORT)WinSendMsg(hwndSpyList, LM_DELETEITEM,
                    (MPARAM)0, 0L);
    }

    /* now for file need cr/lf */
    psz = szOut + cch;    /* point to trailing null */
    *psz++ = '\r';
    *psz++ = '\n';
    *psz = '\0';

    if (spyopt.fFile)
        DosWrite(spyopt.hfileSpy, (PSZ)szOut, cch+2,
                (PUSHORT)&cchWritten);

}


/***************************************************************************\
* MSGI  * PmsgiFromMsg(USHORT msg)
*
* Locate the msg in the array of message items
*
* Returns: pointer to item that has the specified msg, or NULL
\***************************************************************************/
MSGI *PmsgiFromMsg(msg)
USHORT msg;
{
    register MSGI   *pmsgi = rgmsgi;    /* Start at beginning */
    register USHORT i;

    /*
     * Currently is a simple linear search, should be made faster
     * probably by using binary search.
     */
    for (i=0; i< cmsgi; i++) {
        if (pmsgi->msg == msg)
            return (pmsgi);
        if (pmsgi->msg > msg)
            return (NULL);
        pmsgi++;
    };

    return (NULL);
}
