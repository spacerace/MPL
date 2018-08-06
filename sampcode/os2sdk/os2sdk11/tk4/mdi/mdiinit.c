/***************************************************************************\
* mdiinit.c - MDI initialization funtions.
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#define INCL_WINHEAP
#define INCL_GPIBITMAPS
#define INCL_GPILCIDS
#define INCL_DEV

#include <os2.h>
#include "app.h"
#include "appdata.h"
#include "mdi.h"
#include "mdidata.h"

/* Function prototypes */
VOID InitMDIPointers(VOID);
BOOL SetupMenuStuff(HPS hps);
VOID InitSplitbars(VOID);
VOID InitInitialDocPlacement(VOID);


BOOL MDIInit(VOID)
{
    HPS hps;

    /*
     * Register the splitbar window class.
     */
    if (!WinRegisterClass(NULL, szSplitbarClass, (PFNWP)SplitbarWndProc,
            CS_MOVENOTIFY | CS_SYNCPAINT, 0))
        return(FALSE);

    InitMDIPointers();

    pfnMainFrameWndProc = WinSubclassWindow(hwndMDIFrame,
            (PFNWP)MainFrameWndProc);

    hps = WinGetPS(hwndMDI);

    if (SetupMenuStuff(hps) == FALSE)
        return(FALSE);

    WinReleasePS(hps);

    InitSplitbars();

    InitInitialDocPlacement();

    return(TRUE);
}


VOID InitInitialDocPlacement(VOID)
{
    RECTL rclDesktop;

    /*
     * New documents will be 2/3 the width of the
     * screen and 1/2 the height of the screen.
     */
    cxNewDoc = (cxScreen * 2) / 3;
    cyNewDoc = cyScreen / 2;

    WinQueryWindowRect(hwndMDI, &rclDesktop);
    xNextNewDoc = 0;
    yNextNewDoc = (SHORT)rclDesktop.yTop - cyNewDoc;

    xCascadeInc = cxSizeBorder + (cxMinmaxButton / 2);
    yCascadeInc = cySizeBorder + cyTitlebar - cyBorder;

    xCascadeSlot = 0;
}


VOID InitSplitbars()
{
    /*
     * Calculate the values cxVertSplitbar and cyHorzSplitbar.
     *
     * The horzontal splitbar is 1/3 the height of the vertical
     * scrollbar arrow.  We use the aspect ratio of the display
     * to determine the width of the vertical splitbar.
     */
    cyHorzSplitbar = cyVScrollArrow / 3;
    cxVertSplitbar = (USHORT)(cyHorzSplitbar *
            rglDevCaps[CAPS_HORIZONTAL_RESOLUTION] /
            rglDevCaps[CAPS_VERTICAL_RESOLUTION]);
}


VOID InitMDIPointers(VOID)
{
    hptrHorzSplit = WinLoadPointer(HWND_DESKTOP, NULL, IDP_HSPLIT);
    hptrVertSplit = WinLoadPointer(HWND_DESKTOP, NULL, IDP_VSPLIT);
    hptrHVSplit = WinLoadPointer(HWND_DESKTOP, NULL, IDP_HVSPLIT);
    hptrArrow = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, TRUE);
}


