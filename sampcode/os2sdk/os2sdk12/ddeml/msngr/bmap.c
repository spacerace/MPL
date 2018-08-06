/****************************** MODULE Header ******************************\
* Module Name:  bmap.c - Messenger application - bitmap module
*
* Created: 8/1/89  sanfords
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/

#include "msngr.h"
#include "string.h"
#ifdef SLOOP
#define BITMAPINFOHEADER2 BITMAPINFOHEADER
#define PBITMAPINFOHEADER2 PBITMAPINFOHEADER
#define BITMAPINFO2 BITMAPINFO
#define PBITMAPINFO2 PBITMAPINFO
#endif

typedef struct _CDATA {
    RECTL rcl;
    HBITMAP hbm;
    BOOL fSelect;
    BOOL fSelecting;
    NPUSERLIST npUL;
} CDATA;
typedef CDATA *NPCDATA;


typedef struct _PKT {
    HBITMAP hbm;
    SHORT cx;
    SHORT cy;
    char szName[MAX_NAMESTR + 1];
} PKT;
typedef PKT *NPPKT;

extern HWND hwndMsngr;
extern SHORT cyText;
extern HAB hab;
extern HSZ hszAppName;
extern HSZ hszEmailName;
extern char szEmailName[];
extern HWND hwndLB;
extern ITEMLIST msgTopicItemList[];



/*
 * local procs
 */
MRESULT bmpInit(HWND hwnd, NPPKT ppktInit);
MRESULT sndBmapInit(HWND hwnd, NPUSERLIST pUserItem);
BOOL sndBmap(NPCDATA pcd);
HBITMAP SnapRegion(HPS hps, PRECTL prcl);
void DrawRgn(HPS hps, PRECTL prcl);
void SortRect(PRECTL prcl, PRECTL prclSorted);
HDC CreateDC(PSZ lpszDriver, HDC hdcCompat);

#define max(a,b)    (((a) > (b)) ? (a) : (b))

/*
 * file globals
 */
ATOM fmtBmapPkt;
HPOINTER hptrSelBmap = 0;
HPOINTER hptrBmap = 0;

void InitBmapModule()
{
    fmtBmapPkt = WinAddAtom(WinQuerySystemAtomTable(), SZBMAPDATA);
    hptrSelBmap = WinLoadPointer(HWND_DESKTOP, (HMODULE)NULL, IDR_SELBMAP);
    hptrBmap = WinLoadPointer(HWND_DESKTOP, NULL, IDD_GETBITMAP);
} 

void CloseBmapModule()
{
    WinDeleteAtom(WinQuerySystemAtomTable(), fmtBmapPkt);
    WinDestroyPointer(hptrSelBmap);
    WinDestroyPointer(hptrBmap);
}

HDMGDATA bmpXfer(pXferInfo)
PXFERINFO pXferInfo;
{
    PBYTE pbuf;
    PKT pkt;
    PBITMAPINFO pbmi;
    HDC hdc;
    HPS hpsMem;
    SIZEL size;

    if (pXferInfo->usFmt != fmtBmapPkt)
        return(DDE_NOTPROCESSED);

    if (pXferInfo->usType == XTYP_POKE) {
        /*
         * we have bitmap bits...stick them into pkt.hbm.
         */
        pbuf = DdeAccessData(pXferInfo->hDmgData);
        
        DdeCopyBlock(pbuf, (PBYTE)&pkt.szName[0], MAX_NAMESTR + 1L);
        pbmi = (PBITMAPINFO)(pbuf + MAX_NAMESTR + 1);

        pkt.cx = pbmi->cx;
        pkt.cy = pbmi->cy;
        size.cx = (LONG)pkt.cx;
        size.cy = (LONG)pkt.cy;
        hdc = CreateDC((PSZ)"MEMORY", (HDC)NULL);
        hpsMem = GpiCreatePS(hab, hdc, &size,
                PU_ARBITRARY | GPIT_NORMAL | GPIA_ASSOC );
        pkt.hbm = GpiCreateBitmap(hpsMem, (PBITMAPINFOHEADER2)pbmi, CBM_INIT,
                (PBYTE)&pbmi->argbColor[1 << pbmi->cBitCount],
                (PBITMAPINFO2)pbmi);
        GpiAssociate(hpsMem, NULL);
        GpiDestroyPS(hpsMem);
        DevCloseDC(hdc);
        DdeFreeData(pXferInfo->hDmgData);
        
        WinLoadDlg(HWND_DESKTOP, hwndMsngr, BmpDlgProc, 0L, IDD_GETBITMAP,
                (PVOID)&pkt);
        return(1);
    }
    return(0);
}


/*
 * This is the proc used for receiving a bitmap
 */
