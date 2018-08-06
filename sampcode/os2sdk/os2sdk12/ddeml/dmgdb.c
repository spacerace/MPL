/****************************** Module Header ******************************\
* Module Name: DMGDB.C
*
* DDE manager data handling routines
*
* Created: 12/14/88 Sanford Staab
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "ddemlp.h"

/***************************** Private Function ****************************\
* PAPPINFO GetCurrentAppInfo()
*
* DESCRIPTION:
* This routine uses the pid of the current thread to locate the information
* pertaining to that thread.  If not found, 0 is returned.
*
* This call fails if the DLL is in a callback state to prevent recursion.
* if fChkCallback is set.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
PAPPINFO GetCurrentAppInfo(fChkCallback)
BOOL fChkCallback;
{
    PAPPINFO pai;

    SemEnter();
    if (pAppInfoList == NULL || !CheckSel(SELECTOROF(pAppInfoList))) {
        SemLeave();
        return(0);
    }
    pai = pAppInfoList;
    while (pai) {
        if (pai->pid == FSRSemDmg.pid && pai->tid == FSRSemDmg.tid) {
            if (fChkCallback && pai->cInCallback > MAX_RECURSE) {
                pai->LastError = DMGERR_REENTRANCY;
                break;
            } else {
                SemLeave();
                return(pai);
            }
        }
        pai = pai->next;
    }
    SemLeave();
    return(0);
}


/***************************** Private Function ****************************\
* void UnlinkAppInfo(pai)
* PAPPINFO pai;
*
* DESCRIPTION:
*   unlinks an pai safely.  Does nothing if not linked.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
void UnlinkAppInfo(pai)
PAPPINFO pai;
{
    PAPPINFO paiT;

    AssertF(pai != NULL, "UnlinkAppInfo - NULL input");
    SemEnter();
    if (pai == pAppInfoList) {
        pAppInfoList = pai->next;
        SemLeave();
        return;
    }
    paiT = pAppInfoList;
    while (paiT && paiT->next != pai)
        paiT = paiT->next;
    if (paiT)
        paiT->next = pai->next;
    SemLeave();
    return;
}





/***************************** Private Functions ***************************\
* General List management functions.
*
* History:
*   Created     12/15/88    sanfords
\***************************************************************************/
PLST CreateLst(hheap, cbItem)
HHEAP hheap;
USHORT cbItem;
{
    PLST pLst;

    SemEnter();
    if (!(pLst = (PLST)FarAllocMem(hheap, sizeof(LST)))) {
        SemLeave();
        return(NULL);
    }
    pLst->hheap = hheap;
    pLst->cbItem = cbItem;
    pLst->pItemFirst = (PLITEM)NULL;
    SemLeave();
    return(pLst);
}



void FlushLst(pLst)
PLST pLst;
{
    if (pLst == NULL)
        return;
    SemEnter();
    while (pLst->pItemFirst) 
        RemoveLstItem(pLst, pLst->pItemFirst);
    SemLeave();
}



void DestroyLst(pLst)
PLST pLst;
{
    if (pLst == NULL)
        return;
    SemEnter();
    while (pLst->pItemFirst) 
        RemoveLstItem(pLst, pLst->pItemFirst);
    FarFreeMem(pLst->hheap, pLst, sizeof(LST));
    SemLeave();
}



PLITEM FindLstItem(pLst, npfnCmp, piSearch)
PLST pLst;
NPFNCMP npfnCmp;
PLITEM piSearch;
{
    PLITEM pi;

    if (pLst == NULL)
        return(NULL);
    SemEnter();
    pi = pLst->pItemFirst;
    while (pi) {
        if ((*npfnCmp)
                ((PBYTE)pi + sizeof(LITEM), (PBYTE)piSearch + sizeof(LITEM))) {
            SemLeave();
            return(pi);
        }
        pi = pi->next;
    }
    SemLeave();
}



/*
 * Comparison functions for FindLstItem() and FindPileItem()
 */

BOOL CmpULONG(pb1, pb2)
PBYTE pb1;
PBYTE pb2;
{
    return(*(PULONG)pb1 == *(PULONG)pb2);
}

