/****************************** Module Header ******************************\
* Module Name:  util.c
*
* Utility functions used by msngr app.
*
* Created:  1/1/89  sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "msngr.h"

extern NPUSERLIST gnpUL;
extern HAB hab;

/***************************** Public  Function ****************************\
* Concatonates psz1 and psz2 into pszDest.
*
* History:  1/1/89  created sanfords
\***************************************************************************/
void lstrcat(pszDest, psz1, psz2)
PSZ pszDest, psz1, psz2;
{
    while (*psz1 != '\0') {
        *pszDest++ = *psz1++;
    }
    while (*psz2 != '\0') {
        *pszDest++ = *psz2++;
    }
    *pszDest = '\0';
}


void lstrcpy(pszDst, pszSrc)
PSZ pszDst, pszSrc;
{
    lstrcat(pszDst, pszSrc, "");
}


/***************************** Public  Function ****************************\
* Concatonates psz1 and psz2 into pszDest but leaves a '\0' in between.
*
* History:  1/1/89  created sanfords
\***************************************************************************/
void lstrpak(pszDest, psz1, psz2)
PSZ pszDest, psz1, psz2;
{
    while (*psz1 != '\0') {
        *pszDest++ = *psz1++;
    }
    *pszDest++ = '\0';
    while (*psz2 != '\0') {
        *pszDest++ = *psz2++;
    }
    *pszDest = '\0';
}
/***************************** Private Function ****************************\
*
* returns string length not counting null terminator.
*
* History:  1/1/89  created     sanfords
\***************************************************************************/
int lstrlen(psz)
PSZ psz;
{
    int c = 0;

    while (*psz != 0) {
        psz++;
        c++;
    }
    return(c);
}



/***************************** Private Function ****************************\
* DESCRIPTION:
*   These functions handle hConv-hsz relationship control.
*
* History:      8/23/89 Created         sanfords
\***************************************************************************/
void DestroyUser(npUL)
NPUSERLIST npUL;
{
    PNPUSERLIST ppList;
    
    ppList = &gnpUL;
    while (*ppList && *ppList != npUL) {
        ppList = &((*ppList)->next);
    }
    if (!ppList) {
        return;
    }
    DdeFreeHsz((*ppList)->hsz);
    if (WinIsWindow(hab, (*ppList)->hwndLink))
        WinDestroyWindow((*ppList)->hwndLink);
    if ((*ppList)->hConvMsg) {
        DdeDisconnect((*ppList)->hConvMsg);
    }
    freeUserList(ppList);
}


/***************************** Private Function ****************************\
* DESCRIPTION:
*   These functions handle hConv-hsz relationship control.
*
* History:      1/13/89 Created         sanfords
\***************************************************************************/
VOID AddUser(
HCONV hConvMsg,
HSZ hsz,
HAPP hApp)
{
    NPUSERLIST pList;
    UCHAR sz[MAX_NAMESTR + 1];
    SHORT lit;
    extern HWND hwndLB;

    if ((pList = (NPUSERLIST)WinAllocMem(hheap, sizeof(USERLIST))) == 0)
        return;
    pList->hConvMsg = hConvMsg;
    pList->hConvLink = 0;
    pList->hwndLink = NULL;
    pList->hsz = hsz;
    pList->hApp = hApp;
    pList->next = gnpUL;  /* link in front */
    gnpUL = pList;
    DdeGetHszString(hsz, sz, MAX_NAMESTR + 1L);
    lit = (SHORT)WinSendMsg(hwndLB, LM_INSERTITEM,
            MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(sz));
    WinSendMsg(hwndLB, LM_SETITEMHANDLE, (MPARAM)lit, (MPARAM)pList);
}





NPUSERLIST FindUser(pList, hsz)
NPUSERLIST pList;
HSZ hsz;
{
    while (pList) {
        if (hsz == pList->hsz)
            return(pList);
        pList = pList->next;
    }
    return(0);

}




void freeUserList(ppList)
PNPUSERLIST ppList;
{
    NPUSERLIST pListT;

    while (*ppList) {
        pListT = *ppList;
        *ppList = pListT->next;
        WinFreeMem(hheap, (NPBYTE)pListT, sizeof(USERLIST));
    }
}




PVOID FarAllocMem(hheap, cb)
HHEAP hheap;
USHORT cb;
{
    return((PVOID)((PBYTE)WinLockHeap(hheap) + (USHORT)WinAllocMem(hheap, cb)));
}



void MyPostError(err)
USHORT err;
{
    switch (err) {
    case DMGERR_SERVER_DIED:
    case DMGERR_NO_CONV_ESTABLISHED:
        break;
    default:
        DdePostError(err);
        break;
    }
}




