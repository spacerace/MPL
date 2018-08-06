/* pmserver.c RHS
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
#include<mt\stdio.h>
#include"pmserver.h"
#include"pmsvmsgs.h"
#include"msgq.h"
#include"client.h"
#include"errexit.h"

#define    MAXCLIENTS    25
CLIENT clients[MAXCLIENTS];

#define OBJWINDTHREADSTACK  8192

HAB     hab;
HWND    PMSWinHdl;
#define QMGRTHREADSTACK        2048
BYTE    QMgrThreadStack[QMGRTHREADSTACK];
HQUEUE  qhandle;

int main(void);
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1,
    MPARAM mp2);
void QMgrThread(void);
USHORT QMgrFindClient(PID pid,HQUEUE *qhandle,USHORT *position);

int main(void)
    {
    static ULONG flFrameFlags = FCF_TASKLIST;

    static CHAR  szClientClass[] = "PMServer";
    HMQ          hmq;
    HWND         hwndFrame, hwndClient;
    QMSG         qmsg;

    MsgQCreate(&qhandle,PMSERVERQUE);           // create OS/2 queue

    hab = WinInitialize(0);                     // initialize window

    if(_beginthread(QMgrThread,QMgrThreadStack, // start qmgr thread
            sizeof(QMgrThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(QMgrThread)");

    hmq = WinCreateMsgQueue(hab, 0);            // create msg queue

                                                // register window
    WinRegisterClass(hab, szClientClass, ClientWndProc, 0L, 0);

                                                // create window
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
        0L, &flFrameFlags, szClientClass, NULL, 0L, NULL, 0L,
        &hwndClient);

    while(WinGetMsg(hab, &qmsg, NULL, 0, 0))    // process messages
        WinDispatchMsg(hab, &qmsg);

    WinDestroyWindow(hwndFrame);                // destroy window
    WinDestroyMsgQueue(hmq);                    // destroy msg queue
    WinTerminate(hab);                          // terminate window
    return 0;
    }

MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1,
    MPARAM mp2)
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
            PMSWinHdl = hwnd;
            return 0;

        // **************Clipboard handling *********

        case PMSV_COPY:                         // put data in Clpbrd
            DosAllocSeg(SHORT1FROMMP(mp2),&ClipTextSel,SEG_GIVEABLE);
            temp1 = MAKEP(ClipTextSel,0);
            temp2 = PVOIDFROMMP(mp1);
            memmove(temp1,temp2,SHORT1FROMMP(mp2));
            temp1 = temp2 = NULL;

            WinOpenClipbrd(hab);                // open the clipboard
            WinEmptyClipbrd(hab);               // empty it
                                                // put new data in 
            WinSetClipbrdData(hab, (ULONG)ClipTextSel, CF_TEXT,
                CFI_SELECTOR);
            WinCloseClipbrd(hab);               // close clipboard
            return 0;

        case PMSV_PASTE:                        // get frm Clpbrd
            WinOpenClipbrd(hab);                // open the clipboard
            ClipTextSel = (SEL)WinQueryClipbrdData(hab, CF_TEXT);
            if(ClipTextSel)                     // if data in it
                {
                temp3 = temp1 = MAKEP(ClipTextSel,0);
                for( len = 0; temp3[len]; len++); // get data length
                len++;                          // include the NULL
                                                // send data 
                MsgQSend(SHORT1FROMMP(mp1),temp1,len,
                    PMS_CLPBRD_DATA);
                }
            else                                // nothing in it
                MsgQSend(SHORT1FROMMP(mp1),NULL,0,PMS_CLPBRD_EMPTY);
            WinCloseClipbrd(hab);
            return 0;

        case PMSV_PASTE_MSG:                    // client query
                                                // tell client 
            MsgQSend(SHORT1FROMMP(mp1),NULL,0,
                    (WinQueryClipbrdFmtInfo(hab,CF_TEXT,&fmtInfo) ?
                    PMS_CLPBRD : PMS_CLPBRD_EMPTY));
            return 0;

        case PMSV_THREAD_TERMINATE:             // obj window term
            {
            int i;

            for( i = 0; i < MAXCLIENTS; i++)
                if(clients[i].hwnd == HWNDFROMMP(mp1))
                    {
                    clients[i].isddeclient = FALSE;
                    free(clients[i].threadstack);
                    clients[i].hwnd = NULL;
                    clients[i].appname[0] = '\0';
                    clients[i].topicname[0] = '\0';
                    break;
                    }
            }
            return 0;

        case WM_CLOSE:                          // server terminate
            {
            int i;

            for(i = 0; i < MAXCLIENTS; i++)
                if(clients[i].kclient_pid)
                    {
                    if(clients[i].isddeclient)  // notify obj window
                        WinPostMsg(clients[i].hwnd,PMSV_TERMINATE,
                            0,0);
                                                // notify client
                    MsgQSend(clients[i].qhandle,NULL,0,
                        PMS_SERVER_TERM);
                    }                           // notify Qmgr
            MsgQSend(qhandle,NULL,0,PMS_SERVER_TERM);
            WinPostMsg(hwnd,WM_QUIT,0,0);
            return 0;
            }

        }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
    }


void QMgrThread(void)
    {
    PVOID   msgdata = NULL;
    USHORT  msg,i,msgsize,position,server = TRUE;
    HQUEUE  temphandle;
    PBYTE   tempname;
    PID     temppid;

    memset(clients,0,sizeof(clients));

    while(server)
        {
        temppid = 0;
        MsgQGet(qhandle, &msgdata, &msgsize, &msg);

        switch(msg)
            {
            case PMS_CLPBRD_COPY:
                break;
            case PMS_INIT:
            case PMS_TERMINATE:
            case PMS_CLPBRD_QUERY:
            case PMS_CLPBRD_PASTE:
            default:
                temppid = (msgdata ? CLIENTDATAMSG(msgdata)->pid :
                    0);
                break;
            }

        if(msg != PMS_INIT) // if client not registered, forget it
            if(!QMgrFindClient(temppid,&temphandle,&position))
                continue;

        switch(msg)
            {
            case PMS_INIT:  // client is initializing to use server
                if(msgdata)                     // if valid message
                    {
                    tempname = CLIENTDATAMSG(msgdata)->qname;
                    if(*tempname)               // client queue
                        MsgQOpen(&temphandle,tempname);

                                                // find open slot
                    for( i = 0; i < MAXCLIENTS &&
                        clients[i].kclient_pid; i++);
                    if(i == MAXCLIENTS)            // no slots found
                        {
                        if(temphandle)
                            MsgQSend(temphandle,NULL,0,PMS_NO_INIT);
                        }
                    else                        // got open slot
                        {
                        clients[i].kclient_pid = temppid;
                        if(temphandle)
                            {
                            clients[i].qhandle = temphandle;
                            MsgQSend(temphandle,NULL,0,PMS_INIT_ACK);
                            }
                        }
                    }
                tempname = NULL;
                break;

            case PMS_TERMINATE:
                    // client terminating or not using PMServer
                    // if not last slot, shift down 1
                if(position != MAXCLIENTS-1)
                    memmove(&clients[position],&clients[position+1],
                        (sizeof(CLIENT)*(MAXCLIENTS-position-1)));
                                            // clear last slot
                memset(&clients[MAXCLIENTS-1],0,sizeof(CLIENT));
                break;

            case PMS_SERVER_TERM:
                    // server is terminating, close it up!
                server = FALSE;
                break;


            // ************** Clipboard Handling

            case PMS_CLPBRD_COPY:  // client has data for Clipboard
                WinPostMsg(PMSWinHdl,PMSV_COPY,MPFROMP(msgdata),
                        MPFROMSHORT(msgsize));
                msgdata = NULL;
                break;

            case PMS_CLPBRD_QUERY:
                    // client query: if something's in the clipboard
                WinPostMsg(PMSWinHdl,PMSV_PASTE_MSG,
                    MPFROMSHORT(temphandle),NULL);
                break;

            case PMS_CLPBRD_PASTE:
                    // client requesting copy of the Clipboard data
                WinPostMsg(PMSWinHdl,PMSV_PASTE,
                    MPFROMSHORT(temphandle),NULL);
                break;


            // ************** DDE Handling

            case PMS_DDE_INIT:
                    // client requests DDE client services
                {
                PCLIENT pclient = &clients[position];
                PBYTE p = (PBYTE)CLIENTDATAMSG(msgdata)->qname;

                pclient->threadstack = malloc(OBJWINDTHREADSTACK);
                pclient->isddeclient = TRUE;
                pclient->hwndpmserver = PMSWinHdl;
                strcpy(pclient->appname,p);
                p += (strlen(p)+1);
                strcpy(pclient->topicname,p);

                _beginthread(CreateClient,pclient->threadstack,
                    OBJWINDTHREADSTACK,pclient);
                }
                break;

            case PMS_DDE_REQUEST:
            case PMS_DDE_ADVISE:
                    // client wants a data update
                {
                ITEMREQ itemreq;
                PCLIENT pclient = &clients[position];
                PBYTE p = (PBYTE)CLIENTDATAMSG(msgdata)->qname;

                strcpy(itemreq.item,p);
                itemreq.value[0] = '\0';
                if(pclient->isddeclient)
                    WinPostMsg(pclient->hwnd,
                        (msg == PMS_DDE_REQUEST ?
                        PMSV_REQUEST : PMSV_ADVISE),
                        PMSWinHdl, &itemreq);
                }
                break;

            case PMS_DDE_TERMINATE:
                {
                PCLIENT pclient = &clients[position];

                WinPostMsg(pclient->hwnd,WM_DDE_TERMINATE,0L,0L);
                break;
                }

            default:
                // unknown message rec'd, notify client 
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
        if(clients[i].kclient_pid == pid)
            {
            *qhandle = clients[i].qhandle;
            *position = i;
            return TRUE;
            }
    return FALSE;
    }