BOOL CmppHsz(pb1, pb2)
PBYTE pb1;
PBYTE pb2;
{
    return(CmpHsz(*(PHSZ)pb1, *(PHSZ)pb2) ? FALSE : TRUE);
}




/***************************** Private Function ****************************\
* This routine creates a new list item for pLst and links it in according
* to the ILST_ constant in afCmd.  Returns a pointer to the new item
* or NULL on failure.
*
* Note:  This MUST be in the semaphore for use since the new list item
* is filled with garbage on return yet is linked in.  
* 
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
PLITEM NewLstItem(pLst, afCmd)
PLST pLst;
USHORT afCmd;
{
    PLITEM pi, piT;

    if (pLst == NULL)
        return(NULL);
    SemCheckIn();
    
    pi = (PLITEM)FarAllocMem(pLst->hheap, pLst->cbItem + sizeof(LITEM));
    if (pi == NULL) {
        AssertF(FALSE, "NewLstItem - memory failure");    
        return(NULL);
    }

    if (afCmd & ILST_NOLINK) 
        return(pi);
        
    if (((piT = pLst->pItemFirst) == NULL) || (afCmd & ILST_FIRST)) {
        pi->next = piT;
        pLst->pItemFirst = pi;
    } else {                            /* ILST_LAST assumed */
        while (piT->next != NULL) 
            piT = piT->next;
        piT->next = pi;
        pi->next = NULL;
    }
    return(pi);
}



/***************************** Private Function ****************************\
* This routine unlinks and frees pi from pLst.  If pi cannot be located
* within pLst, it is freed anyway.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
BOOL RemoveLstItem(pLst, pi)
PLST pLst;
PLITEM pi;
{
    PLITEM piT;

    if (pLst == NULL || pi == NULL)
        return(FALSE);
        
    SemCheckIn();
    
    if ((piT = pLst->pItemFirst) != NULL) {
        if (pi == piT) {
            pLst->pItemFirst = pi->next;
        } else {
            while (piT->next != pi && piT->next != NULL)
                piT = piT->next;
            if (piT->next != NULL)
                piT->next = pi->next; /* unlink */
        }
    } else {
        AssertF(pi == NULL, "Improper list item removal");
    }
    FarFreeMem(pLst->hheap, pi, pLst->cbItem + sizeof(LITEM));
    return(TRUE);
}



/***************************** Private Function ****************************\
* This routine uses ILST_ constants to insert a list item into the apropriate
* spot of the pLst given.  Only ILST_FIRST or ILST_LAST are allowed.
*
* History:
*   Created     9/11/89    Sanfords
\***************************************************************************/
BOOL InsertLstItem(pLst, pi, afCmd)
PLST pLst;
PLITEM pi;
USHORT afCmd;
{
    PLITEM piT;

    if (pLst == NULL)
        return(FALSE);
        
    SemEnter();
    
    if (pLst->pItemFirst == NULL || afCmd & ILST_FIRST) {
        pi->next = pLst->pItemFirst;
        pLst->pItemFirst = pi;
    } else {                    /* ILST_LAST assumed */
        piT = pLst->pItemFirst;
        while (piT->next) 
            piT = piT->next;
        piT->next = pi;
        pi->next = NULL;
    }
    
    SemLeave();
    return(TRUE);
}




/*
 * ------------- Specific list routines -------------
 */

/*
 * This function is HIGHLY dependent on the ADVLI structure.
 * This will match an exact hsz/fmt pair with a 0 format being wild.
 */
BOOL CmpAdv(pb1, pb2)
PBYTE pb1;
PBYTE pb2;
{
    USHORT usFmt;
    
    if (*(PHSZ)pb1 == *(PHSZ)pb2) {
        if ((usFmt = *(PUSHORT)(pb2 + 4)) == 0)
            return(TRUE);
        if (usFmt == *(PUSHORT)(pb1 + 4))
            return(TRUE);
    }
    return(FALSE);
}