BOOL SetupMenuStuff(HPS hps)
{
    MENUITEM mi;
    BITMAPINFOHEADER bmpNew, bmpCurrent;
    SIZEL gsize;
    RECTL rclSrc;
    POINTL ptlDst;
    DRIVDATA driv;
    DEVOPENSTRUC dop;

    hbmChildSysMenu = WinGetSysBitmap(HWND_DESKTOP, SBMP_CHILDSYSMENU);

    GpiQueryFontMetrics(hps, (ULONG)sizeof(FONTMETRICS),
            (PFONTMETRICS)&fmSystemFont);

    GpiQueryBitmapParameters(hbmChildSysMenu, &bmpCurrent);
    bmpNew.cbFix = sizeof(BITMAPINFOHEADER);
    bmpNew.cx = bmpCurrent.cx + (((bmpCurrent.cx - cxBorder) / 4) * 2);
    bmpNew.cy = bmpCurrent.cy;
    bmpNew.cPlanes = bmpCurrent.cPlanes;
    bmpNew.cBitCount = bmpCurrent.cBitCount;
    hbmAabChildSysMenu = GpiCreateBitmap(hps, &bmpNew, 0L, NULL, NULL);

    driv.cb = sizeof(ULONG) * 11;
    driv.lVersion = 0;
    driv.szDeviceName[0] = 0;
    driv.abGeneralData[0] = 0L;

    dop.pszDriverName = NULL;
    dop.pdriv = (PDRIVDATA)&driv;

    dop.pszLogAddress =
    dop.pszDataType =
    dop.pszComment =
    dop.pszQueueProcName =
    dop.pszQueueProcParams =
    dop.pszSpoolerParams =
    dop.pszNetworkParams = NULL;

    gsize.cx = bmpNew.cx;
    gsize.cy = bmpNew.cy;

    hdcMem = DevOpenDC(hps, OD_MEMORY, "*", 8L, (PDEVOPENDATA)&dop, NULL);
    hpsMem = GpiCreatePS(NULL, hdcMem, (PSIZEL)&gsize,
            PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    GpiSetBitmap(hpsMem, hbmAabChildSysMenu);

    rclSrc.xLeft = 0;
    rclSrc.yBottom = 0;
    rclSrc.xRight = bmpNew.cx;
    rclSrc.yTop = bmpNew.cy;
    WinFillRect(hpsMem, (PRECTL)&rclSrc, CLR_FALSE);

    rclSrc.xRight = bmpCurrent.cx - cxBorder;
    rclSrc.yTop = bmpCurrent.cy;
    ptlDst.x = (bmpCurrent.cx - cxBorder) / 4;
    ptlDst.y = 0;
    WinDrawBitmap(hpsMem, hbmChildSysMenu, (PRECTL)&rclSrc, (PPOINTL)&ptlDst,
            CLR_TRUE, CLR_FALSE, DBM_NORMAL);

    GpiSetBitmap(hpsMem, NULL);
    GpiDestroyPS(hpsMem);
    DevCloseDC(hdcMem);

    /*
     * Get menu window handles.
     */
    hwndMainMenu = WinWindowFromID(hwndMDIFrame, FID_MENU);
    WinSendMsg(hwndMainMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_WINDOW, FALSE),
               MPFROMP(&mi));
    hwndWindowMenu = mi.hwndSubMenu;
    hwndSysMenu = WinWindowFromID(hwndMDIFrame, FID_SYSMENU);
    hwndAppMenu = WinWindowFromID(hwndMDIFrame, FID_MENU);

    /* set up menuitem for application menu bar sysmenu */
    WinSendMsg(hwndMainMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_AABDOCSYSMENU, FALSE),
               MPFROMP(&miAabSysMenu));
    miAabSysMenu.hItem = (ULONG)hbmAabChildSysMenu;
    WinSendMsg(hwndMainMenu, MM_REMOVEITEM, MPFROM2SHORT(IDM_AABDOCSYSMENU, FALSE),
               (MPARAM)NULL);

    return(TRUE);
}


VOID MDITerminate(VOID)
{
    /*
     * Only destroy the AabSysMenu if it isn't already in the AAB.
     * If it is we don't need to destroy it since the frame window
     * manager will.
     */
    if (fAabSysMenu == FALSE)
        WinDestroyWindow(miAabSysMenu.hwndSubMenu);

    GpiDeleteBitmap(hbmChildSysMenu);
    GpiDeleteBitmap(hbmAabChildSysMenu);

    WinDestroyPointer(hptrHorzSplit);
    WinDestroyPointer(hptrVertSplit);
    WinDestroyPointer(hptrHVSplit);
    WinDestroyPointer(hptrArrow);
}