MRESULT EXPENTRY BmpDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    HBITMAP hbm;
    HPS hps;
    WRECT wrc;
    
    switch(msg) {
    case WM_INITDLG:
        return(bmpInit(hwnd, (NPPKT)(SHORT)mp2));
        break;

    case WM_DESTROY:
        if (hbm = ((NPPKT)WinQueryWindowUShort(hwnd, QWS_USER))->hbm)
            GpiDeleteBitmap(hbm);
        WinFreeMem(hheap, (NPBYTE)WinQueryWindowUShort(hwnd, QWS_USER),
                sizeof(PKT));
        break;

    case WM_WINDOWPOSCHANGED:
        /*
         * hide the OK button when minimized since it messes up the icon.
         */
        if ((LONG)mp2 & AWP_MINIMIZED)
            WinShowWindow(WinWindowFromID(hwnd, MBID_OK), FALSE);
        else if ((LONG)mp2 & AWP_RESTORED)
            WinShowWindow(WinWindowFromID(hwnd, MBID_OK), TRUE);
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;

    case WM_COMMAND:
        WinDestroyWindow(hwnd);
        break;
        
    case WM_PAINT:
        WinDefDlgProc(hwnd, msg, mp1, mp2);
        /*
         * draw the bitmap just above the OK button.
         */
        hps = WinGetPS(hwnd);
        WinQueryWindowRect(WinWindowFromID(hwnd, MBID_OK), (PRECTL)&wrc);
        WinMapWindowPoints(WinWindowFromID(hwnd, MBID_OK), hwnd, (PPOINTL)&wrc, 2);
        wrc.yBottom = wrc.yTop + cyText / 2;
        hbm = ((NPPKT)WinQueryWindowUShort(hwnd, QWS_USER))->hbm;
        WinDrawBitmap(hps, hbm, (PRECTL)NULL, (PPOINTL)&wrc, 0L, 0L, DBM_NORMAL);
        WinReleasePS(hps);
        break;

    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
    }
    return(0);
}


MRESULT bmpInit(hwnd, ppktInit)
HWND hwnd;
NPPKT ppktInit;
{
    char szTitle[MAX_TITLESTR];
    WRECT wrc;
    NPPKT ppkt;
    SHORT cxMin;

    if (!(ppkt = (NPPKT)WinAllocMem(hheap, sizeof(PKT))))
        return(1);    
    *ppkt = *ppktInit;
    WinSetWindowUShort(hwnd, QWL_USER, (USHORT)ppkt);
    /*
     * This is required because currently, automatic ICON resource loading
     * is not supported for dialogs.
     */
    WinSendMsg(hwnd, WM_SETICON, (MPARAM)hptrBmap, 0L);
    /*
     * Set up title.
     */
    WinQueryWindowText(hwnd, MAX_TITLESTR, (PSZ)szTitle);
    lstrcat(szTitle, szTitle, ppkt->szName);
    WinSetWindowText(hwnd, (PSZ)szTitle);
    /*
     * resize the dialog so the bitmap just fits.
     */
    WinQueryWindowRect(hwnd, (PRECTL)&wrc);
    cxMin = wrc.xRight;
    WinQueryWindowRect(WinWindowFromID(hwnd, MBID_OK), (PRECTL)&wrc);
    WinMapWindowPoints(WinWindowFromID(hwnd, MBID_OK), hwnd, (PPOINTL)&wrc, 2);
    WinSetWindowPos(hwnd, NULL, 0, 0, max(wrc.xLeft * 2 + ppkt->cx, cxMin),
            wrc.yTop + ppkt->cy + cyText +
            (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR), SWP_SIZE);
    return(0);
}



