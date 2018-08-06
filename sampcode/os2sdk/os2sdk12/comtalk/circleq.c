/*
    Circular Queue buffer implementation (which gets read by AVIO module)
    Created by Microsoft Corporation, 1989
*/
#define INCL_DOSSEMAPHORES
#include <os2.h>		/* Need USHORT for global.h */
#include <string.h>		/* One strcpy call */
#include "global.h"
#include "circleq.h"

#define	TIMEOUT	1000L		/* A second */

LineInfo aliRing[QUEUESIZE];	/* The Circular Queue...*/
int  iHead, iTail;
BOOL fFirst;			/* Are we just starting? */
LONG lSemMyQueue;		/* Queue lock */

void LineCopy(Line, Line);
void QueFill(void);

#define QueLock()   DosSemRequest(&lSemMyQueue, -1L)
#define QueUnlock() DosSemClear(&lSemMyQueue)

#define Fix(n) (((n) >= 0) ? (n) : ((n) + QUEUESIZE))
#define Circle(x)	((x) % QUEUESIZE)
#define Incr(x) 	(x = Circle(x + 1))
#define Decr(x)		(x = (x > 0) ? (x - 1) : (QUEUESIZE - 1))

void QueFill(void) {
    int i, j;

    for (i = 0; i < 25; i++) {
	aliRing[i].cch = MAXLINELEN;
	for (j = 0; j < MAXLINELEN; j++)
	    aliRing[i].szText[j] = (char) (((i * j) % 10) + '0');
    }
    iHead = 0; iTail = 24;
}

void QueInit(void) {
    int i;

    fFirst = TRUE;
    QueLock();
    iHead = 0; iTail = 0;
    for (i = 0; i < QUEUESIZE; i++) aliRing[i].cch = 0;
    QueUnlock();
}

void QueAdvance(int n) {
    QueLock();
    iHead = Circle(iHead + n);
    QueUnlock();
}

Line QueQuery(int LineNum) { return &aliRing[Circle(iHead + LineNum)]; }

BOOL QueInsertLine(Line pli) {
/*
    Return FALSE if we try to overwrite the head
*/
    QueLock();
    /*
	Initialize the queue
    */
    if (fFirst) fFirst = FALSE;
    /*
	Increment TAIL, act if queue full
	Overwrite if last entry was incomplete
    */
    else if (aliRing[iTail].fComplete && (Incr(iTail) == iHead)) {
	/*
	    We are overflowing...
	*/
	Decr(iTail);
	QueUnlock();
	return FALSE;
    }
    /*
	Insert the element
    */
    LineCopy(pli, &aliRing[iTail]);
    QueUnlock();
    return TRUE;
}

BOOL QueCompleteLine(void) { return aliRing[iTail].fComplete; }

void LineCopy(Line pliSrc, Line pliDst) {
    int i;

    pliDst->fDrawn		= pliSrc->fDrawn;
    pliDst->fComplete		= pliSrc->fComplete;
    pliDst->cch			= pliSrc->cch;
    for (i = 0; i < (int) pliSrc->cch; i++) pliDst->szText[i] = pliSrc->szText[i];
}

int QueUpdateHead(int nRows, BOOL bPage, BOOL bPaging) {
    int i, nLines;

    nLines = Fix(Circle(iTail - iHead));
    nLines = (nLines >= nRows) ? (nLines - nRows + 1) : 0;
    if ((nLines = Min(nLines, nRows)) > 0) {
	if (bPage) {
	    if (nLines < nRows) {
		QueLock();
		for (i = nLines; i < nRows; i++)
		    aliRing[Circle(iHead + nRows + i)].cch = 0;
		QueUnlock();
	    }
	    nLines = nRows;
	}
	else if (bPaging) nLines = 0;
	QueLock();
	iHead = Circle(iHead + nLines);
	QueUnlock();
    }
    return nLines;
}

Line QueLastLine(void) {
    QueLock();
    aliRing[iTail].szText[aliRing[iTail].cch] = '\0';
    QueUnlock();
    return &aliRing[iTail];
}

int QuePageUp(int nRows) {
    int i, nLines;

    QueLock();
    nLines = Min((QUEUESIZE - 1) - Fix(Circle(iTail - iHead)), nRows);
    if (nLines) {
	iHead = Fix(Circle(iHead - nLines));
	for (i = 0; i < nLines; i++)
	    aliRing[Circle(iHead + nRows + i)].fDrawn = FALSE;
    }
    QueUnlock();
    return nLines;
}
