#define INCL_WINCATCHTHROW 
#include "ddemlp.h"
#include "stdio.h"

int	APIENTRY DebugOutput( PCH );        /* private import */
CATCHBUF catchbuf;

void DumpItems(PULONG pul, USHORT c);
void err(PSZ psz);
void err2(PSZ psz, ULONG ul);

typedef struct _ULLI {
    PLITEM next;
    ULONG ul;
} ULLI;
typedef ULLI FAR *PULLI;

/*
 * exported for testing the DLL
 */
void EXPENTRY DdeTest()
{
#if 0    
    SEL sel;
    USHORT c;
    ULLI SchItem;
    PLST plst;
    PPILE ppile;
    HHEAP hheap;
    PLITEM pli;
    PBYTE pbytes;

    AssertF(FALSE, "Testing assertion failure");
    
    DebugOutput("\n\rList functions test\n\r");
    SemEnter();
    if (DosAllocSeg(2000, &sel, SEG_NONSHARED))
        err("DosAllocSeg failed");
    if (!(hheap = MyCreateHeap(sel, 0, 0, 0, 0, HEAPFLAGS)))
        err("CreateHeap failed");
    if (!(pbytes = FarAllocMem(hheap, 1000)))
        err("Could not allocate bytes");
    plst = NULL;
    SchItem.ul = 4L;
    SemLeave();
    DebugOutput("\n\rNULL list test....");
    SemEnter();
    DestroyLst(plst);
    FlushLst(plst);
    FindLstItem(plst, CmpULONG, (PLITEM)&SchItem);
    FindLstItem(plst, CmpULONG, NULL);
    FindLstItem(plst, NULL, (PLITEM)&SchItem);
    FindLstItem(plst, NULL, NULL);
    NewLstItem(plst, ILST_NOLINK);
    NewLstItem(plst, ILST_FIRST);
    NewLstItem(plst, ILST_LAST);
    PopLi(plst);
    RemoveLstItem(plst, (PLITEM)&SchItem);
    RemoveLstItem(plst, NULL);
    InsertLstItem(plst, (PLITEM)&SchItem, ILST_FIRST);
    SemLeave();
    DebugOutput("\n\rCreateList...");
    SemEnter();
    plst = CreateLst(hheap, sizeof(ULONG));
    if (!plst) 
        err("CreateLst() failed");
    pli = NewLstItem(plst, ILST_FIRST);
    if (!pli)
        err("NewLstItem() failed");
    ((PULLI)pli)->ul = 1L;
    SchItem.ul = 1L;
    if (pli != FindLstItem(plst, CmpULONG, (PLITEM)&SchItem))
        err("FindLstItem() failed");
    if (pli != PopLi(plst)) {
        err("PopLi() failed");
    }
    if (FindLstItem(plst, NULL, NULL)) 
        err("unexpected success of FindLstItem()");
    for (SchItem.ul = 1L; SchItem.ul < 100L; SchItem.ul++) {
        if (!(pli = NewLstItem(plst, ILST_FIRST)))
            err("NewLstItem() failed");
        ((PULLI)pli)->ul = SchItem.ul;
        if (!FindLstItem(plst, CmpULONG, (PLITEM)&SchItem))
            err2("Item %ul not found", SchItem.ul);
    }
    SchItem.ul = 25L;
    if (!RemoveLstItem(plst, FindLstItem(plst, CmpULONG, (PLITEM)&SchItem)))
        err2("RemoveLstItem() failed on %ul", SchItem.ul);
    SchItem.ul = 47L;
    if (!RemoveLstItem(plst, FindLstItem(plst, CmpULONG, (PLITEM)&SchItem)))
        err2("RemoveLstItem() failed on %ul", SchItem.ul);
    SchItem.ul = 78L;
    if (!RemoveLstItem(plst, FindLstItem(plst, CmpULONG, (PLITEM)&SchItem)))
        err2("RemoveLstItem() failed on %ul", SchItem.ul);
    for (SchItem.ul = 1L; SchItem.ul < 100L; SchItem.ul++) {
        if (SchItem.ul == 25L || SchItem.ul == 47L || SchItem.ul == 78L) {
            if (FindLstItem(plst, CmpULONG, (PLITEM)&SchItem))
                err2("Unexpected find of %ul", SchItem.ul);
        } else if (!FindLstItem(plst, CmpULONG, (PLITEM)&SchItem))
            err2("Unexpected failure to find %ul", SchItem.ul);
    }
    FlushLst(plst);
    if (PopLi(plst))
        err("Unexpected pass of PopLi()");
    DestroyLst(plst);
    plst = NULL;
    SemLeave();
    DebugOutput("\n\rList test passed.\n\r");


    DebugOutput("\n\rNULL Pile test...");
    SemEnter();
    ppile = NULL;
    DestroyPile(ppile);
    FlushPile(ppile);
    if (QPileItemCount(ppile))
        err("Unexpected pass of QPileItemCount()");
    if (CopyPileItems(ppile, pbytes))
        err("Unexpected pass of CopyPileItems()");
    if (FindPileItem(ppile, NULL, NULL, FPI_COUNT))
        err("Wrong count from FindPileItem()");
    if (FindPileItem(ppile, NULL, NULL, FPI_DELETE))
        err("Unexpected pass of FindPileItem()");
    if (AddPileItem(ppile, NULL, NULL))
        err("Unexpected pass of AddPileItem()");
    
    SemLeave();
    DebugOutput("\n\rPile test...");
    SemEnter();
    if (!(ppile = CreatePile(hheap, sizeof(ULONG), 8)))
        err("Unexpected failure of CreatePile()");
    for (SchItem.ul = 1L; SchItem.ul < 100; SchItem.ul++) {
        AddPileItem(ppile, (PBYTE)&SchItem.ul, CmpULONG);
        if (SchItem.ul > 25) 
            if (AddPileItem(ppile, (PBYTE)&SchItem.ul, CmpULONG))
                err("Unexpected TRUE(1) return by AddPileItem()");
        if (SchItem.ul > 50) 
            if (AddPileItem(ppile, (PBYTE)&SchItem.ul, CmpULONG))
                err("Unexpected TRUE(2) return by AddPileItem()");
        if (SchItem.ul > 75) 
            if (AddPileItem(ppile, (PBYTE)&SchItem.ul, CmpULONG))
                err("Unexpected TRUE(3) return by AddPileItem()");
    }
    if (QPileItemCount(ppile) != 99)
        err("Total pile count wrong");
    for (SchItem.ul = 1L; SchItem.ul < 100L; SchItem.ul++) {
        if (1 != (USHORT)FindPileItem(ppile, CmpULONG, (PBYTE)&SchItem.ul, FPI_COUNT))
            err("Wrong count of items found by FindPileItem()");
    }                                                 
    for (SchItem.ul = 26L; SchItem.ul < 100L; SchItem.ul++) {
        AddPileItem(ppile, (PBYTE)&SchItem.ul, NULL);
        if (SchItem.ul > 50) 
            if (!AddPileItem(ppile, (PBYTE)&SchItem.ul, NULL))
                err("Unexpected FALSE(2) return by AddPileItem()");
        if (SchItem.ul > 75) 
            if (!AddPileItem(ppile, (PBYTE)&SchItem.ul, NULL))
                err("Unexpected FALSE(3) return by AddPileItem()");
    }
    for (SchItem.ul = 1L; SchItem.ul < 26L; SchItem.ul++) {
        c = (USHORT)FindPileItem(ppile, CmpULONG, (PBYTE)&SchItem.ul, FPI_COUNT);
        if (c != 1)
            err("Wrong count of items found by FindPileItem()");
    }                                                 
    for (SchItem.ul = 26L; SchItem.ul < 51L; SchItem.ul++) {
        c = (USHORT)FindPileItem(ppile, CmpULONG, (PBYTE)&SchItem.ul, FPI_COUNT);
        if (c != 2)
            err("Wrong count of items found by FindPileItem()");
    }                                                 
    for (SchItem.ul = 51L; SchItem.ul < 76L; SchItem.ul++) {
        c = (USHORT)FindPileItem(ppile, CmpULONG, (PBYTE)&SchItem.ul, FPI_COUNT);
        if (c != 3)
            err("Wrong count of items found by FindPileItem()");
    }                                                 
    for (SchItem.ul = 76L; SchItem.ul < 100L; SchItem.ul++) {
        c = (USHORT)FindPileItem(ppile, CmpULONG, (PBYTE)&SchItem.ul, FPI_COUNT);
        if (c != 4)
            err("Wrong count of items found by FindPileItem()");
    }
    if ((c = QPileItemCount(ppile)) != 99 + 74 + 49 + 24)
        err("Total pile count wrong");
    CopyPileItems(ppile, pbytes);
    DestroyPile(ppile);
    SemLeave();
    DumpItems((PULONG)pbytes, c);
    MyDestroyHeap(hheap);
#endif    
    return;
}



void DumpItems(pul, c)
PULONG pul;
USHORT c;
{
    char sz[100];
    char far *psz;

    psz = sz;
    while (c--) {
        if ((c % 7) == 0) {
            *psz++ = '\n';
            *psz++ = '\r';
            *psz++ = '\0';
            DebugOutput(sz);
            psz = sz;
        }
        psz = ltoa(*pul, psz, &sz[99]);
        *psz++ = ' ';
        pul++;
    }
    *psz++ = '\n';
    *psz++ = '\r';
    *psz++ = '\0';
    DebugOutput(sz);
}



void err(psz)
PSZ psz;
{
    SemLeave();
    DebugOutput(psz);
}

void err2(psz, ul)
PSZ psz;
ULONG ul;
{
    char sz[80];

    SemLeave();
    sz[0] = '\000';
    psz = lstrcat(sz, psz, &sz[79]);
    psz = ltoa(ul, psz, &sz[79]);
    DebugOutput(sz);
}
