/*
    SHOWCOM.C --- Simple program to display current COM1 
                  setup using the DosDevIOCtl calls.
    Copyright (C) 1987 Ray Duncan

    Build:  C>CL showcom.c
    Usage:  C>SHOWCOM
*/

#include <stdio.h>

#define COMPORT "COM1"              /* COM device name */

#define API unsigned extern far pascal

API DosClose(unsigned); 
API DosDevIOCtl(void far *, void far *, unsigned, unsigned, unsigned);
API DosOpen(char far *, unsigned far *, unsigned far *, unsigned long,
            unsigned, unsigned, unsigned, unsigned long);           

struct F61Struc {
    int BaudRate;                   /* 110, 150, 300, ... 19200 */
    } F61Info ;

struct F62Struc {
    unsigned char DataBits;         /* possible values 5-8 */
    unsigned char Parity;           /* 0=N 1=O 2=E 3=Mark 4=Space */
    unsigned char StopBits;         /* 0=1 bit, 1=1.5 bits, 2=2 bits */
    unsigned char Break;            /* 0=not xmitting break */
    } F62Info;

struct F64Struc {
    unsigned char ComPortStatus;    /* bit  meaning         */
    } F64Info;                      /*  0   Tx waiting for CTS ON
                                        1   Tx waiting for DSR ON
                                        2   Tx waiting for DCD ON
                                        3   Tx waiting, XOFF rcvd
                                        4   Tx waiting, XOFF sent
                                        5   Tx waiting, break xmit
                                        6   Char waiting to xmit
                                        7   Rcv waiting for DSR on  */

struct F65Struc {
    unsigned char XmitDataStatus;   /* bit  meaning         */
    } F65Info;                      /*  0   write in prog or queued
                                        1   data in xmit queue
                                        2   char xmit in progress
                                        3   char waiting for xmit
                                        4   waiting to send XON
                                        5   waiting to send XOFF
                                        6-7 undefined       */

struct F66Struc {
    unsigned char OutputSignals;    /* bit  meaning         */
    } F66Info;                      /*  0   DTR (data terminal ready)
                                        1   RTS (request to send)
                                        2-7 undefined       */

struct F67Struc {
    unsigned char InputSignals;     /* bit  meaning         */
    } F67Info;                      /* 0-3  undefined
                                        4   CTS (clear to send)
                                        5   DSR (data set ready)    */

struct F68Struc {
    unsigned CharsQueued;           /* chars received and waiting */
    unsigned QueueSize;             /* size of receive char. queue */
    } F68Info;

struct F69Struc {
    unsigned CharsQueued;           /* chars waiting for xmit */
    unsigned QueueSize;             /* size of transmit char. queue */
    } F69Info;

struct F6dStruc {
    unsigned ComError;              /* bit  meaning         */
    } F6dInfo;                      /*  0   receive queue overrun
                                        1   receive hardware overrun
                                        2   parity error detected
                                        3   framing error detected
                                        4-15 undefined      */

struct F72Struc {
    unsigned EventWord;             /* bit  meaning         */
    } F72Info;                      /*  0   set when char read
                                            placed in receive queue
                                        1   undefined
                                        2   set when last char in
                                            transmit queue is sent
                                        3   change in CTS state
                                        4   change in DSR state
                                        5   change in DCD state
                                        6   break detected
                                        7   parity or framing error
                                        8   trailing edge of ring
                                            indicator detected
                                        9-15 undefined      */

static char *ParityNames[] = { "None", "Odd", "Even", "Mark", "Space" };
static char *StopNames[]   = { "1", "1.5", "2" };

main (int argc,char *argv[])
{
    int action, handle, status;     /* scratch variables */
    int openflag = 0x01;                        /* DosOpen fail if device not found */
    int openmode = 0x42;            /* DosOpen read/write, deny none */

                                    /* open device or exit */
    if(DosOpen(COMPORT, &handle, &action, 0L, 0, openflag, openmode, 0L))
    {   
        fprintf(stderr, "\nCan't open %s device\n", COMPORT);
        exit(1);
    }                               /* got device OK */
    printf("\n%s device opened, handle = %d\n", COMPORT, handle);

                                    /* now display device info */
    status = DosDevIOCtl(&F61Info, NULL, 0x61, 1, handle);
    if(! status) printf("\nBaud rate:          %d", F61Info.BaudRate);

    status = DosDevIOCtl(&F62Info, NULL, 0x62, 1, handle);
    if(! status)
    {   
        printf("\nData Bits:          %d", F62Info.DataBits);
        printf("\nParity:             %s", ParityNames[F62Info.Parity]);
        printf("\nStop Bits:          %s", StopNames[F62Info.StopBits]);
    }

    status = DosDevIOCtl(&F64Info, NULL, 0x64, 1, handle);
    if(! status) printf("\nCom Port Status:    %.2xh", F64Info.ComPortStatus);

    status = DosDevIOCtl(&F65Info, NULL, 0x65, 1, handle);
    if(! status) printf("\nTransmit Status:    %.2xh", F65Info.XmitDataStatus);

    status = DosDevIOCtl(&F66Info, NULL, 0x66, 1, handle);
    if(! status) printf("\nOutput Signals:     %.2xh", F66Info.OutputSignals);

    status = DosDevIOCtl(&F67Info, NULL, 0x67, 1, handle);
    if(! status) printf("\nInput Signals:      %.2xh", F67Info.InputSignals);

    status = DosDevIOCtl(&F68Info, NULL, 0x68, 1, handle);
    if(! status) 
    {   
        printf("\nRcv Chars Queued:   %d", F68Info.CharsQueued);
        printf("\nRcv Queue Size:     %d", F68Info.QueueSize);
    }

    status = DosDevIOCtl(&F69Info, NULL, 0x69, 1, handle);
    if(! status) 
    {   
        printf("\nXmit Chars Queued:  %d", F69Info.CharsQueued);
        printf("\nXmit Queue Size:    %d", F69Info.QueueSize);
    }

    status = DosDevIOCtl(&F6dInfo, NULL, 0x6d, 1, handle);
    if(! status) printf("\nCom Error Info:     %.4xh", F6dInfo.ComError);

    status = DosDevIOCtl(&F72Info, NULL, 0x72, 1, handle);
    if(! status) printf("\nEvent Word:         %.4xh", F72Info.EventWord);

    status = DosClose(handle);      /* an unnecessary nicety */
    if(! status) printf("\n\n%s device closed\n", COMPORT);
}