MRESULT EXPENTRY SendBitmapDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    NPCDATA pcd;
    POINTL pt;
    HPS hps;

    pcd = (NPCDATA)WinQueryWindowUShort(hwnd, QWS_USER);
    
    switch (msg) {
    case WM_INITDLG:
        return(sndBmapInit(hwnd, (NPUSERLIST)(SHORT)mp2));
        break;

    case WM_DESTROY:
        WinFreeMem(hheap, (NPBYTE)pcd, sizeof(CDATA));
        break;
        
    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {
        case IDC_SENDBITMAP:
            if (sndBmap(pcd))
                WinDismissDlg(hwnd, 0);
            break;
            
        case MBID_CANCEL:                    
            WinDismissDlg(hwnd, 0);
            break;
            
        case IDC_SELECT:
            pcd->fSelect = TRUE;
            WinSetCapture(HWND_DESKTOP, hwnd);
            break;
        }
        break;

    case WM_BUTTON1DOWN:
        if (pcd->fSelect) {
            if (pcd->hbm) {
                GpiDeleteBitmap(pcd->hbm);
                pcd->hbm = NULL;
            }
            WinSetRect(hab, &pcd->rcl, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1),
                    SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
            WinMapWindowPoints(hwnd, (HWND)HWND_DESKTOP, (PPOINTL)&pcd->rcl, 2);
            hps = WinGetScreenPS(HWND_DESKTOP);
            DrawRgn(hps, &pcd->rcl);
            WinReleasePS(hps);
            pcd->fSelecting = TRUE;
        }
        break;

    case WM_MOUSEMOVE:
        if (pcd->fSelect) {
            WinSetPointer(HWND_DESKTOP, hptrSelBmap);
        } else {
            WinSetPointer(HWND_DESKTOP,
                    WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
        }
        if (pcd->fSelecting) {
            hps = WinGetScreenPS(HWND_DESKTOP);
            DrawRgn(hps, &pcd->rcl);    /* erase old rect */
            pt.x = SHORT1FROMMP(mp1);
            pt.y = SHORT2FROMMP(mp1);
            WinMapWindowPoints(hwnd, HWND_DESKTOP, &pt, 1);
            pcd->rcl.xRight = pt.x;
            pcd->rcl.yTop = pt.y;
            DrawRgn(hps, &pcd->rcl);    /* draw new one */
            WinReleasePS(hps);
        }
        break;

    case WM_BUTTON1UP:
        if (pcd->fSelecting) {
            WinSetCapture(HWND_DESKTOP, (HWND)NULL);
            hps = WinGetScreenPS(HWND_DESKTOP);
            DrawRgn(hps, &pcd->rcl);
            pcd->hbm = SnapRegion(hps, &pcd->rcl);
            WinReleasePS(hps);
            pcd->fSelecting = FALSE;
            pcd->fSelect = FALSE;
            WinEnableWindow(WinWindowFromID(hwnd, IDC_SENDBITMAP),
                    !WinIsRectEmpty(hab, &pcd->rcl));
        }
        break;

        
    default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0);
}


/*
 * returns fFailed
 */
MPARAM sndBmapInit(hwnd, pUserItem)
HWND hwnd;
NPUSERLIST pUserItem;
{
    NPCDATA pcd;
    char szTitle[MAX_TITLESTR];
    char szName[MAX_NAMESTR];
    
    if (!(pcd = (NPCDATA)WinAllocMem(hheap, sizeof(CDATA))))
        return(1);
    WinSetRectEmpty(hab, &pcd->rcl);
    pcd->hbm = NULL;
    pcd->fSelect = FALSE;
    pcd->fSelecting = FALSE;
    pcd->npUL = pUserItem;
    if (pcd->npUL->hConvMsg == NULL) {
        NotifyUser(SZCANTCONNECT);
        return(1);
    }
    WinQueryWindowText(hwnd, MAX_TITLESTR, szTitle);
    DdeGetHszString(pcd->npUL->hsz, szName, (LONG)MAX_NAMESTR);
    lstrcat(szTitle, szTitle, szName);
    WinSetWindowText(hwnd, (PSZ)szTitle);
    WinSetWindowUShort(hwnd, QWS_USER, (USHORT)pcd);
    return(0);
}


BOOL sndBmap(pcd)
NPCDATA pcd;
{
    BITMAPINFOHEADER bih;
    SHORT cbBuffer, cbBitmapInfo;
    PBYTE pbBuffer;
    PBITMAPINFO pbmi;
    PSZ pszName;
    SEL sel;
    HPS hps;
    HDC hdc;
    SIZEL size;
    
    /*
     * Compute the size of the image-data buffer and the bitmap information
     * structure.
     */
    GpiQueryBitmapParameters(pcd->hbm, &bih);
    cbBuffer = (((bih.cBitCount * bih.cx) + 31) / 32) * 4;
    if (cbBuffer > 0xFFFF / bih.cy / bih.cPlanes) {
        NotifyUser(SZTOOBIG);
        return(FALSE);
    }
    cbBuffer *= bih.cy * bih.cPlanes;
    cbBitmapInfo = sizeof(BITMAPINFO) +
        (sizeof(RGB) * (1 << bih.cBitCount));
    
    /*
     * Allocate memory for the image data-buffer and the bitmap information
     * structure.
     */
    DosAllocSeg(cbBuffer + cbBitmapInfo + MAX_NAMESTR + 1, &sel, 0);
    pszName = (PSZ)MAKEP(sel, 0);
    lstrcpy(pszName, szEmailName);
    pbmi = (PBITMAPINFO)(pszName + MAX_NAMESTR + 1);
    pbBuffer = (PBYTE)&pbmi->argbColor[1 << bih.cBitCount];
    *(PBITMAPINFOHEADER)pbmi = bih;

    size.cx = (LONG)bih.cx;
    size.cy = (LONG)bih.cy;
    hdc = CreateDC((PSZ)"MEMORY", (HDC)NULL);
    hps = GpiCreatePS(hab, hdc, &size,
            PU_ARBITRARY | GPIT_NORMAL | GPIA_ASSOC );
    GpiSetBitmap(hps, pcd->hbm);
    GpiQueryBitmapBits(hps, 0L, (LONG)bih.cy, (PBYTE)pbBuffer,
            (PBITMAPINFO2)pbmi);
    GpiAssociate(hps, NULL);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    if (!DdeClientXfer(pszName,
            (LONG)(cbBuffer + cbBitmapInfo + MAX_NAMESTR + 1),
            pcd->npUL->hConvMsg,
            msgTopicItemList[IIL_BMPXFER].hszItem,
            fmtBmapPkt, XTYP_POKE, ulTimeout, 0L)) {
        MyPostError(DdeGetLastError());
    }

    DosFreeSeg(sel);
    GpiDeleteBitmap(pcd->hbm);
    pcd->hbm = NULL;
    WinSetRectEmpty(hab, &pcd->rcl);
    return(TRUE);
}