BOOL fSearchHwndList(pLst, hwnd)
PLST pLst;
HWND hwnd;
{
    HWNDLI hwndi;

    hwndi.hwnd = hwnd;
    return((BOOL)FindLstItem(pLst, CmpHwnd, (PLITEM)&hwndi));
}



void AddHwndList(hwnd, pLst)
HWND hwnd;
PLST pLst;
{
    HWNDLI hwndli;
    PHWNDLI pli;

    AssertF(pLst != NULL, "AddHwndList - NULL pLst");
    AssertF(pLst->cbItem == sizeof(HWNDLI), "AddHwndList - Bad item size");
    SemEnter();
    hwndli.hwnd = hwnd;
    if ((hwnd == NULL) || FindLstItem(pLst, CmpHwnd, (PLITEM)&hwndli)) {
        SemLeave();
        return;
    }
    pli = (PHWNDLI)NewLstItem(pLst, ILST_FIRST);
    pli->hwnd = hwnd;
    SemLeave();
}



/*
 * Insert the given data into the list if one does not already exist
 * under the given hwnd.
 */
void AddAckHwndList(hwnd, hszApp, hszTopic, pLst)
HWND hwnd;
HSZ hszApp;
HSZ hszTopic;
PLST pLst;
{
    HWNDLI hwndli;
    PACKHWNDLI pli;

    AssertF(pLst != NULL, "AddAckHwndList - NULL pLst");
    AssertF(pLst->cbItem == sizeof(ACKHWNDLI), "AddAckHwndList - Bad item size");
    SemEnter();
    hwndli.hwnd = hwnd;
    if ((hwnd == NULL) || FindLstItem(pLst, CmpHwnd, (PLITEM)&hwndli)) {
        SemLeave();
        return;
    }
    pli = (PACKHWNDLI)NewLstItem(pLst, ILST_FIRST);
    pli->hwnd = hwnd;
    pli->hszApp = hszApp;
    pli->hszTopic = hszTopic;
    SemLeave();
}




/***************************** Private Function ****************************\
* hwnd-hsz list functions
*
* History:      1/20/89     Created         sanfords
\***************************************************************************/
void AddHwndHszList(hsz, hwnd, pLst)
HSZ hsz;
HWND hwnd;
PLST pLst;
{
    PHWNDHSZLI phhi;

    AssertF(pLst->cbItem == sizeof(HWNDHSZLI), "AddHwndHszList - Bad item size");
    SemEnter();
    if ((hsz == NULL) || (BOOL)HwndFromHsz(hsz, pLst)) {
        SemLeave();
        return;
    }
    phhi = (PHWNDHSZLI)NewLstItem(pLst, ILST_FIRST);
    phhi->hwnd = hwnd;
    phhi->hsz = hsz;
    IncHszCount(hsz);
    SemLeave();
}


void DestroyHwndHszList(pLst)
PLST pLst;
{
    AssertF(pLst->cbItem == sizeof(HWNDHSZLI), "DestroyHwndHszList - Bad item size");
    SemEnter();
    while(pLst->pItemFirst) {
        FreeHsz(((PHWNDHSZLI)pLst->pItemFirst)->hsz);
        RemoveLstItem(pLst, pLst->pItemFirst);
    }
    FarFreeMem(pLst->hheap, pLst, sizeof(LST));
    SemLeave();
}



HWND HwndFromHsz(hsz, pLst)
HSZ hsz;
PLST pLst;
{
    HWNDHSZLI hhli;
    PHWNDHSZLI phhli;

    hhli.hsz = hsz;
    if (!(phhli = (PHWNDHSZLI)FindLstItem(pLst, CmppHsz, (PLITEM)&hhli)))
        return(NULL);
    return(phhli->hwnd);
}



