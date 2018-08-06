/* pmserver.c RHS 8/24/89
 *
 * Pm program that supplies PM services to VIO apps
 *
 */

#define INCL_WIN
#define INCL_VIO
#define INCL_AVIO
#define INCL_DOS
#include<os2.h>
#include<mt\stdlib.h>
#include<mt\process.h>
#include<mt\string.h>
#include"pmserver.h"
#include"msgq.h"
#include"errexit.h"

#define WM_COPY             (WM_USER + 0)
#define WM_PASTE            (WM_USER + 1)
#define WM_PASTE_MSG        (WM_USER + 2)

typedef struct _client
    {
    PID     clientpid;
    HQUEUE  clientqhandle;
    } CLIENT;

#define    MAXCLIENTS    25
CLIENT clients[MAXCLIENTS];


HAB     hab;
HWND    ThreadWindowHdl;
#define QMGRTHREADSTACK        500
BYTE    QMgrThreadStack[QMGRTHREADSTACK];
HQUEUE  qhandle;

int main(void);
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
void QMgrThread(void);
USHORT QMgrFindClient(PID pid,HQUEUE *qhandle,USHORT *position);

int main(void)
    {
    static CHAR  szClientClass[] = "PMServer";
    static ULONG flFrameFlags = FCF_TASKLIST;
    HMQ          hmq;
    HWND         hwndFrame, hwndClient;
    QMSG         qmsg;

    MsgQCreate(&qhandle,PMSERVERQUE);           // create input queue

    hab = WinInitialize(0);                     // initialize window

    if(_beginthread(QMgrThread,QMgrThreadStack, // start queue manager thread
            sizeof(QMgrThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(QMgrThread)");

    hmq = WinCreateMsgQueue(hab, 0);            // create window message queue

                                                // register window class
    WinRegisterClass(hab, szClientClass, ClientWndProc, 0L, 0);

                                                // create window
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0L,
            &flFrameFlags, szClientClass, NULL,
            0L, NULL, 0L, &hwndClient);

    while(WinGetMsg(hab, &qmsg, NULL, 0, 0))    // process messages
        WinDispatchMsg(hab, &qmsg);

    WinDestroyWindow(hwndFrame);                // destroy window
    WinDestroyMsgQueue(hmq);                    // destroy message queue
    WinTerminate(hab);                          // terminate window
    return 0;
    }

MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
    {
    SEL             ClipTextSel;
    PVOID           temp1,temp2;
    USHORT          len;
    PCH             temp3;
    USHORT          fmtInfo;

    switch(msg)
        {
        // ************* Create processing **********
        case WM_CREATE:
            ThreadWindowHdl = hwnd;
            return 0;

        // **************Clipboard handling *********

        case WM_COPY:                           // place client data in Clpbrd
            DosAllocSeg(SHORT1FROMMP(mp2),&ClipTextSel,SEG_GIVEABLE);
            temp1 = MAKEP(ClipTextSel,0);
            temp2 = PVOIDFROMMP(mp1);
            memmove(temp1,temp2,SHORT1FROMMP(mp2));
            temp1 = temp2 = NULL;

            WinOpenClipbrd(hab);                // open the clipboard
            WinEmptyClipbrd(hab);               // empty it
                                                // put the new data in it
            WinSetClipbrdData(hab, (ULONG)ClipTextSel, CF_TEXT, CFI_SELECTOR);
            WinCloseClipbrd(hab);               // close the clipboard
            return 0;

        case WM_PASTE:                          // get client data from Clpbrd
            WinOpenClipbrd(hab);                // open the clipboard
            ClipTextSel = (SEL)WinQueryClipbrdData(hab, CF_TEXT);
            if(ClipTextSel)                     // if data in clipboard
                {
                temp3 = temp1 = MAKEP(ClipTextSel,0);
                for( len = 0; temp3[len]; len++); // get data length
                len++;                          // include the NULL
                                                // send data to client
                MsgQSend(SHORT1FROMMP(mp1),temp1,len,PMS_CLPBRD_DATA);
                }
            else                                // nothing in clipboard
                MsgQSend(SHORT1FROMMP(mp1),NULL,0,PMS_CLPBRD_EMPTY);
            WinCloseClipbrd(hab);
            return 0;

        case WM_PASTE_MSG:                      // client query about Clpbrd
                                                // tell client whether avail
            MsgQSend(SHORT1FROMMP(mp1),NULL,0,
                    (WinQueryClipbrdFmtInfo(hab,CF_TEXT,&fmtInfo) ?
                    PMS_CLPBRD : PMS_CLPBRD_EMPTY));
            return 0;
        }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
    }


void QMgrThread(void)
    {
    PVOID   msgdata = NULL;
    USHORT  msg,i,msgsize,position;
    HQUEUE  temphandle;
    PBYTE   tempname;
    PID     temppid;

    memset(clients,0,sizeof(clients));

    while(TRUE)
        {
        temppid = 0;
        MsgQGet(qhandle, &msgdata, &msgsize, &msg);

        switch(msg)
            {
            case PMS_COPY:
                break;
            case PMS_INIT:
            case PMS_TERMINATE:
            case PMS_CLPBRD_QUERY:
            case PMS_PASTE:
            default:
                temppid = (msgdata ? CLIENTDATAMSG(msgdata)->pid : 0);
                break;
            }

        switch(msg)
            {
            case PMS_INIT:  // client is initializing to use the server
                if(msgdata)                        // if valid message packet
                    {
                    tempname = CLIENTDATAMSG(msgdata)->qname;
                    if(*tempname)               // client has return queue
                        MsgQOpen(&temphandle,tempname);

                                                // find open slot
                    for( i = 0; i < MAXCLIENTS && clients[i].clientpid; i++);
                    if(i == MAXCLIENTS)            // no slots found
                        {
                        if(temphandle)
                            MsgQSend(temphandle,NULL,0,PMS_NO_INIT);
                        }
                    else                        // got open slot
                        {
                        clients[i].clientpid = CLIENTDATAMSG(msgdata)->pid;
                        if(temphandle)
                            {
                            clients[i].clientqhandle = temphandle;
                            MsgQSend(temphandle,NULL,0,PMS_INIT_ACK);
                            }
                        }
                    }
                tempname = NULL;
                break;

            case PMS_COPY:  // client has data for the Clipboard
                WinPostMsg(ThreadWindowHdl,WM_COPY,MPFROMP(msgdata),
                        MPFROMSHORT(msgsize));
                msgdata = NULL;
                break;

            case PMS_CLPBRD_QUERY:
                    // client wants to know if something's in the clipboard
                if(QMgrFindClient(temppid,&temphandle,&position))
                    WinPostMsg(ThreadWindowHdl,WM_PASTE_MSG,
                            MPFROMSHORT(temphandle),NULL);
                break;

            case PMS_PASTE:
                    // client is requesting a copy of the Clipboard data
                if(QMgrFindClient(temppid,&temphandle,&position))
                    WinPostMsg(ThreadWindowHdl,WM_PASTE,
                            MPFROMSHORT(temphandle),NULL);
                break;

            case PMS_TERMINATE:
                    // client is terminating or is no longer using PM Services
                if(QMgrFindClient(temppid,&temphandle,&position))
                    {
                        // if not last slot, shift down 1
                    if(position != MAXCLIENTS-1)
                        memmove(&clients[position],&clients[position+1],
                            (sizeof(CLIENT)*(MAXCLIENTS-position-1)));

                                            // clear last slot
                    memset(&clients[MAXCLIENTS-1],0,sizeof(CLIENT));
                    }
                break;

            default:
                // unknown message rec'd, notify client if it has a queue
                if(QMgrFindClient(temppid,&temphandle,&position))
                    MsgQSend(temphandle,NULL,0,PMS_MSG_UNKNOWN);
                break;
            }
        if(msgdata)
            DosFreeSeg(SELECTOROF(msgdata));
        msgdata = NULL;
        }
    }

// finds PMServer client and qhandle in table
USHORT QMgrFindClient(PID pid,HQUEUE *qhandle,USHORT *position)
    {
    USHORT  i;

    for( i = 0; i < MAXCLIENTS; i++)
        if(clients[i].clientpid == pid)
            {
            *qhandle = clients[i].clientqhandle;
            *position = i;
            return TRUE;
            }
    return FALSE;
    }














