// client.c RHS


#define INCL_WIN
#define INCL_DOSPROCESS
#define INCL_DOSQUEUES
#include <os2.h>
#include <mt\string.h>
#include <mt\stdio.h>
#include <mt\malloc.h>
#include "msgq.h"
#include "pmserver.h"
#include "pmsvmsgs.h"
#include "client.h"


//
// Defines
//
#define QWP_USER    0

//
// Structures
//
typedef struct clientdata 
    {
    HAB             hab;
    HWND            hwndServer;
    PCLIENT         pclient;
    } CDATA;
typedef CDATA *PCDATA;

//
// Function prototypes
//
extern MRESULT CALLBACK DdeClientProc(HWND, USHORT, MPARAM, MPARAM);
PDDESTRUCT MakeDDESeg(HWND Destwin, PSZ itemname, USHORT status,
        USHORT format, PVOID dataptr, USHORT datalen);


// creates DDE client object window

VOID    FAR    CreateClient(PCLIENT pclient)
    {
    CDATA    ClientData;
    QMSG    qmsg;
    HMQ     hmq;

    ClientData.pclient = pclient;
    ClientData.hab = WinInitialize(0);
    hmq = WinCreateMsgQueue(ClientData.hab, 0);

    WinRegisterClass(ClientData.hab, "DdeClient", DdeClientProc, 0L,
        sizeof(PCDATA));

    pclient->hwnd = WinCreateWindow(HWND_OBJECT,
                                "DdeClient",
                                NULL,
                                0L,
                                0, 0, 0, 0,
                                NULL,
                                HWND_TOP,
                                1,
                                &ClientData,
                                NULL);

    while (WinGetMsg(NULL, (PQMSG)&qmsg, NULL, 0, 0))
        WinDispatchMsg(NULL, (PQMSG)&qmsg);

    WinDestroyWindow(pclient->hwnd);            // destroy window
    WinDestroyMsgQueue(hmq);                    // destroy msg queue
    DosEnterCritSec();                          // freeze threads
                                                // tell PMServer
    WinPostMsg(pclient->hwndpmserver,PMSV_THREAD_TERMINATE,
        pclient->hwnd,0L);
    WinTerminate(ClientData.hab);               // discard resources
    DosExit(EXIT_THREAD, 0);                    // exit and unfreeze
    }


    // DDE client object window proc
