/*
    snap.c -- PM snapshot utility
    Created by Microsoft Corporation, 1989
*/
#define		INCL_WIN
#define		INCL_GPI
#define		INCL_DEV
#include	<os2.h>
#include	"snap.h"

/* Global variables */
char szSnap[5];

HAB    habSnap;
HMQ    hmqSnap;
HWND   hwndSnap, hwndSnapFrame;
HWND   hwndNextClipViewer;
HPS    hpsScr;
HHEAP  hHeap;

HPOINTER hptrSnap;
HPOINTER hptrHand;
HPOINTER hptrSelect;

RECTL wrcRgn;                   /* holds coordinates of selected region. */
RECTL rcScreen;                 /* rectangle for the screen for bounding */

int AboutCount = 0;

int  wSnapMode = IDM_WINDOW;    /* snap either selected window or region. */

BOOL fSnapWnd = FALSE;  /* snap selected window? */
BOOL fSnapRgn = FALSE;  /* snap selected region? */
BOOL fSelect = FALSE;   /* in the process of selecting region? */
BOOL fNCArea = FALSE;   /* exclude nonclient area of window in snap? */
BOOL fHide = TRUE;      /* hide snap's window while snapping */


/* Function prototypes */
MRESULT EXPENTRY SnapWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AboutWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY SaveFileDlgProc(HWND, USHORT, MPARAM, MPARAM);
HFILE OpenSaveFile(HWND);
void SnapWindow(ULONG);
BOOL SnapInit(void);
void SnapPaint(HPS);
void SnapRegion(HPS);
void DrawRgn(HPS);
void SortRect(PRECTL, PRECTL);
void SaveBitmap(void);
void SaveBitmap2(HFILE hFile, HBITMAP hbm);
void convert(NPBYTE pbuf, USHORT cch, USHORT cch2);
HDC CreateDC(PSZ, HDC);
void Copy(NPBYTE, NPBYTE, USHORT);
int cdecl main(void);


/* Routines */
int cdecl main(void)
{
    QMSG msg;
    ULONG ctlData;

    if (!SnapInit())
        return(FALSE);

    ctlData = FCF_TITLEBAR | FCF_MINMAX | FCF_SIZEBORDER |
            FCF_SYSMENU | FCF_MENU | FCF_ICON;
    hwndSnapFrame = WinCreateStdWindow(HWND_DESKTOP,
            FS_ICON, &ctlData,
            (PCH)szSnap, (PCH)szSnap,
            0L,
            (HMODULE) NULL, 1,
            (HWND far *)&hwndSnap);


    if (!hwndSnap)
        return(FALSE);

    WinSetWindowPos(hwndSnapFrame, 0, 0, 0, 200, 75, SWP_SIZE | SWP_MOVE);
    WinShowWindow(hwndSnapFrame, TRUE);

    WinSetFocus(HWND_DESKTOP, hwndSnap);
    WinQueryWindowRect(HWND_DESKTOP, &rcScreen);

    if (WinOpenClipbrd(habSnap)) {
        WinSetClipbrdViewer(habSnap, hwndSnap);
        WinCloseClipbrd(habSnap);
    }

    while (WinGetMsg(habSnap, (PQMSG)&msg, NULL, 0, 0)) {
        WinDispatchMsg(habSnap, (PQMSG)&msg);
    }

    WinDestroyPointer(hptrSnap);
    WinDestroyPointer(hptrHand);
    WinDestroyPointer(hptrSelect);

    WinDestroyWindow(hwndSnapFrame);
    WinDestroyHeap(hHeap);
    WinDestroyMsgQueue(hmqSnap);

    WinTerminate(habSnap);

    return 0;
} /* end winmain */


HDC CreateDC(lpszDriver, hdcCompat)
PSZ lpszDriver;
HDC hdcCompat;
{
    struct {
        ULONG FAR *lpLogAddr;
        PSZ lpszDriver;
    } opendc;

    opendc.lpLogAddr = NULL;
    opendc.lpszDriver = lpszDriver;

    return((HDC)DevOpenDC(habSnap, OD_MEMORY, (PSZ)"*", 2L,
            (PDEVOPENDATA)&opendc, hdcCompat));
}

