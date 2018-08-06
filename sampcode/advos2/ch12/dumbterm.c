/*
	DUMBTERM.C

	A simple multithreaded OS/2 terminal program that exchanges
	characters between the console and the serial port.

	Communication parameters for COM1 are set at 2400 baud,
	8 data bits, no parity, and 1 stop bit.

	Compile with:  C> cl /F 2000 dumbterm.c

	Usage is:  C> dumbterm

	Press Alt-X to terminate the program.

	Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>

#define WAIT   0                        /* parameters for KbdCharIn */
#define NOWAIT 1

#define EXITKEY  0x2d                   /* Exit key = Alt-X */

#define STKSIZE 2048                    /* stack size for threads */

#define COMPORT "COM1"                  /* COM port configuration */
#define BAUD    2400                    /* 110, 150 ... 19200 */
#define PARITY	0			/* 0 = N, 1 = O, 2 = E, 3 = M, 4 = S */
#define DATABIT 8                       /* 5-8 allowed */
#define STOPBIT 0			/* 0 = 1, 1 = 1.5, 2 = 2 */

#define API unsigned extern far pascal	/* OS/2 API prototypes */

API DosClose(unsigned); 
API DosCreateThread(void (far *)(), unsigned far *, void far *);
API DosDevIOCtl(void far *, void far *, unsigned, unsigned, unsigned);
API DosExit(unsigned, unsigned);
API DosOpen(char far *, unsigned far *, unsigned far *, unsigned long,
            unsigned, unsigned, unsigned, unsigned long);           
API DosRead(unsigned, void far *, int, unsigned far *);
API DosSemClear(unsigned long far *);
API DosSemSet(unsigned long far *);
API DosSemWait(unsigned long far *, unsigned long);
API DosSuspendThread(unsigned);
API DosWrite(unsigned, void far *, int, unsigned far *);
API KbdCharIn(void far *, unsigned, unsigned);
API KbdGetStatus(void far *, unsigned);
API KbdSetStatus(void far *, unsigned);
API VioWrtTTY(char far *, int, unsigned);

void far comin(void);                   /* local function prototypes */
void far comout(void);
void errexit(char *);

struct _F41Info {                       /* DosDevIOCTl info structure */
    int BaudRate;                       /* 110, 150 ... 19200 */
    } F41Info;

struct _F42Info {                       /* DosDevIOCtl info structure */
    char DataBits;                      /* character length 5-8 bits */
    char Parity;			/* 0=N, 1=O, 2=E, 3=Mark, 4=Space */
    char StopBits;                      /* 0=1, 1=1.5, 2=2 stop bits */
    } F42Info;

struct _KbdInfo {                       /* KbdCharIn info structure */
    char CharCode;                      /* ASCII character code */
    char ScanCode;                      /* keyboard scan code */
    char Status;                        /* misc. status flags */
    char Reserved1;                     /* reserved byte */
    unsigned ShiftState;                /* keyboard shift state */
    long TimeStamp;                     /* character timestamp */
    } KbdInfo;

struct _KbdStatus {                     /* KbdGetStatus info structure */
    int Length;                         /* length of structure */
    unsigned Mask;                      /* keyboard state flags */
    char TurnAround[2];                 /* logical end-of-line */
    unsigned Interim;                   /* interim character flags */
    unsigned ShiftState;                /* keyboard shift state */
    } KbdStatus;

unsigned chandle;                       /* handle for COM port */
unsigned long exitsem;                  /* Alt-X exit semaphore */

main()
{
    unsigned action;                    /* result of DosOpen */
    unsigned openflag = 0x01;           /* fail if device not found */
    unsigned openmode = 0x12;           /* read/write, deny all */
    unsigned cominID;                   /* COM input thread ID */
    unsigned comoutID;                  /* COM output thread ID */
    char cominstk[STKSIZE];             /* COM input thread stack */
    char comoutstk[STKSIZE];            /* COM output thread stack */

                                        /* open COM port */
    if(DosOpen(COMPORT, &chandle, &action, 0L, 0, openflag, openmode, 0L))
        errexit("\nCan't open COM device.\n");

    F41Info.BaudRate = BAUD;            /* configure COM port */
    F42Info.DataBits = DATABIT;         /* using DosDevIOCtl */
    F42Info.Parity = PARITY;            /* Category 1 functions */
    F42Info.StopBits = STOPBIT;    
    if(DosDevIOCtl(NULL, &F41Info, 0x41, 1, chandle))
        errexit("\nCan't set baud rate.\n");
    if(DosDevIOCtl(NULL, &F42Info, 0x42, 1, chandle))
        errexit("\nCan't configure COM port.\n");

    KbdStatus.Length = 10;              /* force keyboard */
    KbdGetStatus(&KbdStatus, 0);        /* into binary mode */
    KbdStatus.Mask &= 0xfff0;           /* with echo off */
    KbdStatus.Mask |= 0x06;
    KbdSetStatus(&KbdStatus, 0);    

    exitsem = 0L;                       /* initialize and */
    DosSemSet(&exitsem);                /* set exit semaphore */

                                        /* create COM input thread */
    if(DosCreateThread(comin, &cominID, cominstk+STKSIZE))
        errexit("\nCan't create COM input thread.\n");

                                        /* create COM output thread */
    if(DosCreateThread(comout, &comoutID, comoutstk+STKSIZE))
        errexit("\nCan't create COM output thread.\n");

    puts("\nCommunicating...");         /* sign-on message */
    
    DosSemWait(&exitsem, -1L);          /* wait for exit signal */

    DosSuspendThread(cominID);          /* freeze COM input and */
    DosSuspendThread(comoutID);         /* output threads */

    puts("\nTerminating...");           /* sign-off message */

    DosExit(1, 0);                      /* terminate all threads */
                                        /* return code = 0 */
}

/*
    The 'comin' thread reads characters one at a time from the
    COM port and sends them to the display.
*/
void far comin(void)
{
    unsigned rlen;                      /* scratch variable */  
    char inchar;                        /* character input buffer */

    while(1)                            
    {					/* read character from COM */
        DosRead(chandle, &inchar, 1, &rlen);
	VioWrtTTY(&inchar, 1, 0);	/* send character to display */
    }
}

/*
    The 'comout' thread reads characters one at a time from the
    keyboard and sends them to the COM port.  If the exit key is
    detected, the main thread is signaled to clean up and exit.
*/
void far comout(void)
{
    unsigned wlen;                      /* scratch variable */  

    while(1)
    {
        KbdCharIn(&KbdInfo, WAIT, 0);   /* read keyboard */
	wlen = 1;			/* assume 1-byte character */
        
                                        /* extended character? */
        if((KbdInfo.CharCode == 0) || (KbdInfo.CharCode == 0xe0))
        {
            wlen = 2;                   /* yes, set length = 2 */
            
            if(KbdInfo.ScanCode == EXITKEY)
            {                           /* if exit key detected */
                DosSemClear(&exitsem);  /* signal thread 1 to exit */      
                wlen = 0;               /* and discard character */
            }
        }                               
                                        /* write COM port */
        DosWrite(chandle, &KbdInfo.CharCode, wlen, &wlen); 
    }
}

/*
    Common error exit routine; displays error message
    and terminates process.
*/
void errexit(char *msg)
{
    VioWrtTTY(msg, strlen(msg), 0);     /* display error message */
    DosExit(1, 1);                      /* terminate, exit code = 1 */
}                                       

