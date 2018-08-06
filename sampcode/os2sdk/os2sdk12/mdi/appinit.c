/*
    mdiinit.c - MDI initialization funtions.
    Created by Microsoft Corporation, 1989
*/
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
BOOL RegisterWindowClasses(VOID);
VOID InitSysValues(VOID);


BOOL AppInit(VOID)
{
    ULONG ctlData;
    HPS hps;
    HDC hdc;

    hab = WinInitialize(0);

    hmqMDI = WinCreateMsgQueue(hab, 0);

    if (!RegisterWindowClasses())
        return(FALSE);

    ctlData = FCF_TITLEBAR | FCF_MINMAX | FCF_SIZEBORDER | FCF_SYSMENU |
              FCF_MENU | FCF_TASKLIST | FCF_SHELLPOSITION | FCF_ICON;

    hwndMDIFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE,
            (VOID FAR *)&ctlData, szMDIClass, (PSZ)NULL,
            WS_VISIBLE | WS_CLIPCHILDREN, NULL, IDR_MDI,
            (HWND FAR *)&hwndMDI);


    if (hwndMDIFrame == NULL)
        return(FALSE);

/* MULTIPLEMENU */

    /* Remember the first menu so we can put it back when all the documents are
       closed */
    hwndFirstMenu=WinWindowFromID(hwndMDIFrame, FID_MENU);

    hHeap = WinCreateHeap(0, 0, 0, 0, 0, 0);

    if (hHeap == NULL)
        return(FALSE);

    hps = WinGetPS(hwndMDI);

    hdc = GpiQueryDevice(hps);
    DevQueryCaps(hdc, CAPS_FAMILY, CAPS_VERTICAL_FONT_RES, (PLONG)rglDevCaps);

    WinReleasePS(hps);

    InitSysValues();

    return(TRUE);
}


VOID InitSysValues(VOID)
{
    cyTitlebar = (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
    cyIcon = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYICON);

    cxBorder = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
    cyBorder = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);

    cxSizeBorder = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
    cySizeBorder = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);

    cxByteAlign = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXBYTEALIGN);
    cyByteAlign = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYBYTEALIGN);

    cxVScroll = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYHSCROLL);
    cyVScrollArrow = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYVSCROLLARROW);
    cyHScroll = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);

    cxScreen = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    cyScreen = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

    cxMinmaxButton = (USHORT)WinQuerySysValue(HWND_DESKTOP, SV_CXMINMAXBUTTON);
}


BOOL RegisterWindowClasses(VOID)
{
    if (!WinRegisterClass(NULL, szMDIClass, (PFNWP)MDIWndProc,
            CS_SYNCPAINT, 0))
        return(FALSE);

    if (!WinRegisterClass(NULL, szDocClass, (PFNWP)DocWndProc,
            0L, sizeof(NPVIEW)))
        return(FALSE);

    return(TRUE);
}


VOID AppTerminate(VOID)
{
    WinDestroyWindow(hwndMDIFrame);

    WinDestroyHeap(hHeap);

    WinDestroyMsgQueue(hmqMDI);

    WinTerminate(hab);
}