BOOL SnapInit(void)
{
    /*
     * Initialize the HAB
     */
    habSnap = WinInitialize(0);

    hmqSnap = WinCreateMsgQueue(NULL, 0);

    WinLoadString(habSnap, (HMODULE) NULL, IDS_SNAP, sizeof(szSnap), (PCH)szSnap);

    hptrSnap = WinLoadPointer(HWND_DESKTOP, (HMODULE)NULL, IDR_PTR_SNAP);
    hptrHand = WinLoadPointer(HWND_DESKTOP, (HMODULE)NULL, IDR_PTR_HAND);
    hptrSelect = WinLoadPointer(HWND_DESKTOP, (HMODULE)NULL, IDR_PTR_SELECT);

    if (!WinRegisterClass(habSnap, (PCH)szSnap, SnapWndProc, (ULONG)0, 0))
        return(FALSE);

    if ((hHeap = WinCreateHeap(0, 0, 0, 0, 0, 0)) == NULL)
        return(FALSE);
    /*
     * We assume an empty clipboard when we start.
     */
    WinOpenClipbrd(habSnap);
    WinEmptyClipbrd(habSnap);
    WinCloseClipbrd(habSnap);
} /* end snapinit */


MRESULT EXPENTRY SnapWndProc(hwnd, message, mp1, mp2)
HWND    hwnd;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
    HWND hwndMenu;
    HPS hps;
    RECTL wrcUpdate;
    POINTL wptTemp;

    switch (message) {
        case WM_BUTTON1DOWN:
            if (fSnapWnd) {
                WinSetPointer(HWND_DESKTOP, hptrSnap);
                SnapWindow(LONGFROMMP(mp1));
                WinSetCapture(HWND_DESKTOP, (HWND)NULL);
                fSnapWnd = FALSE;
            } else if (fSnapRgn) {
                wptTemp.x = SHORT1FROMMP(mp1);
                wptTemp.y = SHORT2FROMMP(mp1);
                WinMapWindowPoints(hwndSnap, (HWND)HWND_DESKTOP,
                        (PPOINTL)&wptTemp, 1);
                wrcRgn.yTop = wrcRgn.yBottom = wptTemp.y;
                wrcRgn.xRight = wrcRgn.xLeft = wptTemp.x;
                hpsScr = WinGetScreenPS(HWND_DESKTOP);
                DrawRgn(hpsScr);
                WinReleasePS(hpsScr);
                fSelect = TRUE;
                break;
            } else {
                return(WinDefWindowProc(hwnd, message, mp1, mp2));
            }
            break;

        case WM_MOUSEMOVE:
            if (fSelect) {
                hpsScr = WinGetScreenPS(HWND_DESKTOP);
                DrawRgn(hpsScr);
                wptTemp.x = LOUSHORT(mp1);
                wptTemp.y = HIUSHORT(mp1);
                WinMapWindowPoints(hwndSnap, (HWND)HWND_DESKTOP,
                        (PPOINTL)&wptTemp, 1);
                wrcRgn.yTop = wptTemp.y;
                wrcRgn.xRight = wptTemp.x;
                DrawRgn(hpsScr);
                WinReleasePS(hpsScr);
                break;
            }
            if (fSnapWnd || fSnapRgn) {
                break;
            } else {
                return(WinDefWindowProc(hwnd, message, mp1, mp2));
            }
            break;

        case WM_BUTTON1UP:
            if (fSelect) {
                WinSetCapture(HWND_DESKTOP, (HWND)NULL);
                hpsScr = WinGetScreenPS(HWND_DESKTOP);
                DrawRgn(hpsScr);
                SnapRegion(hpsScr);
                WinReleasePS(hpsScr);
                fSnapRgn = FALSE;
                fSelect = FALSE;
            }
            break;

        case WM_DRAWCLIPBOARD:
            WinInvalidateRect(hwnd, (PRECTL)NULL, TRUE);
            break;

        case WM_PAINT:
            hps = WinBeginPaint(hwnd, NULL, &wrcUpdate);
            WinFillRect(hps, &wrcUpdate, SYSCLR_WINDOW);
            SnapPaint(hps);
            WinEndPaint(hps);
            break;

        case WM_INITMENU:
            hwndMenu = WinWindowFromID(hwndSnapFrame, FID_MENU);
            WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_REGION, TRUE),
                MPFROM2SHORT(MIA_CHECKED, wSnapMode == IDM_REGION ? MIA_CHECKED : 0));
            WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_WINDOW, TRUE),
                MPFROM2SHORT(MIA_CHECKED, wSnapMode == IDM_WINDOW ? MIA_CHECKED : 0));
            WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_NCAREA, TRUE),
                MPFROM2SHORT(MIA_CHECKED, fNCArea ? MIA_CHECKED : 0));
            WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_HIDE, TRUE),
                MPFROM2SHORT(MIA_CHECKED, fHide ? MIA_CHECKED : 0));
            WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDM_NCAREA, TRUE),
                MPFROM2SHORT(MIA_DISABLED, wSnapMode == IDM_WINDOW ? 0 : MIA_DISABLED));
            break;

        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1)) {
                case IDM_SAVE:
                    SaveBitmap();
                    break;

                case IDM_SNAP:
                    WinSetCapture(HWND_DESKTOP, hwnd);
                    if (fHide)
                        WinShowWindow(hwndSnapFrame, FALSE);
                    if (wSnapMode == IDM_WINDOW) {
                        fSnapWnd = TRUE;
                        WinSetPointer(HWND_DESKTOP, hptrHand);
                    } else {
                        fSnapRgn = TRUE;
                        WinSetPointer(HWND_DESKTOP, hptrSelect);
                    }
                    break;

                case IDM_WINDOW:
                case IDM_REGION:
                    wSnapMode = SHORT1FROMMP(mp1);
                    break;

                case IDM_NCAREA:
                    fNCArea = !fNCArea;
                    break;

                case IDM_HIDE:
                    fHide = !fHide;
                    break;

                case IDM_ABOUT:
                    WinDlgBox(HWND_DESKTOP, hwnd,
			(PFNWP)AboutWndProc, (HMODULE) NULL, IDD_INFO, (PCH)NULL);
                    break;
            }
            break;

        default:
            return(WinDefWindowProc(hwnd, message, mp1, mp2));
    }

    return(0L);

} /* end snapwndproc */


