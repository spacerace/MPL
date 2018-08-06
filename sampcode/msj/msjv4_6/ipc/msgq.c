/* msgq.c RHS 7/15/89
 *
 * message queue routines
 */
#define	INCL_DOS
#include<os2.h>

#include"errexit.h"
#include"msgq.h"

void MsgQCreate(HQUEUE *qhandle, char *qname)
	{
	USHORT retval;

	if(retval = DosCreateQueue(qhandle,QUE_FIFO,qname))
		error_exit(retval,"DosCreateQueue");
	}

void MsgQOpen(HQUEUE *qhandle, char *qname)
	{
	PID	qowner;
	USHORT retval;

	if(retval = DosOpenQueue(&qowner,qhandle,qname))
		error_exit(retval,qname);
	}

void MsgQSend(HQUEUE qhandle, USHORT event)
	{
	USHORT retval;

	if(retval = DosWriteQueue(qhandle,event,0,(PBYTE)NULL,0))
		error_exit(retval,"DosWriteQueue");
	}

void MsgQGet(HQUEUE qhandle, USHORT *event)
	{
	QUEUERESULT 	qresult;
	USHORT 			ellength;
	PVOID			dummy;
	BYTE	   		priority;
	USHORT retval;

	if(retval = DosReadQueue(qhandle,&qresult,&ellength,
			(PVOID FAR *)&dummy,0x0000,DCWW_WAIT,&priority,0L))
		error_exit(retval,"DosReadQueue");
	*event = qresult.usEventCode;
	}

void MsgQClose(HQUEUE qhandle)
	{
	DosCloseQueue(qhandle);
	}

	/******************************************************/