MRESULT CALLBACK DdeClientProc(HWND hwnd, USHORT msg, MPARAM mp1,
        MPARAM mp2)
    {
    PCDATA          pClientData = WinQueryWindowPtr(hwnd, QWP_USER);
    PDDESTRUCT      pdde;
    PDDEINIT        pddeinit;
    HWND            hwndSender = HWNDFROMMP(mp1);

    switch (msg)
        {
        case WM_DDE_INITIATE:
            break;
        case WM_CREATE:
            {
            PCLIENT    pclient;

            pClientData = PVOIDFROMMP(mp1);
            pclient = pClientData->pclient;
            WinSetWindowPtr(hwnd, QWP_USER, pClientData);
            pClientData->hwndServer = NULL;
            WinDdeInitiate(hwnd, pclient->appname,
                pclient->topicname);
            WinStartTimer(pClientData->hab, hwnd, 0, 100);
            break;
            }

        case WM_TIMER:
            {
            WinStopTimer(pClientData->hab, hwnd, 0);
            if (!pClientData->hwndServer)
                {
                MsgQSend(pClientData->pclient->qhandle,NULL,0,
                    PMS_DDE_INITNAK);
                WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
                }
            break;
            }

        case WM_DDE_INITIATEACK:
            {
            pddeinit = PVOIDFROMMP(mp2);
            pClientData->hwndServer = hwndSender;
            MsgQSend(pClientData->pclient->qhandle,NULL,0,
                    PMS_DDE_INITACK);
            DosFreeSeg(PDDEITOSEL(pddeinit));
            break;
            }

        case WM_DDE_ACK:
            {
            PITEMREQ    iReq;
            PVOID    pVoid;
            USHORT  size;

            pdde        = PVOIDFROMMP(mp2);
            iReq = malloc(size = (sizeof(ITEMREQ)+
                    (USHORT)pdde->cbData+1));

            strcpy(iReq->item, DDES_PSZITEMNAME(pdde));
            pVoid = DDES_PABDATA(pdde);
            memcpy(iReq->value, DDES_PABDATA(pdde),
                    (size_t)pdde->cbData);

            MsgQSend(pClientData->pclient->qhandle,iReq,size,
                (pdde->fsStatus & DDE_FACK ? PMS_DDE_ACK :
                PMS_DDE_NAK));
            DosFreeSeg(PDDESTOSEL(pdde));
            free(iReq);
            break;
            }

        case WM_DDE_DATA:
            {
            PITEMREQ    iReq;
            char *excel = "excel";
            USHORT size,i;

            pdde = PVOIDFROMMP(mp2);
            iReq = (PITEMREQ)malloc(size = (sizeof(ITEMREQ)+
                    (USHORT)pdde->cbData+2));
            strcpy(iReq->item, DDES_PSZITEMNAME(pdde));
                // if the app is Excel...
            if(!strnicmp(excel,pClientData->pclient->appname,
                    strlen(excel)))
                memcpy(iReq->value, DDES_PABDATA(pdde),
                    i = (USHORT)pdde->cbData);
            else
                memcpy(iReq->value, DDES_PABDATA(pdde),
                    i = ((USHORT)pdde->cbData-pdde->offabData-1));
            iReq->value[i] = '\0';

            MsgQSend(pClientData->pclient->qhandle,iReq,size,
                (pdde->fsStatus & DDE_FNODATA) ? PMS_DDE_DATACHANGE :
                PMS_DDE_DATA);

            DosFreeSeg(PDDESTOSEL(pdde));
            free(iReq);
            break;
            }

        case WM_CLOSE:
        case PMSV_TERMINATE:               // PMServer terminating
        case WM_DDE_TERMINATE:             // server/client is term.
            {
            WinDdePostMsg(
                (msg == WM_DDE_TERMINATE ?
                    hwndSender : pClientData->hwndServer),
                hwnd, WM_DDE_TERMINATE, NULL, FALSE);
            MsgQSend(pClientData->pclient->qhandle,0,0,
                    PMS_DDE_TERMINATE);
            pClientData->hwndServer = NULL;

            WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
            break;
            }

        case PMSV_REQUEST:
        case PMSV_ADVISE:
        case PMSV_ADVISE_NO_DATA:
        case PMSV_POKE:
        case PMSV_UNADVISE:
        case PMSV_EXECUTE:
            {
            PITEMREQ piReq = PVOIDFROMMP(mp2);
            USHORT DDEmsg;

            switch(msg)
                {
                case PMSV_REQUEST:
                    DDEmsg = WM_DDE_REQUEST;
                    break;
                case PMSV_ADVISE:
                case PMSV_ADVISE_NO_DATA:
                    DDEmsg = WM_DDE_ADVISE;
                    break;
                case PMSV_POKE:
                    DDEmsg = WM_DDE_POKE;
                    break;
                case PMSV_UNADVISE:
                    DDEmsg = WM_DDE_UNADVISE;
                    break;
                case PMSV_EXECUTE:
                    DDEmsg = WM_DDE_EXECUTE;
                    break;
                }

             pdde = MakeDDESeg(pClientData->hwndServer,
                ((msg == PMSV_EXECUTE) ? (PSZ)"" : (PSZ)piReq->item),
                ((msg == PMSV_ADVISE_NO_DATA) ? DDE_FNODATA : 0),
                DDEFMT_TEXT,
                ((msg == PMSV_POKE || msg == PMSV_EXECUTE)
                        ? piReq->value : NULL),
                ((msg == PMSV_POKE || msg == PMSV_EXECUTE)
                        ? strlen(piReq->value)+1 : 0));

            WinDdePostMsg(pClientData->hwndServer,
                hwnd,
                DDEmsg,
                pdde,
                TRUE);
            break;
            }
        
        default:
            return (WinDefWindowProc(hwnd, msg, mp1, mp2));
        }
    return (0);
    }


    // creates DDE Seg, returns pointer to it.
PDDESTRUCT MakeDDESeg(HWND Destwin, PSZ itemname, USHORT status,
        USHORT format, PVOID dataptr, USHORT datalen)
    {
    PDDESTRUCT  ddeptr;
    USHORT      itemlen,totalen;
    SEL         localSEL, sharedSEL;
    PID         receiverPID, receiverTID;

    itemlen = strlen(itemname)+1;
    totalen = sizeof(DDESTRUCT) + itemlen + datalen;

    if(!DosAllocSeg(totalen, &localSEL, SEG_GIVEABLE))
        {
        ddeptr = SELTOPDDES(localSEL);
        ddeptr->cbData = totalen;
        ddeptr->fsStatus = status;
        ddeptr->usFormat = format;
        ddeptr->offszItemName = sizeof(DDESTRUCT);
        ddeptr->offabData = ((datalen && dataptr) ?
                (sizeof(DDESTRUCT)+itemlen) : 0);

        strcpy(DDES_PSZITEMNAME(ddeptr),itemname);
        memmove(DDES_PABDATA(ddeptr),dataptr,datalen);
        WinQueryWindowProcess(Destwin, &receiverPID, &receiverTID);

        if(!DosGiveSeg(localSEL,receiverPID,&sharedSEL))
            {
            ddeptr = SELTOPDDES(sharedSEL);
            return ddeptr;
            }
        }
    return NULL;
    }