void SnapPaint(hps)
HPS hps;
{
    HBITMAP hbm;
    POINTL   pt;
    BITMAPINFOHEADER bminfo;
    RECTL rc;

    WinOpenClipbrd(habSnap);
    WinQueryWindowRect(hwndSnap, &rc);

    if (hbm = (HBITMAP) WinQueryClipbrdData(habSnap, CF_BITMAP)) {
        GpiQueryBitmapParameters(hbm, &bminfo);
        pt.x = 0;
        pt.y = rc.yTop - bminfo.cy;
        WinDrawBitmap(hps, hbm, (PRECTL)NULL, (PPOINTL)&pt,
                0L, 0L, DBM_NORMAL | DBM_IMAGEATTRS);
    }

    WinCloseClipbrd(habSnap);

} /* end SnapPaint */


void SnapWindow(loc)
ULONG loc;
{
    BITMAPINFOHEADER bminfo;
    POINTL  pt;
    HWND hwnd;
    HWND hwndT;
    RECTL rc, rcTmp;
    HPS  hpsWnd, hpsMem;
    HDC  hdc;
    HBITMAP hbm, hbmOld;
    int  cx, cy;
    POINTL rgpt[3];
    SIZEL size;

    pt.y = HIUSHORT(loc);
    pt.x = LOUSHORT(loc);

    WinMapWindowPoints(hwndSnap, HWND_DESKTOP, (PPOINTL)&pt, 1);
    if ((hwnd = WinWindowFromPoint(HWND_DESKTOP, (PPOINTL)&pt, FALSE, FALSE)) == NULL)
        return;
    /* get size of target window. clip to screen. */
    if (fNCArea)  /* snap only the client area if it exists */
        if ((hwndT = WinWindowFromID(hwnd, FID_CLIENT)) != NULL)
            hwnd = hwndT;
    WinQueryWindowRect(hwnd, &rcTmp);
    /* bound window rectangle to screen. */
    WinMapWindowPoints(hwnd, HWND_DESKTOP, (PPOINTL)&rcTmp.xLeft, 2);
    WinIntersectRect(habSnap, &rc, &rcTmp, &rcScreen);
    WinMapWindowPoints(HWND_DESKTOP, hwnd, (PPOINTL)&rc.xLeft, 2);
    cx = (USHORT)(rc.xRight - rc.xLeft);
    cy = (USHORT)(rc.yTop - rc.yBottom);

    /* get window PS */
    hpsWnd = WinGetPS(hwnd);

    /* Create a memory DC */
    hdc = CreateDC((PSZ)"MEMORY", (HDC)NULL);

    /* create a memory PS */
    size.cx = cx;
    size.cy = cy;
    hpsMem = GpiCreatePS( habSnap, hdc, &size,
            PU_ARBITRARY | GPIT_NORMAL | GPIA_ASSOC );

    /* Create a bitmap */
    bminfo.cbFix = 12;
    bminfo.cx = cx;
    bminfo.cy = cy;
    bminfo.cPlanes = 1;
    bminfo.cBitCount = 4;
    if (!(hbm = GpiCreateBitmap(hpsMem, (PBITMAPINFOHEADER)&bminfo, 0L, 0, 0)))
        WinMessageBox((HWND)HWND_DESKTOP, hwndSnap,
            (PCH)"Insufficient memory to create the bitmap.", (PCH)NULL,
            0, MB_OK);
    else {
        /* put the bitmap into the memory PS */
        hbmOld = GpiSetBitmap(hpsMem, hbm);

        /* copy the window to the memory PS */
        rgpt[0].x = 0;
        rgpt[0].y = 0;
        rgpt[1].x = cx;
        rgpt[1].y = cy;
        rgpt[2].x = rc.xLeft;
        rgpt[2].y = rc.yBottom;
        GpiBitBlt(hpsMem, hpsWnd, 3L, (PPOINTL)&rgpt[0], ROP_SRCCOPY, 0L);

        /* free the bitmap */
        GpiSetBitmap(hpsMem, hbmOld);

        /* store the bitmap */
        WinOpenClipbrd(habSnap);
        WinEmptyClipbrd(habSnap);
        WinSetClipbrdData(habSnap, (ULONG)hbm, CF_BITMAP, CFI_HANDLE);
        WinCloseClipbrd(habSnap);
    }

    /* destroy the memory DC */
    GpiAssociate( hpsMem, NULL );
    DevCloseDC(hdc);

    /* get rid of the PSs */
    GpiDestroyPS(hpsMem);
    WinReleasePS(hpsWnd);

    if (fHide)
        WinShowWindow(hwndSnapFrame, TRUE);

} /* end snapwindow */