/***************************** Private Function ****************************\
* DESCRIPTION:
*   Advise list helper functions.
*
* History:      1/20/89     Created         sanfords
\***************************************************************************/
BOOL AddAdvList(pLst, hszItem, fsStatus, usFmt)
PLST pLst;
HSZ hszItem;
USHORT fsStatus;
USHORT usFmt;
{
    PADVLI pali;

    AssertF(pLst->cbItem == sizeof(ADVLI), "AddAdvList - bad item size");
    if (hszItem == NULL) 
        return(TRUE);
    SemEnter();
    if (!(pali = FindAdvList(pLst, hszItem, usFmt))) {
        IncHszCount(hszItem);
        pali = (PADVLI)NewLstItem(pLst, ILST_FIRST);
    }
    AssertF((BOOL)pali, "AddAdvList - NewLstItem() failed")
    if (pali != NULL) {
        pali->hszItem = hszItem;
        pali->usFmt = usFmt;
        pali->fsStatus = fsStatus;
    }
    SemLeave();
    return((BOOL)pali);    
}



/*
 * This will delete the matching Advise loop entry.  If usFmt is 0, all
 * entries with the same hszItem are deleted.  Returns fNotEmptyAfterDelete.
 */
BOOL DeleteAdvList(pLst, hszItem, usFmt)
PLST pLst;
HSZ hszItem;
USHORT usFmt;
{
    PADVLI pali;

    if (hszItem == NULL) 
        return((BOOL)pLst->pItemFirst);
    SemEnter();
    while (pali = (PADVLI)FindAdvList(pLst, hszItem, usFmt)) {
        FreeHsz((pali)->hszItem);
        RemoveLstItem(pLst, (PLITEM)pali);
    }
    SemLeave();
    return((BOOL)pLst->pItemFirst);
}



/***************************** Private Function ****************************\
* This routine searches the advise list for and entry in hszItem.  It returns
* pAdvli only if the item is found.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
PADVLI FindAdvList(pLst, hszItem, usFmt)
PLST pLst;
HSZ hszItem;
USHORT usFmt;
{
    ADVLI advli;

    advli.hszItem = hszItem;
    advli.usFmt = usFmt;
    return((PADVLI)FindLstItem(pLst, CmpAdv, (PLITEM)&advli));
}


/***************************** Private Function ****************************\
* This routine searches for the next entry for hszItem.  It returns
* pAdvli only if the item is found.
*
* History:
*   Created     11/15/89    Sanfords
\***************************************************************************/
PADVLI FindNextAdv(padvli, hszItem)
PADVLI padvli;
HSZ hszItem;
{
    
    SemEnter();
    while ((padvli = (PADVLI)padvli->next) != NULL) {
        if (padvli->hszItem == hszItem) {
            SemLeave();
            return(padvli);
        }
    }
    SemLeave();
    return(NULL);
}



/***************************** Pile Functions ********************************\
*
*  A pile is a list where each item is an array of subitems.  This allows
*  a more memory efficient method of handling unordered lists.
*
\*****************************************************************************/

PPILE CreatePile(hheap, cbItem, cItemsPerBlock)
HHEAP hheap;
USHORT cbItem;
USHORT cItemsPerBlock;
{
    PPILE ppile;

    if (!(ppile = (PPILE)FarAllocMem(hheap, sizeof(PILE)))) {
        SemLeave();
        return(NULL);
    }
    ppile->pBlockFirst = (PLITEM)NULL;
    ppile->hheap = hheap;
    ppile->cbBlock = cbItem * cItemsPerBlock + sizeof(PILEB);
    ppile->cSubItemsMax = cItemsPerBlock;
    ppile->cbSubItem = cbItem;
    return(ppile);
}


PPILE DestroyPile(pPile)
PPILE pPile;
{
    if (pPile == NULL)
        return(NULL);
    SemEnter();
    while (pPile->pBlockFirst) 
        RemoveLstItem((PLST)pPile, (PLITEM)pPile->pBlockFirst);
    FarFreeMem(pPile->hheap, pPile, sizeof(PILE));
    SemLeave();
    return(NULL);
}

void FlushPile(pPile)
PPILE pPile;
{
    if (pPile == NULL)
        return;
    SemEnter();
    while (pPile->pBlockFirst) 
        RemoveLstItem((PLST)pPile, (PLITEM)pPile->pBlockFirst);
    SemLeave();
}


