/****************************** Module Header ******************************\
* Module Name: monitor.c
*
* This is a small monitoring test program.  It dumps monitor characters to
* a file called monitor.txt.
*
* Created:      sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#define INCL_WIN
#define INCL_DOSFILEMGR
#define INCL_DOSNLS
#include <os2.h>
#include "ddeml.h"
#include "monitor.h"
#include "res.h"


/*********** declares *********/


int	APIENTRY DebugOutput( PCH );        /* private import */

int lstrlen(PSZ psz);
HDMGDATA EXPENTRY dataxfer(HCONV hConv, HSZ hszTopic, HSZ hszItem,
        USHORT usFmt, USHORT usType, HDMGDATA hDmgData);


/************* GLOBAL VARIABLES  ************/

HAB hab;
HMQ hmq;
HFILE hfile;
HWND hwndFrame;
HWND hwndClient;
USHORT fsOutput = 0;

void cdecl main(argc, argv)
int argc;
char **argv;
{
    USHORT err;
    QMSG qmsg;
    ULONG ctlData;
    argc; argv;

    if (argc > 1) {
        DdeDebugBreak();
    }
    
    hab = WinInitialize(0);

    hmq = WinCreateMsgQueue(hab, 0);

    if (!InitTestSubs())
        DosExit(1, 0);
        
    if (!WinRegisterClass(hab, "Monitor Class", MonWndProc, 0L, 4L))
        DosExit(1, 0);
    
    ctlData = FCF_STANDARD;
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0L, &ctlData, "Monitor Class",
            (PSZ)"DDE Monitoring window", (ULONG)WS_VISIBLE, (HMODULE)NULL, IDR_MAIN, 0L);
    WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 0, 0, SWP_MINIMIZE | SWP_SHOW);

    if (err = DdeInitialize((PFNCALLBACK)dataxfer, DMGCMD_MONITOR, 0L)) {
        DdePostError(err);
        goto abort;
    }
    
    while (WinGetMsg(hab, &qmsg, 0, 0, 0)) {
        WinDispatchMsg(hab, &qmsg);
    }

    DdeUninitialize();

abort:
    DosClose(hfile);
    WinDestroyWindow(hwndFrame);
    WinTerminate(hab);
    DosExit(TRUE, 0);
}



MRESULT EXPENTRY MonWndProc(
HWND hwnd,
USHORT msg,
MPARAM mp1,
MPARAM mp2)
{
    USHORT usAction;
    
    switch (msg) {
    case WM_INITMENU:
        switch (SHORT1FROMMP(mp1)) {
        case IDSM_OUTPUT:
            WinSendMsg((HWND)mp2, MM_SETITEMATTR, MPFROM2SHORT(IDM_OUTPUT_FILE, FALSE),
                    MPFROM2SHORT(MIA_CHECKED,
                    fsOutput & (IDM_OUTPUT_FILE - IDSM_OUTPUT) ?
                    MIA_CHECKED : NULL));
            WinSendMsg((HWND)mp2, MM_SETITEMATTR, MPFROM2SHORT(IDM_OUTPUT_DEBUG, FALSE),
                    MPFROM2SHORT(MIA_CHECKED,
                    fsOutput & (IDM_OUTPUT_DEBUG - IDSM_OUTPUT) ?
                    MIA_CHECKED : NULL));
            WinSendMsg((HWND)mp2, MM_SETITEMATTR, MPFROM2SHORT(IDM_OUTPUT_SCREEN, FALSE),
                    MPFROM2SHORT(MIA_CHECKED,
                    fsOutput & (IDM_OUTPUT_SCREEN - IDSM_OUTPUT) ?
                    MIA_CHECKED : NULL));
            break;
        }
        break;
        
    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case IDM_OUTPUT_FILE:
            if (fsOutput & (IDM_OUTPUT_FILE - IDSM_OUTPUT)) {
                DosClose(hfile);
            } else {
                if (DosOpen("monitor.txt", &hfile, &usAction, 0L, FILE_NORMAL, 
                        FILE_TRUNCATE | FILE_CREATE,
                        OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE |
                        OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_SEQUENTIAL, 0L)) {
                    WinAlarm(HWND_DESKTOP, WA_ERROR);
                    return 0L;
                }
            }
        /* fall through */
        case IDM_OUTPUT_DEBUG:
        case IDM_OUTPUT_SCREEN:
            fsOutput ^= LOUSHORT(mp1) - IDSM_OUTPUT;
            break;
        }
        break;

    case WM_CREATE:
        hwndClient = hwnd;
        /* fall through */
    default:
        return(StrWndProc(hwnd, msg, mp1, mp2));
        break;
    }
    
    return 0L;
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
    char sz[MAX_MONITORSTR + 1];
    USHORT usWritten;

    hConv; hszTopic; hszItem; usFmt; usType;
    
    if (usType == XTYP_MONITOR) {
        DdeGetData(hDmgData, (PBYTE)sz, (ULONG)(MAX_MONITORSTR + 1), 0L);
        if (fsOutput & (IDM_OUTPUT_FILE - IDSM_OUTPUT))
            DosWrite(hfile, sz, lstrlen(sz), &usWritten);
        if (fsOutput & (IDM_OUTPUT_DEBUG - IDSM_OUTPUT))
            DebugOutput(sz);
        if (fsOutput & (IDM_OUTPUT_SCREEN - IDSM_OUTPUT))
            DrawString(hwndClient, sz);
        return(TRUE);
    }
    return(0);
}


int lstrlen(psz)
PSZ psz;
{
    int c = 0;

    while (*psz != 0) {
        psz++;
        c++;
    }
    return(++c);
}