void SnapRegion(hpsScr)
HPS hpsScr;
{
    HDC hdc;
    HBITMAP hbm, hbmOld;
    BITMAPINFOHEADER bminfo;
    RECTL rcTmp;
    int cx, cy;
    POINTL rgpt[3];
    HPS hpsMem;
    SIZEL size;

    SortRect((PRECTL)&wrcRgn, (PRECTL)&rcTmp);

    cx = (USHORT)(rcTmp.xRight - rcTmp.xLeft);
    cy = (USHORT)(rcTmp.yTop - rcTmp.yBottom);

    /* Create a memory DC */
    hdc = CreateDC((PSZ)"MEMORY", (HDC)NULL);

    /* create a memory PS */
    size.cx = cx;
    size.cy = cy;
    hpsMem = GpiCreatePS( habSnap, hdc, &size,
            PU_ARBITRARY | GPIT_NORMAL | GPIA_ASSOC );

    /* Create a bitmap */
    bminfo.cbFix = 12;
    bminfo.cx = cx;
    bminfo.cy = cy;
    bminfo.cPlanes = 1;
    bminfo.cBitCount = 4;
    if (!(hbm = GpiCreateBitmap(hpsMem, (PBITMAPINFOHEADER)&bminfo, 0L, 0, 0))) {
        WinMessageBox((HWND)HWND_DESKTOP, hwndSnap,
            (PCH)"Insufficient memory to create the bitmap.", (PCH)NULL,
            0, MB_OK);
    } else {
        /* put the bitmap into the memory PS */
        hbmOld = GpiSetBitmap(hpsMem, hbm);

        /* copy the window to the memory PS */
        rgpt[0].x = 0;
        rgpt[0].y = 0;
        rgpt[1].x = cx;
        rgpt[1].y = cy;
        rgpt[2].x = rcTmp.xLeft;
        rgpt[2].y = rcTmp.yBottom;
        GpiBitBlt(hpsMem, hpsScr, 3L, (PPOINTL)&rgpt[0], ROP_SRCCOPY, 0L);

        /* free the bitmap */
        GpiSetBitmap(hpsMem, hbmOld);

        /* store the bitmap */
        WinOpenClipbrd(habSnap);
        WinEmptyClipbrd(habSnap);
        WinSetClipbrdData(habSnap, (ULONG)hbm, CF_BITMAP, CFI_HANDLE);
        WinCloseClipbrd(habSnap);
    }
    /* destroy the memory DC */
    GpiAssociate( hpsMem, NULL );
    DevCloseDC(hdc);

    /* get rid of the PS */
    GpiDestroyPS(hpsMem);

    if (fHide)
        WinShowWindow(hwndSnapFrame, TRUE);

} /* end snapregion */