USHORT QPileItemCount(pPile)
PPILE pPile;
{
    register USHORT c;
    PPILEB pBlock;

    if (pPile == NULL)
        return(0);

    SemEnter();
    pBlock = pPile->pBlockFirst;
    c = 0;
    while (pBlock) {
        c += pBlock->cItems;
        pBlock = pBlock->next;
    }
    SemLeave();
    return(c);
}


BOOL CopyPileItems(pPile, pDst)
PPILE pPile;
PBYTE pDst;
{
    PPILEB pBlock;
    
    AssertF(pDst != NULL, "CopyPileItems - NULL destination");
    if (pPile == NULL)
        return(FALSE);

    SemEnter();
    pBlock = pPile->pBlockFirst;
    while (pBlock) {
        CopyBlock((PBYTE)pBlock + sizeof(PILEB), pDst,
                pBlock->cItems * pPile->cbSubItem);
        pDst += pBlock->cItems * pPile->cbSubItem;
        pBlock = pBlock->next;
    }
    SemLeave();

    return(TRUE);
}




/***************************** Private Function ****************************\
* Locate and return the pointer to the pile subitem who's key fields match
* pbSearch using npfnCmp to compare the fields.  If pbSearch == NULL, or
* npfnCmp == NULL, the first subitem is returned.
*
* afCmd may be:
* FPI_DELETE - delete the located item
* FPI_COUNT - count number of items that match
* In this case, the returned pointer is not valid.
*
* pppb points to where to store a pointer to the block which contained
* the located item.
*
* if pppb == NULL, it is ignored.
*
* NULL is returned if pbSearch was not found or if the list was empty.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
PBYTE FindPileItem(pPile, npfnCmp, pbSearch, afCmd)
PPILE pPile;
NPFNCMP npfnCmp;
PBYTE pbSearch;
USHORT afCmd;
{
    PBYTE pb;
    PPILEB ppbT;
    register int i;
    register int c;

    if (pPile == NULL)
        return(NULL);
    c = 0;
    SemEnter();
    ppbT = pPile->pBlockFirst;
    while (ppbT) {
        /*
         * for each block...
         */
        for (pb = (PBYTE)ppbT + sizeof(PILEB), i = 0;
                i < ppbT->cItems; pb += pPile->cbSubItem, i++) {
            /*
             * and each item within that block..
             */
            if (pbSearch == NULL || npfnCmp == NULL ||
                    (*npfnCmp)(pb, pbSearch)) {
                /*
                 * If it matches or we don't care...
                 */
                if (afCmd & FPI_DELETE) {
                    /*
                     * remove entire block if this was the last subitem in it.
                     */
                    if (--ppbT->cItems == 0) {
                        RemoveLstItem((PLST)pPile, (PLITEM)ppbT);
                    } else {
                        /*
                         * copy last subitem in the block over the removed item.
                         */
                        CopyBlock((PBYTE)ppbT + sizeof(PILEB) +
                                pPile->cbSubItem * ppbT->cItems,
                                pb, pPile->cbSubItem);
                    }
                }
                if (afCmd & FPI_COUNT) {
                    c++;
                } else {
                    SemLeave();
                    return(pb);
                }
                if (afCmd & FPI_DELETE) {
                    pb = (PBYTE)ppbT + sizeof(PILEB);
                    i = 0;
                }
            }
        }
        ppbT = (PPILEB)ppbT->next;
    }
    SemLeave();
    return((PBYTE)(ULONG)c);
}


