/*
    threads.c -- Supplementary threads module
    Created by Microsoft Corporation, 1989
*/
#define 	INCL_DOSSEMAPHORES
#define		INCL_DOSMEMMGR
#define		INCL_DOSPROCESS
#define		INCL_WINMESSAGEMGR
#define		INCL_WINTRACKRECT
#include	<os2.h>
#include	"global.h"
#include	"avio.h"
#include	"circleq.h"
#include	"comport.h"
#include	"threads.h"
#include	"malloc.h"

int		rc;
HWND		hWndMaster;
BOOL		fNoUpdate = TRUE;
BOOL		fWrap;
/*
    Declare pointers to stacks
*/
PINT		pStackWPT,
		pStackRPT,
		pStackWST;
/*
    ...Selectors
*/
SEL		selStackWPT,
		selStackRPT,
		selStackWST;
/*
    ...Thread ID numbers
*/
TID		tidWPT,
		tidRPT,
		tidWST;
/*
    ...Control booleans, semaphores
*/
BOOL		fBreak,			/* Break active */
		fAlive;			/* Should the threads be killed? */
LONG		lSemLock,		/* TypeAhead buffer locks... */
		lSemEmpty,
		lSemFull,
		lSemOverflow;		/* Buffer overflow semaphore */
CHAR		TypeAhead[BUFSIZE];	/* TypeAhead buffer and controls */
int		nBufLoc, nChars;
LineInfo	aliReadAhead[RASIZE];
int		cliReadAhead;
LONG		lSemRALock;
LONG		lSemRAFull;
LONG		lSemRAEmpty;
/*
    Messages....
*/
char aszMessage[MBE_NUMMSGS][MAXLINELEN] = {
    "Error opening port",
    "Error writing port",
    "Error reading port",
    "Circular buffer overflowing"
};
/*
    Macros
*/
#define	NOUPDATE		(MRESULT) FALSE
#define	UPDATE			(MRESULT) TRUE
#define	ThdBufNextLoc(n)	(n = ((n + 1) % BUFSIZE))
#define	ThdBufLastLoc(n)	(n = (n > 0) ? (n - 1) : (BUFSIZE - 1))
#define	ThdNextRALoc(n)		(n = ((n + 1) % RASIZE))
#define MessageBox(s, v)	WinPostMsg(hWndMaster, WM_MSGBOX, s, v)
/*
    Local routines
*/
void far WritePortThread(void);
void far ReadPortThread(void);
void far WriteScreenThread(void);
void Process(Line, Line, int far *);

int ThdCreate(PFNTHREAD Routine, PBYTE *pStack, SEL *selStack, TID *tidThread) {
/*
    Initialize the thread
*/
    if (rc = DosAllocSeg(sizeof(int) * STACKSIZE, selStack, 0)) return rc;
    *pStack = (PBYTE) MAKEP(*selStack, 0) + STACKSIZE;
    return (rc = DosCreateThread(Routine, tidThread, *pStack));
}

void ThdInitialize(HWND hWnd, COM Term) {
/*
    Initialize Booleans, Master Window
*/
    fBreak = FALSE;
    fAlive = TRUE;
    hWndMaster = hWnd;
/*
    Initialize TypeAhead buffer
*/
    nBufLoc = -1;
    nChars = 0;
/*
    Initialize ReadAhead buffer
*/
    cliReadAhead = 0;
/*
    Spawn off the threads
*/
    rc = ThdCreate(WritePortThread,
		   (PBYTE *) &pStackWPT, &selStackWPT, &tidWPT);
    rc = ThdCreate(ReadPortThread,
		   (PBYTE *) &pStackRPT, &selStackRPT, &tidRPT);
    rc = ThdCreate(WriteScreenThread,
		   (PBYTE *) &pStackWST, &selStackWST, &tidWST);
/*
    Open up the COM port and the circular queue
*/
    if (rc = ComInit(Term))	{ /* Initialize terminal */
	MessageBox(MBE_OPENPORT, NOUPDATE);
    } 
    fWrap = Term.fWrap;
}