void DrawRgn(hps)
HPS hps;
{
    RECTL rc;

    SortRect((PRECTL)&wrcRgn, (PRECTL)&rc);

    WinDrawBorder(hps, (PRECTL)&rc, 1, 1, SYSCLR_WINDOW, SYSCLR_WINDOW,
            DB_DESTINVERT | DB_STANDARD);
}

void SortRect(pwrcIn, pwrcSorted)
PRECTL pwrcIn, pwrcSorted;
{
    if (pwrcIn->yTop > pwrcIn->yBottom) {
        pwrcSorted->yTop = pwrcIn->yTop;
        pwrcSorted->yBottom = pwrcIn->yBottom;
    } else {
        pwrcSorted->yTop = pwrcIn->yBottom;
        pwrcSorted->yBottom = pwrcIn->yTop;
    }
    if (pwrcIn->xRight > pwrcIn-> xLeft) {
        pwrcSorted->xRight = pwrcIn->xRight;
        pwrcSorted->xLeft = pwrcIn->xLeft;
    } else {
        pwrcSorted->xRight = pwrcIn->xLeft;
        pwrcSorted->xLeft = pwrcIn->xRight;
    }
}

MRESULT EXPENTRY AboutWndProc(hwnd, message, mp1, mp2)
HWND    hwnd;
USHORT  message;
MPARAM   mp1;
MPARAM   mp2;
{
    switch (message) {
        case WM_COMMAND:
            WinDismissDlg(hwnd, TRUE);
            break;
        default:
            return(WinDefDlgProc(hwnd, message, mp1, mp2));
            break;
    }
    return 0L;

} /* end aboutwndproc */

void SaveBitmap(void)
{
    HFILE hFile;
    HBITMAP hbm;
    HPOINTER hptr, hptrWait;

    WinOpenClipbrd(habSnap);
    if ((hbm = (HBITMAP) WinQueryClipbrdData(habSnap, CF_BITMAP)) == NULL) {
        WinCloseClipbrd(habSnap);
        return;
    }

    if (hFile = OpenSaveFile(hwndSnap)) {
        hptr = WinQueryPointer(HWND_DESKTOP);
        hptrWait = WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, TRUE);
        WinSetPointer(HWND_DESKTOP, hptrWait);
        SaveBitmap2(hFile, hbm);
        WinSetPointer(HWND_DESKTOP, hptr);
        WinDestroyPointer(hptrWait);
    } else
        WinAlarm(HWND_DESKTOP, WA_ERROR);

    WinCloseClipbrd(habSnap);
}