/***************************** Private Function ****************************\
* Places a copy of the subitem pointed to by pb into the first available
* spot in the pile pPile.  If npfnCmp != NULL, the pile is first searched
* for a pb match.  If found, pb replaces the located data but FALSE is
* returned to show that no real addition took place.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
BOOL AddPileItem(pPile, pb, npfnCmp)
PPILE pPile;
PBYTE pb;
BOOL (*npfnCmp)(PBYTE pb, PBYTE pbSearch);
{
    PBYTE pbDst;
    PPILEB ppb;

    if (pPile == NULL)
        return(FALSE);
    SemEnter();
    if (npfnCmp != NULL &&
            (pbDst = FindPileItem(pPile, npfnCmp, pb, 0)) != NULL) {
        CopyBlock(pb, pbDst, pPile->cbSubItem);
        SemLeave();
        return(FALSE);
    }
    ppb = pPile->pBlockFirst;
    /*
     * locate a block with room
     */
    while ((ppb != NULL) && ppb->cItems == pPile->cSubItemsMax) {
        ppb = (PPILEB)ppb->next;
    }
    /*
     * If all full or no blocks, make a new one, link it on the bottom.
     */
    if (ppb == NULL) {
        if ((ppb = (PPILEB)NewLstItem((PLST)pPile, ILST_LAST)) == NULL) {
            SemLeave();
            return(FALSE);
        }
        ppb->cItems = 0;
    }
    /*
     * add the subitem
     */
    CopyBlock(pb, (PBYTE)ppb + sizeof(PILEB) + pPile->cbSubItem * ppb->cItems++,
        pPile->cbSubItem);
     
    SemLeave();
    return(TRUE);
}




/***************************** Private Function ****************************\
* Fills pb with a copy of the top item's data and removes it from the pile.
* returns FALSE if the pile was empty.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
BOOL PopPileSubitem(pPile, pb)
PPILE pPile;
PBYTE pb;
{
    PPILEB ppb;
    PBYTE pSrc;

    
    if ((pPile == NULL) || ((ppb = pPile->pBlockFirst) == NULL))
        return(FALSE);
        
    SemEnter();
    pSrc = (PBYTE)pPile->pBlockFirst + sizeof(PILEB);
    CopyBlock(pSrc, pb, pPile->cbSubItem);
    /*
     * remove entire block if this was the last subitem in it.
     */
    if (pPile->pBlockFirst->cItems == 1) {
        RemoveLstItem((PLST)pPile, (PLITEM)pPile->pBlockFirst);
    } else {
        /*
         * move last item in block to replace copied subitem and decrement
         * subitem count.
         */
        CopyBlock(pSrc + pPile->cbSubItem * --pPile->pBlockFirst->cItems,
                pSrc, pPile->cbSubItem);
    }
    SemLeave();
    return(TRUE);
}
    

/***************************** Semaphore Functions *************************\
* SemEnter() and SemLeave() are macros.
*
* History:      1/1/89  Created         sanfords
\***************************************************************************/
void SemInit()
{
    PBYTE pSem;
    SHORT c;

    pSem = (PBYTE)&FSRSemDmg;
    c = 0;
    while (c++ < sizeof(DOSFSRSEM)) {
        *pSem++ = 0;
    }
    FSRSemDmg.cb = sizeof(DOSFSRSEM);
}

#ifdef DEBUG
void SemCheckIn()
{                                                                
    PIDINFO pi;
    BOOL fin;

    DosGetPID(&pi);
    fin = (FSRSemDmg.cUsage > 0) &&
            (FSRSemDmg.pid == pi.pid) &&
            ((FSRSemDmg.tid == pi.tid) || (FSRSemDmg.tid == -1));
    /*
     * !!! NOTE: during exitlists processing, semaphore TIDs are set to -1
     */
    AssertF(fin, "SemCheckIn - Out of Semaphore");
    if (!fin)
        SemEnter();
}

void SemCheckOut()
{
    PIDINFO pi;
    BOOL fOut;

    DosGetPID(&pi);
    fOut = FSRSemDmg.cUsage == 0 || FSRSemDmg.pid != pi.pid ||
                FSRSemDmg.tid != pi.tid;
    AssertF(fOut, "SemCheckOut - In Semaphore");
    if (!fOut)
        while (FSRSemDmg.cUsage)
            SemLeave();
        
}
#endif


void SemEnter()
{
    DosFSRamSemRequest(&FSRSemDmg, SEM_INDEFINITE_WAIT);
}


void SemLeave()
{
    DosFSRamSemClear(&FSRSemDmg);
}