void ThdTerminate(void) {
/*
    Kill the threads (maybe time delayed) and clean up.
    Yes, I throw away the return codes.

    Problem:  The ReadPort thread might not die, because it blocks
    waiting for a character to be read.
*/
    if (fAlive) {
	fAlive = FALSE;			/* Kill thread loops		*/
	if (fBreak) rc = ComUnbreak();	/* Remove break signal		*/
	rc = ComClose();		/* Close the port 		*/
	DosSemClear(&lSemEmpty);	/* Make WritePort unblock	*/
	DosSemClear(&lSemRAFull);	/* Make ReadPort unblock	*/
	DosSemClear(&lSemRAEmpty);	/* Make WriteScreen unblock	*/
    }
}

void ThdDoBreak(void) {
/*
    Try to send break for a second
*/
    ComBreak();
    DosSleep(1000L);
    ComUnbreak();
}

int ThdPutChar(char ch) {
/*
    Perhaps we will enter an entire key packet
    But we can't really block, because we gotta respond.
    Solution:  This time, we time out.
*/
/*
    Manipulate the typeahead buffer (circular queue)
*/
    if (nChars >= BUFSIZE) {		/* Block if buffer full */
	DosSemSet(&lSemFull);
	/*
	    Timeout possibility; probably want TIMEOUT < 1 second
	*/
	if (rc = DosSemWait(&lSemFull, TIMEOUT)) return rc;
    }
    ThdBufNextLoc(nBufLoc);		/* Increments to next location */
    /*
	Be really impatient...
	This protects the queue, but we should never read/write
	the same queue location.  Maybe I'll move it out later.
    */
    if (rc = DosSemRequest(&lSemLock, TIMEOUT)) { /* Another quick timeout */
	ThdBufLastLoc(nBufLoc);
	return rc;
    }
    TypeAhead[nBufLoc] = ch; 
    nChars++;
    DosSemClear(&lSemEmpty);
    DosSemClear(&lSemLock);
    return 0;
}

void far WritePortThread(void) {
/*
    The routine which writes your WM_CHARS to the port,
    one at a time, nice and easy.  We can even wait all
    day (and will, at the rate a user types....)

    The typeahead buffer is protected with semaphores,
    although a move is probably an atomic operation;
    also, it should be secure to read the element, without
    having to protect the buffer.
*/
    int MyLoc = -1;
    char ch;

    while (fAlive) {
	if (nChars < 1) {		/* Wait if the queue is empty */
	    DosSemSet(&lSemEmpty);
	    DosSemWait(&lSemEmpty, MAXTIMEOUT);
	} else if (!(rc = DosSemRequest(&lSemLock, MAXTIMEOUT))) {
	    ThdBufNextLoc(MyLoc);
	    ch = TypeAhead[MyLoc];	/* writing to the port is slow, and */
	    nChars--;			/* we want to release the semaphore */
	    DosSemClear(&lSemFull);
	    DosSemClear(&lSemLock);
	    if (rc = ComWrite(TypeAhead[MyLoc])) {
		/* Post the message... */
		MessageBox(MBE_WRITEPORT, NOUPDATE);
	    }
	}
    }
    DosExit(EXIT_THREAD, 0);
}

void far ReadPortThread(void) {
/*
    Read from the port, a line at a time
    The semaphores force private queue access
*/
    int iLine = 0;

    while (fAlive) {
	if (cliReadAhead < RASIZE) {
	    if (ComRead(&aliReadAhead[iLine]))
		MessageBox(MBE_COMREAD, (MPARAM) ComError());
	    else if (&aliReadAhead[iLine].cch) {
		DosSemRequest(&lSemRALock, MAXTIMEOUT);
		cliReadAhead++;
		DosSemClear(&lSemRALock);
		DosSemClear(&lSemRAEmpty);
		ThdNextRALoc(iLine);
	    }
	} else {
	    DosSemSet(&lSemRAFull);
	    DosSemWait(&lSemRAFull, TIMEOUT);
	}
    }
    DosExit(EXIT_THREAD, 0);
}

int ThdPutString(char a[], int n) {
    int i, rc = 0;

    for (i = 0; i < n; i++)
	if (!rc) rc = ThdPutChar(a[i]);
    return rc;
}