/***************************************************************************\
* hFile is a handle to an open file.  This is closed on exit.
\***************************************************************************/
void SaveBitmap2(hFile, hbm)
HFILE hFile;
HBITMAP hbm;
{
    /*
     * Currently, this puts stuff out in Win386 paint format.
     */
    typedef struct _WIN386PAINT {
        USHORT key1;
        USHORT key2;
        USHORT dxFile;
        USHORT dyFile;
        USHORT ScrAspectX;
        USHORT ScrAspectY;
        USHORT PrnAspectX;
        USHORT PrnAspectY;
        USHORT dxPrinter;
        USHORT dyPrinter;
        USHORT AspCorX;
        USHORT AspCorY;
        USHORT wCheck;
        USHORT res1;
        USHORT res2;
        USHORT res3;
    } FHDR;
    FHDR hdr;
    USHORT i;
    USHORT cBytesWritten;
    USHORT *pIndex;
    USHORT *pIndexT;
    NPBYTE pScanLine, pBits, pBitsT;
    USHORT *phdr;
    USHORT cbIndexTable, cbScanLine, cbScanLineExp, cbBmpLine;
    HDC hdc;
    HBITMAP hbmOld;
    BITMAPINFOHEADER bminfo;
    HPS hpsMem;
    SIZEL size;

    /*
     * write header
     */
    GpiQueryBitmapParameters(hbm, &bminfo);
    hdr.key1 = 0x694C;
    hdr.key2 = 0x536E;
    hdr.dxFile = bminfo.cx;
    hdr.dyFile = bminfo.cy;
    hdr.ScrAspectX = 26;
    hdr.ScrAspectY = 30;
    hdr.PrnAspectX = 0x12c;
    hdr.PrnAspectY = 0x12c;
    hdr.dxPrinter = 0x8df;
    hdr.dyPrinter = 0xce1;
    hdr.AspCorX = 0;
    hdr.AspCorY = 0;

    phdr = (USHORT *)&hdr;
    hdr.wCheck = 0;
    for (i=0; i < 12; i++)
        hdr.wCheck ^= *phdr++;

    hdr.res1 = 0;
    hdr.res2 = 0;
    hdr.res3 = 0;

    DosWrite(hFile, (PSZ)&hdr, sizeof(FHDR), (PUSHORT)&cBytesWritten);

    /* calculate sizes */
    cbIndexTable = sizeof(unsigned int) * bminfo.cy;
    cbScanLine = (bminfo.cx + 7) >> 3;
    cbScanLineExp = cbScanLine + ((cbScanLine + 0xff) >> 8);
    cbBmpLine = (cbScanLine + 3) & 0xfffc;

    /*
     * Write index table - (no compression)
     */
    /* allocate pIndex */
    if ((pIndex = (USHORT *)WinAllocMem(hHeap, cbIndexTable)) == NULL) {
        WinFreeMem(hHeap, (NPBYTE)pIndex, cbIndexTable);
        goto Exit;
    }
    pIndexT = pIndex;
    for (i=0; i < bminfo.cy; i++)
        *pIndexT++ = cbScanLineExp;

    DosWrite(hFile, (PSZ)pIndex, cbIndexTable, &cBytesWritten);

    /* free pIndex */
    WinFreeMem(hHeap, (NPBYTE)pIndex, cbIndexTable);

    /*
     * Write out each scan line
     */
    /* allocate pScanLine */
    if ((pScanLine = WinAllocMem(hHeap, cbScanLineExp)) == NULL) {
        WinFreeMem(hHeap, (NPBYTE)pScanLine, cbScanLineExp);
        goto Exit;
    }
    /* allocate pBits */
    if ((pBits = WinAllocMem(hHeap, cbBmpLine * bminfo.cy)) == NULL) {
        WinFreeMem(hHeap, pBits, cbBmpLine * bminfo.cy);
        goto Exit;
    }

    /* specify the bitmap format we want */
    bminfo.cPlanes = 1;
    bminfo.cBitCount = 1;

    /* Create memory DC */
    hdc = CreateDC((PSZ)"MEMORY", (HDC)NULL);

    /* create a memory PS */
    size.cx = bminfo.cx;
    size.cy = bminfo.cy;
    hpsMem = GpiCreatePS( habSnap, hdc, &size,
            PU_ARBITRARY | GPIT_NORMAL | GPIA_ASSOC);

    hbmOld = GpiSetBitmap(hpsMem, hbm);
    GpiQueryBitmapBits(hpsMem, (LONG)0, (LONG)bminfo.cy, (PBYTE)pBits,
            (PBITMAPINFO)&bminfo);

    pBitsT = pBits + cbBmpLine * bminfo.cy;
    for (i = 0; i < bminfo.cy; i++) {
        pBitsT -= cbBmpLine;
        Copy(pBitsT, pScanLine, cbScanLine);
        convert(pScanLine, cbScanLine, cbScanLineExp);
        DosWrite(hFile, (PSZ)pScanLine, cbScanLineExp, &cBytesWritten);
    }

    /* free pBits and pScanLine */
    WinFreeMem(hHeap, pBits, cbScanLine * bminfo.cy);
    WinFreeMem(hHeap, pScanLine, cbScanLineExp);

    GpiAssociate(hpsMem, NULL);
    DevCloseDC(hdc);
    GpiDestroyPS(hpsMem);

Exit:
    DosClose(hFile);
}