void EXPENTRY ExlstAbort(usTermCode)
USHORT usTermCode;
{
    PAPPINFO pai;
    usTermCode;
    
    SemEnter();     /* get any other processes out of the semaphore */
    if (pai = GetCurrentAppInfo(FALSE)) {
        pai->cInCallback = 0;  /* so Unregister call will work */
        DdeUninitialize();
    } else {
        SemLeave();
        DosExitList(EXLST_REMOVE, (PFNEXITLIST)ExlstAbort);
    }
    DosExitList(EXLST_EXIT, 0);
}

BOOL CopyHugeBlock(pSrc, pDst, cb)
PBYTE pSrc;
PBYTE pDst;
ULONG cb;
{
    ULONG cFirst;
    /*
     *  |____________|   |___________|   |____________|  |____________|
     *     ^src                                 ^
     *
     *  |____________|   |___________|   |____________|  |____________|
     *             ^dst                                   ^
     */
    cFirst = (ULONG)min((~(USHORT)pSrc), (~(USHORT)pDst)) + 1L;
    if (cb < cFirst) {
        CopyBlock(pSrc, pDst, (USHORT)cb);
        return(TRUE);
    }

    goto copyit;
        
    /*
     * Now at least one of the pointers is on a segment boundry.
     */
    while (cb) {
        cFirst = min(0x10000 - ((USHORT)pSrc | (USHORT)pDst), cb);
copyit:
        if (HIUSHORT(cFirst)) {
            /*
             * special case where pSrc and pDst both are on segment
             * bounds.  Copy half at a time.
             */
            /*
             *  |___________|   |____________|  |____________|
             *  ^src                               ^
             *
             *  |___________|   |____________|  |____________|
             *  ^dst                               ^
             */
            cFirst >>= 1;
            CopyBlock(pSrc, pDst, (USHORT)cFirst);
            pSrc += cFirst;
            pDst += cFirst;
            cb -= cFirst;
        }
        CopyBlock(pSrc, pDst, (USHORT)cFirst);
        pSrc = HugeOffset(pSrc, cFirst);
        pDst = HugeOffset(pDst, cFirst);
        cb -= cFirst;
    /*
     *  |____________|   |___________|   |____________|  |____________|
     *           ^src                           ^
     *
     *  |____________|   |___________|   |____________|  |____________|
     *                   ^dst                             ^
     */
    }
    return(TRUE);
}




/***************************************************************************\
* Kills windows but avoids invalid window rips in debugger.
\***************************************************************************/
BOOL DestroyWindow(hwnd)
HWND hwnd;
{
    if (WinIsWindow(DMGHAB, hwnd))
        return(WinDestroyWindow(hwnd));
    return(TRUE);
}


