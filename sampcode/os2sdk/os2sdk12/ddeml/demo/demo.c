/****************************** Module Header ******************************\
* Module Name:  demo.c - Demo application
*
* Created:
*
* Copyright (c) 1987  Microsoft Corporation
*
\***************************************************************************/

#include "demo.h"
#include <stdlib.h>

/************* GLOBAL VARIABLES         */

char szDemoClass[] = "Demo";

HAB  hab;
HMQ  hmqDemo;
HWND hwndDemo;
HWND hwndDemoFrame;
HCONV hconv = NULL;
HSZ hszTitle, hszTopicChase, hszItemPos;
USHORT fmtSWP;
SWP SWPTarget = { 0 };
PFNWP RealFrameProc;
BOOL flee = FALSE;
USHORT cServers = 0;
USHORT cxScreen, cyScreen;

#define TIMEOUT 100
#define TIMERSPEED 1000

/**************************************/

VOID CommandMsg(USHORT cmd)
{
    UNUSED cmd;
}

BOOL DemoInit()
{
    hab = WinInitialize(0);

    hmqDemo = WinCreateMsgQueue(hab, 0);

    cxScreen = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    cyScreen = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
    srand(2);
    
    if (!WinRegisterClass(hab, szDemoClass, (PFNWP)DemoWndProc,
            CS_SIZEREDRAW, 0))
         return(FALSE);

    /*
     * Initialize the ddeml
     */
    if (DdeInitialize((PFNCALLBACK)callback, 0L, 0L))
        return(FALSE);

    /*
     * Now create HSZs for each of our DDE strings.
     */
    fmtSWP = WinAddAtom(WinQuerySystemAtomTable(), "SWP FORMAT");
    hszTitle = DdeGetHsz("Demo", 0, 0);
    hszTopicChase = DdeGetHsz("Chaser", 0, 0);
    hszItemPos = DdeGetHsz("Position", 0, 0);

    /*
     * let others know we are here - available as a server and turn on
     * filtering so we don't get bothered with any initiates for any
     * other app names.
     */
    DdeAppNameServer(hszTitle, ANS_REGISTER | ANS_FILTERON);
        
    return(TRUE);
}

int cdecl main(int argc, char** argv)
{
    ULONG fcf;
    QMSG qmsg;

    UNUSED argc;
    UNUSED argv;

    if (!DemoInit()) {
        WinAlarm(HWND_DESKTOP, WA_ERROR);
        return(0);
    }

    fcf = FCF_STANDARD;

    hwndDemoFrame = WinCreateStdWindow(
            HWND_DESKTOP,
            WS_VISIBLE,
            &fcf,
            szDemoClass,
            "",
            WS_VISIBLE, 
            NULL,
            IDR_DEMO,
            &hwndDemo);

    WinSetFocus(HWND_DESKTOP, hwndDemo);

    while (WinGetMsg(hab, (PQMSG)&qmsg, NULL, 0, 0)) {
        WinDispatchMsg(hab, (PQMSG)&qmsg);
    }

    WinDestroyWindow(hwndDemoFrame);

    WinDestroyMsgQueue(hmqDemo);
    WinTerminate(hab);

    return(0);
}

/********** Demo Window Procedure **************/