void Process(Line lCmd, Line lOutput, int far *pi) {
/*
    This routine filters characters from the port, before we
    display them to the screen.

    To make this a full blown terminal emulator application,
    all that's needed is to trap cursor movement sequences
    here, and then to retrieve the appropriate queue line.
*/
    USHORT usTemp;
    int i;

    while ((lCmd->szText[*pi] != '\n') && ((*pi) < (int) lCmd->cch)) {
	switch (lCmd->szText[*pi]) {
	    case '\b':
		if (lOutput->cch > 0) lOutput->cch--;
		break;
	    case '\r':
	    case '\0':
		break;
	    case '\007':	/* Ctrl G */
		WinAlarm(HWND_DESKTOP, WA_NOTE);
		break;
	    case '\t':
		if ((usTemp = (((lOutput->cch >> 3) + 1) << 3)) < MAXLINELEN) {
		    for (i = lOutput->cch; i < (SHORT) usTemp; i++)
			lOutput->szText[i] = ' ';
		    lOutput->cch = usTemp;
		}
		break;
	    default:
		if (fWrap) {
		    if (lOutput->cch >= MAXLINELEN) {
			lOutput->fComplete = TRUE;
			lOutput->fDrawn = FALSE;
			return;
		    } else lOutput->szText[lOutput->cch++] = lCmd->szText[*pi];
		} else {
		    if (lOutput->cch < MAXLINELEN) /* Straight nowrap */
			lOutput->szText[lOutput->cch++] = lCmd->szText[*pi];
		}
		break;
	}
	(*pi)++;
    }
    /*
	Complete line if it doesn't exhaust the command string, and
	ends with a newline (\n).  If so, increment string length, but
	don't write (potentially) out of the array bounds.
    */
    if (lOutput->fComplete =
		(((*pi) < (int) lCmd->cch) && (lCmd->szText[*pi] == '\n')))
	(*pi)++;

    lOutput->fDrawn = FALSE;
}

void ThdReset(void) {
    DosSemClear(&lSemOverflow);
}

void far WriteScreenThread(void) {
    int		iLine = 0;
    LineInfo	liQueueEntry;
    int		iLinePos = 0;
    BOOL	fMore = FALSE;

    liQueueEntry.cch = 0;
    while (fAlive) {
	if ((cliReadAhead > 0) || fMore) {
	    Process(&aliReadAhead[iLine], &liQueueEntry, &iLinePos);
	    /*
		Add the entry to the queue if it's:
		    1) A complete line
		    2) There's nothing more to read
	    */
	    if (liQueueEntry.fComplete || (cliReadAhead <= 1)) {
		while (!QueInsertLine(&liQueueEntry)) {
		    MessageBox(MBE_QUEUEFULL, UPDATE);
		    DosSemSet(&lSemOverflow);
		    DosSemWait(&lSemOverflow, MAXTIMEOUT);
		}
		/*
		    If complete line is read, reset line pointer
		*/
		if (liQueueEntry.fComplete) {
		    liQueueEntry.cch = 0;
		    /*
			If done moving, move cursor to the right place.
			(the beginning of the next line), so that the
			user knows when they've hit <Enter>.
		    */
		    if (cliReadAhead <= 1) {
			liQueueEntry.fComplete = liQueueEntry.fDrawn = FALSE;
			while (!QueInsertLine(&liQueueEntry)) {
			    MessageBox(MBE_QUEUEFULL, UPDATE);
			    DosSemSet(&lSemOverflow);
			    DosSemWait(&lSemOverflow, MAXTIMEOUT);
			}
		    }
		}
	    }
	    /*
		If there's no more to process, bump the read pointer
		and possibly unblock the Read Port thread
	    */
	    if (!(fMore = (iLinePos < (SHORT)aliReadAhead[iLine].cch))) {
		ThdNextRALoc(iLine);
		iLinePos = 0;
		DosSemRequest(&lSemRALock, MAXTIMEOUT);
		cliReadAhead--;
		DosSemClear(&lSemRALock);
		DosSemClear(&lSemRAFull);
	    }
	} else {
	    if (fNoUpdate) {
		fNoUpdate = WinPostMsg(hWndMaster, WM_AVIOUPDATE, NULL, NULL);
	    }
	    DosSemSet(&lSemRAEmpty);
	    DosSemWait(&lSemRAEmpty, TIMEOUT);
	}
    }
    DosExit(EXIT_THREAD, 0);
}
