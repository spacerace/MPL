/* Global data extern declarations */
extern char szMDIClass[], szDocClass[];
extern USHORT cxBorder, cyBorder, cyHScroll, cxVScroll, cyVScrollArrow;
extern USHORT cxScreen, cyScreen, cyIcon, cxByteAlign, cyByteAlign;
extern USHORT cxSizeBorder, cySizeBorder;
extern ULONG clrNext;
extern LONG rglDevCaps[];

extern HAB hab;
extern HMQ hmqMDI;
extern HHEAP hHeap;
extern HWND hwndMDI, hwndMDIFrame, hwndActiveDoc;
extern HWND hwndMainMenu, hwndWindowMenu;
extern HWND hwndSysMenu, hwndAppMenu;
extern FONTMETRICS fmSystemFont;
extern NPDOC npdocFirst;


/* Procedure declarations */
MRESULT CALLBACK MDIWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK DocWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK DocFrameWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK MainFrameWndProc(HWND, USHORT, MPARAM, MPARAM);