MRESULT FAR PASCAL DemoWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    HPS   hps;
    RECTL rclPaint;
    SWP swp;
    SHORT speed;

    switch (msg) {
    case WM_CREATE:
        /* Set up this global first thing in case we need it elsewhere */
        hwndDemo = hwnd;
        hwndDemoFrame = WinQueryWindow(hwnd, QW_PARENT, FALSE);
        RealFrameProc = WinSubclassWindow(hwndDemoFrame, DemoFrameWndProc);
        WinSetWindowPos(hwndDemoFrame, NULL, 0, 0, 0, 0, SWP_MINIMIZE | SWP_SHOW);
        /*
         * start the timer so we will keep looking for another app like ours
         * to chase.
         */
        WinStartTimer(hab, hwndDemo, 1, TIMERSPEED);
        return(MRFROMSHORT(FALSE));
        break;

    case WM_TIMER:
        /*
         * We use a timer to keep us moving.
         */
        if (!hconv) {
            HDMGDATA hData;
            PHSZHAPP phszhapp;
            /*
             * no active conversation, try to make one.
             */
            WinStopTimer(hab, hwndDemo, 1);
            /*
             * find out if any others like us are out there
             */
            hData = DdeAppNameServer(hszTitle, ANS_QUERYALLBUTME);
            if (!hData) {
                /*
                 * wait till others arrive.
                 */
                return(0);
            }
            /*
             * extract the first hApp from the hData so we can connect to it
             */
            phszhapp = (PHSZHAPP)DdeAccessData(hData);
            if (phszhapp->hsz == 0) {
                DdeFreeData(hData);
                return(0);
            }
            /*
             * perform directed connection to our target
             */
            hconv = DdeConnect(hszTitle, hszTopicChase, NULL, phszhapp->hApp);
            /*
             * free the hData now that we are done using it.
             */
            DdeFreeData(hData);
            WinStartTimer(hab, hwndDemo, 1, TIMERSPEED);
            if (!hconv) {
                /*
                 * cant make one, try again later.
                 */
                return(0);
            }
            /*
             * Get the target's position into SWPTarget.
             */
            if (hData = DdeClientXfer(NULL, 0L, hconv, hszItemPos, fmtSWP,
                    XTYP_REQUEST, TIMEOUT, NULL)) {
                DdeCopyBlock(DdeAccessData(hData), (PBYTE)&SWPTarget,
                        sizeof(SWP));
                DdeFreeData(hData);
            }
            /*
             * set up an advise loop so our moving target keeps us informed
             * of where he is.
             */
            DdeClientXfer(NULL, 0L, hconv, hszItemPos, fmtSWP,
                    XTYP_ADVSTART, TIMEOUT, NULL);
        }
        
        if (WinIsWindow(hab, SWPTarget.hwnd)) {
            /*
             * target data must be valid, move toward it.
             */
            speed = 1;
            WinQueryWindowPos(hwndDemoFrame, &swp);
            if (swp.x > SWPTarget.x) 
                swp.x -= speed;
            if (swp.x < SWPTarget.x)
                swp.x += speed;
            if (swp.y > SWPTarget.y) 
                swp.y -= speed;
            if (swp.y < SWPTarget.y)
                swp.y += speed;
            swp.fs = SWP_MOVE | SWP_NOADJUST;
            WinSetMultWindowPos(hab, &swp, 1);
            if ((swp.x == SWPTarget.x) && (swp.y == SWPTarget.y) && (!flee)) {
                /*
                 * he's cought stop chasing him and go find another.
                 */
                WinAlarm(HWND_DESKTOP, WA_NOTE);
                DdeDisconnect(hconv);
                hconv = NULL;
                /*
                 * move to a random position
                 */
                WinSetWindowPos(hwndDemoFrame, HWND_TOP, rand() % cxScreen,
                        rand() % cyScreen, 0, 0,
                        SWP_MOVE | SWP_ZORDER | SWP_NOADJUST);
            }
        } else if (hconv) {
            /*
             * Target is invalid, disconnect and try a reconnect later.
             */
            DdeDisconnect(hconv);
            hconv = NULL;
        }
        break;

    case WM_PAINT:
        hps = WinBeginPaint(hwnd, (HPS)NULL, &rclPaint);
        DemoPaint(hwnd, hps, &rclPaint);
        WinEndPaint(hps);
        break;

    case WM_COMMAND:
        CommandMsg(LOUSHORT(mp1));
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0L);
}



MRESULT FAR PASCAL DemoFrameWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
    case WM_MOVE:
        DdePostAdvise(hszTopicChase, hszItemPos);
        /* fall through */
    default:
        return(RealFrameProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0L);
}


VOID DemoPaint(HWND hwnd, HPS hps, RECTL* prcl)
{
    RECTL rcl;

    UNUSED prcl;
    
    /* get window interior rect */
    WinQueryWindowRect(hwnd, &rcl);

    /* print "Hello World" centered horizontally and vertically */
    WinDrawText(hps, -1, "Hello World", &rcl, SYSCLR_WINDOWTEXT,
            SYSCLR_WINDOW, DT_CENTER | DT_VCENTER | DT_ERASERECT);

    /* draw interior border */
    WinDrawBorder(hps, &rcl, 6, 6, SYSCLR_WINDOWTEXT, SYSCLR_WINDOW,
            DB_STANDARD);
}



HDMGDATA EXPENTRY callback(
HCONV hConv,
HSZ hszTopic,
HSZ hszItem,
USHORT usFmt,
USHORT usType,
HDMGDATA hDmgData)
{
    SWP swp;

    UNUSED hConv;

    if (usType == XTYP_REGISTER && hszItem == hszTitle && !hconv) {
        /*
         * someone else came onboard, if we are looking for a target,
         * restart our clock.
         */
        WinStartTimer(hab, hwndDemo, 1, TIMERSPEED);
    }
    
    /*
     * we only care about stuff on our topic.
     */    
    if (hszTopic != hszTopicChase)
        return(0);

    switch (usType) {
        
    case XTYP_ADVSTART:
        /*
         * Always allow advises on our item
         */
        return(hszItem == hszItemPos);
        break;

    case XTYP_ADVDATA:
        /*
         * Always accept advise data on our target's latest position.
         */
        if (hszItem == hszItemPos) 
            DdeGetData(hDmgData, (PBYTE)&SWPTarget, sizeof(SWP), 0L);
        DdeFreeData(hDmgData);
        return(0);
        break;
                
    case XTYP_INIT:
        /*
         * always allow others to initiate with us on our topic.
         */
        return(hszItem == hszTitle && hszTopic == hszTopicChase);
        break;

    case XTYP_REQUEST:
    case XTYP_ADVREQ:
        /*
         * Respond to data requests as to our whereabouts item and format are
         * ok.
         */
        if (hszItem != hszItemPos || usFmt != fmtSWP) 
            return(0);
        WinQueryWindowPos(hwndDemoFrame, &swp);
        return(DdePutData((PBYTE)&swp, sizeof(SWP), 0L, hszItemPos, fmtSWP, 0));
        break;

    default:
        return(0);
    }
}



