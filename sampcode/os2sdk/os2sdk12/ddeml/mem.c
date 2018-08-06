#include "ddemlp.h"

/*
 * Memory routines
 *
 *  These routines simplify and centralize memory handling to help reduce
 *  selector usage overhead and ease transport to 32 bit memory models.
 */


/*
 * Debugging routines --------------------------------------------------
 */

PBYTE FarAllocMem(hheap, cb)
HHEAP hheap;
USHORT cb;
{
    register NPBYTE pb;

    pb = MyAllocMem(hheap, cb);
    if (pb == NULL)
        return(NULL);
    return(MAKEP(SELECTOROF(hheap), pb));
}


#ifdef DEBUG

#define MAX_SQUIRL 400
#define HPSIG 0xfedc

PUSHORT asqu[MAX_SQUIRL] = { NULL };
USHORT cMaxSqu = 0;
USHORT cCurSqu = 0;

VOID Walk(VOID);
VOID Squirl(PBYTE);
VOID Unsquirl(PBYTE);


VOID Walk()
{
    static BOOL fRecurse = FALSE;
    register USHORT i;

    if (fRecurse)
        return;
    fRecurse = TRUE;
    for (i = 0; i < cMaxSqu; i++) {
        if (OFFSETOF(asqu[i]) < CheckSel(SELECTOROF(asqu[i])))
            if (!(*asqu[i] == HPSIG &&
                    (*(PUSHORT)((PBYTE)asqu[i] +
                    (*(asqu[i] - 1)) - 2) == HPSIG))) {
                AssertF(FALSE, "Heap Walk failure");
                asqu[i] = NULL;
            }
    }
    fRecurse = FALSE;
}

VOID Squirl(p)
PBYTE p;
{
    register USHORT i;
    
    for (i = 0; i < cMaxSqu + 1; i++) {
        if (asqu[i] == NULL) {
            asqu[i] = (PUSHORT)p;
            cCurSqu = min(cCurSqu + 1, MAX_SQUIRL - 1);
            cMaxSqu = max(cCurSqu, cMaxSqu);
            return;
        }
    }
}

VOID Unsquirl(p)
PBYTE p;
{
    register USHORT i;
    
    for (i = 0; i < cMaxSqu + 1; i++) {
        if (asqu[i] == (PUSHORT)p) {
            asqu[i] = NULL;
            cCurSqu--;
            return;
        }
    }
    AssertF(cMaxSqu == (MAX_SQUIRL - 1), "Unsquirl error");
}


HHEAP DbgCreateHeap(
SEL sel,
USHORT cb,
USHORT cbGrow,
USHORT cbMinDed,
USHORT cbMaxDed,
USHORT fs)
{
    HHEAP hheap;
    register USHORT i;

    hheap = WinCreateHeap(sel, cb, cbGrow, cbMinDed, cbMaxDed, fs);
    sel = HIUSHORT(WinLockHeap(hheap));
    /*
     * remove all squirled pointers in this segment so we don't
     * get errors later.
     */
    for (i = 0; i < cMaxSqu; i++) {
        if (SELECTOROF(asqu[i]) == sel) {
            asqu[i] = NULL;
            cCurSqu--;
        }
    }
    return(hheap);
}



HHEAP DbgDestroyHeap(
HHEAP hheap)
{
    /*
     * purge squirl list of any of these pointers.
     */
    register USHORT i;
    
    for (i = 0; i < cMaxSqu; i++) {
        if (SELECTOROF(asqu[i]) == SELECTOROF(hheap)) {
            asqu[i] = NULL;
            cCurSqu--;
        }
    }
    return(WinDestroyHeap(hheap));
}



NPBYTE DbgAllocMem(hheap, cb)
HHEAP hheap;
USHORT cb;
{
    register PBYTE p;

    Walk();
    p = MAKEP(SELECTOROF(hheap), WinAllocMem(hheap, cb + 4));
    if (LOUSHORT(p) == NULL) 
        return(NULL);
    Squirl(p);
    *(PUSHORT)p = HPSIG;
    *(PUSHORT)(p + cb + 2) = HPSIG;
    return((NPBYTE)(SHORT)p + 2);
}


/*
 * note that cbOld perameter includes the signatures since it was derived
 * from the heap itself.  cbNew, however does not incude the signatures.
 */
NPBYTE DbgReallocMem(hheap, npMem, cbOld, cbNew)
HHEAP hheap;
NPBYTE npMem;
USHORT cbOld;
USHORT cbNew;
{
    register PBYTE p;

    Walk();
    p = MAKEP(SELECTOROF(hheap), npMem - 2);
    *(PUSHORT)p = 0;
    *(PUSHORT)(p + cbOld - 2) = 0;
    Unsquirl(p);
    p = MAKEP(SELECTOROF(hheap), WinReallocMem(hheap, npMem - 2, cbOld, cbNew + 4));
    if (LOUSHORT(p) == NULL) {
        AssertF(FALSE, "Reallocation failure");
        return(NULL);
    }
    Squirl(p);
    *(PUSHORT)p = HPSIG;
    *(PUSHORT)(p + cbNew + 2) = HPSIG;
    return((NPBYTE)(SHORT)p + 2);
}



/*
 * note that cbMem perameter includes the signatures since it was derived
 * from the heap itself.
 */
NPBYTE DbgFreeMem(hheap, npMem, cbMem)
HHEAP hheap;
NPBYTE npMem;
USHORT cbMem;
{
    register PBYTE p;

    Walk();
    p = MAKEP(SELECTOROF(hheap), npMem - 2);
    Unsquirl(p);
    cbMem += 4;
    AssertF(cbMem == *(PUSHORT)(p - 2), "FreeMem:Incorrect cbMem perameter")
        
    if (*(PUSHORT)p != HPSIG || *(PUSHORT)(p + cbMem - 2) != HPSIG)
        AssertF(FALSE, "Freed memory is corrupted");
    *(PUSHORT)p = 0;
    *(PUSHORT)(p + cbMem - 2) = 0;
    p = (PBYTE)WinFreeMem(hheap, npMem - 2, cbMem);
    if (LOUSHORT(p) != 0)
        AssertF(FALSE, "Memory freeing failed");
    return((NPBYTE)LOUSHORT(p));
}

#endif /* DEBUG ------------------------------------------------------   */
