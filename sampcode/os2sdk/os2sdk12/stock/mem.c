/***	mem.c - memory definitions
 *
 *	routines that layer C run-time library memory
 *	routines to provide debugging assistance.
 *
 *	History
 *	    07-Mar-1990 bens	Initial version.
 *	    08-Mar-1990 bens	Added heap check code
 *	    16-Mar-1990 bens	MyAlloc should return NULL on failure!
 */
#ifdef CHECKASSERTS

#define INCL_WINWINDOWMGR

#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "mem.h"

typedef ULONG MEMSIG;	 // ms
typedef MEMSIG *PMEMSIG; // pms

typedef struct mh_s {	 // mh
    MEMSIG	    ms; 		// Head signature
    USHORT	    cb; 		// Size of user block
    struct mh_s *   pmhNext;		// Next block
    struct mh_s *   pmhPrev;		// Previous block
    // char ach[?];			// User block; length is cb
    // MEMSIG ms[?];			// Tail signature area
} MEMHDR;
typedef MEMHDR *PMEMHDR; // pmh

#define MEMSIG_HEAD    0x12345678L
#define MEMSIG_TAIL    0x87654321L
#define MEMSIG_BAD     0L
#define TAIL_SIZE   2			// 8-byte tail

void FailAssert(char *sz);

char achBug[CCHMAXPATH];		// Buffer for formatting errors
PMEMHDR pmhList=NULL;			// List of memory blocks


/***	FailAssert - common exit for assertion failure
*
*	ENTRY	sz - message string
*
*	EXIT	issue message and exit
*/
void FailAssert(char *sz)
{
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, sz,
	"Assertion Failure", 0, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
    DosExit(1,2);
}


void MyCheckHeap(char *pszFile,USHORT line)
{
    PMEMHDR pmh;

    for (pmh = pmhList; pmh != NULL; pmh = pmh->pmhNext)
	MyAssert((char *)pmh+sizeof(MEMHDR),pszFile,line);
}


void MyAssert(void *pv,char *pszFile,USHORT line)
{
    USHORT  cb;
    int     i;
    PMEMHDR pmh;
    PMEMSIG pms;

    pmh = (PMEMHDR)((char *)pv - sizeof(MEMHDR));
    if (pmh > pv) {			// Pointer wrapped
	sprintf(achBug,"%s:%d: panic: NULL pointer(?)",pszFile,line);
	FailAssert(achBug);
    }

    // Test head signature
    if (pmh->ms != MEMSIG_HEAD) {
	sprintf(achBug,"%s:%d: panic: memory header trashed",pszFile,line);
	FailAssert(achBug);
    } else {
	// Test tail signatures
	pms = (PMEMSIG)( (char *)pmh + sizeof(MEMHDR) + pmh->cb );
	for (i=0; i<TAIL_SIZE; i++) {
	    if (*pms++ != MEMSIG_TAIL) {
		sprintf(achBug,"%s:%d: panic: memory tail trashed",pszFile,line);
		FailAssert(achBug);
	    }
	}
    }

}


void MyFree(void *pv,char *pszFile,USHORT line)
{
    PMEMHDR pmh;

    MyAssert(pv,pszFile,line);

    pmh = (PMEMHDR)((char *)pv - sizeof(MEMHDR));

    // Make previous block point to next block
    if (pmh->pmhPrev != NULL) { 	// pmh is not at front of list
	// before: a->p->?
	pmh->pmhPrev->pmhNext = pmh->pmhNext;
	// after:  a->?
    }
    else {				// pmh is at front of list
	// before: list->p->?
	pmhList = pmh->pmhNext;
	// after: list->?
    }

    // Make next block point to previous block
    if (pmh->pmhNext != NULL) { 	// pmh is not at end of list
	// before: ?<-p<->a
	pmh->pmhNext->pmhPrev = pmh->pmhPrev;
	// after:  ?<-a
    }

    // Obliterate signature
    pmh->ms = MEMSIG_BAD;

    // Free memory
    free((char *)pmh);
}


void *MyAlloc(USHORT cb,char *pszFile,USHORT line)
{
    USHORT	cbAlloc;
    int 	i;
    char *	pch;
    PMEMHDR	pmh;
    PMEMSIG	pms;

    MyCheckHeap(pszFile,line);

    cbAlloc = cb+sizeof(MEMHDR)+sizeof(MEMSIG_TAIL)*TAIL_SIZE;
    pmh = malloc(cbAlloc);
    if (pmh != NULL) {
	pmh->ms = MEMSIG_HEAD;		// Store head signature
	pmh->cb = cb;			// Store size of user block

	// Add block to front of list (Easiest code!)
	if (pmhList != NULL)		// List is not empty
	    pmhList->pmhPrev = pmh;	// Point old top block back at us
	pmh->pmhNext = pmhList; 	// Next element is old top block
	pmh->pmhPrev = NULL;		// We are first, so no prev block
	pmhList = pmh;			// Make ourselves first

	// Fill in tail signatures
	pms = (PMEMSIG)( (char *)pmh + sizeof(MEMHDR) + pmh->cb );
	for (i=0; i<TAIL_SIZE; i++)
	    *pms++ = MEMSIG_TAIL;
	return ((char *)pmh + sizeof(MEMHDR));
    }
    else {
	printf("panic: out of memory in MyAlloc\n");
	printf("\n");
	printf("Dump of heap (newest alloc to oldest)\n");
	printf("\n");
	printf("Size  Addr Content\n");
	printf("----- ---- -------\n");
	for (pmh = pmhList; pmh != NULL; pmh = pmh->pmhNext) {
	    pch = (char *)pmh + sizeof(MEMHDR);
	    printf("%5d %04x %s\n",pmh->cb,(USHORT)pch,pch);
	}
	return NULL;
    }
}


char *MyStrDup(char *pch,char *pszFile,USHORT line)
{
    USHORT  cb;
    char *  pchDst;

    cb = strlen(pch)+1; 		// Count NUL terminator
    pchDst = MyAlloc(cb,pszFile,line);	// Alloc new copy
    if (pchDst != NULL) {		//  Success
	memcpy(pchDst,pch,cb);		// Copy string
    }
    return pchDst;			// Return string copy
}
#endif
