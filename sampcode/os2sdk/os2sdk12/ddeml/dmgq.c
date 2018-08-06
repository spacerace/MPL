/****************************** Module Header ******************************\
* Module Name: DMGQ.C
*
* DDE Manager queue control functions.
*
* Created: 9/1/89 Sanford Staab
*
* This is a general queue manager - yes another one!
* Queues are each allocated within their own segment and have a
* QST structure associated with that heap.  Each queue item
* is allocated within the heap segment.  The offset of the items
* address combined with an instance count is used as the item ID.
* This is both unique and allows for instant location of an item.
* New items are added to the head of the queue which is a doubly linked
* list.  The next links point to more recent entries, the prev pointers
* to older entries.  The next of the head is the tail.  The prev of the
* tail is the head.  All pointers are far.
* Queue Data may be of any structure type that begins identical to
* a QUEUEITEM structure.  Functions that require an cbItem perameter
* should be given the size of the specialized queue item structure.
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#include "ddemlp.h"


/***************************** Private Function ****************************\
*
* Creates a Queue for items of cbItem.
* Returns NULL on error.
*
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
PQST CreateQ(cbItem)
USHORT cbItem;
{
    QST cq;
    PQST pQ;
    
    cq.cItems = 0;
    cq.instLast = 0;
    cq.cbItem = cbItem;
    cq.pqiHead = NULL;
    if (!(cq.hheap = MyCreateHeap(0, sizeof(QST) + cbItem << 3,
            cbItem << 3, cbItem, cbItem, HEAPFLAGS)))
        return(NULL);
    if (!(pQ = (PQST)FarAllocMem(cq.hheap, sizeof(QST)))) {
        MyDestroyHeap(cq.hheap);
        return(0);
    }
    *pQ = cq;
    return(pQ);
}



/***************************** Private Function ****************************\
*
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
BOOL DestroyQ(pQ)
PQST pQ;
{
    if (pQ)
        MyDestroyHeap(pQ->hheap);
    return(TRUE);
}



/***************************** Private Function ****************************\
*
* returns a long pointer to the queue item data created.  The new item
* is added to the head of the queue.  The queue's cbItem specified at
* creation is used for allocation.
*
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
PQUEUEITEM Addqi(pQ)
PQST pQ;
{
    PQUEUEITEM pqi;

    if ((pqi = (PQUEUEITEM)FarAllocMem(pQ->hheap, pQ->cbItem)) == NULL) {
        if (pQ->cItems == 0)
            return(0);
        /*
         * remove the oldest item to make room for the new.
         */
        pqi = pQ->pqiHead->next;
        SemEnter();
        pqi->prev->next = pqi->next;
        pqi->next->prev = pqi->prev;
        SemLeave();
    }

    SemEnter();
    if (pQ->cItems == 0) {
        pQ->pqiHead = pqi->prev = pqi->next = pqi;
    } else {
        pqi->prev = pQ->pqiHead;
        pqi->next = pQ->pqiHead->next;
        pQ->pqiHead->next->prev = pqi;
        pQ->pqiHead->next = pqi;
        pQ->pqiHead = pqi;
    }
    SemLeave();
    pQ->cItems++;
    pqi->inst = ++pQ->instLast;
    return(pqi);
}




/***************************** Private Function ****************************\
*
*  The id given is an external LONG id, not an item instance number.
*  If id is QID_NEWEST, the head item is deleted.
*  If id is QID_OLDEST, the tail item is deleted.
*
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
void Deleteqi(pQ, id)
PQST pQ;
ULONG id;
{
    PQUEUEITEM pqi;

    SemEnter();
    pqi = Findqi(pQ, id);
    if (pqi == NULL) {
        SemLeave();
        return;
    }
    pqi->prev->next = pqi->next;
    pqi->next->prev = pqi->prev;
    if (pqi == pQ->pqiHead)
        pQ->pqiHead = pqi->prev;
    if (!(--pQ->cItems))
        pQ->pqiHead = NULL;
    FarFreeMem(pQ->hheap, pqi, pQ->cbItem);
    SemLeave();
}






/***************************** Private Function ****************************\
*
*  The id given is an external LONG id, not an item instance number.
*
*  if id == QID_NEWEST, returns the head queue data item.
*  if id == QID_OLDEST, returns the tail queue data item.
*  if the id is not found or the queue is empty, NULL is returned.
*  if found, pqi is returned.
*
*
* History:
*   Created     9/1/89    Sanfords
\***************************************************************************/
PQUEUEITEM Findqi(pQ, id)
PQST pQ;
ULONG id;
{
    PQUEUEITEM pqi;

    SemCheckIn();
    if (pQ == NULL || pQ->pqiHead == NULL)
        return(NULL);
        
    if (id == QID_OLDEST) 
        return(pQ->pqiHead->next);
        
    if (id == QID_NEWEST) 
        return(pQ->pqiHead);
        
    if (id) {
        pqi = PFROMID(pQ, id);
        if (pqi->inst == HIUSHORT(id)) {
            return(pqi);
        }
        return(NULL);
    }
}