/***************************** Private Function ****************************\
* Returns hConv of the window passed in is one of the ddeml windows.
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
HCONV IsDdeWindow(hwnd)
HWND hwnd;
{
    PAPPINFO pai;

    pai = pAppInfoList;
    
    while (pai && WinIsChild(hwnd, pai->hwndDmg)) 
        pai = pai->next;
        
    if (pai)
        return((HCONV)hwnd);
    else
        return(0L);
}


/***************************** Private Function ****************************\
* This routine only frees a MYDDES segment if this process is not the owner.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
void FreeData(
PMYDDES pmyddes,
PAPPINFO pai)
{
    TID tid;
    if (!CheckSel(SELECTOROF(pmyddes)) ||
            (   pmyddes->offszItemName == sizeof(MYDDES) &&
                pmyddes->magic == MYDDESMAGIC &&
                pmyddes->fs & HDATA_APPOWNED &&
                pmyddes->pai == pai) )
        return;

    SemEnter();
    FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&pmyddes, FPI_DELETE);
    tid = pai->tid;
    do {
        if (FindPileItem(pai->pHDataPile, CmpULONG, (PBYTE)&pmyddes, FPI_COUNT)) {
            SemLeave();
            return;
        }
        pai = pai->nextThread;
    } while (pai && pai->tid != tid);
    SemLeave();
    DosFreeSeg(SELECTOROF(pmyddes));
}



#ifdef DEBUG
int APIENTRY DebugOutput(PCH);
void fAssert(f, pszComment, line, szfile)
BOOL f;
PSZ pszComment;
USHORT line;
PSZ szfile;
{
    char szT[90];
    PSZ psz, pszLast;

    if (!f) {
        szT[0] = '\000';
        psz = szT;
        pszLast = &szT[89];
        psz = lstrcat(psz, "\n\rAssertion failure: ", pszLast);
        psz = lstrcat(psz, szfile, pszLast);
        psz = lstrcat(psz, ":", pszLast);
        psz = dtoa(psz, line, FALSE);
        psz = lstrcat(psz, " ", pszLast);
        psz = lstrcat(psz, pszComment, pszLast);
        psz = lstrcat(psz, "\n\r", pszLast);
        DebugOutput(szT);
        DebugBreak(); 
    }
}
#endif



HDMGDATA PutData(pSrc, cb, cbOff, hszItem, usFmt, afCmd, pai)
PBYTE pSrc;
ULONG cb;
ULONG cbOff;
HSZ hszItem;
USHORT usFmt;
USHORT afCmd;
PAPPINFO pai;
{
    PMYDDES pmyddes;
    
    if ((pmyddes = (PMYDDES)AllocDDESel(0, usFmt, hszItem, cb + cbOff, pai))
            == NULL) {
        pai->LastError = DMGERR_MEMORY_ERROR;
        return(0L);
    }
    pmyddes->fs = afCmd;
    
    if (afCmd & HDATA_APPFREEABLE) {
        if (!AddPileItem(pai->pHDataPile, (PBYTE)&pmyddes, CmpULONG)) {
            DosFreeSeg(SELECTOROF(pmyddes));
            pai->LastError = DMGERR_MEMORY_ERROR;
            return(0L);
        } 
    }
    if (pSrc)
        CopyHugeBlock(pSrc, HugeOffset(DDES_PABDATA(pmyddes), cbOff), cb);
    return(pmyddes);
}


/*
 * This routine adds all HSZ/HAPP pairs it finds for the given pai matching
 * hszApp to hDataAdd.
 * poffAdd is the offset into the hDataAdd to start inserting HSZ/HAPP
 * pairs.  It then truncates the list with a 0 HSZ and returns the offset
 * to the terminator (ready to be called again to add more).
 *
 * returns 0L on error.
 */
ULONG
QueryAppNames(
PAPPINFO pai,
HDMGDATA hDataAdd,
HSZ hszApp,
ULONG offAdd)
{
    USHORT chsz;
    PHSZ phsz, phszPile;
    PPILEB pBlock;

    AssertF(sizeof(HSZ) == sizeof(HAPP), "Type size conflict");
        
    SemEnter();
    if (chsz = (USHORT)FindPileItem(pai->pAppNamePile,
            hszApp ? CmpULONG : NULL, (PBYTE)&hszApp, FPI_COUNT)) {
        /*
         * allocate for additions.
         */
        if (!DdeAddData(hDataAdd, NULL,
                (chsz + 1L) * (sizeof(HSZ) + sizeof(HDMGDATA)), offAdd)) {
            offAdd = 0L;
            GetCurrentAppInfo(FALSE)->LastError = DMGERR_MEMORY_ERROR;
            goto Exit;
        }
        
        phsz = DDES_PABDATA((PDDESTRUCT)hDataAdd) + offAdd;
        if (hszApp) {
            *phsz++ = hszApp;       /* only one per thread expected */
            *phsz++ = (HSZ)pai->hwndFrame;
        } else {
            pBlock = pai->pAppNamePile->pBlockFirst;
            while (pBlock) {
                phszPile = (PHSZ)(pBlock + 1);
                for (chsz = 0; chsz < pBlock->cItems; chsz++) {
                    *(phsz++) = *(phszPile++);
                    *(phsz++) = (HSZ)pai->hwndFrame;
                }
                pBlock = pBlock->next;
            }
        }
        *phsz = 0L;
        offAdd = phsz - DDES_PABDATA((PDDESTRUCT)hDataAdd);
    }
Exit:    
    SemLeave();
    return(offAdd);
}

