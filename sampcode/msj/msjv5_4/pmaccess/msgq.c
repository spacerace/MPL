/* msgq.c RHS
 *
 * message queue routines
 */
#define    INCL_DOS
#include<os2.h>
#include<string.h>
#include<malloc.h>
#include"errexit.h"
#include"msgq.h"

typedef struct _qtable            // message queue table stru
    {
    HQUEUE      qhandle;
    PID         qowner;
    } QTABLE;

#define MAXQUEUES    15

QTABLE qtable[MAXQUEUES];        // message queue table
qtableindex = 0;

// creates a new message queue
void MsgQCreate(HQUEUE *qhandle, char *qname)
    {
    USHORT retval;

    if(retval = DosCreateQueue(qhandle,QUE_FIFO,qname))
        error_exit(retval,"DosCreateQueue");
    }

// opens an existing message queue
USHORT MsgQOpen(HQUEUE *qhandle, char *qname)
    {
    PID    qowner;
    USHORT retval;

    if(retval = DosOpenQueue(&qowner,qhandle,qname)) // open queue
        {
        *qhandle = 0;
        return retval;
        }

    qtable[qtableindex].qhandle = *qhandle;   // put handle and PID
    qtable[qtableindex].qowner = qowner;      // into table
    qtableindex++;

    return 0;
    }

// sends a message
void MsgQSend(HQUEUE qhandle, PVOID event, USHORT size, USHORT msg)
    {
    USHORT retval,i;
    SEL sel,newsel;
    PVOID qptr;

    if(event != NULL)                       //if data w/ the message
        {
        for(i = 0; i < qtableindex; i++)    // find the queue
            if(qtable[i].qhandle == qhandle)
                break;
                                         // create a segment for data
        if(retval = DosAllocSeg(size,&sel,(SEG_GIVEABLE)))
            error_exit(retval,"DosAllocSeg");
        DosGiveSeg(sel,qtable[i].qowner,&newsel); // make it giveable
        qptr = MAKEP(newsel,0);
        memmove(qptr,event,size);               // put the data in it
        }
    else
        qptr = NULL;
                                           // write message+data to q
    if(retval = DosWriteQueue(qhandle,msg,size,(PBYTE)qptr,0))
        error_exit(retval,"DosWriteQueue");
    if((qptr != NULL) && (sel != newsel))  // free seg if not ours
        DosFreeSeg(sel);
    }

// get a message from a queue
void MsgQGet(HQUEUE qhandle, PVOID *event, USHORT *size, USHORT *msg)
    {
    QUEUERESULT     qresult;
    BYTE               priority;
    USHORT retval;

    if(retval = DosReadQueue(qhandle,&qresult,size,
            (PVOID FAR *)event,0x0000,DCWW_WAIT,&priority,0L))
        error_exit(retval,"DosReadQueue");
    *msg = qresult.usEventCode;
    }

// close queue and remove from table
void MsgQClose(HQUEUE qhandle)
    {
    USHORT i;

    DosCloseQueue(qhandle);
    for(i = 0; i < qtableindex; i++)
        if(qtable[i].qhandle == qhandle)
            break;
    if(i != qtableindex)
        memmove(&qtable[i],&qtable[i+1],
                (sizeof(QTABLE)*(qtableindex-(i+1))));
    qtableindex--;
    }

    /******************************************************/