/***************************************************************************\
* Insert apropriate repeat count bytes into the scanline given.
\***************************************************************************/
void convert(pbuf, cch, cch2)
NPBYTE pbuf;
USHORT cch; /* exact length of buffer BEFORE count bytes have been added */
USHORT cch2; /* AFTER conversion size */
{
    NPBYTE pWrite, pRead;

    pWrite = pbuf + cch2 - 1;
    pRead = pbuf + cch - 1;
    while (pWrite > pbuf) {
        *pWrite-- = *pRead--;
        if (((pWrite - pbuf) & 0x00ff) == 0) {
            *pWrite-- = (BYTE)((pbuf + cch2 - pWrite) < 0x100 ?
                    (pbuf + cch2 - pWrite) : 0xff);
        }
    }
}

void Copy(src, dest, cb)
NPBYTE src;
NPBYTE dest;
USHORT cb;
{
    USHORT i;

    for (i = 0; i < cb; i++)
        *dest++ = *src++;
}

HFILE OpenSaveFile(hwnd)
HWND hwnd;
{
    HFILE   hFile= 0L;
    DLGP    dlgp;
    char    fName[32];
    USHORT  action;

    dlgp.cch = 32;
    dlgp.psz = fName;
    if (WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)SaveFileDlgProc, (HMODULE) NULL,
            IDD_SAVEFILE, (PVOID)&dlgp) != 0) {
        DosOpen(fName, &hFile, &action, 0L, 0, 0x0011, 0x0011, 0L);
    }
    return(hFile);
}

MRESULT EXPENTRY SaveFileDlgProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    DLGP FAR *pdlgp;
    SHORT cch;

    switch (msg) {
    case WM_INITDLG:
        /* squirl away the dlgp pointer in a reentrant fashion */
        WinSetWindowULong(hwnd, QWL_USER, (ULONG)mp2);
        break;

    case WM_CHAR:
        if ( (SHORT1FROMMP(mp1) & KC_VIRTUALKEY) &&
                ((SHORT2FROMMP(mp2) == VK_NEWLINE) ||
                 (SHORT2FROMMP(mp2) == VK_ENTER)) ) {
            if (pdlgp = (DLGP FAR *)WinQueryWindowULong(hwnd, QWL_USER)) {
                cch = WinQueryWindowText(WinWindowFromID(hwnd, ID_FILEENTRY),
                        pdlgp->cch, pdlgp->psz);
                WinDismissDlg(hwnd, cch);
            } else {
                WinDismissDlg(hwnd, 0);
            }
            return(0);
        }
        break;
    }

    return(WinDefDlgProc(hwnd, msg, mp1, mp2));
}