HBITMAP SnapRegion(hps, prcl)
HPS hps;
PRECTL prcl;
{
    HDC hdc;
    HBITMAP hbm, hbmOld;
    BITMAPINFOHEADER bih;
    POINTL rgpt[3];
    HPS hpsMem;
    SIZEL size;

    SortRect(prcl, prcl);
    WinInflateRect(hab, prcl, -1, -1);

    size.cx = (USHORT)(prcl->xRight - prcl->xLeft);
    size.cy = (USHORT)(prcl->yTop - prcl->yBottom);

    /* Create a memory DC */
    hdc = CreateDC((PSZ)"MEMORY", (HDC)NULL);

    /* create a memory PS */
    hpsMem = GpiCreatePS(hab, hdc, &size,
            PU_ARBITRARY | GPIT_NORMAL | GPIA_ASSOC );

    /* Create a bitmap */
    bih.cbFix = sizeof(BITMAPINFOHEADER);
    bih.cx = (SHORT)size.cx;
    bih.cy = (SHORT)size.cy;
    bih.cPlanes = 1;
    bih.cBitCount = 8;
    hbm = GpiCreateBitmap(hpsMem, (PBITMAPINFOHEADER2)&bih, 0L, 0, 0);
    if (hbm == GPI_ERROR) 
        return(0);
    
    /* put the bitmap into the memory PS */
    hbmOld = GpiSetBitmap(hpsMem, hbm);

    /* copy the window to the memory PS */
    rgpt[0].x = 0;
    rgpt[0].y = 0;
    rgpt[1].x = size.cx;
    rgpt[1].y = size.cy;
    rgpt[2].x = prcl->xLeft;
    rgpt[2].y = prcl->yBottom;
    GpiBitBlt(hpsMem, hps, 3L, (PPOINTL)&rgpt[0], ROP_SRCCOPY, 0L);

    /* free the bitmap */
    GpiSetBitmap(hpsMem, hbmOld);

    /* destroy the memory DC */
    GpiAssociate(hpsMem, NULL);
    GpiDestroyPS(hpsMem);
    DevCloseDC(hdc);
    return(hbm);
} /* end snapregion */


HDC CreateDC(lpszDriver, hdcCompat)
PSZ  lpszDriver;
HDC hdcCompat;
{
    struct {
        ULONG FAR *lpLogAddr;
        PSZ  lpszDriver;
    } opendc;

    opendc.lpLogAddr = NULL;
    opendc.lpszDriver = lpszDriver;

    return((HDC)DevOpenDC(hab, OD_MEMORY, (PSZ)"*", 2L,
            (PDEVOPENDATA)&opendc, hdcCompat));
}


void DrawRgn(hps, prcl)
HPS hps;
PRECTL prcl;
{
    RECTL rclSorted;
    
    SortRect(prcl, &rclSorted);
    WinDrawBorder(hps, &rclSorted, 1, 1, SYSCLR_WINDOW, SYSCLR_WINDOW,
            DB_DESTINVERT | DB_STANDARD);
}

void SortRect(prcl, prclSorted)
PRECTL prcl;
PRECTL prclSorted;
{
    LONG l;

    WinCopyRect(hab, prclSorted, prcl);
    if (prclSorted->yTop < prclSorted->yBottom) {
        l = prclSorted->yBottom;
        prclSorted->yBottom = prclSorted->yTop;
        prclSorted->yTop = l;
    }
    
    if (prclSorted->xRight < prclSorted-> xLeft) {
        l = prclSorted->xRight;
        prclSorted->xRight = prclSorted->xLeft;
        prclSorted->xLeft = l;
    }
}




